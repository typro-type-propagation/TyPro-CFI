#include "nlohmann/json.hpp"
#include <atomic>
#include <csignal>
#include <cstddef>
#include <cstring>
#include <map>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#ifndef __THROW
#define __THROW
#endif

extern "C" {
// NOLINTNEXTLINE
[[maybe_unused]] void __calltargets_initialize(const char *CallName, size_t CallCount, void **Ident);
// NOLINTNEXTLINE
[[maybe_unused]] void __calltargets_add_function(const char *FunctionName, void *FunctionAddress);
// NOLINTNEXTLINE
[[maybe_unused]] void __calltargets_count(void **Ident, size_t CallNumber, void *Function);

int kill(pid_t Pid, int Sig) __THROW;
void _exit(int Status);
}

namespace {

void *SharedMemoryAllocatorBase = nullptr;
#define SharedMemoryLength (4096 * 4096)


template <class T>
struct SharedMemoryAllocator {
  using value_type = T;
  SharedMemoryAllocator() noexcept {
    if (!SharedMemoryAllocatorBase) {
      SharedMemoryAllocatorBase = mmap(0, SharedMemoryLength, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
      if (SharedMemoryAllocatorBase == MAP_FAILED) {
        perror("mmap");
        _exit(1);
      }
      new (SharedMemoryAllocatorBase) std::atomic_size_t(sizeof(std::atomic_size_t));
    }
  }
  T* allocate (std::size_t N) {
    std::atomic_size_t *Size = (std::atomic_size_t*) SharedMemoryAllocatorBase;
    size_t Offset = Size->fetch_add(sizeof(T) * N);
    return (T*) (((char*) SharedMemoryAllocatorBase) + Offset);
  }
  void deallocate (T* P, std::size_t N) {}
};


struct BitSet {
  unsigned char *Buffer = nullptr;
  inline void set(size_t Bit, size_t Max) {
    if (Buffer == nullptr) {
      // Buffer = new unsigned char[(Max + 8) / 8];
      SharedMemoryAllocator<unsigned char> Allocator;
      Buffer = Allocator.allocate((Max + 8) / 8);
      bzero(Buffer, (Max + 8) / 8);
    }
    Buffer[Bit / 8] |= (1 << (Bit % 8));
  }
  inline bool get(size_t Bit) { return (Buffer[Bit / 8] & (1 << (Bit % 8))) != 0; }
};

struct CTModule {
  std::vector<const char *> CallNames;
  std::vector<BitSet, SharedMemoryAllocator<BitSet>> CalledFunctions; // call id => used function ids
};

struct CTInfos {
  std::map<void *, size_t> FunctionAddressToIndex;
  std::vector<const char *> FunctionNames{"<<undefined>>"};
  std::vector<std::unique_ptr<CTModule>> Modules;
};

CTInfos *CallTargetInfos = new CTInfos();

std::vector<std::string> getArgv() {
  std::vector<std::string> Result;
  FILE *F = fopen("/proc/self/cmdline", "r");
  if (F) {
    std::string Buffer;
    int C = fgetc(F);
    while (C != EOF) {
      if (C == '\0') {
        Result.push_back(Buffer);
        Buffer.clear();
      } else {
        Buffer.push_back((char) C);
      }
      C = fgetc(F);
    }
  }
  return Result;
}

struct CallTargetReporter {
  static thread_local bool IsCurrentlyInternal;

  CallTargetReporter() {
    // Use these lines to attach a debugger while running under SPEC's runner
    // fprintf(stderr, "=== PID = %d ===\n\n\n", getpid());
    // sleep(30);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGUSR2, signalHandler);
  }
  ~CallTargetReporter() {
    IsCurrentlyInternal = true;
    report();
    IsCurrentlyInternal = false;
  }
  static void signalHandler(int Signal) { _exit(Signal); }

