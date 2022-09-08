#ifndef LLVM_TYPEGRAPHS_STRINGCONTAINER_H
#define LLVM_TYPEGRAPHS_STRINGCONTAINER_H

#include "basicdefs.h"

#ifndef WITHOUT_LLVM
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
#include <memory>
#else
#include <string>
#endif

namespace typegraph {

struct StringContainerEntry {
  std::string Str;
  int32_t MapIndex;

  inline StringContainerEntry(const std::string &Str, int MapIndex) : Str(Str), MapIndex(MapIndex) {}
};

class StringContainer {
  int32_t NextUniqueId = 0;

#ifndef WITHOUT_LLVM
  llvm::StringMap<std::unique_ptr<StringContainerEntry>> Pointers;
  // llvm::DenseMap<const std::string*, int32_t> UniqueIds;

public:
  inline const std::string *get(const llvm::StringRef Key) {
    auto &It = Pointers[Key];
    if (!It) {
      It = std::make_unique<StringContainerEntry>(Key, NextUniqueId++);
    }
    return &It->Str;
  }

#else

  MapCls<std::string, std::unique_ptr<StringContainerEntry>> Pointers;
  // std::map<const std::string*, int32_t> UniqueIds;

public:
  inline const std::string *get(const std::string &Key) {
    auto &It = Pointers[Key];
    if (!It) {
      It = std::make_unique<StringContainerEntry>(Key, NextUniqueId++);
    }
    return &It->Str;
  }
#endif

  int32_t getUniqueId(const std::string *Str) const {
    return ((const StringContainerEntry *) Str)->MapIndex;
  }

  int32_t nextId() const {
    return NextUniqueId;
  }
};

template<class T>
class StringContainerMap {
  VectorCls<T> Entries;

  inline int32_t getID(const std::string *Str) {
    return ((const StringContainerEntry *) Str)->MapIndex;
  }

public:

  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::const_reference const_reference;

  StringContainerMap(const StringContainer &Container, const T& Initial) : Entries(Container.nextId(), Initial) {}

  const_reference operator [](const std::string *Str) const {
    return Entries[getID(Str)];
  }

  reference operator [](const std::string *Str) {
    return Entries[getID(Str)];
  }
};

} // namespace typegraph

#endif // LLVM_TYPEGRAPHS_STRINGCONTAINER_H
