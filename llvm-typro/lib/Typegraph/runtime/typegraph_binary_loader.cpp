#include "typegraph_binary_loader.h"
#include "enforcing_rt.h"

#include <iostream>
#include <string>

namespace typegraph {



BinaryStreamReader loadTypegraphFromBinary(typegraph::TypeGraph &Graph, const char *GraphData, std::vector<std::unique_ptr<rt::FunctionInfos>> &Functions, void** &References) {
  BinaryStreamReader Stream(GraphData);
  long NumFunctions = Stream.readLong();
  long NumVertices = Stream.readLong();
  long NumEdges = Stream.readLong();
  long NumFunctionUses = Stream.readLong();
  long NumCallInfos = Stream.readLong();
  long NumInterfaces = Stream.readLong();

  // Read functions
  int FunctionsArraySize = Functions.size();
  for (; NumFunctions-- ;) {
    long ID = Stream.readLong();
    void* Function = *(References++);
    Functions.emplace_back(std::make_unique<rt::FunctionInfos>(ID, Function));
  }

  Graph.parserReset();
  // Vertices
  Graph.parserNodesStart();
  for (; NumVertices-- > 0;) {
    long V = Stream.readLong();
    Graph.parserAddNode(V, Stream.readString());
  }
  Graph.parserNodesFinished();

  // Edges
  for (; NumEdges--;) {
    long S = Stream.readLong();
    long T = Stream.readLong();
    Graph.parserAddEdge(S, T, Stream.readString());
  }

  // Function uses
  for (; NumFunctionUses--;) {
    long V = Stream.readLong();
    auto ContextName = Stream.readString();
    auto SymbolName = Stream.readString();
    long FunctionIndex = Stream.readLong();
    auto *F = FunctionIndex == -1 ? nullptr : Functions[FunctionsArraySize + FunctionIndex].get();
    Graph.parserAddFunctionUse(V, ContextName, SymbolName, F);
  }

  // Calls
  for (; NumCallInfos-- ;) {
    long V = Stream.readLong();
    auto S = Stream.readString();
    Graph.parserAddIndirectCall(V, S);
  }

  // Interfaces
  for (; NumInterfaces-- ;) {
    Graph.parserAddInterface(Stream.readString(), nullptr);
  }

  Graph.parserReset();

  return Stream;
}
} // namespace typegraph
