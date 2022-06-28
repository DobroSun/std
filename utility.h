#pragma once

#define KB(x) 1024*(x)
#define MB(x) 1024*KB(x)
#define GB(x) 1024*MB(x)


void swap(void* a, void* b, size_t size_of) {
  u8 *t = (u8*) alloca(size_of);
  memcpy(t, a, size_of);
  memcpy(a, b, size_of);
  memcpy(b, t, size_of);
}

#define Allocate_Struct(type) ((type*) alloc(sizeof(type))) 

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define red(A) ANSI_COLOR_RED A ANSI_COLOR_RESET
#define green(A) ANSI_COLOR_GREEN A ANSI_COLOR_RESET
#define yellow(A) ANSI_COLOR_YELLOW A ANSI_COLOR_RESET
#define blue(A) ANSI_COLOR_BLUE A ANSI_COLOR_RESET
#define magenta(A) ANSI_COLOR_MAGENTA A ANSI_COLOR_RESET
#define cyan(A) ANSI_COLOR_CYAN A ANSI_COLOR_RESET

#define CONCAT_LINES(...) __VA_ARGS__


#define defer auto ANONYMOUS_NAME = Junk{} + [&]()
#define ANONYMOUS_NAME CONCAT(GAMMA, __LINE__)
#define CONCAT(A, B) CONCAT_IMPL(A, B)
#define CONCAT_IMPL(A, B) A##B

template<class T>
struct Defer {
  const T func;
  Defer(const T f) : func(f) {}
  ~Defer()         { func(); }
};

struct Junk {};
template<class T> inline const Defer<T> operator+(Junk, const T f) { return f; }


struct literal {
  const char *data;
  size_t      count;

  char operator[](size_t i) const {
    assert(i < count);
    return data[i];
  }
};

// #define make_literal(x) (literal){ (x), sizeof(x)-1 }
inline literal XXX_visual_studio_sucks(const char* x, size_t s) { return { x, s }; }
#define make_literal(x) XXX_visual_studio_sucks((x), sizeof(x)-1)


inline bool operator==(char s, literal l) { return l.count == 1 && l.data[0] == s; }
inline bool operator==(literal l, char s) { return l.count == 1 && l.data[0] == s; }

inline bool operator==(const char *s, literal l) { return l.count && !strncmp(s, l.data, l.count); }
inline bool operator==(literal l, const char *s) { return l.count && !strncmp(s, l.data, l.count); }
inline bool operator!=(const char *s, literal l) { return !(s == l); }
inline bool operator!=(literal l, const char *s) { return !(s == l); }

inline bool operator==(literal l1, literal l2) {
  if(l1.count == l2.count) {
    return !strncmp(l1.data, l2.data, l1.count);
  } else {
    return false;
  }
}

inline bool operator!=(literal l1, literal l2) { return !(l1 == l2); }

inline std::ostream& operator<<(std::ostream &os, literal l) {
  for(size_t i = 0; i < l.count; i++) {
    os << l.data[i];
  }
  return os;
}

#define static_string_from_literal(name, l) \
  char name[l.count+1] = {}; \
  memcpy(name, l.data, l.count);

inline char *dynamic_string_from_literal(literal l) {
  char *r = (char *)alloc(l.count+1);
  memcpy(r, l.data, l.count);
  return r;
}

#define strncpy(...) __strncpy(__VA_ARGS__)
void __strncpy(char* dst, const char* src, size_t count) {
  while(*dst = *(src++)) {}
}

inline size_t write_string(char *r, size_t cursor, const char *s, size_t size) {
  strncpy(r+cursor, s, size);
  return cursor + size;
}

inline size_t write_string(char *r, size_t cursor, literal l) {
  return write_string(r, cursor, l.data, l.count);
}

inline size_t write_string(char *r, size_t cursor, const char *s) {
  return write_string(r, cursor, s, strlen(s));
}

inline size_t count_format(const char *fmt, va_list args) {
  return vsnprintf(NULL, 0, fmt, args)+1;
}

inline size_t count_format(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t s = count_format(fmt, args);
  va_end(args);
  return s;
}

inline size_t write_format(char *r, size_t cursor, size_t size, const char *fmt, va_list args) {
  return cursor + vsnprintf(r+cursor, size, fmt, args);
}


inline size_t write_format(char *r, size_t cursor, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  cursor = write_format(r, cursor, size, fmt, args);
  va_end(args);
  return cursor;
}


inline size_t write_format(char *r, size_t cursor, const char *fmt, va_list args) { // unchecked.
  size_t size;
  {
    va_list copy;
    va_copy(copy, args);
    size = count_format(fmt, copy);
  }
  return write_format(r, cursor, size, fmt, args);
}

inline size_t write_format(char *r, size_t cursor, const char *fmt, ...) {
  size_t size;
  {
    va_list args;
    va_start(args, fmt);
    size = count_format(fmt, args);
    va_end(args);
  }
  {
    va_list args;
    va_start(args, fmt);
    size = write_format(r, cursor, size, fmt, args);
    va_end(args);
  }
  return size;
}


