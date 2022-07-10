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

#define Allocate_Struct(...) ((__VA_ARGS__*) alloc(sizeof(__VA_ARGS__))) 

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

int len(const char* s) {
  if (s) {
    const char* e = s;
    while (*e != '\0') {
      e++;
    }
    return e - s;
  } else {
    return 0;
  }
}


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

literal strip_front(literal string) {
  while (string.count) {
    char c = string.data[0];
    if (c == ' ' || c == '\t' || c == '\v' || c == '\b' || c == '\r') {
      string.data  += 1;
      string.count -= 1;
    } else {
      break;
    }
  }
  return string;
}

const char* make_c_string(literal l) { 
  char* v = (char*) alloc(temp_allocator(), l.count+1);
  memset(v, '\0', l.count+1);
  memcpy(v, l.data, l.count);
  return v;
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
  return write_string(r, cursor, s, len(s));
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
  literal l = { s, len(s) }; // @Incomplete: @AsciiOnly: 
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

#endif

void reverse(literal* string) {
    int start = 0;
    int end   = string->count-1;
    while (start < end) {
        char ss = string->data[start];
        char ee = string->data[end];
        ((char*) string->data)[start] = ee;
        ((char*) string->data)[end]   = ss;
        start++;
        end--;
    }
}

int integer_to_string(int64 number, int base, char* string) {
  int count = 0;
  bool is_negative = false; // @Note: I'm always negative.

  if (number == 0) {
    string[count++] = '0';
    string[count++] = '\0';
    return 1;
  }

  if (number < 0 && base == 10) {
    is_negative = true;
    number = -number;
  }

  while (number != 0) {
    int rem = number % base;
    string[count++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
    number = number / base;
  }

  if (is_negative) {
    string[count++] = '-';
  }

  string[count] = '\0';

  literal l = { string, count };
  reverse(&l);
  return count;
}

int uinteger_to_string(uint64 number, int base, char* string) {
  int count = 0;
  bool is_negative = false; // @Note: I'm always negative.

  if (number == 0) {
    string[count++] = '0';
    string[count++] = '\0';
    return 1;
  }

  while (number != 0) {
    int rem = number % base;
    string[count++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
    number = number / base;
  }

  if (is_negative) {
    string[count++] = '-';
  }

  string[count] = '\0';

  literal l = { string, count };
  reverse(&l);
  return count;
}

int float_to_string(double fp, int number_of_digits_after_decimal_point, char* string) {
  char conversion[1076], intPart_reversed[311];
  int char_count = 0;

  double fp_int;
  double fp_frac = modf(fp, &fp_int); // Separate integer/fractional parts

  while (fp_int > 0) {
    // Convert integer part, if any
    intPart_reversed[char_count++] = '0' + (int)fmod(fp_int, 10);
    fp_int = floor(fp_int / 10);
  }

  for (int i = 0; i < char_count; i++) {
    // Reverse the integer part, if any
    conversion[i] = intPart_reversed[char_count-i-1];
  }

  conversion[char_count++] = '.';
  int decimal_index = char_count;

  while (fp_frac > 0) {
    // Convert fractional part, if any
    fp_frac *=10;
    fp_frac  = modf(fp_frac, &fp_int);
    conversion[char_count++] = '0' + (int)fp_int;
  }

  conversion[char_count] = '\0'; // null terminator
  memcpy(string, conversion, decimal_index + number_of_digits_after_decimal_point);
  return decimal_index + number_of_digits_after_decimal_point;
}

void ftoa(float f, const char* string) {
  ftoa((double) f, string);
}


typedef array<char> String_Builder;

static int32 indentation_level = 0;

void ensure_space(String_Builder* builder, int space) {
  array_reserve(builder, builder->size + space);
}

void put(String_Builder* builder, literal s) {
  array_add(builder, (char*) s.data, s.count);
}

void put(String_Builder* builder, const char* s) {
  put(builder, {s, strlen(s)}); // @Speed: no need to do strlen(...) we always use raw string literals, so we can make a template here that will know about its size.
}

void put(String_Builder* builder, int64 a, int base) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = integer_to_string(a, base, builder->data + builder->size);

  assert(count < 32);
  builder->size += count;
}

void put(String_Builder* builder, uint64 a, int base) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = uinteger_to_string(a, base, builder->data + builder->size);

  assert(count < 32);
  builder->size += count;
}

void put(String_Builder* builder, float64 a, int number_of_digits_after_decimal_point) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = float_to_string(a, number_of_digits_after_decimal_point, builder->data + builder->size);

  assert(count < 32);
  builder->size += count;
}


