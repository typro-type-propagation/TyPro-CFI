#ifndef LLVM_TYPEGRAPHS_TESTDATASTRUCT_H
#define LLVM_TYPEGRAPHS_TESTDATASTRUCT_H

#define CastRelationDataUsed CastRelationData2
//#define CastRelationDataUsed CastRelationDataOld
#define CastRelationDataUsed1 CastRelationData1
#define USE_ARITY

// 32MB limit for 1-dimensional structures
#define MAX_MEMORY_SIZE_1 33554432
// 512MB limit for 2-dimensional structures
#define MAX_MEMORY_SIZE_2 134217728

#define DEBUG_ALLOC false

size_t max_number_of_nodes = -1;

struct CastRelationDataOld {
#ifdef USE_ARITY
  static constexpr Relation::arity_type Arity = 2;
#endif
  using t_ind_0 = Trie<2>;
  t_ind_0 ind_0;
  using t_ind_1 = Trie<2>;
  t_ind_1 ind_1;
  using t_tuple = t_ind_0::entry_type;
  class iterator_0 : public std::iterator<std::forward_iterator_tag, t_tuple> {
    using nested_iterator = typename t_ind_0::iterator;
    nested_iterator nested;
    t_tuple value;

  public:
    iterator_0() = default;
    iterator_0(const nested_iterator &iter) : nested(iter), value(orderOut_0(*iter)) {}
    iterator_0(const iterator_0 &other) = default;
    iterator_0 &operator=(const iterator_0 &other) = default;
    bool operator==(const iterator_0 &other) const { return nested == other.nested; }
    bool operator!=(const iterator_0 &other) const { return !(*this == other); }
    const t_tuple &operator*() const { return value; }
    const t_tuple *operator->() const { return &value; }
    iterator_0 &operator++() {
      ++nested;
      value = orderOut_0(*nested);
      return *this;
    }
  };
  class iterator_1 : public std::iterator<std::forward_iterator_tag, t_tuple> {
    using nested_iterator = typename t_ind_1::iterator;
    nested_iterator nested;
    t_tuple value;

  public:
    iterator_1() = default;
    iterator_1(const nested_iterator &iter) : nested(iter), value(orderOut_1(*iter)) {}
    iterator_1(const iterator_1 &other) = default;
    iterator_1 &operator=(const iterator_1 &other) = default;
    bool operator==(const iterator_1 &other) const { return nested == other.nested; }
    bool operator!=(const iterator_1 &other) const { return !(*this == other); }
    const t_tuple &operator*() const { return value; }
    const t_tuple *operator->() const { return &value; }
    iterator_1 &operator++() {
      ++nested;
      value = orderOut_1(*nested);
      return *this;
    }
  };
  using iterator = iterator_0;
  struct context {
    t_ind_0::op_context hints_0;
    t_ind_1::op_context hints_1;
  };
  context createContext() { return context(); }
  bool insert(const t_tuple &t) {
    context h;
    return insert(t, h);
  }
  bool insert(const t_tuple &t, context &h) {
    if (ind_0.insert(orderIn_0(t), h.hints_0)) {
      ind_1.insert(orderIn_1(t), h.hints_1);
      return true;
    } else
      return false;
  }
  bool insert(const RamDomain *ramDomain) {
    RamDomain data[2];
    std::copy(ramDomain, ramDomain + 2, data);
    const t_tuple &tuple = reinterpret_cast<const t_tuple &>(data);
    context h;
    return insert(tuple, h);
  }
  bool insert(RamDomain a0, RamDomain a1) {
    RamDomain data[2] = {a0, a1};
    return insert(data);
  }
  bool contains(const t_tuple &t, context &h) const { return ind_0.contains(orderIn_0(t), h.hints_0); }
  bool contains(const t_tuple &t) const {
    context h;
    return contains(t, h);
  }
  std::size_t size() const { return ind_0.size(); }
  iterator find(const t_tuple &t, context &h) const { return ind_0.find(orderIn_0(t), h.hints_0); }
  iterator find(const t_tuple &t) const {
    context h;
    return find(t, h);
  }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper, context &h) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator_0> lowerUpperRange_11(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_0.template getBoundaries<2>(orderIn_0(lower), h.hints_0);
    return make_range(iterator_0(r.begin()), iterator_0(r.end()));
  }
  range<iterator_0> lowerUpperRange_11(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_11(lower, upper, h);
  }
  range<iterator_1> lowerUpperRange_10(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_1.template getBoundaries<1>(orderIn_1(lower), h.hints_1);
    return make_range(iterator_1(r.begin()), iterator_1(r.end()));
  }
  range<iterator_1> lowerUpperRange_10(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_10(lower, upper, h);
  }
  range<iterator_0> lowerUpperRange_01(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_0.template getBoundaries<1>(orderIn_0(lower), h.hints_0);
    return make_range(iterator_0(r.begin()), iterator_0(r.end()));
  }
  range<iterator_0> lowerUpperRange_01(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_01(lower, upper, h);
  }
  bool empty() const { return ind_0.empty(); }
  std::vector<range<iterator>> partition() const {
    std::vector<range<iterator>> res;
    for (const auto &cur : ind_0.partition(10000)) {
      res.push_back(make_range(iterator(cur.begin()), iterator(cur.end())));
    }
    return res;
  }
  void purge() {
    ind_0.clear();
    ind_1.clear();
  }
  iterator begin() const { return iterator_0(ind_0.begin()); }
  iterator end() const { return iterator_0(ind_0.end()); }
  void printStatistics(std::ostream &o) const {
    o << " arity 2 brie index 0 lex-order [1,0]\n";
    ind_0.printStats(o);
    o << " arity 2 brie index 1 lex-order [0,1]\n";
    ind_1.printStats(o);
  }

