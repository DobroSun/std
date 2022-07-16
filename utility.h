#pragma once

#define KB(x) 1024*(x)
#define MB(x) 1024*KB(x)
#define GB(x) 1024*MB(x)
#define TB(x) 1024*GB(x)



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

#define red(A)     ANSI_COLOR_RED     A ANSI_COLOR_RESET
#define green(A)   ANSI_COLOR_GREEN   A ANSI_COLOR_RESET
#define yellow(A)  ANSI_COLOR_YELLOW  A ANSI_COLOR_RESET
#define blue(A)    ANSI_COLOR_BLUE    A ANSI_COLOR_RESET
#define magenta(A) ANSI_COLOR_MAGENTA A ANSI_COLOR_RESET
#define cyan(A)    ANSI_COLOR_CYAN    A ANSI_COLOR_RESET

#define CONCATENATE_LINES(...) __VA_ARGS__


#define defer auto ANONYMOUS_NAME = Junk{} + [&]()
#define ANONYMOUS_NAME CONCATENATE(GAMMA, __LINE__)
#define CONCATENATE(A, B) CONCATENATE_IMPL(A, B)
#define CONCATENATE_IMPL(A, B) A##B

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
    my_assert(i < count, "Index out of range! Expected in range (0, %), but passed %", (int64) count-1, i);
    return data[i];
  }
};

// #define make_literal(x) (literal){ (x), sizeof(x)-1 }
inline literal XXX_visual_studio_sucks(const char* x, size_t s) { return { x, s }; }
#define make_literal(x) XXX_visual_studio_sucks((x), sizeof(x)-1)