  static bool HasReported;
  static void report() {
    size_t TotalCalls = 0;
    for (auto &Module: CallTargetInfos->Modules) {
      TotalCalls += Module->CallNames.size();
    }
    if (TotalCalls > 0) {
      if (HasReported)
        return;
      HasReported = true;
      // collect data
      nlohmann::json J;
      J["binary"] = program_invocation_name;
      J["cmdline"] = getArgv();
      J["num_calls"] = TotalCalls;
      J["num_functions"] = CallTargetInfos->FunctionAddressToIndex.size();
      J["module_count"] = CallTargetInfos->Modules.size();
      for (auto &Module: CallTargetInfos->Modules) {
        for (size_t CallId = 0; CallId < Module->CallNames.size(); CallId++) {
          auto Arr = nlohmann::json::array();
          if (Module->CalledFunctions[CallId].Buffer != nullptr) {
            for (size_t FuncId = 0; FuncId < CallTargetInfos->FunctionNames.size(); FuncId++) {
              if (Module->CalledFunctions[CallId].get(FuncId)) {
                Arr.push_back(CallTargetInfos->FunctionNames[FuncId]);
              }
            }
          }
          J["calls"][Module->CallNames[CallId]] = Arr;
        }
      }
      // get a unique filename
      std::string Filename;
      std::string Basename(program_invocation_name);
      // if (Basename.find("redis-server") == std::string::npos) return;
      if (getenv("TG_RT_CALLTARGET_BASE")) {
        Basename = getenv("TG_RT_CALLTARGET_BASE");
      }
      int I = 0;
      do {
        Filename = Basename + ".calltargets" + std::to_string(I++) + ".json";
      } while (access(Filename.c_str(), F_OK) == 0);
      // write file
      auto *F = fopen(Filename.c_str(), "w");
      if (!F) {
        if (!getenv("TG_RT_SILENT"))
          fprintf(stderr, "[%d] ERROR: Can't write \"%s\".\n", getpid(), Filename.c_str());
        return;
      }
      auto S = J.dump(2);
      fputs(S.c_str(), F);
      fclose(F);
      if (!getenv("TG_RT_SILENT"))
        fprintf(stderr, "[%d] Wrote report: \"%s\"\n", getpid(), Filename.c_str());
    } else {
      // fprintf(stderr, "No report, because no indirect calls happened\n");
    }
  }
};
bool CallTargetReporter::HasReported = false;
thread_local bool CallTargetReporter::IsCurrentlyInternal = false;

CallTargetReporter Reporter;

struct CurrentlyInteralGuard {
  inline CurrentlyInteralGuard() { CallTargetReporter::IsCurrentlyInternal = true; }
  inline ~CurrentlyInteralGuard() { CallTargetReporter::IsCurrentlyInternal = false; }
};

} // namespace

// NOLINTNEXTLINE
void __calltargets_initialize(const char *CallName, size_t CallCount, void **Ident) {
  CurrentlyInteralGuard Guard;
  std::unique_ptr<CTModule> Module = std::make_unique<CTModule>();
  Module->CallNames.reserve(CallCount);
  for (size_t I = 0; I < CallCount; I++) {
    Module->CallNames.push_back(CallName);
    CallName += strlen(CallName) + 1;
  }
  Module->CalledFunctions.assign(Module->CallNames.size(), BitSet());
  *Ident = (void*) Module.get();
  CallTargetInfos->Modules.emplace_back(std::move(Module));
}

// NOLINTNEXTLINE
[[maybe_unused]] void __calltargets_add_function(const char *FunctionName, void *FunctionAddress) {
  CurrentlyInteralGuard Guard;
  CallTargetInfos->FunctionAddressToIndex[FunctionAddress] = CallTargetInfos->FunctionNames.size();
  CallTargetInfos->FunctionNames.push_back(FunctionName);
}

// NOLINTNEXTLINE
void __calltargets_count(void **Ident, size_t CallNumber, void *Function) {
  if (*Ident == nullptr || CallTargetReporter::IsCurrentlyInternal)
    return;
  CurrentlyInteralGuard Guard;
  auto It = CallTargetInfos->FunctionAddressToIndex.find(Function);
  auto FunctionNumber = It == CallTargetInfos->FunctionAddressToIndex.end() ? 0 : It->second;
  ((CTModule**) Ident)[0]->CalledFunctions[CallNumber].set(FunctionNumber, CallTargetInfos->FunctionNames.size());
}

void _exit(int Status) {
  CurrentlyInteralGuard Guard;
  CallTargetReporter::report();
  syscall(231, Status); // EXIT_GROUP
  syscall(60, Status);  // EXIT
  abort();
}

int kill(pid_t Pid, int Sig) __THROW {
  if (Sig == SIGKILL || Sig == SIGTERM) {
    int Result1 = syscall(62, Pid, SIGUSR2);
    usleep(200000);
    int Result2 = syscall(62, Pid, Sig);
    errno = Result1 == 0 ? Result1 : Result2;
  } else {
    errno = syscall(62, Pid, Sig);
  }
  return errno == 0 ? 0 : -1;
}
