#include "TypegraphFunctionInfos.h"
#include <algorithm>
#include <cstring>
#include <vector>
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace {

struct LibraryFunctionInfo {
  const char *Name;
  LibraryFunctionHandling Handling;
  std::vector<int> Indices;
};

bool operator==(const LibraryFunctionInfo &S1, const LibraryFunctionInfo &S2) {
  return strcmp(S1.Name, S2.Name) == 0;
}

bool operator==(const LibraryFunctionInfo &S1, const char *S2) {
  return strcmp(S1.Name, S2) == 0;
}

bool operator<(const LibraryFunctionInfo &S1, const LibraryFunctionInfo &S2) {
  return strcmp(S1.Name, S2.Name) < 0;
}

bool operator<(const LibraryFunctionInfo &S1, const char *S2) {
  return strcmp(S1.Name, S2) < 0;
}

LibraryFunctionInfo KnownStdlibFunctions[] = {
    {"__assert_fail", IGNORE},
    {"__ctype_b_loc", IGNORE},
    {"__ctype_tolower_loc", IGNORE},
    {"__ctype_toupper_loc", IGNORE},
    {"__errno_location", IGNORE},
    {"__fprintf_chk", IGNORE_BUT_FILE},
    {"__fxstat", IGNORE},
    {"__fxstat64", IGNORE},
    {"__isoc99_fscanf", IGNORE},
    {"__isoc99_sscanf", IGNORE},
    {"__libc_sigaction", RESOLVE_DEEP_ALWAYS, {1, 0, 0}},
    {"__lxstat", IGNORE},
    {"__lxstat64", IGNORE},
    {"__memcpy_chk", PER_CALL_CONTEXT},
    {"__memmove_chk", PER_CALL_CONTEXT},
    {"__printf_chk", IGNORE},
    {"__pthread_once", RESOLVE},
    {"__restore", IGNORE},
    {"__restore_rt", IGNORE},
    {"__snprintf_chk", IGNORE},
    {"__strncpy_chk", PER_CALL_CONTEXT},
    {"__syslog_chk", IGNORE},
    {"__timezone", IGNORE},
    {"__vsnprintf_chk", IGNORE},
    {"__xpg_strerror_r", IGNORE},
    {"__xstat", IGNORE},
    {"__xstat64", IGNORE},
    {"_exit", IGNORE},
    {"_obstack_begin", RESOLVE},
    {"_pthread_cleanup_push", RESOLVE},
    {"_setjmp", IGNORE},
    {"abort", IGNORE},
    {"accept", IGNORE},
    {"access", IGNORE},
    {"acos", IGNORE},
    {"alarm", IGNORE},
    {"asin", IGNORE},
    {"asprintf", IGNORE},
    {"atan", IGNORE},
    {"atan2", IGNORE},
    {"atexit", RESOLVE},
    {"atoi", IGNORE},
    {"bcmp", IGNORE},
    {"bind", IGNORE},
    {"bsearch", RESOLVE},
    {"calloc", IGNORE},
    {"ceil", IGNORE},
    {"chdir", IGNORE},
    {"chmod", IGNORE},
    {"chown", IGNORE},
    {"chroot", IGNORE},
    {"clearenv", IGNORE},
    {"clearerr", IGNORE},
    {"clock_gettime", IGNORE},
    {"close", IGNORE},
    {"closedir", IGNORE},
    {"closelog", IGNORE},
    {"connect", IGNORE},
    {"cos", IGNORE},
    {"cosh", IGNORE},
    {"ctime", IGNORE},
    {"ctime_r", IGNORE},
    {"dladdr", IGNORE},
    {"dlclose", IGNORE},
    {"dlopen", IGNORE},
    {"dlsym", DLSYM},
    {"dup", IGNORE},
    {"dup2", IGNORE},
    {"environ", IGNORE},
    {"epoll_create", IGNORE},
    {"epoll_ctl", IGNORE},
    {"epoll_wait", IGNORE},
    {"error", IGNORE},
    {"execl", IGNORE},
    {"execv", IGNORE},
    {"execve", IGNORE},
    {"execvp", IGNORE},
    {"exit", IGNORE},
    {"exp", IGNORE},
    {"fchmod", IGNORE},
    {"fclose", IGNORE_BUT_FILE},
    {"fcntl", IGNORE},
    {"fcntl64", IGNORE},
    {"fdatasync", IGNORE},
    {"fdopen", IGNORE},
    {"feof", IGNORE_BUT_FILE},
    {"ferror", IGNORE_BUT_FILE},
    {"fflush", IGNORE_BUT_FILE},
    {"fgetc", IGNORE_BUT_FILE},
    {"fgets", IGNORE_BUT_FILE},
    {"fgets_unlocked", IGNORE},
    {"flock", IGNORE},
    {"floor", IGNORE},
    {"fmod", IGNORE},
    {"fopen", IGNORE_BUT_FILE},
    {"fopen64", IGNORE_BUT_FILE},
    {"fork", IGNORE},
    {"fprintf", IGNORE_BUT_FILE},
    {"fputc", IGNORE_BUT_FILE},
    {"fputc_unlocked", IGNORE_BUT_FILE},
    {"fputs", IGNORE_BUT_FILE},
    {"fread", IGNORE_BUT_FILE},
    {"fread_unlocked", IGNORE_BUT_FILE},
    {"free", IGNORE},
    {"freeaddrinfo", IGNORE},
    {"freopen", IGNORE_BUT_FILE},
    {"fseek", IGNORE_BUT_FILE},
    {"fstat", IGNORE},
    {"fsync", IGNORE},
    {"ftell", IGNORE_BUT_FILE},
    {"ftello64", IGNORE_BUT_FILE},
    {"ftruncate", IGNORE},
    {"ftruncate64", IGNORE},
    {"fwrite", IGNORE_BUT_FILE},
    {"fwrite_unlocked", IGNORE_BUT_FILE},
    {"gcvt", IGNORE},
    {"getaddrinfo", IGNORE},
    {"getc", IGNORE_BUT_FILE},
    {"getcwd", IGNORE},
    {"getegid", IGNORE},
    {"getenv", IGNORE},
    {"geteuid", IGNORE},
    {"getgid", IGNORE},
    {"getgrgid", IGNORE},
    {"getgroups", IGNORE},
    {"getopt_long", IGNORE},
    {"getopt_long_only", IGNORE},
    {"getpagesize", IGNORE},
    {"getpeername", IGNORE},
    {"getpid", IGNORE},
    {"getrlimit64", IGNORE},
    {"getrusage", IGNORE},
    {"getsockname", IGNORE},
    {"getsockopt", IGNORE},
    {"gettimeofday", IGNORE},
    {"getuid", IGNORE},
    {"gmtime", IGNORE},
    {"inet_ntop", IGNORE},
    {"inet_pton", IGNORE},
    {"ioctl", IGNORE},
    {"isatty", IGNORE},
    {"kill", IGNORE},
    {"ldexp", IGNORE},
    {"lfind", RESOLVE},
    {"link", IGNORE},
    {"listen", IGNORE},
    {"localeconv", IGNORE},
    {"localtime", IGNORE},
    {"localtime_r", IGNORE},
    {"log", IGNORE},
    {"log10", IGNORE},
    {"longjmp", IGNORE},
    {"lsearch", RESOLVE},
    {"lseek", IGNORE},
    {"lseek64", IGNORE},
    {"malloc", IGNORE},
    {"memchr", IGNORE},
    {"memcmp", IGNORE},
    {"memcpy", MEMCPY},
    {"memmove", MEMCPY},
    {"memset", PER_CALL_CONTEXT},
    {"mkdir", IGNORE},
    {"mkostemp64", IGNORE},
    {"modf", IGNORE},
    {"nanosleep", IGNORE},
    {"open", IGNORE},
    {"open64", IGNORE},
    {"opendir", IGNORE},
    {"openlog", IGNORE},
    {"perror", IGNORE},
    {"pipe", IGNORE},
    {"poll", IGNORE},
    {"pow", IGNORE},
    {"prctl", IGNORE},
    {"printf", IGNORE},
    {"pthread_atfork", RESOLVE},
    {"pthread_cleanup_push", RESOLVE},
    {"pthread_create", RESOLVE_WITH_DATA, {3, 0}},
    {"pthread_once", RESOLVE},
    {"putc", IGNORE},
    {"putchar", IGNORE},
    {"putenv", IGNORE},
    {"puts", IGNORE},
    {"qsort", RESOLVE},
    {"qsort_r", RESOLVE},
    {"rand", IGNORE},
    {"random", IGNORE},
    {"read", IGNORE},
    {"readdir", IGNORE},
    {"realloc", PER_CALL_CONTEXT},
    {"recv", IGNORE},
    {"recvmsg", IGNORE},
    {"remove", IGNORE},
    {"rename", IGNORE},
    {"rewind", IGNORE},
    {"rmdir", IGNORE},
    {"round", IGNORE},
    {"roundf", IGNORE},
    {"scandirat", RESOLVE},
    {"select", IGNORE},
    {"send", IGNORE},
    {"sendmsg", IGNORE},
    {"setbuf", IGNORE},
    {"setegid", IGNORE},
    {"setenv", IGNORE},
    {"seteuid", IGNORE},
    {"setgid", IGNORE},
    {"setitimer", IGNORE},
    {"setlocale", IGNORE},
    {"setrlimit64", IGNORE},
    {"setsid", IGNORE},
    {"setsockopt", IGNORE},
    {"setuid", IGNORE},
    {"setvbuf", IGNORE},
    {"sfprintf", IGNORE},
    {"sigaction", RESOLVE_DEEP, {1, 0, 0}},
    {"sigaddset", IGNORE},
    {"sigemptyset", IGNORE},
    {"signal", RESOLVE},
    {"sin", IGNORE},
    {"sinh", IGNORE},
    {"sleep", IGNORE},
    {"snprintf", IGNORE},
    {"socket", IGNORE},
    {"sprintf", IGNORE},
    {"sqrt", IGNORE},
    {"srand", IGNORE},
    {"srandom", IGNORE},
    {"stat", IGNORE},
    {"stat64", IGNORE},
    {"strcasecmp", IGNORE},
    {"strcat", PER_CALL_CONTEXT},
    {"strchr", IGNORE},
    {"strcmp", IGNORE},
    {"strcoll", IGNORE},
    {"strcpy", PER_CALL_CONTEXT},
    {"strcspn", IGNORE},
    {"strdup", PER_CALL_CONTEXT},
    {"strerror", IGNORE},
    {"strftime", IGNORE},
    {"strlen", IGNORE},
    {"strncasecmp", IGNORE},
    {"strncat", IGNORE},
    {"strncmp", IGNORE},
    {"strncpy", PER_CALL_CONTEXT},
    {"strpbrk", IGNORE},
    {"strrchr", IGNORE},
    {"strspn", IGNORE},
    {"strstr", IGNORE},
    {"strtod", IGNORE},
    {"strtok", IGNORE},
    {"strtol", IGNORE},
    {"strtoul", IGNORE},
    {"strtoull", IGNORE},
    {"sync_file_range", IGNORE},
    {"sysconf", IGNORE},
    {"syslog", IGNORE},
    {"tan", IGNORE},
    {"tanh", IGNORE},
    {"tdelete", RESOLVE},
    {"tdestroy", RESOLVE},
    {"tfind", RESOLVE},
    {"time", IGNORE},
    {"timegm", IGNORE},
    {"timezone", IGNORE},
    {"tmpfile", IGNORE},
    {"tolower", IGNORE},
    {"truncate", IGNORE},
    {"truncate64", IGNORE},
    {"tsearch", RESOLVE},
    {"twalk", RESOLVE},
    {"twalk_r", RESOLVE},
    {"tzset", IGNORE},
    {"umask", IGNORE},
    {"uname", IGNORE},
    {"ungetc", IGNORE},
    {"unlink", IGNORE},
    {"unsetenv", IGNORE},
    {"usleep", IGNORE},
    {"vasprintf", IGNORE},
    {"vfprintf", IGNORE_BUT_FILE},
    {"vsnprintf", IGNORE},
    {"vsprintf", IGNORE},
    {"waitpid", IGNORE},
    {"write", IGNORE},
    {"xcalloc", IGNORE},
    {"xfree", IGNORE},
    {"xmalloc", IGNORE},
    {"xrealloc", PER_CALL_CONTEXT},
    {"xstat", IGNORE},
    {"xstat64", IGNORE},
};

} // namespace