size_t len(const char* s) {
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

inline bool operator==(const char *s, literal l) { return l.count && strncmp(s, l.data, l.count); }
inline bool operator==(literal l, const char *s) { return l.count && strncmp(s, l.data, l.count); }
inline bool operator!=(const char *s, literal l) { return !(s == l); }
inline bool operator!=(literal l, const char *s) { return !(s == l); }

inline bool operator==(literal l1, literal l2) {
  if(l1.count == l2.count) {
    return strncmp(l1.data, l2.data, l1.count);
  } else {
    return false;
  }
}

inline bool operator!=(literal l1, literal l2) { return !(l1 == l2); }

literal concat(literal s1, literal s2, literal* s3 = NULL, literal* s4 = NULL) {
  int l1 = s1.count;
  int l2 = s2.count;
  int l3 = (s3) ? s3->count : 0;
  int l4 = (s4) ? s4->count : 0;

  size_t allocated = l1 + l2 + l3 + l4 + 1;
  char* r = (char*) alloc(temp_allocator(), allocated);
  r[allocated] = '\0';

  memcpy(r,      s1.data, l1);
  memcpy(r + l1, s2.data, l2);
  if (s3) memcpy(r + l1 + l2,      s3->data, l3);
  if (s4) memcpy(r + l1 + l2 + l3, s4->data, l4);

  return { r, allocated-1 };
}

literal strip_file_extension(literal filename) {
  size_t i;
  for (i = filename.count-1; i > 0; i--) {
    if (filename[i] == '.') {
      break;
    }
  }

  literal result;
  result.data  = filename.data;
  result.count = i == 0 ? filename.count : i;
  return result;
}


literal strip_front(literal string) {
  while (string.count) {
    char c = string.data[0];
    if (c == ' ' || c == '\t' || c == '\v' || c == '\b' || c == '\r' || c == '\n') {
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

// @Incomplete: null-terminator???
inline char *dynamic_string_from_literal(literal l) {
  char *r = (char *)alloc(l.count+1);
  memcpy(r, l.data, l.count);
  return r;
}

bool is_digit(char c) {
  switch (c) {
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    return true;

  default:
    return false;
  }
}

// @Incomplete: only base 10 for now.
int string_to_integer(const char* string, char** end = NULL) {
  if (!string) return 0;

  bool is_negative = *string == '-'; // @Note: I'm always negative.
  if  (is_negative) string += 1;

  int result = 0;
  while (is_digit(*string)) {
    int n = *string - '0';

    // '123' -> (0 * 10 + 1) == 1 -> (1 * 10 + 2) == 12 -> (12 * 10 + 3) == 123 -> ... 
    result = result * 10 + n;

    string += 1;
  }

  if (end) *end = (char*) string;
  return (is_negative) ? -result : result;
}

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


typedef array<char> String_Builder; // @Incomplete: do the real thing, Bucket_Array or smth.

static int32 indentation_level = 0;

void ensure_space(String_Builder* builder, int space) {
  array_reserve(builder, builder->size + space);
}

void put(String_Builder* builder, literal s) {
  array_add(builder, (char*) s.data, s.count);
}

void put(String_Builder* builder, const char* s) {
  put(builder, {s, (size_t) len(s)}); // @Speed: no need to do len(...), we always use raw string literals, so we can make a template here that will know about its size.
}

void put(String_Builder* builder, int64 a, int base) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = integer_to_string(a, base, builder->data + builder->size);

  my_assert(count < 32, "");
  builder->size += count;
}

void put(String_Builder* builder, uint64 a, int base) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = uinteger_to_string(a, base, builder->data + builder->size);

  my_assert(count < 32, "");
  builder->size += count;
}

void put(String_Builder* builder, float64 a, int number_of_digits_after_decimal_point) {
  ensure_space(builder, 32); // @Note: won't support larger numbers.

  int count = float_to_string(a, number_of_digits_after_decimal_point, builder->data + builder->size);

  my_assert(count < 32, "");
  builder->size += count;
}


void put_spaces(String_Builder* builder) {
  char* spaces = (char*) alloca(indentation_level);
  memset(spaces, ' ', indentation_level);

  literal l = { spaces, (size_t)indentation_level };
  put(builder, l);
}

template<class... Args>
literal sprint(const char* format, Args... args);

void print_string(literal string);

template<class ...Args>
void __my_assert(Source_Location loc, bool expression, const char* message, Args... args) {
  if (!expression) {
    // 
    // @MemoryLeak: sprint(...)
    // 
    // @Incomplete: ? we can't use our print & tprints in there, because of how temporary_allocations work, we can't reallocate 2 or more arrays at once, just one at a time.
    // but when we call print from inside another print, we do exactly that and crash.
    // 
    const char* format = "\r" red("Assertion failed") ": %0%1file %, function %, line %\n";
    literal     string = sprint(format, sprint(message, args...), len(message) ? ", " : "", loc.file, loc.function, loc.line);
    print_string(string);

    __debugbreak(); // @Incomplete: @MSVCOnly: well, it is just `cc` instruction, so why can't we do that in other compilers? 
    ExitProcess(1); // @Incomplete: is `1` a real error code? look this up in documentation.
  }
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
  PRINT_VOID_POINTER,
  PRINT_C_STRING,
  PRINT_LITERAL,
};

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

    void* pointer;

    const char* c_string;
    literal     string;
  };

  Print_Variable() {
    type = PRINT_NONE;
  }

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

  Print_Variable(void* v) {
    type = PRINT_VOID_POINTER;
    pointer = v;
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

literal print_to_buffer(Allocator allocator, const char* format) {
  if (!format) return {};

  int num_arguments_passed = 0;
  int num_arguments_expected_from_format = 0;

  const char* cursor = format;
  while (1) {
    literal format_percent = make_literal("%");
    literal double_percent = make_literal("%%");

    if (*cursor == '\0') { break; }

    if (cursor == double_percent) {
      cursor += double_percent.count;

    } else if (cursor == format_percent) {
      num_arguments_expected_from_format += 1;
      cursor += format_percent.count;

      if (is_digit(*cursor)) {
        char* end;
        int number = string_to_integer(cursor, &end);
        my_assert(number < num_arguments_passed, "print_to_buffer expected index in range (0, %), but got %", num_arguments_passed-1, number);

        cursor = end;
      }
    } else {
      cursor += 1;
    }
  }

  my_assert(num_arguments_passed == num_arguments_expected_from_format, "print_to_buffer expected % number of arguments from format, but % arguments was passed", num_arguments_expected_from_format, num_arguments_passed);
  return { format, (size_t) len(format) };
}

template<class... Args>
literal print_to_buffer(Allocator allocator, const char* format, Args... args) {
  if (!format) return {};

  Print_Variable variables[] = { Print_Variable(args)... };

  int num_arguments_passed = static_array_size(variables);
  int num_arguments_expected_from_format = 0;

  // 
  // @Incomplete: no need to use String_Builder here, just use plain array<char>.
  // 
  String_Builder builder;
  builder.allocator = allocator;

  int argument       = 0;
  const char* cursor = format;
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

      int index;
      if (is_digit(*cursor)) {
        char* end;
        int number = string_to_integer(cursor, &end);
        my_assert(number < num_arguments_passed, "print_to_buffer expected index in range (0, %), but got %", num_arguments_passed-1, number);

        index    = number;
        argument = number + 1;
        cursor   = end;

      } else {
        if (argument >= num_arguments_passed) { break; } // @Incomplete: we just break in there, but what if we did %200, and then got in there with 201 that doesn't exist? We will report some stupid error message confusing everyone around.

        index     = argument;
        argument += 1;
      }

      Print_Variable v = variables[index];

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

      case PRINT_VOID_POINTER: put(&builder, make_literal("0x")); put(&builder, (uint64) v.pointer, 16); break;
      case PRINT_C_STRING:     put(&builder, v.c_string); break;
      case PRINT_LITERAL:      put(&builder, v.string);   break;
      }

    } else {
      put(&builder, { cursor, 1 });
      cursor += 1;
    }
  }

  my_assert(num_arguments_passed == num_arguments_expected_from_format, "print_to_buffer expected % number of arguments from format, but % arguments was passed", num_arguments_expected_from_format, num_arguments_passed);
  return { builder.data, builder.size };
}