private:
  static t_tuple orderIn_0(const t_tuple &t) {
    t_tuple res;
    res[0] = t[1];
    res[1] = t[0];
    return res;
  }
  static t_tuple orderOut_0(const t_tuple &t) {
    t_tuple res;
    res[1] = t[0];
    res[0] = t[1];
    return res;
  }
  static t_tuple orderIn_1(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    res[1] = t[1];
    return res;
  }
  static t_tuple orderOut_1(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    res[1] = t[1];
    return res;
  }
};

struct CastRelationData2 {
#ifdef USE_ARITY
  static constexpr Relation::arity_type Arity = 2;
#endif
  using t_ind_0 = Trie<2>;
  t_ind_0 ind_0;
  using t_ind_1 = Trie<2>;
  t_ind_1 ind_1;
  using t_tuple = t_ind_0::entry_type;
  class iterator_0 : public std::iterator<std::forward_iterator_tag, t_tuple> {
    using nested_iterator = typename t_ind_0::iterator;
    nested_iterator nested;
    t_tuple value;

  public:
    iterator_0() = default;
    iterator_0(const nested_iterator &iter) : nested(iter), value(orderOut_0(*iter)) {}
    iterator_0(const iterator_0 &other) = default;
    iterator_0 &operator=(const iterator_0 &other) = default;
    bool operator==(const iterator_0 &other) const { return nested == other.nested; }
    bool operator!=(const iterator_0 &other) const { return !(*this == other); }
    const t_tuple &operator*() const { return value; }
    const t_tuple *operator->() const { return &value; }
    iterator_0 &operator++() {
      ++nested;
      value = orderOut_0(*nested);
      return *this;
    }
  };
  class iterator_1 : public std::iterator<std::forward_iterator_tag, t_tuple> {
    using nested_iterator = typename t_ind_1::iterator;
    nested_iterator nested;
    t_tuple value;