LibraryFunctionHandling GetHandlingForFunction(const char *Name) {
  auto *End = KnownStdlibFunctions + (sizeof(KnownStdlibFunctions) / sizeof(LibraryFunctionInfo));
  const LibraryFunctionInfo *Info = std::lower_bound(KnownStdlibFunctions, End, Name);
  if (Info < End && *Info == Name) {
    return Info->Handling;
  }
  return DEFAULT;
}

LibraryFunctionInfo *GetLibraryInfoForFunction(const char *Name) {
  auto *End = KnownStdlibFunctions + (sizeof(KnownStdlibFunctions) / sizeof(LibraryFunctionInfo));
  LibraryFunctionInfo *Info = std::lower_bound(KnownStdlibFunctions, End, Name);
  if (Info < End && *Info == Name) {
    return Info;
  }
  return nullptr;
}

const std::vector<int> &GetHandlingIndicesForFunction(const char *Name) {
  auto *End = KnownStdlibFunctions + (sizeof(KnownStdlibFunctions) / sizeof(LibraryFunctionInfo));
  const LibraryFunctionInfo *Info = std::lower_bound(KnownStdlibFunctions, End, Name);
  if (Info < End && *Info == Name) {
    return Info->Indices;
  }
  abort();
}

void TestHandlingForFunction() {
  auto *End = KnownStdlibFunctions + (sizeof(KnownStdlibFunctions) / sizeof(LibraryFunctionInfo));
  auto *It = KnownStdlibFunctions;
  while (It+1 < End) {
    if (!(*It < *(It+1))) {
      fprintf(stderr, "Invalid order: \"%s\" < \"%s\" = false\n", It->Name, It[1].Name);
    }
    assert(*It < *(It+1) && "KnownStdlibFunctions is not sorted!");
    It++;
  }
}

void TypegraphFunctionsSetArch(llvm::Triple::ArchType A) {
  if (A == llvm::Triple::mips64el) {
    GetLibraryInfoForFunction("sigaction")->Indices = {1, 1, 0};
  }
}

} // namespace clang
