#include "llvm/Typegraph/TypegraphSettings.h"
#include <cstdlib>
#include <string>

namespace typegraph {

namespace {
bool env_to_bool(const char *env, bool def = false) {
  auto *cp = getenv(env);
  if (cp) {
    std::string s(cp);
    if (s == "1" || s == "on" || s == "true")
      return true;
    if (s == "0" || s == "off" || s == "false")
      return false;
  }
  return def;
}

int env_to_int(const char *env, int def = 0) {
  auto *cp = getenv(env);
  if (cp) {
    return std::stoi(std::string(cp));
  }
  return def;
}

int env_to_ulong(const char *env, unsigned long def = 0) {
  auto *cp = getenv(env);
  if (cp) {
    return std::stoul(std::string(cp));
  }
  return def;
}
} // namespace

TypegraphSettings::TypegraphSettings() {
  enabled = env_to_bool("TG_ENABLED", true);
  clang_show_graph = env_to_bool("TG_CLANG_SHOW_GRAPH");
  clang_minimize_graph = env_to_bool("TG_CLANG_MINIMIZE_GRAPH", true);
  clang_intermediate_graph_output = env_to_bool("TG_CLANG_INTERMEDIATE_GRAPH_OUTPUT");
  clang_show_graph_all = env_to_bool("TG_CLANG_SHOW_GRAPH_ALL");
  clang_layering = env_to_bool("TG_CLANG_LAYERING", false);
  tool_show_graph = env_to_bool("TG_TOOL_SHOW_GRAPH");
  graph_output = getenv("TG_GRAPH_OUTPUT");
  facts_output = getenv("TG_FACTS_OUTPUT");
  instrument_collectcalltargets = env_to_bool("TG_INSTRUMENT_COLLECTCALLTARGETS");
  linktime_layering = env_to_bool("TG_LINKTIME_LAYERING", false);
  linktime_layering_debug = env_to_bool("TG_LINKTIME_LAYERING_DEBUG");
  enforce_simple = env_to_bool("TG_ENFORCE_SIMPLE");
  enforce_argnum = env_to_bool("TG_ENFORCE_ARGNUM");
  enforce = env_to_bool("TG_ENFORCE", true);
  dynlib_support = env_to_bool("TG_DYNLIB_SUPPORT");
  enforce_id_bitwidth = env_to_int("TG_ENFORCE_ID_BITWIDTH", 31);
  enforce_min_id = env_to_ulong("TG_ENFORCE_MIN_ID", 2);
  enforce_dispatcher_limit = env_to_int("TG_ENFORCE_DISPATCHER_LIMIT", 7);
  enforce_debug = env_to_bool("TG_ENFORCE_DEBUG");
  simple_rules = env_to_bool("TG_SIMPLE_RULES");
  icfi_output = getenv("TG_ICFI_OUTPUT"); // Handled manually in LowerTypeTests
  ifcc_output = getenv("TG_IFCC_OUTPUT");
  tgcfi_output = getenv("TG_CFI_OUTPUT");
}

void TypegraphSettings::setOutput(const std::string &Filename) {
  // TODO remove pathname
  output_filename = Filename;
}

TypegraphSettings Settings;

} // namespace typegraph