  public:
    iterator_1() = default;
    iterator_1(const nested_iterator &iter) : nested(iter), value(orderOut_1(*iter)) {}
    iterator_1(const iterator_1 &other) = default;
    iterator_1 &operator=(const iterator_1 &other) = default;
    bool operator==(const iterator_1 &other) const { return nested == other.nested; }
    bool operator!=(const iterator_1 &other) const { return !(*this == other); }
    const t_tuple &operator*() const { return value; }
    const t_tuple *operator->() const { return &value; }
    iterator_1 &operator++() {
      ++nested;
      value = orderOut_1(*nested);
      return *this;
    }
  };
  using iterator = iterator_0;
  struct context {
    t_ind_0::op_context hints_0;
    t_ind_1::op_context hints_1;
  };
  context createContext() { return context(); }
  bool insert(const t_tuple &t) {
    context h;
    return insert(t, h);
  }
  bool insert_old(const t_tuple &t, context &h) {
    if (ind_0.insert(orderIn_0(t), h.hints_0)) {
      ind_1.insert(orderIn_1(t), h.hints_1);
      return true;
    } else
      return false;
  }
  bool insert(const RamDomain *ramDomain) {
    RamDomain data[2];
    std::copy(ramDomain, ramDomain + 2, data);
    const t_tuple &tuple = reinterpret_cast<const t_tuple &>(data);
    context h;
    return insert(tuple, h);
  }
  bool insert(RamDomain a0, RamDomain a1) {
    RamDomain data[2] = {a0, a1};
    return insert(data);
  }
  bool contains_old(const t_tuple &t, context &h) const {
    return ind_0.contains(orderIn_0(t), h.hints_0);
  }
  bool contains(const t_tuple &t) const {
    context h;
    return contains(t, h);
  }
  std::size_t size() const { return ind_0.size(); }
  iterator find(const t_tuple &t, context &h) const { return ind_0.find(orderIn_0(t), h.hints_0); }
  iterator find(const t_tuple &t) const {
    context h;
    return find(t, h);
  }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper, context &h) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator_0> lowerUpperRange_11(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_0.template getBoundaries<2>(orderIn_0(lower), h.hints_0);
    return make_range(iterator_0(r.begin()), iterator_0(r.end()));
  }
  range<iterator_0> lowerUpperRange_11(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_11(lower, upper, h);
  }
  range<iterator_1> lowerUpperRange_10(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_1.template getBoundaries<1>(orderIn_1(lower), h.hints_1);
    return make_range(iterator_1(r.begin()), iterator_1(r.end()));
  }
  range<iterator_1> lowerUpperRange_10(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_10(lower, upper, h);
  }
  range<iterator_0> lowerUpperRange_01(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_0.template getBoundaries<1>(orderIn_0(lower), h.hints_0);
    return make_range(iterator_0(r.begin()), iterator_0(r.end()));
  }
  range<iterator_0> lowerUpperRange_01(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_01(lower, upper, h);
  }
  bool empty() const { return ind_0.empty(); }
  std::vector<range<iterator>> partition() const {
    std::vector<range<iterator>> res;
    for (const auto &cur : ind_0.partition(10000)) {
      res.push_back(make_range(iterator(cur.begin()), iterator(cur.end())));
    }
    return res;
  }
  void purge() {
    ind_0.clear();
    ind_1.clear();
    if (matrix) {
      bzero(matrix, sizeof(std::atomic_bool) * max_number * max_number);
    }
  }
  iterator begin() const { return iterator_0(ind_0.begin()); }
  iterator end() const { return iterator_0(ind_0.end()); }
  void printStatistics(std::ostream &o) const {
    o << " arity 2 brie index 0 lex-order [1,0]\n";
    ind_0.printStats(o);
    o << " arity 2 brie index 1 lex-order [0,1]\n";
    ind_1.printStats(o);
  }

private:
  static t_tuple orderIn_0(const t_tuple &t) {
    t_tuple res;
    res[0] = t[1];
    res[1] = t[0];
    return res;
  }
  static t_tuple orderOut_0(const t_tuple &t) {
    t_tuple res;
    res[1] = t[0];
    res[0] = t[1];
    return res;
  }
  static t_tuple orderIn_1(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    res[1] = t[1];
    return res;
  }
  static t_tuple orderOut_1(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    res[1] = t[1];
    return res;
  }

