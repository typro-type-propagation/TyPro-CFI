#include "llvm/IR/AutoUpgrade.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Typegraph/Typegraph.h"
#include "llvm/Typegraph/TypegraphSettings.h"
#include "llvm/Typegraph/timeclock.h"
#include "llvm/Typegraph/typegraph_llvm_utils.h"
#include <llvm/Typegraph/typegraph_layering.h>
#include <stdio.h>

using namespace llvm;
using namespace typegraph;

static cl::list<std::string> InputFilenames(cl::Positional, cl::OneOrMore,
                                            cl::desc("<input graph files>"));

static cl::opt<std::string> OutputFilename("o", cl::desc("Output filename"),
                                           cl::init("-"),
                                           cl::value_desc("filename"));

static cl::opt<bool> ComputeFunctionUsage("function-usages",
                                          cl::desc("Compute function usages"));

static cl::opt<bool> ComputeEquivalences("equivalences",
                                         cl::desc("Compute equivalence classes"));

static cl::opt<bool> ComputeEquivalencesLite("equivalences-lite",
                                         cl::desc("Compute equivalence classes, but respect types"));

static cl::opt<bool> CheckIntegrity("check-integrity",
                                          cl::desc("Check integrity of graph/IR"));

static cl::opt<bool> Verbose("v",
                             cl::desc("Print information about actions taken"));

static ExitOnError ExitOnErr;

namespace {
inline bool startsWith(const std::string &Check, const std::string &Prefix) {
  return std::equal(Prefix.begin(), Prefix.end(), Check.begin());
}

inline bool endsWith(std::string const &FullString, std::string const &Ending) {
  if (FullString.length() >= Ending.length()) {
    return (0 == FullString.compare(FullString.length() - Ending.length(),
                                    Ending.length(), Ending));
  }
  return false;
}
} // namespace

static std::unique_ptr<Module> loadFile(const std::string &FN,
                                        LLVMContext &Context) {
  SMDiagnostic Err;
  std::unique_ptr<Module> Result = parseIRFile(FN, Err, Context);
  if (!Result) {
    Err.print("llvm-typegraph", errs());
    return nullptr;
  }
  ExitOnErr(Result->materializeMetadata());
  UpgradeDebugInfo(*Result);
  return Result;
}

int main(int argc, char **argv) {
  InitLLVM X(argc, argv);
  ExitOnErr.setBanner(std::string(argv[0]) + ": ");
  LLVMContext Context;
  cl::ParseCommandLineOptions(argc, argv, "llvm typegraph utility\n");

  TypeGraph Graph;
  auto Composite = std::make_unique<Module>("llvm-typegraph", Context);
  Linker L(*Composite);

  TimeClock Clock;
  for (const auto &File : InputFilenames) {
    if (endsWith(File, ".typegraph")) {
      Graph.loadFromFile(File);
    } else {
      auto M = loadFile(File, Context);
      auto Err = L.linkInModule(std::move(M), Linker::Flags::None);
      if (Err)
        return 1;
    }
  }
  ParseTypegraphFromMetadata(Graph, *Composite);
  if (Verbose)
    Clock.report("graph loading");

  if (CheckIntegrity) {
    Clock.restart();
    auto Errors = CheckGraphForIntegrity(Graph, *Composite);
    Clock.report("integrity check");
    if (Errors > 0) {
      llvm::errs() << "Graph not fully matching IR, terminating ...\n";
      return 1;
    }
  }

  if (Verbose) {
    llvm::outs() << "Graph: " << Graph.num_vertices() << " nodes, " << Graph.num_edges() << " edges\n";
  }

  if (ComputeFunctionUsage) {
    Clock.restart();
    Graph.computeInterfaceRelations();
    if (Settings.linktime_layering) {
      applyLinktimeLayering(Graph);
    }
    if (ComputeEquivalences) {
      Graph.combineEquivalencesInline(true, false);
    }
    if (ComputeEquivalencesLite) {
      Graph.combineEquivalencesInline(true, false, true);
    }
    if (Settings.facts_output) {
      Graph.computeFunctionUsesDebug(Settings.facts_output);
    }
    Graph.computeReachability(true, false, false);
    // Graph.computeReachabilityInline(true, false, false);
    if (Verbose)
      Clock.report("closure computation");
  } else if (ComputeEquivalences) {
    // auto Graph2 = Graph.computeEquivalenceClasses(false);
    // Graph = std::move(*Graph2);
    Graph.combineEquivalencesInline(false, false);
    if (Verbose) {
      Clock.report("equivalence computation");
      llvm::outs() << "Graph: " << Graph.num_vertices() << " nodes, " << Graph.num_edges() << " edges\n";
    }
  } else if (ComputeEquivalencesLite) {
    Graph.combineEquivalencesInline(false, false, true);
    if (Verbose) {
      Clock.report("equivalence computation (lite)");
      llvm::outs() << "Graph: " << Graph.num_vertices() << " nodes, " << Graph.num_edges() << " edges\n";
    }
  }

  if (!OutputFilename.empty()) {
    Clock.restart();
    Graph.saveToFile(OutputFilename);
    if (Verbose)
      Clock.report("export to file");
    if (Settings.tool_show_graph) {
      std::string Cmd =
          "python3 ../../../tests/show_typegraph.py '" + OutputFilename + "'";
      (void)system(Cmd.c_str());
    }
    // llvm::outs() << "Saved file " << OutputFilename << "\n";
  }

  return 0;
}
