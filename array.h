#pragma once

#define GROW_FUNCTION(x) (2*(x) + 8)

// start static_array
template<class T, uint64 N>
T* array_first(static_array<T, N>* a) {
  return &a->data[0];
}

template<class T, uint64 N>
T* array_last(static_array<T, N>* a) {
  return &a->data[a->size-1];
}

template<class T, uint64 N, class F>
T* array_find_by_predicate(static_array<T, N>* a, F predicate) {
  for(T& p : *a) {
    if(predicate(p)) {
      return &p;
    }
  }
  return NULL;
}

template<class T, uint64 N>
T* array_find(static_array<T, N>* a, T v) {
  return static_array_find_by_predicate(a, [=](T& p) { return p == v; });
}

template<class T, uint64 N>
bool array_contains(static_array<T, N>* a, T v) {
  return static_array_find(a, v) != NULL;
}

template<class T, uint64 N, class F>
uint64 array_index_by_predicate(static_array<T, N>* a, F predicate) {
  for(size_t i = 0; i < a->size; i++) {
    T& p = a->data[i];
    if(predicate(p)) {
      return i;
    }
  }
  return ARRAY_INDEX_NOT_FOUND;
}

template<class T, uint64 N>
uint64 array_index(static_array<T, N>* a, T v) {
  return array_index_by_predicate(a, [=](T& p) { return p == v; });
}
// end of static_array.


// start of array
template<class T>
void array_reserve(array<T>* a, s64 new_capacity) {
  assert(new_capacity > a->capacity && "array<T>;:reserve new_capacity is <= then array one, can't do anything!");

  T* new_data = (T*) alloc(a->allocator, sizeof(T)*new_capacity); // @Incomplete: realloc.
  memcpy(new_data, a->data, sizeof(T)*a->size);
  array_free(a);

  a->data     = new_data;
  a->capacity = new_capacity;
}

template<class T>
T* array_add(array<T>* a) {
  if(a->size == a->capacity) {
    array_reserve(a, GROW_FUNCTION(a->capacity));
  }
  return &a->data[a->size++];
}

template<class T>
T* array_add(array<T>* a, T v) {
  return &(*array_add(a) = v);
}

template<class T, class F>
T* array_find_by_predicate(array<T>* a, F predicate) {
  for(T& p : *a) {
    if(predicate(p)) {
      return &p;
    }
  }
  return NULL;
}

template<class T>
T* array_find(array<T>* a, T v) {
  return array_find_by_predicate(a, [=](T& p) { return p == v; });
}

template<class T>
bool array_contains(array<T>* a, T v) {
  return array_find(a, v) != NULL;
}

template<class T, class F>
s64 array_index_by_predicate(array<T>* a, F predicate) {
  for(s64 i = 0; i < a->size; i++) {
    T* p = &a->data[i];
    if(predicate(*p)) {
      return i;
    }
  }
  return ARRAY_INDEX_NOT_FOUND;
}

template<class T>
s64 array_index(array<T>* a, T v) {
  return array_index_by_predicate(a, [=](T& p) { return p == v; });
}

template<class T>
T* array_remove(array<T>* a, size_t index) {
  if(index >= a->size) { return &a->data[a->size--]; }

  T* r = &a->data[index];

  a->size--;
  s64 size_to_copy = a->size - index;

  T tmp[size_to_copy];
  memcpy(tmp, a->data+index+1, sizeof(tmp)); // @CleanUp: memmove?
  memcpy(a->data+index, tmp, sizeof(tmp));
  return r;
}

template<class T>
T* array_pop(array<T>* a) {
  return &a->data[--a->size];
}

template<class T>
T* array_first(array<T>* a) {
  return &a->data[0];
}

template<class T>
T* array_last(array<T>* a) {
  return &a->data[a->size-1];
}

template<class T>
void array_resize(array<T>* a, size_t new_size) {
  array_reserve(a, new_size);
  a->size = a->capacity;
}

template<class T>
void array_clear(array<T>* a) {
  a->size = 0;
}

template<class T>
void array_copy(array<T>* a, const array<T>* b) {
  if(a->capacity < b->capacity) {
    dealloc(a->allocator, a->data);
    a->data     = (T*) alloc(a->allocator, sizeof(T)*b->capacity);
    a->capacity = b->capacity;
  }
  memcpy(a->data, b->data, sizeof(T)*b->size);
  a->size = b->size;
}

template<class T>
array<T> array_copy(const array<T>* b) {
  array<T> a;
  a.data     = (T*) alloc(a.allocator, sizeof(T)*b->capacity);
  a.size     = b->size;
  a.capacity = b->capacity;
  memcpy(a.data, b->data, sizeof(T)*b->size);
  return a;
}

template<class T>
void array_free(array<T>* a) {
  dealloc(a->allocator, a->data);
}
// end of array.

#undef GROW_FUNCTION

