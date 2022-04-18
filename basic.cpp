#include "pch.h"

#if 0
void* primary_fiber;
void print_bits(void* v, int s) {
  uint8_t* bits = (uint8_t*) v;
  for(int i = 0; i < s; i++) {
    uint8_t c = bits[i];
    printf("%d", ((c >> 7) & 1));
    printf("%d", ((c >> 6) & 1));
    printf("%d", ((c >> 5) & 1));
    printf("%d", ((c >> 4) & 1));
    printf("%d", ((c >> 3) & 1));
    printf("%d", ((c >> 2) & 1));
    printf("%d", ((c >> 1) & 1));
    printf("%d", ((c >> 0) & 1));
    puts("");
  }
}

void Fiber1(LPVOID lpFiberParameter) {
  puts("Fiber1!");
  SwitchToFiber(primary_fiber);
  puts("AnotherFiber1!");
  SwitchToFiber(primary_fiber);
}

void Fiber2(LPVOID lpFiberParameter) {
  puts("Fiber2!");
  SwitchToFiber(primary_fiber);
  puts("AnotherFiber2!");
  SwitchToFiber(primary_fiber);
}
  
void Fiber3(int v) {}


struct Coroutine {
  void* fiber;
  int   return_value;
};

Coroutine coro_create() {
  return {};
}

Coroutine coro_next(Coroutine* ) {
  return {};
}

int main() {
  sparse_array<const char*> name_of_entity;

  const size_t some_random_entity = 100;
  sparse_find_or_add(&name_of_entity, some_random_entity);

  const char* hello = "hello world!";
  name_of_entity[some_random_entity] = hello;

  printf("\nsparse contains a message := %s\n\n", name_of_entity[some_random_entity]);

  static_array<int, 2> c;
  c[0] = 10;
  c[1] = 10;

  static_array<int, 2> d = c;

  d[0] = 2;

  printf("%d\n", c[0]);
  printf("%d\n", c[1]);


  #if 0
  puts("ConvertThreadToFiber!");
  primary_fiber = ConvertThreadToFiber(NULL); // here we get our main fiber to come back to.
  void* fiber1 = CreateFiber(0, Fiber1, NULL); // this just creates a fiber. but does not start to execute it.
  void* fiber2 = CreateFiber(0, Fiber2, NULL); // this just creates a fiber. but does not start to execute it.

  SwitchToFiber(fiber1); // actually schedules a fiber to run in another execution context.
  puts("main!");
  SwitchToFiber(fiber2);
  puts("main!");
  SwitchToFiber(fiber2);
  puts("main!");
  SwitchToFiber(fiber1);
  puts("main!");
  #endif

  Memory_Arena arena;
  Allocator arena_allocator = begin_memory_arena(&arena);

  end_memory_arena(&arena);


  return 0;
}
#endif


int main() {
  hash_table<int, const char*> table;

  table_add(&table, 10, "hello world!");
  table_add(&table, 11, "hello world again!");
  table_add(&table, 12, "there is another one!");

  table_free(&table);

  return 0;
}
