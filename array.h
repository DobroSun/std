#pragma once

#define GROW_FUNCTION(x) (2*(x) + 8)
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )

#define temp_array(type, N) { (type*) alloca(sizeof(type) * N), N, N, get_global_allocator() }


void swap(void*, void*, size_t);

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
void array_reserve_(array<T>* a, s64 new_capacity, Source_Location loc) {
  //assert(new_capacity > a->capacity && "array<T>;:reserve new_capacity is <= then array one, can't do anything!");

  a->data = (T*) __realloc(a->allocator, a->data, sizeof(T)*new_capacity, loc);
  a->capacity = new_capacity;
}

template<class T>
T* array_add_(array<T>* a, Source_Location loc) {
  if(a->size == a->capacity) {
    array_reserve_(a, GROW_FUNCTION(a->capacity), loc);
  }
  return &a->data[a->size++];
}

template<class T>
T* array_add_(array<T>* a, T v, Source_Location loc) {
  return &(*array_add_(a, loc) = v);
}

template<class T>
void array_add_(array<T>* a, T* v, Source_Location loc) {
  size_t count = 0;
  while (*v) {
    v++;
    count++;
  }

  array_add_(a, v, count, loc);
}

template<class T>
void array_add_(array<T>* a, T* v, size_t s, Source_Location loc) {
  array_ensure_capacity_(a, a->size + s, loc);
  memcpy(a->data + a->size, v, sizeof(T) * s);
  a->size += s;
}

template<class T>
void array_add_(array<T>* a, const array<T>* b, Source_Location loc) {
  array_ensure_capacity_(a, a->size + b->size, loc);
  memcpy(a->data + a->size, b->data, sizeof(T) * b->size);
  a->size += b->size;
}

template<class T>
void array_add_unique_(array<T>* a, T v, Source_Location loc) {
  if (array_find(a, v)) {
    return;
  } else {
    array_add_(a, v, loc);
  }
}

template<class T>
void array_ensure_capacity_(array<T>* a, size_t n, Source_Location loc) {
  if (n > a->capacity) {
    array_reserve_(a, n, loc);
  }
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
  memcpy(tmp, a->data+index+1, sizeof(tmp));
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
  array_reserve_(a, new_size);
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
array<T> array_copy(const T* data, size_t size) {
  array<T> b;
  b.data = (T*) data;
  b.size = size;
  b.capacity = size;
  return array_copy(&b);
}

template<class T>
void array_copy_range(array<T>* a, const array<T>* b, size_t first, size_t last) {
  assert(first <= b->size && last <= b->size);
  size_t size = last - first;
  array_resize(a, size);
  memcpy(a->data, &b->data[first], sizeof(T) * size);
}

template<class T, class F>
void array_sort(array<T>* a, F f) {
  // 
  // @Incomplete: copy&paste real sort in here.
  // 

  for (size_t i = 0; i < a->size; i++) {
    T* current = &(*a)[i];

    for (size_t j = 0; j < a->size; j++) {
      if (i == j) continue;

      T* test_with = &(*a)[j];
      if (f(*current, *test_with)) {
        swap(current, test_with, sizeof(T));
      }
    }
  }
}

template<class T>
void array_free(array<T>* a) {
  dealloc(a->allocator, a->data);
}
// end of array.

#undef max
#undef GROW_FUNCTION

