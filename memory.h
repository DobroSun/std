#pragma once


Allocator default_allocator();
Allocator logging_allocator(struct Allocation_Storage*);
Allocator memory_arena_allocator(struct Memory_Arena*);
Allocator default_allocator();
Allocator logging_allocator(struct Allocation_Storage*);
Allocator logging_allocator();
Allocator memory_arena_allocator(struct Memory_Arena*);
void      report_all_memory_leaks(struct array<struct Allocation_Chunk>*);


struct Memory_Arena {
	void*  memory;
	size_t top;
	size_t allocated;
};

struct Allocation_Chunk {
  void  *ptr;
  size_t allocated;
  Source_Location loc;
};

struct Allocation_Storage {
  array<Allocation_Chunk> error_reports; // @CleanUp: SOA > AOS;

  Allocation_Storage() {
    error_reports.allocator = default_allocator();
  }

  ~Allocation_Storage() {
    report_all_memory_leaks(&error_reports);
    array_free(&error_reports);
  }
};


#ifdef NDEBUG
static const Allocator global_internal_allocator = default_allocator();
static       Allocator global_allocator          = default_allocator();
#else
static       Allocation_Storage allocation_storage;                                        // @ThreadSafety:
static const Allocator global_internal_allocator = logging_allocator(&allocation_storage); // @ThreadSafety: 
static       Allocator global_allocator          = logging_allocator(&allocation_storage); // @ThreadSafety: 
#endif

#define EXPAND(x) x

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define alloc(...)    EXPAND(GET_MACRO(__VA_ARGS__, alloc2,   alloc1)(__VA_ARGS__))
#define dealloc(...)  EXPAND(GET_MACRO(__VA_ARGS__, dealloc2, dealloc1)(__VA_ARGS__))

#define alloc2(allocator, bytes) __alloc(allocator,        bytes, { __LINE__, __FILE__, __func__ })
#define alloc1(bytes)            __alloc(global_allocator, bytes, { __LINE__, __FILE__, __func__ })
#define dealloc2(allocator, ptr) __dealloc(allocator,        ptr, { __LINE__, __FILE__, __func__ })
#define dealloc1(ptr)            __dealloc(global_allocator, ptr, { __LINE__, __FILE__, __func__ })


void* __alloc(Allocator allocator, size_t bytes, Source_Location loc) { return allocator.allocate(allocator.allocator_data, bytes, loc); }
void  __dealloc(Allocator allocator, void* ptr, Source_Location loc)  { return allocator.deallocate(allocator.allocator_data, ptr, loc); }

void* default_allocate(void* data, size_t bytes, Source_Location loc) {
  return malloc(bytes);
}

void  default_deallocate(void* data, void* ptr, Source_Location loc)  {
  return free(ptr);
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

void print_formatted_number(size_t n) {
  static const size_t KB = 1024;
  static const size_t MB = KB * 1024;
  static const size_t GB = MB * 1024;
  static const size_t TB = GB * 1024;

  if(n > TB) {
    printf("%.2f terabytes", n/(double)TB);
  } else if(n > GB) {
    printf("%.2f gigabytes", n/(double)GB);
  } else if(n > MB) {
    printf("%.2f megabytes", n/(double)MB);
  } else if(n > KB) {
    printf("%.2f kilobytes", n/(double)KB);
  } else {
    printf("%zu bytes", n);
  }
}

void report_all_memory_leaks(array<Allocation_Chunk>* error_reports) {
  size_t total = 0;
  for(const Allocation_Chunk& it : *error_reports) {
    if(it.allocated) {
      printf("%s: %zu: %s  \t::\twas allocated ", it.loc.file, it.loc.line, it.loc.function);
      print_formatted_number(it.allocated);
      printf(", but never freed!\n");
      total += it.allocated;
    }
  }

  if(total == 0) {
    printf("No memory leaks!\n");
  } else {
    printf("%s", "Total memory leaked: ");
    print_formatted_number(total);
    puts("");
  }
}

void* arena_allocate(void* data, size_t bytes, Source_Location loc) {
	Memory_Arena* arena = (Memory_Arena*) data;
	assert(arena->top + bytes < arena->allocated);
	void* r = (char*)arena->memory + arena->top;
	arena->top += bytes;
	return r;
}

void arena_deallocate(void* data, void* ptr, Source_Location loc) {
}

Allocator begin_memory_arena(Memory_Arena* arena, size_t bytes = 5e6) {
	arena->memory = alloc(global_internal_allocator, bytes);
	arena->top = 0;
	arena->allocated = bytes;
  return memory_arena_allocator(arena);
}

void reset_memory_arena(Memory_Arena* arena) {
  arena->top = 0;
}

void end_memory_arena(Memory_Arena* arena) {
  dealloc(global_internal_allocator, arena->memory);
}


Allocator default_allocator()                      { return { default_allocate, default_deallocate, NULL }; }
Allocator logging_allocator(Allocation_Storage* s) { return { log_allocate, log_deallocate, s }; }
Allocator memory_arena_allocator(Memory_Arena* a)  { return { arena_allocate, arena_deallocate, a }; }

inline Allocator get_global_allocator() { return global_allocator; }