// 
// function: print(format, args...)
// print("hello {}{}", "world", '!') -> "hello world!"
// print("only format{}!")           -> "only format!"
// print("{} + {} = {}", 1, 2, 3)    -> "1 + 2 = 3"
// 

#if 0
struct Format_String_Info {
  array<uint> format_positions;
  uint        num_format_args;
};

Format_String_Info check_format_string(literal s) {
  Format_String_Info info = {};

  const char* cursor = s.data;
  uint count = 0;

  while (*cursor != '\0') {
    if (*cursor == '%') { 
      info.num_format_args += 1;
      array_add(&info.format_positions, count);
    }
    cursor += 1;
    count  += 1;
  }

  return info;
}

template<class T>
void print_unit(T unit);

template<>
void print_unit(const char* unit) {
  printf(unit);
}

template<class T, class ...Args>
void print(const char *s, T first, Args... rest) {
  literal l = { s, strlen(s) }; // @Incomplete: @AsciiOnly: 
  Format_String_Info info = check_format_string(l);

  defer { array_free(&info.format_positions); }; // @Incomplete: @UseTemporaryStorage: 

  uint previous_pos = 0;
  for (size_t i = 0; i < info.num_format_args; i++) {
    uint format_pos = info.format_positions[i];
    literal p = { &s[previous_pos], format_pos-previous_pos };
    if (p.count > 0) {
      printf("%.*s", p.count, p.data);
    }

    print_unit(first);
    previous_pos = format_pos+1;
  }
  return;
}
#else 
template<class T> class array;

template<class T>
void print_obj(T obj) {
  std::cout << obj; // @Incomplete: do the thing for every thing.
}

template<>
void inline print_obj<void*>(void* obj) {
  printf("%p", obj);
}

template<>
void inline print_obj<bool>(bool obj) {
  printf("%s", (obj) ? "true" : "false");
}

template<>
void inline print_obj<literal>(literal obj) {
  printf("%.*s", (int) obj.count, obj.data);
}

template<class T>
void print_obj(array<T> a) {
  printf("[");
  for(s64 index = 0; index < a->size; index++) {
    print_obj(a[index]);
    printf("%s", ((index != a.count-1) ? ", " : ""));
  }
  printf("]");
}

size_t inline print_count(const char *s) {
  size_t count = 0;
  while(s[count]) {
    if(s[count] == '%') {
      break;
    }
    count++;
  }
  printf("%.*s", (int)count, s);
  return count;
}

void inline print(const char *s) {
  size_t count = print_count(s);
  if(s[count]) {
    s += count+1;
    print(s);
  }
}

template<class T, class ...Args>
void print(const char *s, T first, Args... rest) {
  size_t count = print_count(s);

  if(s[count]) {
    print_obj(first);
    s += count+1;
    print(s, rest...);
  }
}

// print w/o format string.

inline void print() {
  printf("\n");
}

template<class T>
void print(T last) {
  print_obj(last);
  print();
}

template<class T, class ...Args>
void print(T first, Args... rest) {
  print_obj(first);
  printf(", ");
  print(rest...);
}
#endif

struct Timer {
  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;

  Timer()  { start = std::chrono::steady_clock::now(); }
  ~Timer() { 
    end = std::chrono::steady_clock::now(); 
  
    const f64 delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    if(delta < 1000.) {
      print("% ns", delta);
    } else if(delta >= 1000. && delta < 1000000.) {
      print("% us", delta/1000.);
    } else {
      print("% ms", delta/1000000.);
    }
  }
};

template<class T>
s32 get_number_of_digits(T v) {
  s32 n = 0;
  while(v) {
    n++;
    v /= 10;
  }
  return n;
}


#define define_functor(name, op) \
  struct name { \
    template<class T, class U> \
    auto operator()(T a, U b) -> decltype(a op b) { \
      return a op b; \
    } \
  }

#define define_functor2(name, op) \
  struct name { \
    template<class T> \
    T operator()(T a) {\
      return op a; \
    } \
  }

define_functor(add, +);
define_functor(subtract, -);
define_functor(multiply, *);
define_functor(divide, /);
define_functor(bitwise_and, &);
define_functor(bitwise_or, |);
define_functor(bitwise_xor, ^);
define_functor(equals, ==);
define_functor(not_equals, !=);
define_functor(modulo, %);
define_functor(less, <);
define_functor(greater, >);
define_functor(less_or_equals, <=);
define_functor(greater_or_equals, >=);
define_functor(bit_and, &);
define_functor(bit_or, |);
define_functor(bit_xor, ^);
define_functor(logic_and, &&);
define_functor(logic_or, ||);

define_functor2(plus, +);
define_functor2(minus, -);
define_functor2(logic_not, !);
define_functor2(bit_not, ~);


