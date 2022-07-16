#pragma once

template<class ...Args>
void __my_assert(Source_Location loc, bool expression, const char* message, Args... args);

// 
// @Incomplete: be careful with using my_asserts in allocators code, because it might cause some bugs if we couldn't allocate memory and we are going to try to allocate even more for a print.
// 

// 
//
// @Incomplete: memory allocators should not just return operate with void* but with a pair of { void*, size_t }, rewrite everything to handle that.
// 
// 
// 
// struct Memory_Bulk {
//  void*  memory;
//  size_t allocated;
// };


Allocator default_allocator();
Allocator logging_allocator(struct Allocation_Storage*);
Allocator memory_arena_allocator(struct Memory_Arena*);
Allocator default_allocator();
Allocator logging_allocator(struct Allocation_Storage*);
Allocator logging_allocator();
Allocator memory_arena_allocator(struct Memory_Arena*);
Allocator temporary_storage_allocator(struct Temporary_Storage*);
void      report_all_memory_leaks();


struct Memory_Arena {
  Allocator allocator;
	void*  memory;
	size_t top;
	size_t allocated;
};

struct Temporary_Storage {
	Memory_Arena arena;
	size_t highest_water_mark;
};

struct Allocation_Chunk {
  void  *ptr;
  size_t allocated;
  Source_Location loc;
};

struct Allocation_Storage {
  array<Allocation_Chunk> error_reports; // @CleanUp: SOA > AOS;
  bool should_report_memory_leaks = true;

  Allocation_Storage() {
    error_reports.allocator = default_allocator();
  }

  ~Allocation_Storage() {
    array_free(&error_reports);
  }
};


static Allocation_Storage allocation_storage;

#ifdef NDEBUG
static const Allocator global_internal_allocator = default_allocator();
static       Allocator global_allocator          = default_allocator();
#else
static const Allocator global_internal_allocator = logging_allocator(&allocation_storage); // @ThreadSafety: 
static       Allocator global_allocator          = logging_allocator(&allocation_storage); // @ThreadSafety: 
#endif

static Temporary_Storage temporary_storage;

#define EXPAND(x) x
#define GET_MACRO(_1, _2, NAME, ...) NAME
#define GET_MACRO3(_1, _2, _3, NAME, ...) NAME
#define alloc(...)       EXPAND(GET_MACRO(__VA_ARGS__, alloc2,   alloc1)(__VA_ARGS__))
#define dealloc(...)     EXPAND(GET_MACRO(__VA_ARGS__, dealloc2, dealloc1)(__VA_ARGS__))
#define reallocate(...)  EXPAND(GET_MACRO3(__VA_ARGS__, realloc3, realloc2, realloc1)(__VA_ARGS__)) // @Rename: 

#define alloc2(allocator, bytes)        __alloc(allocator,        bytes,        { __LINE__, __FILE__, __func__ })
#define alloc1(bytes)                   __alloc(global_allocator, bytes,        { __LINE__, __FILE__, __func__ })
#define dealloc2(allocator, ptr)        __dealloc(allocator,        ptr,        { __LINE__, __FILE__, __func__ })
#define dealloc1(ptr)                   __dealloc(global_allocator, ptr,        { __LINE__, __FILE__, __func__ })
#define realloc3(allocator, ptr, bytes) __realloc(allocator,        ptr, bytes, { __LINE__, __FILE__, __func__ })
#define realloc2(ptr, bytes)            __realloc(global_allocator, ptr, bytes, { __LINE__, __FILE__, __func__ })

void* __alloc(Allocator allocator, size_t bytes,              Source_Location loc) { return allocator.allocate_(allocator.allocator_data, bytes, loc); }
void  __dealloc(Allocator allocator, void* ptr,               Source_Location loc) { return allocator.deallocate_(allocator.allocator_data, ptr, loc); }
void* __realloc(Allocator allocator, void* ptr, size_t bytes, Source_Location loc) { return allocator.reallocate_(allocator.allocator_data, ptr, bytes, loc); }

void* default_allocate(void* data, size_t bytes, Source_Location loc) {
  return malloc(bytes);
}

void  default_deallocate(void* data, void* ptr, Source_Location loc)  {
  return free(ptr);
}

void* default_reallocate(void* data, void* ptr, size_t bytes, Source_Location loc) {
    return realloc(ptr, bytes);
}


void* log_allocate(void* data, size_t bytes, Source_Location loc) {
  Allocation_Storage* storage = (Allocation_Storage*) data;

  // @Incomplete: default_allocator()
  void *r = malloc(bytes);
  assert(r);
  memset(r, 0, bytes);

  Allocation_Chunk *chunk = array_add(&storage->error_reports);
  chunk->ptr = r;
  chunk->allocated = bytes;
  chunk->loc = loc;
  return r;
}

