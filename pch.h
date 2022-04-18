
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include "types.h"

#include <iostream>
#include <chrono>


struct Source_Location {
  size_t      line;
  const char* file;
  const char* function;
};

// @Incomplete: right now Allocator is 32 bytes, but we can be smarter than this, we know that allocate, deallocate, reallocate function pointers will be constant and assigned only once. So we can switch these 3 variables to one constant pointer. 32 bytes -> 16 bytes.
struct Allocator {
  void* (*allocate)(void* data, size_t bytes, Source_Location location);
  void  (*deallocate)(void* data, void* ptr, Source_Location location);
//void  (*reallocate)(void* data, void* ptr, Source_Location location);
  void* allocator_data;
};
inline Allocator get_global_allocator();

template<class T>
struct iterator {
  uint64 index; T *p;
  
  explicit iterator(uint64 i)       { index = i; }
  explicit iterator(uint64 i, T* d) { index = i; p = d; }
  iterator& operator++()            { ++index; return *this; }
  iterator& operator++(int)         { ++index; return *this; }
  bool operator==(iterator o) const { return index == o.index; }
  bool operator!=(iterator o) const { return index != o.index; }
  T& operator*()              const { return p[index]; }
  T* operator->()             const { return &p[index]; }
};

template<class T>
struct const_iterator {
  uint64 index; const T *p;
  
  explicit const_iterator(uint64 i)             { index = i; }
  explicit const_iterator(uint64 i, const T* d) { index = i; p = d; }
  const_iterator& operator++()            { ++index; return *this; }
  const_iterator& operator++(int)         { ++index; return *this; }
  bool operator==(const_iterator o) const { return index == o.index; }
  bool operator!=(const_iterator o) const { return index != o.index; }
  const T& operator*()              const { return p[index]; }
  const T* operator->()             const { return &p[index]; }
};


template<class T, uint64 N>
struct static_array {
  T data[N];
  static const uint64 size     = N;
  static const uint64 max_size = N;
  static const uint64 capacity = N;

  
  // overloaded for: 'static_array<int, 2> d = { 42, 23 };'
  static_array<T, N>& operator=(const T(&initializer)[N]) {
    memcpy(data, initializer, sizeof(data));
    return *this;
  }

  T& operator[](uint64 index) {
    assert(index < size);
    return data[index];
  }

  const T& operator[](uint64 index) const {
    assert(index < size);
    return data[index];
  }

  iterator<T>       begin()       { return iterator<T>(0, data); }
  iterator<T>       end()         { return iterator<T>(size);    }
  const_iterator<T> begin() const { return const_iterator<T>(0, data); }
  const_iterator<T> end()   const { return const_iterator<T>(size);    }
};

template<class T>
struct array {
  T*     data     = NULL;
  uint64 size     = 0;
  uint64 capacity = 0;
  Allocator allocator = get_global_allocator();

  T& operator[](s64 index) {
    assert(index < size);
    return data[index];
  }

  const T& operator[](s64 index) const {
    assert(index < size);
    return data[index];
  }

  iterator<T>       begin()       { return iterator<T>(0, data); }
  iterator<T>       end()         { return iterator<T>(size);    }
  const_iterator<T> begin() const { return const_iterator<T>(0, data); }
  const_iterator<T> end()   const { return const_iterator<T>(size);    }
};

static uint64 ARRAY_INDEX_NOT_FOUND = -1;

template<class T>          T* array_add(array<T>*);
template<class T, class F> T* array_find_by_predicate(array<T>*, F);
template<class T>          T* array_pop(array<T>*);
template<class T>          void array_free(array<T>*);

#include "memory.h"
#include "array.h"

#include "utility.h"
#include "sparse_array.h"
#include "hash_table.h"
#include "math.h"
#include "platform.h"