template<class... Args>
literal sprint(const char* format, Args... args) {
  return print_to_buffer(get_global_allocator(), format, args...);
}

template<class... Args>
literal tprint(const char* format, Args... args) {
  return print_to_buffer(temp_allocator(), format, args...);
}

template<class... Args>
void print(const char* format, Args... args) {
  // 
  // @Incomplete: use Temporary_Storage::get_water_mark() and ::set_water_mark(); because print is going to be used a LOT!
  // 
  print_string(tprint(format, args...));
}

void print(Print_Variable v) {
  print("%\n", v);
}

void print_string(literal string) {
  if (string.data && string.count) {
    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), string.data, string.count, &written, NULL);
  }
}

struct Timer {
  std::chrono::steady_clock::time_point start;
  std::chrono::steady_clock::time_point end;

  Timer()  { start = std::chrono::steady_clock::now(); }
  ~Timer() { 
    end = std::chrono::steady_clock::now(); 
  
    double delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    literal print_number_in_seconds(double);
    print(print_number_in_seconds(delta));
  }
};

literal print_number_in_seconds(double n) {
  if(n < 1000.) {
    return tprint("% ns", n);
  } else if(n >= 1000. && n < 1000000.) {
    return tprint("% us", n/1000.);
  } else {
    return tprint("% ms", n/1000000.);
  }
}

// 
// @Incomplete: handle this through Print_Formatted_Integer, just have a tag? to specify that we want to print this in bytes, meters, ...
// 
literal print_number_in_bytes(size_t n) {
  static const size_t KB = 1024;
  static const size_t MB = KB * 1024;
  static const size_t GB = MB * 1024;
  static const size_t TB = GB * 1024;

  if(n > TB) {
    return tprint("% terabytes", formatted_float(n/(double)TB, 2));
  } else if(n > GB) {
    return tprint("% gigabytes", formatted_float(n/(double)GB, 2));
  } else if(n > MB) {
    return tprint("% megabytes", formatted_float(n/(double)MB, 2));
  } else if(n > KB) {
    return tprint("% kilobytes", formatted_float(n/(double)KB, 2));
  } else {
    return tprint("% bytes", n);
  }
}


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


void report_all_memory_leaks() {
  auto error_reports = allocation_storage.error_reports;

  size_t total = 0;
  for(Allocation_Chunk it : error_reports) {
    if(it.allocated) {
      // 
      // @Incomplete: line up all strings on ':' symbols.
      // 
      print("%: %: % <---- was allocated %, but never freed!\n", it.loc.file, it.loc.line, it.loc.function, print_number_in_bytes(it.allocated)); // @Incomplete: forward declare print and use it in here.
      total += it.allocated;
    }
  }

  if(total == 0) {
    print("No memory leaks!\n");
  } else {
    print("Total memory leaked: %\n", print_number_in_bytes(total));
  }
}

literal print_source_location(Source_Location loc) {
  return tprint("%: %: %", loc.file, loc.line, loc.function);
}
