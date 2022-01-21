#ifndef LLVM_TYPEGRAPHS_TYPEGRAPHFORMATTERS_H
#define LLVM_TYPEGRAPHS_TYPEGRAPHFORMATTERS_H

#include "dotwriter.h"
#include "llvm/Typegraph/TGCallGraph.h"

struct CGNodeWriter {
  inline void operator()(std::ostream &Out, typegraph::TGCallGraph &Graph, typegraph::CGSCC V) {
    Out << " [label=\"";
    Out << "v" << V << " ";
    if (Graph[V].Contexts.size() == 1) {
      Out << **Graph[V].Contexts.begin();
    } else {
      bool NotFirst = false;
      for (auto &Ctx : Graph[V].Contexts) {
        if (NotFirst) {
          Out << "\\n";
        } else {
          NotFirst = true;
        }
        Out << "- " << *Ctx;
      }
    }
    Out << "\"]";
  };
};

struct CGEdgeWriter {
  inline void operator()(std::ostream &Out, const typegraph::TGCallGraph &Graph, const typegraph::CGSCCEdge &E){};
};

struct CGLNodeWriter {
  inline void operator()(std::ostream &Out, typegraph::TGLayeredCallGraph &Graph, typegraph::CGSCC V) {
    Out << " [label=\"";
    Out << "v" << V << " ";
    if (Graph[V].Contexts.size() == 1) {
      auto &It = *Graph[V].Contexts.begin();
      Out << *It.first << "#" << It.second;
    } else {
      bool NotFirst = false;
      for (auto &Ctx : Graph[V].Contexts) {
        if (NotFirst) {
          Out << "\\n";
        } else {
          NotFirst = true;
        }
        Out << "- " << *Ctx.first << "#" << Ctx.second;
      }
    }
    Out << "\"]";
  };
};

struct CGLEdgeWriter {
  inline void operator()(std::ostream &Out, const typegraph::TGLayeredCallGraph &Graph, const typegraph::CGSCCEdge &E){};
};

#endif // LLVM_TYPEGRAPHS_TYPEGRAPHFORMATTERS_H