public:
  const size_t max_number;
  std::atomic_bool *matrix = nullptr;

  CastRelationData2(size_t max_number = max_number_of_nodes) : max_number(max_number) {
    if (sizeof(std::atomic_bool) * max_number * max_number < MAX_MEMORY_SIZE_2) {
      if (DEBUG_ALLOC)
        std::cerr << "ALLOC 2: " << (sizeof(std::atomic_bool) * max_number * max_number / 1024 / 1024) << " MB"
                << std::endl;
      matrix = static_cast<std::atomic_bool *>(malloc(sizeof(std::atomic_bool) * max_number * max_number));
      purge();
    }
  }
  ~CastRelationData2() {
    if (matrix) {
      if (DEBUG_ALLOC)
        std::cerr << "FREE 2:  " << (sizeof(std::atomic_bool) * max_number * max_number / 1024 / 1024) << " MB"
                << std::endl;
      free(matrix);
    }
  }

  inline std::atomic_bool &index(const t_tuple &t) { return matrix[t[0] * max_number + t[1]]; }
  inline const std::atomic_bool &index(const t_tuple &t) const { return matrix[t[0] * max_number + t[1]]; }

  bool insert(const t_tuple &t, context &h) {
    if (matrix && index(t).exchange(true)) {
      return false;
    } else {
      return insert_old(t, h);
    }
  }

  inline bool contains(const t_tuple &t, context &h) const { return matrix ? (bool) index(t) : contains_old(t, h); }
};

/*
struct CastRelationData2Try1 {
  static constexpr Relation::arity_type Arity = 2;
  using t_tuple = Tuple<RamDomain, 2>;

  struct iterator_0 {

  };

  using iterator = iterator_0;

  struct context{};
  context createContext() { return context(); }
  bool insert(const t_tuple &t) {
    context h;
    return insert(t, h);
  }
  bool insert(const RamDomain *ramDomain) {
    RamDomain data[2];
    std::copy(ramDomain, ramDomain + 2, data);
    const t_tuple &tuple = reinterpret_cast<const t_tuple &>(data);
    context h;
    return insert(tuple, h);
  }
  bool insert(RamDomain a0, RamDomain a1) {
    RamDomain data[2] = {a0, a1};
    return insert(data);
  }
  bool contains(const t_tuple &t) const {
    context h;
    return contains(t, h);
  }
  iterator find(const t_tuple &t) const {
    context h;
    return find(t, h);
  }



  // Real impls
  const size_t max_number;
  std::atomic_size_t num_elements = 0;
  std::atomic_bool* matrix;

  CastRelationData2(size_t max_number=max_number_of_nodes) : max_number(max_number) {
    matrix = static_cast<std::atomic_bool *>(malloc(sizeof(std::atomic_bool) * max_number * max_number));
    purge();
  }
  ~CastRelationData2() {
    free(matrix);
  }

  inline std::atomic_bool &index(const t_tuple &t) {
    return matrix[t[0]*max_number + t[1]];
  }
  inline const std::atomic_bool &index(const t_tuple &t) const {
    return matrix[t[0]*max_number + t[1]];
  }

  bool insert(const t_tuple &t, context &h) {
    auto &x = index(t);
    if (x.exchange(true)) {
      return false;
    } else {
      num_elements++;
      return true;
    }
  }

  inline bool contains(const t_tuple &t, context &h) const {
    return index(t);
  }

  std::size_t size() const { return num_elements; }

  bool empty() const { return num_elements == 0; }

  void purge() {
    bzero(matrix, sizeof(std::atomic_bool) * max_number * max_number);
    num_elements = 0;
  }

  iterator find(const t_tuple &t, context &h) const {
    return {};
  }

  std::vector<range<iterator>> partition() const {
    std::vector<range<iterator>> res;
    for (const auto &cur : ind_0.partition(10000)) {
      res.push_back(make_range(iterator(cur.begin()), iterator(cur.end())));
    }
    return res;
  }


};

*/

struct CastRelationData1 {
  static constexpr Relation::arity_type Arity = 1;
  using t_ind_0 = Trie<1>;
  t_ind_0 ind_0;
  using t_tuple = t_ind_0::entry_type;
  class iterator_0 : public std::iterator<std::forward_iterator_tag, t_tuple> {
    using nested_iterator = typename t_ind_0::iterator;
    nested_iterator nested;
    t_tuple value;

