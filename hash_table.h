
template<class Key>
uint64 default_hash_function(void* in) {
  return assert(0 && "Hash function for Key type is not implemented!"), 0;
}

template<>
uint64 defualt_hash_function<uint8*>(void* in) {
  uint8* str = (uint8*) in;
  uint8 c;

  uint64 hash = 5381;
  while (c = *str++) {
    hash = (hash * 33) ^ c;
  }

  return hash;
}

template<>
uint64 default_hash_function<int>(void* in) {
  int key = *(int*) in;
  // do thomas wang hashing
  return key;
}


#define GROW_FUNCTION(x) (((x) * 2) + 8)


template<class Key, class T>
struct hash_table {
  //void* memory;

  array<Key>      keys;
  array<T>      values;
  array<uint64> hashes;

  size_t allocated = 0;
  size_t count     = 0;

  Allocator allocator = global_allocator;
  uint64 (*hash_function)(void*) = default_hash_function<Key>;

  static const int   initial_size = 8;
  static const int loading_factor = 7; // @LoveYou: loading factor is actually 0.7, but since C++ compiler can't put 'static const float' as member variable we do that.

  // 
  // Memory Layout:
  //   uint64_t allocated;
  //   Key      keys  [count];
  //   T        values[count];
  //   uint64_t hashes[count];
  //
};

//#define GET_CAPACITY(table) ((uint64_t*)(table->memory))
//#define GET_KEYS(table)     (

uint64 table_compute_hash(uint64 (*hash_function)(void*), int key) {
  uint64 hash = hash_function(&key);
  hash = hash == 0 ? hash+1 : hash;
  // hash == 0 is a default 'empty' value and we don't want our hash function to return that.
  return hash;
}

template<class Key, class T>
T* table_add(hash_table<Key, T>* table, Key key) {

  assert(table->allocated == 0 ? table->count == 0 : true);

  if (table->count >= table->loading_factor/10.f * table->allocated) {
    table->allocated = GROW_FUNCTION(table->allocated);

    array<Key>    new_keys;
    array<T>      new_values;
    array<uint64> new_hashes;

    array_resize(&new_keys,   table->allocated);
    array_resize(&new_values, table->allocated);
    array_resize(&new_hashes, table->allocated);

    for (size_t i = 0; i < table->count; i++) {
      Key*     key = &table->keys[i];
      T*     value = &table->values[i];
      uint64* hash = &table->hashes[i];

      assert(hash != 0);

      uint64 new_entry = *hash % table->allocated;
      new_keys  [new_entry] = *key;
      new_values[new_entry] = *value;
      new_hashes[new_entry] = *hash;
    }

    array_free(&table->keys);
    array_free(&table->values);
    array_free(&table->hashes);

    table->keys   = new_keys;
    table->values = new_values;
    table->hashes = new_hashes;
  }
  assert(table->count < table->loading_factor/10.f * table->allocated); // check that there is enough space for an element in a table
  
  uint64 hash        = table_compute_hash(table->hash_function, key);
  uint64 entry_index = hash % table->allocated;

  assert(hash != 0);

  while (table->hashes[entry_index] != 0) {
    entry_index += 1;
    if (entry_index == table->allocated) { entry_index = 0; }
  }

  table->count += 1;
  table->keys  [entry_index] = key;
  table->hashes[entry_index] = hash;
  return &table->values[entry_index];
}

template<class Key, class T>
T* table_add(hash_table<Key, T>* table, Key key, T value) {
  return &(*table_add(table, key) = value);
}

#if 0
template<class Key, class T>
T* table_find(hash_table<Key, T>* table, Key key) {
}
#endif

template<class Key, class T>
void table_free(hash_table<Key, T>* table) {
  array_free(&table->keys);
  array_free(&table->values);
  array_free(&table->hashes);
  *table = {};
}


#undef GROW_FUNCTION

#define hash_set hash_table
#define set_free table_free