void put_spaces(String_Builder* builder) {
  char* spaces = (char*) alloca(indentation_level);
  memset(spaces, ' ', indentation_level);

  literal l = { spaces, (size_t)indentation_level };
  put(builder, l);
}


enum Print_Type {
  PRINT_NONE = 0,
  PRINT_INT_8,
  PRINT_INT_16,
  PRINT_INT_32,
  PRINT_INT_64,
  PRINT_UINT_8,
  PRINT_UINT_16,
  PRINT_UINT_32,
  PRINT_UINT_64,
  PRINT_FLOAT_32,
  PRINT_FLOAT_64,
  PRINT_C_STRING,
  PRINT_LITERAL,
  PRINT_FORMATTED_INT,
  PRINT_FORMATTED_FLOAT,
};

// @Incomplete: what about different types?
template<class T>
struct Print_Formatted_Integer {
  T value;
  int base = 10;
};

template<class T>
struct Print_Formatted_Float {
  T value;
  int number_of_digits_after_decimal_point = 5;
};

typedef Print_Formatted_Integer<int8>   Print_Formatted_Int8;
typedef Print_Formatted_Integer<int16>  Print_Formatted_Int16;
typedef Print_Formatted_Integer<int32>  Print_Formatted_Int32;
typedef Print_Formatted_Integer<int64>  Print_Formatted_Int64;
typedef Print_Formatted_Integer<uint8>  Print_Formatted_Uint8;
typedef Print_Formatted_Integer<uint16> Print_Formatted_Uint16;
typedef Print_Formatted_Integer<uint32> Print_Formatted_Uint32;
typedef Print_Formatted_Integer<uint64> Print_Formatted_Uint64;
typedef Print_Formatted_Float<float32>  Print_Formatted_Float32;
typedef Print_Formatted_Float<float64>  Print_Formatted_Float64;



struct Print_Variable {
  Print_Type type;
  union {
    Print_Formatted_Int8 i8;
    Print_Formatted_Int16 i16;
    Print_Formatted_Int32 i32;
    Print_Formatted_Int64 i64;
    Print_Formatted_Uint8 u8;
    Print_Formatted_Uint16 u16;
    Print_Formatted_Uint32 u32;
    Print_Formatted_Uint64 u64;

    Print_Formatted_Float32 f32;
    Print_Formatted_Float64 f64;

    const char* c_string;
    literal     string;
  };

  Print_Variable(int8 v) {
    type = PRINT_INT_8;
    i8 = {};
    i8.value = v;
  }

  Print_Variable(int16 v) {
    type = PRINT_INT_16;
    i16 = {};
    i16.value = v;
  }

  Print_Variable(int32 v) {
    type = PRINT_INT_32;
    i32 = {};
    i32.value = v;
  }

  Print_Variable(int64 v) {
    type = PRINT_INT_64;
    i64 = {};
    i64.value = v;
  }

  Print_Variable(uint8 v) {
    type = PRINT_UINT_8;
    u8 = {};
    u8.value = v;
  }

  Print_Variable(uint16 v) {
    type = PRINT_UINT_16;
    u16 = {};
    u16.value = v;
  }

  Print_Variable(uint32 v) {
    type = PRINT_UINT_32;
    u32 = {};
    u32.value = v;
  }

  Print_Variable(uint64 v) {
    type = PRINT_UINT_64;
    u64 = {};
    u64.value = v;
  }

  Print_Variable(float32 v) {
    type = PRINT_FLOAT_32;
    f32 = {};
    f32.value = v;
  }

  Print_Variable(float64 v) {
    type = PRINT_FLOAT_64;
    f64 = {};
    f64.value = v;
  }

  Print_Variable(Print_Formatted_Integer<int8> v) {
    type = PRINT_INT_8;
    i8 = v;
  }

  Print_Variable(Print_Formatted_Integer<int16> v) {
    type = PRINT_INT_16;
    i16 = v;
  }


  Print_Variable(Print_Formatted_Integer<int32> v) {
    type = PRINT_INT_32;
    i32 = v;
  }