  public:
    iterator_0() = default;
    iterator_0(const nested_iterator &iter) : nested(iter), value(orderOut_0(*iter)) {}
    iterator_0(const iterator_0 &other) = default;
    iterator_0 &operator=(const iterator_0 &other) = default;
    bool operator==(const iterator_0 &other) const { return nested == other.nested; }
    bool operator!=(const iterator_0 &other) const { return !(*this == other); }
    const t_tuple &operator*() const { return value; }
    const t_tuple *operator->() const { return &value; }
    iterator_0 &operator++() {
      ++nested;
      value = orderOut_0(*nested);
      return *this;
    }
  };
  using iterator = iterator_0;
  struct context {
    t_ind_0::op_context hints_0;
  };
  context createContext() { return context(); }
  inline bool insert(const t_tuple &t) {
    context h;
    return insert(t, h);
  }
  bool insert_old(const t_tuple &t, context &h) {
    if (ind_0.insert(orderIn_0(t), h.hints_0)) {
      return true;
    } else
      return false;
  }
  inline bool insert(const RamDomain *ramDomain) {
    RamDomain data[1];
    std::copy(ramDomain, ramDomain + 1, data);
    const t_tuple &tuple = reinterpret_cast<const t_tuple &>(data);
    context h;
    return insert(tuple, h);
  }
  inline bool insert(RamDomain a0) {
    RamDomain data[1] = {a0};
    return insert(data);
  }
  inline bool contains_old(const t_tuple &t, context &h) const {
    return ind_0.contains(orderIn_0(t), h.hints_0);
  }
  inline bool contains(const t_tuple &t) const {
    context h;
    return contains(t, h);
  }
  std::size_t size() const { return ind_0.size(); }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper, context &h) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator> lowerUpperRange_0(const t_tuple &lower, const t_tuple &upper) const {
    return range<iterator>(ind_0.begin(), ind_0.end());
  }
  range<iterator_0> lowerUpperRange_1(const t_tuple &lower, const t_tuple &upper, context &h) const {
    auto r = ind_0.template getBoundaries<1>(orderIn_0(lower), h.hints_0);
    return make_range(iterator_0(r.begin()), iterator_0(r.end()));
  }
  range<iterator_0> lowerUpperRange_1(const t_tuple &lower, const t_tuple &upper) const {
    context h;
    return lowerUpperRange_1(lower, upper, h);
  }
  bool empty() const { return ind_0.empty(); }
  std::vector<range<iterator>> partition() const {
    std::vector<range<iterator>> res;
    for (const auto &cur : ind_0.partition(10000)) {
      res.push_back(make_range(iterator(cur.begin()), iterator(cur.end())));
    }
    return res;
  }
  void purge() {
    ind_0.clear();
    if (matrix) {
      bzero(matrix, sizeof(std::atomic_bool) * max_number);
    }
  }
  iterator begin() const { return iterator_0(ind_0.begin()); }
  iterator end() const { return iterator_0(ind_0.end()); }
  void printStatistics(std::ostream &o) const {
    o << " arity 1 brie index 0 lex-order [0]\n";
    ind_0.printStats(o);
  }
  static t_tuple orderIn_0(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    return res;
  }
  static t_tuple orderOut_0(const t_tuple &t) {
    t_tuple res;
    res[0] = t[0];
    return res;
  }

public:
  const size_t max_number;
  std::atomic_bool *matrix = nullptr;

  CastRelationData1(size_t max_number = max_number_of_nodes) : max_number(max_number) {
    if (sizeof(std::atomic_bool) * max_number < MAX_MEMORY_SIZE_1) {
      if (DEBUG_ALLOC)
        std::cerr << "ALLOC 1: " << (sizeof(std::atomic_bool) * max_number / 1024 / 1024) << " MB" << std::endl;
      matrix = static_cast<std::atomic_bool *>(malloc(sizeof(std::atomic_bool) * max_number));
      purge();
    }
  }
  ~CastRelationData1() {
    if (matrix) {
      if (DEBUG_ALLOC)
        std::cerr << "FREE 1:  " << (sizeof(std::atomic_bool) * max_number / 1024 / 1024) << " MB" << std::endl;
      free(matrix);
    }
  }

  inline std::atomic_bool &index(const t_tuple &t) { return matrix[t[0]]; }
  inline const std::atomic_bool &index(const t_tuple &t) const { return matrix[t[0]]; }

  bool insert(const t_tuple &t, context &h) {
    if (matrix && index(t).exchange(true)) {
      return false;
    } else {
      return insert_old(t, h);
    }
  }

  inline bool contains(const t_tuple &t, context &h) const { return matrix ? (bool) index(t) : contains_old(t, h); }
};

#endif // LLVM_TYPEGRAPHS_TESTDATASTRUCT_H
