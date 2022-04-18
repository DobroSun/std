#pragma once

#define GROW_FUNCTION(x) (2*(x) + 8)

template<class T>
struct sparse_array {
  s64* indecies = NULL;
  T*   data     = NULL;
  s64  per_array_size     = 0;
  s64  per_array_capacity = 0;

  Allocator allocator = get_global_allocator();

  T& operator[](s64 index) {
    T* found = sparse_find(this, index);
    assert(found && "sparse_array<T>::operataor[] couldn't find an index!");
    return *found;
  }
  const T& operator[](s64 index) const {
    const T* found = sparse_find(this, index);
    assert(found && "sparse_array<T>::operataor[] couldn't find an index!");
    return *found;
  }
};

template<class T>
void sparse_reserve(sparse_array<T>* s, s64 new_capacity) {
  assert(new_capacity > s->per_array_capacity && "sparse_array<T>;:reserve new capacity is <= then array one, can't do anything!");

  s64* pool = (s64*) alloc(s->allocator, (sizeof(s64)+sizeof(T)) * new_capacity);
  T*   data = (T*)(pool + new_capacity);

  memcpy(pool, s->indecies, sizeof(s64) * s->per_array_size);
  memcpy(data, s->data,     sizeof(T) * s->per_array_size);

  sparse_free(s);

  s->indecies = pool;
  s->data     = data;
  s->per_array_capacity = new_capacity;
}

template<class T>
T* sparse_add(sparse_array<T>* s, s64 index) {
  if(s->per_array_capacity == s->per_array_size) {
    sparse_reserve(s, GROW_FUNCTION(s->per_array_capacity));
  }

  s->indecies[s->per_array_size] = index;
  return &s->data[s->per_array_size++];
}

template<class T>
T* sparse_add(sparse_array<T>* s, s64 index, T value) {
  return &(*sparse_add(s, index) = value);
}

template<class T>
T* sparse_find(const sparse_array<T>* s, s64 index) {
  // @Speed: do binary search or use hash map to store things.
  T* pointer = NULL;
  for(s64 i = 0; i < s->per_array_size; i++, pointer++) {
    if(s->indecies[i] == index) {
      pointer = &s->data[i];
      break;
    }
  }
  return pointer;
}

template<class T>
T* sparse_find_or_add(sparse_array<T>* s, s64 index) {
  T* pointer = sparse_find(s, index);
  
  if(pointer) {
    return pointer;
  } else {
    return sparse_add(s, index);
  }
}

template<class T>
T* sparse_find_or_add(sparse_array<T>* s, s64 index, T value) {
  return &(*sparse_find_or_add(s, index) = value);
}

template<class T>
void sparse_free(sparse_array<T>* s) {
  dealloc(s->allocator, s->indecies); // correct! we keep it in a single allocation.
}

#undef GROW_FUNCTION