void  log_deallocate(void* data, void* ptr, Source_Location loc) {
  if(!ptr) return;

  Allocation_Storage* storage = (Allocation_Storage*) data;
  Allocation_Chunk*   found   = array_find_by_predicate(&storage->error_reports, [=](Allocation_Chunk& it) {
    return it.ptr == ptr;
  });
  if(found) {
    *found = *array_pop(&storage->error_reports);
  }

  free(ptr);
}

void* log_reallocate(void* data, void* ptr, size_t bytes, Source_Location loc) {
    void* r = realloc(ptr, bytes);

    Allocation_Storage* storage = (Allocation_Storage*)data;
    Allocation_Chunk* found = array_find_by_predicate(&storage->error_reports, [=](Allocation_Chunk& it) {
        return it.ptr == ptr;
    });

    Allocation_Chunk chunk;
    chunk.ptr = r;
    chunk.allocated = bytes;
    chunk.loc = loc;
    if (found) {
        *found = chunk;
    } else {
        array_add(&storage->error_reports, chunk);
    }
    return r;
}

void* arena_allocate(void* data, size_t bytes, Source_Location loc) {
	Memory_Arena* arena = (Memory_Arena*) data;
	assert(arena->top + bytes <= arena->allocated);
	void* r = (char*) arena->memory + arena->top;
	arena->top += bytes;
	return r;
}

void arena_deallocate(void* data, void* ptr, Source_Location loc) {
}

void* arena_reallocate(void* data, void* ptr, size_t bytes, Source_Location loc) {
	Memory_Arena* arena = (Memory_Arena*) data;
  if (ptr) {
    size_t how_much_was_allocated_before = ((char*) arena->memory + arena->top) - ptr;
    arena->top += bytes - how_much_was_allocated_before;
    my_assert(bytes > how_much_was_allocated_before, "This data was already reallocated somewhere else. Bytes to allocte := %, How much was already allocated := %", bytes, how_much_was_allocated_before);
    return ptr;
  } else {
    return arena_allocate(data, bytes, loc);
  }
}

void begin_memory_arena(Memory_Arena* arena, size_t bytes = 5e6) {
  arena->memory = alloc(global_internal_allocator, bytes);
  arena->top = 0;
  arena->allocated = bytes;
  arena->allocator = memory_arena_allocator(arena);
}

void reset_memory_arena(Memory_Arena* arena) {
  arena->top = 0;
}

void end_memory_arena(Memory_Arena* arena) {
  dealloc(global_internal_allocator, arena->memory);
}

void* temporary_allocate(void* data, size_t bytes, Source_Location loc) {
	Temporary_Storage* storage = (Temporary_Storage*) data;

  void* r = arena_allocate(&storage->arena, bytes, loc);
  storage->highest_water_mark = max(storage->arena.top, storage->highest_water_mark);
	return r;
}

void temporary_deallocate(void* data, void* ptr, Source_Location loc) {
}

void* temporary_reallocate(void* data, void* ptr, size_t bytes, Source_Location loc) {
	Temporary_Storage* storage = (Temporary_Storage*) data;

  void* r = arena_reallocate(data, ptr, bytes, loc);
  storage->highest_water_mark = max(storage->arena.top, storage->highest_water_mark);
  return r;
}

void begin_temporary_storage(Temporary_Storage* storage, size_t bytes = 5e6) {
  begin_memory_arena(&storage->arena, bytes);
  storage->arena.allocator = temporary_storage_allocator(storage);
  storage->highest_water_mark = 0;
}

void reset_temporary_storage(Temporary_Storage* storage) {
  reset_memory_arena(&storage->arena);
}

void end_temporary_storage(Temporary_Storage* storage) {
  end_memory_arena(&storage->arena);
}


Allocator default_allocator()                      { return { default_allocate, default_deallocate, default_reallocate, NULL }; }
Allocator logging_allocator(Allocation_Storage* s) { return { log_allocate, log_deallocate, log_reallocate, s }; }
Allocator memory_arena_allocator(Memory_Arena* a)  { return { arena_allocate, arena_deallocate, arena_reallocate, a }; }
Allocator temporary_storage_allocator(Temporary_Storage* s) { return { temporary_allocate, temporary_deallocate, temporary_reallocate, s }; }
Allocator temp_allocator()                         { return temporary_storage.arena.allocator; }

inline Allocator get_global_allocator() { return global_allocator; }


bool compare(Allocator a, Allocator b) {
  return a.allocate_   == b.allocate_   && 
         a.deallocate_ == b.deallocate_ && 
         a.reallocate_ == b.reallocate_ && 
         a.allocator_data == b.allocator_data;
}