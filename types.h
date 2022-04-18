#pragma once

#ifdef _MSC_VER
  typedef __int8           int8;
  typedef __int16          int16;
  typedef __int32          int32;
  typedef __int64          int64;
  typedef unsigned __int32 uint32;
  typedef unsigned __int64 uint64;
  typedef unsigned __int8  uint8;
  typedef unsigned __int16 uint16;
#else
  #include <cstdint>
  typedef int8_t   int8;
  typedef int16_t  int16;
  typedef int32_t  int32;
  typedef int64_t  int64;
  typedef uint8_t  uint8;
  typedef uint16_t uint16;
  typedef uint32_t uint32;
  typedef uint64_t uint64;
  typedef float    float32;
  typedef double   float64;
#endif


typedef int8   s8;
typedef int16  s16;
typedef int32  s32;
typedef int64  s64;
typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;
typedef float  f32;
typedef double f64;

static_assert(sizeof(s8)  == sizeof(u8), "");
static_assert(sizeof(s16) == sizeof(u16), "");
static_assert(sizeof(s32) == sizeof(u32), "");
static_assert(sizeof(s64) == sizeof(u64), "");
static_assert(sizeof(s8)  == 1, "");
static_assert(sizeof(s16) == 2, "");
static_assert(sizeof(s32) == 4, "");
static_assert(sizeof(s64) == 8, "");
static_assert(sizeof(f32) == 4, "");
static_assert(sizeof(f64) == 8, "");

