#ifndef LLVM_TYPEGRAPHSETTINGS_H
#define LLVM_TYPEGRAPHSETTINGS_H

#include <string>

namespace typegraph {

struct TypegraphSettings {
  TypegraphSettings();

  void setOutput(const std::string &Filename);

  bool enabled;
  bool protected_libc;

  // Clang
  bool clang_show_graph;
  bool clang_minimize_graph;
  bool clang_intermediate_graph_output;
  bool clang_show_graph_all;
  bool clang_layering;
  bool clang_export_types;

  // Graphtool
  bool tool_show_graph;

  // LINK TIME (graphtool + lld)
  // write graphs (before/after analysis) to file
  const char *graph_output; // TG_GRAPH_OUTPUT
  // write facts before running souffle program
  const char *facts_output;
  // instrument all indirect calls to collect targets
  bool instrument_collectcalltargets;
  // layering?
  bool linktime_layering;
  bool linktime_layering_debug;

  bool enforce_simple;
  bool enforce_argnum;
  bool enforce;
  bool dynlib_support;
  int enforce_id_bitwidth;
  unsigned long enforce_min_id;
  int enforce_dispatcher_limit; // min number of calls to a target set => calls get replaced by single dispatcher
  bool enforce_debug;

  // all
  // use "simple" instead of "fast" rules (for debugging)
  bool simple_rules;
  bool consider_return_type; // when a fp has return type "void", functions also should not return anything

  // LLVM CFI (icfi)
  const char *icfi_output;
  // IFCC CFI
  const char *ifcc_output;
  // TG CFI
  const char *tgcfi_output;
  // TG Ordered json object list name;
  const char *ordered_json_object_name;
  // LLVM output
  const char *llvm_output;

  // Options from lld command line
  bool lld_is_shared = false;
  std::string output_filename;
  bool link_with_libc = false; // if libc is STATICALLY linked. auto-detected at link time
};
extern TypegraphSettings Settings;

} // namespace typegraph

#endif // LLVM_TYPEGRAPHSETTINGS_H