  Print_Variable(Print_Formatted_Integer<int64> v) {
    type = PRINT_INT_64;
    i64 = v;
  }

  Print_Variable(Print_Formatted_Integer<uint8> v) {
    type = PRINT_UINT_8;
    u8 = v;
  }

  Print_Variable(Print_Formatted_Integer<uint16> v) {
    type = PRINT_UINT_16;
    u16 = v;
  }

  Print_Variable(Print_Formatted_Integer<uint32> v) {
    type = PRINT_UINT_32;
    u32 = v;
  }

  Print_Variable(Print_Formatted_Integer<uint64> v) {
    type = PRINT_UINT_64;
    u64 = v;
  }

  Print_Variable(Print_Formatted_Float<float32> v) {
    type = PRINT_FLOAT_32;
    f32 = v;
  }

  Print_Variable(Print_Formatted_Float<float64> v) {
    type = PRINT_FLOAT_64;
    f64 = v;
  }

  Print_Variable(const char* v) {
    type = PRINT_C_STRING;
    c_string = v;
  }

  Print_Variable(char* v) {
    type = PRINT_C_STRING;
    c_string = v;
  }

  Print_Variable(literal v) {
    type = PRINT_LITERAL;
    string = v;
  }
};

template<class T>
Print_Formatted_Integer<T> formatted_int(T value, int base = 10) {
  return { value, base };
}

template<class T>
Print_Formatted_Float<T> formatted_float(T value, int number_of_digits_after_decimal_point = 5) {
  return { value, number_of_digits_after_decimal_point };
}

void print(const char* format) {
  printf("%.*s", len(format), format);
}

template<class... Args>
void print(const char* format, Args... args) {
  Print_Variable variables[] = { Print_Variable(args)... };

  int num_arguments_passed = static_array_size(variables);
  int num_arguments_expected_from_format = 0;

  // 
  // @Incomplete: use Temporary_Storage here, for now just manually deallocate String_Builder.
  // 
  String_Builder builder;
  defer { array_free(&builder); };

  int argument       = 0;
  const char* cursor = format;
  if (cursor) {
    while (1) {

      literal format_percent = make_literal("%");
      literal double_percent = make_literal("%%");

      if (*cursor == '\0') { break; }

      if (cursor == double_percent) {
        cursor += double_percent.count;
        put(&builder, (char*) "%");

      } else if (cursor == format_percent) {
        num_arguments_expected_from_format += 1;
        cursor += format_percent.count;

        if(argument >= num_arguments_passed) { break; }

        Print_Variable v = variables[argument];
        argument += 1;

        switch (v.type) {
        case PRINT_INT_8:  put(&builder, (int64) v.i8.value, v.i8.base); break; 
        case PRINT_INT_16: put(&builder, (int64) v.i16.value, v.i16.base); break;
        case PRINT_INT_32: put(&builder, (int64) v.i32.value, v.i32.base); break;
        case PRINT_INT_64: put(&builder, (int64) v.i64.value, v.i64.base); break;

        case PRINT_UINT_8:  put(&builder, (uint64) v.u8.value, v.u8.base); break;
        case PRINT_UINT_16: put(&builder, (uint64) v.u16.value, v.u16.base); break;
        case PRINT_UINT_32: put(&builder, (uint64) v.u32.value, v.u32.base); break;
        case PRINT_UINT_64: put(&builder, (uint64) v.u64.value, v.u64.base); break;

        case PRINT_FLOAT_32: put(&builder, (float64) v.f32.value, v.f32.number_of_digits_after_decimal_point); break;;
        case PRINT_FLOAT_64: put(&builder, (float64) v.f64.value, v.f64.number_of_digits_after_decimal_point); break;

        case PRINT_C_STRING: put(&builder, v.c_string); break;
        case PRINT_LITERAL:  put(&builder, v.string);   break;
        }

      } else {
        put(&builder, { cursor, 1 });
        cursor += 1;
      }
    }

    if (num_arguments_passed < num_arguments_expected_from_format) {
      assert(0 && "number of arguments passed in print is less than number of arguments expected from format");
    } else if (num_arguments_passed > num_arguments_expected_from_format) {
      assert(0 && "number of arguments passed in print is greater than number of arguments expected from format");
    } else {
      printf("%.*s", (int) builder.size, builder.data);
    }
  }
}

void print(Print_Variable v) {
  print("%\n", v);
}

#if 0

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

