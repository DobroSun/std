

// @CleanUp: clean up all of this stuff.

#define OPERATOR_INDEX_CONST_VECTOR() \
	const double& operator[](size_t index) const { assert(index < N); return data[index]; }

#define OPERATOR_INDEX_VECTOR() \
	      double& operator[](size_t index)       { assert(index < N); return data[index]; } \
        OPERATOR_INDEX_CONST_VECTOR();

#define OPERATOR_INDEX_MATRIX() \
	Vector_View operator[](size_t index) { \
    assert(index < N); \
    Vector_View r; \
    r.data = (double*) data[index]; \
    r.size = M; \
    return r; \
  } \
	const Vector_View operator[](size_t index) const { \
    assert(index < N); \
    Vector_View r; \
    r.data = (double*) data[index]; \
    r.size = M; \
    return r; \
  }

#define OPERATOR_INDEX_MATRIX_VIEW() \
	Vector_View operator[](size_t index) { \
    assert(index < N); \
    Vector_View r; \
    r.data = (double*) (((int8*)data) + sizeof(double) * M * index); \
    r.size = M; \
    return r; \
  } \
	const Vector_View operator[](size_t index) const { \
    assert(index < N); \
    Vector_View r; \
    r.data = (double*) (((int8*)data) + sizeof(double) * M * index); \
    r.size = M; \
    return r; \
  }

template<class T>
struct v2_base {
  typedef T base_type;

  union {
    struct {
      T x, y;
    };
    struct {
      T a, b;
    };
    T    E[2];
    T data[2];
  };

  static const uint32_t N = 2;
  static const uint32_t num_elements = 2;
  static const uint32_t size = 2;
  
  v2_base<T> operator*=(T a) {
    x *= a;
    y *= a;
    return {x, y};
  }

  v2_base<T> operator/=(T a) {
    x /= a;
    y /= a;
    return {x, y};
  }

  v2_base<T> operator+=(v2_base<T> v) {
    x += v.x;
    y += v.y;
    return {x, y};
  }

  v2_base<T> operator-=(v2_base<T> v) {
    x -= v.x;
    y -= v.y;
    return {x, y};
  }

  OPERATOR_INDEX_VECTOR();
};

template<class T>
struct v3_base {
  typedef T base_type;

  union {
    struct {
      T x, y, z;
    };

    struct {
      T r, g, b;
    };

    struct {
      v2_base<T> xy;
      T          zz;
    };

    struct {
      T          xx;
      v2_base<T> yz;
    };

    T    E[3];
    T data[3];
  };

  static const uint32_t N = 3;
  static const uint32_t num_elements = 3;
  static const uint32_t size = 3;


  v3_base<T> operator*=(T a) {
    x *= a;
    y *= a;
    z *= a;
    return {x, y, z};
  }

  v3_base<T> operator/=(T a) {
    x /= a;
    y /= a;
    z /= a;
    return {x, y, z};
  }

  v3_base<T> operator+=(v3_base<T> v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return {x, y, z};
  }

  v3_base<T> operator-=(v3_base<T> v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return {x, y, z};
  }

  OPERATOR_INDEX_VECTOR();
};

template<class T>
struct v4_base {
  typedef T base_type;

  union {
    struct {
      T x, y, z, w;
    };

    struct {
      T r, g, b, a;
    };

    struct {
      v2_base<T> xy;
      v2_base<T> zw;
    };

    struct {
      T xxx_placeholder;
      v2_base<T> yz;
      T zzz_placeholder;
    };

    struct {
      v3_base<T> xyz;
      T www_placeholder;
    };

    struct {
      T xxx_placeholder2;
      v3_base<T> yzw;
    };

    T    E[4];
    T data[4];
  };

  static const uint32_t N = 4;
  static const uint32_t num_elements = 4;
  static const uint32_t size = 4;


  v4_base<T> operator*=(T a) {
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return {x, y, z, w};
  }

  v4_base<T> operator/=(T a) {
    x /= a;
    y /= a;
    z /= a;
    w /= a;
    return {x, y, z, w};
  }

  v4_base<T> operator+=(v4_base<T> v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return {x, y, z, w};
  }

  v4_base<T> operator-=(v4_base<T> v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return {x, y, z, w};
  }

  OPERATOR_INDEX_VECTOR();
};

typedef v2_base<s8>  v2_s8;
typedef v2_base<s16> v2_s16;
typedef v2_base<s32> v2_s32;
typedef v2_base<s64> v2_s64;
typedef v2_base<s8>  v2_i8;
typedef v2_base<s16> v2_i16;
typedef v2_base<s32> v2_i32;
typedef v2_base<s64> v2_i64;

typedef v2_base<u8>  v2_u8;
typedef v2_base<u16> v2_u16;
typedef v2_base<u32> v2_u32;
typedef v2_base<u64> v2_u64;

typedef v2_base<f32> v2_f32;
typedef v2_base<f64> v2_f64;

typedef v2_f32 v2;
typedef v2_s32 v2s;
typedef v2_s32 v2i;
typedef v2_u32 v2u;
typedef v2_f32 v2f;
typedef v2_f64 v2d;

typedef v3_base<s8>  v3_s8;
typedef v3_base<s16> v3_s16;
typedef v3_base<s32> v3_s32;
typedef v3_base<s64> v3_s64;
typedef v3_base<s8>  v3_i8;
typedef v3_base<s16> v3_i16;
typedef v3_base<s32> v3_i32;
typedef v3_base<s64> v3_i64;

typedef v3_base<u8>  v3_u8;
typedef v3_base<u16> v3_u16;
typedef v3_base<u32> v3_u32;
typedef v3_base<u64> v3_u64;

typedef v3_base<f32> v3_f32;
typedef v3_base<f64> v3_f64;

typedef v3_f32 v3;
typedef v3_s32 v3s;
typedef v3_s32 v3i;
typedef v3_u32 v3u;
typedef v3_f32 v3f;
typedef v3_f64 v3d;

typedef v4_base<s8>  v4_s8;
typedef v4_base<s16> v4_s16;
typedef v4_base<s32> v4_s32;
typedef v4_base<s64> v4_s64;
typedef v4_base<s8>  v4_i8;
typedef v4_base<s16> v4_i16;
typedef v4_base<s32> v4_i32;
typedef v4_base<s64> v4_i64;

typedef v4_base<u8>  v4_u8;
typedef v4_base<u16> v4_u16;
typedef v4_base<u32> v4_u32;
typedef v4_base<u64> v4_u64;

typedef v4_base<f32> v4_f32;
typedef v4_base<f64> v4_f64;

typedef v4_f32 v4;
typedef v4_s32 v4s;
typedef v4_s32 v4i;
typedef v4_u32 v4u;
typedef v4_f32 v4f;
typedef v4_f64 v4d;


// **********************
static const int S8  = 10;
static const int U8  = 11;
static const int S16 = 12;
static const int U16 = 13;
static const int S32 = 14;
static const int U32 = 15;
static const int S64 = 16;
static const int U64 = 17;
static const int F32 = 18;
static const int F64 = 19;

template<class T> struct Meta_Type_Info { static const int precedence = 0; static const int size_of = 0; };
template<> struct Meta_Type_Info<s8>  { static const int precedence = S8; static const int size_of = sizeof(s8); };
template<> struct Meta_Type_Info<u8>  { static const int precedence = U8; static const int size_of = sizeof(u8); };
template<> struct Meta_Type_Info<s16> { static const int precedence = S16; static const int size_of = sizeof(s16); };
template<> struct Meta_Type_Info<u16> { static const int precedence = U16; static const int size_of = sizeof(u16); };
template<> struct Meta_Type_Info<s32> { static const int precedence = S32; static const int size_of = sizeof(s32); };
template<> struct Meta_Type_Info<u32> { static const int precedence = U32; static const int size_of = sizeof(u32); };
template<> struct Meta_Type_Info<s64> { static const int precedence = S64; static const int size_of = sizeof(s64); };
template<> struct Meta_Type_Info<u64> { static const int precedence = U64; static const int size_of = sizeof(u64); };
template<> struct Meta_Type_Info<f32> { static const int precedence = F32; static const int size_of = sizeof(f32); };
template<> struct Meta_Type_Info<f64> { static const int precedence = F64; static const int size_of = sizeof(f64); };

template<int Precedence> struct Meta_Info_From_Precedence {};
template<> struct Meta_Info_From_Precedence<S8> { typedef Meta_Type_Info<s8> info; };
template<> struct Meta_Info_From_Precedence<U8> { typedef Meta_Type_Info<u8> info; };
template<> struct Meta_Info_From_Precedence<S16> { typedef Meta_Type_Info<s16> info; };
template<> struct Meta_Info_From_Precedence<U16> { typedef Meta_Type_Info<u16> info; };
template<> struct Meta_Info_From_Precedence<S32> { typedef Meta_Type_Info<s32> info; };
template<> struct Meta_Info_From_Precedence<U32> { typedef Meta_Type_Info<u32> info; };
template<> struct Meta_Info_From_Precedence<S64> { typedef Meta_Type_Info<s64> info; };
template<> struct Meta_Info_From_Precedence<U64> { typedef Meta_Type_Info<u64> info; };
template<> struct Meta_Info_From_Precedence<F32> { typedef Meta_Type_Info<f32> info; };
template<> struct Meta_Info_From_Precedence<F64> { typedef Meta_Type_Info<f64> info; };

template<bool C, class T, class U> struct Meta_Conditional        {};
template<class T, class U> struct Meta_Conditional<true,  T, U> { typedef T value; };
template<class T, class U> struct Meta_Conditional<false, T, U> { typedef U value; };

template<int N, int M> struct Meta_GT { static const bool value = N > M; };


template<class T>
struct float_or_more {
  typedef typename Meta_Conditional<
                     Meta_GT< Meta_Type_Info<T>::precedence, Meta_Type_Info<float>::precedence >::value,
                     T,
                     float
                   > ::value type;
};

template<class T>
using float_or_more_t = typename float_or_more<T>::type;



template<class T> auto operator*(v2_base<T> v, T a) -> v2_base<T> { return v *= a; }
template<class T> auto operator*(T a, v2_base<T> v) -> v2_base<T> { return v *= a; }
template<class T> auto operator/(v2_base<T> v, T a) -> v2_base<T> { return v /= a; }
template<class T> auto operator*(v3_base<T> v, T a) -> v3_base<T> { return v *= a; }
template<class T> auto operator*(T a, v3_base<T> v) -> v3_base<T> { return v *= a; }
template<class T> auto operator/(v3_base<T> v, T a) -> v3_base<T> { return v /= a; }
template<class T> auto operator*(v4_base<T> v, T a) -> v4_base<T> { return v *= a; }
template<class T> auto operator*(T a, v4_base<T> v) -> v4_base<T> { return v *= a; }
template<class T> auto operator/(v4_base<T> v, T a) -> v4_base<T> { return v /= a; }

template<class T> auto operator+(v2_base<T> v1, v2_base<T> v2) -> v2_base<T> { return v1 += v2; }
template<class T> auto operator+(v3_base<T> v1, v3_base<T> v2) -> v3_base<T> { return v1 += v2; }
template<class T> auto operator+(v4_base<T> v1, v4_base<T> v2) -> v4_base<T> { return v1 += v2; }
template<class T> auto operator-(v2_base<T> v1, v2_base<T> v2) -> v2_base<T> { return v1 -= v2; }
template<class T> auto operator-(v3_base<T> v1, v3_base<T> v2) -> v3_base<T> { return v1 -= v2; }
template<class T> auto operator-(v4_base<T> v1, v4_base<T> v2) -> v4_base<T> { return v1 -= v2; }

template<class T> auto length_sq(v2_base<T> v) -> float_or_more_t<T> { return v.x*v.x + v.y*v.y; }
template<class T> auto length_sq(v3_base<T> v) -> float_or_more_t<T> { return v.x*v.x + v.y*v.y + v.z*v.z; }
template<class T> auto length_sq(v4_base<T> v) -> float_or_more_t<T> { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }
template<class T> auto length   (v2_base<T> v) -> float_or_more_t<T> { return sqrt(length_sq(v)); }
template<class T> auto length   (v3_base<T> v) -> float_or_more_t<T> { return sqrt(length_sq(v)); }
template<class T> auto length   (v4_base<T> v) -> float_or_more_t<T> { return sqrt(length_sq(v)); }

template<class T> auto dot(v2_base<T> v1, v2_base<T> v2) -> T { return v1.x*v2.x + v1.y*v2.y; }
template<class T> auto dot(v3_base<T> v1, v3_base<T> v2) -> T { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
template<class T> auto dot(v4_base<T> v1, v4_base<T> v2) -> T { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w; }

template<class U, class T> v2_base<typename U::base_type> cast(v2_base<T> v) { return { (typename U::base_type)(v.x), (typename U::base_type)(v.y) }; }
template<class U, class T> v3_base<typename U::base_type> cast(v3_base<T> v) { return { (typename U::base_type)(v.x), (typename U::base_type)(v.y), (typename U::base_type)(v.z) }; }
template<class U, class T> v4_base<typename U::base_type> cast(v4_base<T> v) { return { (typename U::base_type)(v.x), (typename U::base_type)(v.y), (typename U::base_type)(v.z), (typename U::base_type)(v.w) }; }

template<class T> T cross(v3_base<T> a, v3_base<T> b) {
  v3_base<T> c;
  c.x = a.y*b.z - a.z*b.y;
  c.y = a.z*b.x - a.x*b.z;
  c.z = a.x*b.y - a.y*b.x;
  return c;
}

#define ANONYMOUS_NAME_(x) CONCAT(ANONYMOUS_NAME, x)
#define CONCAT(A, B) CONCAT_IMPL(A, B)
#define CONCAT_IMPL(A, B) A##B

#define for_vector(v, i) for (uint32_t i = 0; i < (v).N; i++)
#define for_rows(m, i) for(uint32_t i = 0; i < (m).rows; i++)
#define for_cols(m, j) for(uint32_t j = 0; j < (m).cols; j++)
#define for_matrix(m, i, j) for_rows(m, i) for_cols(m, j)



template<size_t S>
struct Static_Vector {
  union {
    double E[S];
    double data[S];
  };
  static const uint32_t N = S;
  static const uint32_t num_elements = S;
  static const uint32_t size = S;

  OPERATOR_INDEX_VECTOR();
};


struct Vector {
  union {
    double* E;
    double* data;
  };
  union {
    uint32_t N;
    uint32_t num_elements;
    uint32_t size;
  };

  Allocator allocator = global_allocator;

  OPERATOR_INDEX_VECTOR();
};

struct Vector_View {
  union {
    double* E;
    double* data;
  };
  union {
    uint32_t N;
    uint32_t num_elements;
    uint32_t size;
  };

  
  Vector_View() = default;
  Vector_View(double* vdata, uint32_t vN) { data = vdata; N = vN; }
  Vector_View(v2_base<double> v)          { data = v.data; N = v.N; }
  Vector_View(v3_base<double> v)          { data = v.data; N = v.N; }
  Vector_View(v4_base<double> v)          { data = v.data; N = v.N; }

  template<size_t N>
  Vector_View(Static_Vector<N> v) { data = v.data; N = v.N; }
  Vector_View(Vector v)           { data = v.data; N = v.N; }


  Vector_View operator+=(Vector_View b) {
    assert(size == b.size);
    for_vector(*this, i) {
      data[i] += b[i];
    }
    return *this;
  }

  Vector_View operator-=(Vector_View b) {
    assert(size == b.size);
    for_vector(*this, i) {
      data[i] -= b[i];
    }
    return *this;
  }

  Vector_View operator*=(double b) {
    for_vector(*this, i) {
      data[i] *= b;
    }
    return *this;
  }

  Vector_View operator/=(double b) {
    for_vector(*this, i) {
      data[i] /= b;
    }
    return *this;
  }

  OPERATOR_INDEX_VECTOR();
};

#define make_temp_vector(s) Vector_View((double*) alloca(sizeof(double) * (s)), (s))

struct Matrix {
  union {
    double** data;
    double** m;
  };
  union {
    struct { uint32_t size[2]; };
    struct { uint32_t N, M; };
    struct { uint32_t num_rows, num_columns; };
    struct { uint32_t rows, cols; };
  };

  Allocator allocator = global_allocator;

  OPERATOR_INDEX_MATRIX();
};

struct Matrix2x2 {
  double data[2][2];
  uint32_t N, M;

  OPERATOR_INDEX_MATRIX();
};

struct Matrix_View {
  double* data;
  uint32_t N, M;

  Matrix_View(Matrix2x2 m) { data = (double*) m.data; N = m.N; M = m.M; }

  OPERATOR_INDEX_MATRIX_VIEW();
};

void vector_print(Vector_View a) {
  printf("[");
  for_vector(a, i) {
    if(a.size-1 == i) {
      printf("%g", a[i]);
    } else {
      printf("%g, ", a[i]);
    }
  }
  printf("]");
}

void matrix_print(Matrix a) {
  printf("[");
  for_rows(a, i) {
    vector_print(a[i]);

    if(a.rows-1 != i) {
      printf(",\n");
    }
  }
  printf("]");
}

void next_permutation(Vector_View v) {
	uint32_t n = v.N;

	int64_t j = n - 2;
	while (j != -1 && v[j] >= v[j + 1]) j--;
	assert(j != -1 && "There is no more permutatoins!");

	int64_t k = n - 1;
	while (v[j] >= v[k]) {
    k--;
  }
	swap(&v[j], &v[k], sizeof(v[0]));

	int64_t l = j + 1, r = n - 1;
	while (l < r) {
		swap(&v[l++], &v[r--], sizeof(v[0]));
  }

}

const int8_t PERMUTATION_IS_EVEN = 1;
const int8_t PERMUTATION_IS_ODD = -1;
int8_t permutation_sign(Vector_View v) {
	uint32_t n_inversions = 0;
	for (uint32_t i = 0; i < v.N-1; i++) {
		for (uint32_t j = i+1; j < v.N; j++) {
			if (v[i] > v[j]) { 
				n_inversions++;
			}
		}
	}
	return n_inversions % 2 == 0 ? PERMUTATION_IS_EVEN : PERMUTATION_IS_ODD;
}

size_t factorial(size_t x) {
	size_t r = 1;
	for (size_t i = 1; i <= x; i++) { r *= i; }
	return r;
}

Vector make_vector(uint32_t N, Allocator allocator = global_allocator) {
	Vector r;
  r.data = (double*) alloc(allocator, sizeof(double) * N);
  r.size = N;
  r.allocator = allocator;
	return r;
}

void vector_free(Vector r) {
  dealloc(r.allocator, r.data);
}

Vector_View vector_copy(Vector_View to, const Vector_View from) {
  assert(to.size == from.size);
  memcpy(to.data, from.data, sizeof(double) * from.size);
  return to;
}

Vector_View vector_zero_inplace(Vector_View r) {
  memset(r.data, 0, sizeof(double) * r.size);
  return r;
}

double dot(Vector_View a, Vector_View b) {
	assert(a.N == b.N);
	double r = 0;
	for_vector(a, i) { r += a[i] * b[i]; }
	return r;
}

double norm(Vector_View a) { return sqrt(dot(a, a)); }


Vector matrix_apply(Matrix a, Vector_View r) {
  Vector x = make_vector(r.size);
  for_vector(x, i) {
    x[i] = dot(a[i], r);
  }
  return x;
}

bool vector_match(Vector_View a, Vector_View b, const double epsilon = 1e-10) {
  if(a.size != b.size) {
    return false;
  }
  for_vector(a, i) {
    if(fabs(a[i] - b[i]) > epsilon) return false;
  }
  return true;
}

Matrix make_matrix(uint32_t N, uint32_t M, Allocator allocator = global_allocator) {
	Matrix r;
  r.data = (double**) alloc(allocator, sizeof(double*) * N);
  r.rows = N;
  r.cols = M;
  r.allocator = allocator;

  for_rows(r, i) {
		r.data[i] = (double*) alloc(allocator, sizeof(double) * M);
	}
	return r;
}

void matrix_free(Matrix a) {
  for_rows(a, i) {
    dealloc(a.allocator, a.data[i]);
  }
  dealloc(a.allocator, a.data);
}

Matrix make_zeros(size_t N, size_t M) {
	Matrix r = make_matrix(N, M);
	for_rows(r, i) {
     vector_zero_inplace(r[i]);
  }
	return r;
}


Matrix make_identity(size_t N) {
	Matrix r = make_zeros(N, N);
	for_rows(r, i) {
		r[i][i] = 1.0;
	}
	return r;
}

Matrix matrix_copy(Matrix to, const Matrix from) {
  assert(to.rows == from.rows && to.cols == from.cols);
  for_rows(from, i) {
    vector_copy(to[i], from[i]);
  }
  return to;
}

Matrix matrix_copy(Matrix a) {
  return matrix_copy(make_matrix(a.rows, a.cols), a);
}

// Operator boilerplate.
#define OPERATOR_VV(type) \
  Vector operator type(Vector_View a, Vector_View b) { \
    assert(a.size == b.size); \
    Vector x = make_vector(a.size); \
    for_vector(x, i) { \
      x[i] = a[i] type b[i]; \
    } \
    return x; \
  }

#define OPERATOR_VS(type) \
  Vector operator type(Vector_View a, double b) { \
    Vector x = make_vector(a.size); \
    for_vector(x, i) { \
      x[i] = a[i] type b; \
    } \
    return x; \
  } \
  Vector operator type(double b, Vector_View a) { \
    Vector x = make_vector(a.size); \
    for_vector(x, i) { \
      x[i] = a[i] type b; \
    } \
    return x; \
  }


#define OPERATOR_MM(type) \
  Matrix operator type(Matrix a, Matrix b) { \
    assert(a.rows == b.rows && a.cols == b.cols); \
    Matrix x = make_matrix(a.rows, a.cols); \
    for_matrix(x, i, j) { \
      x[i][j] = a[i][j] type b[i][j]; \
    } \
    return x; \
  }

#define OPERATOR_MS(type) \
  Matrix operator type(Matrix a, double b) { \
    Matrix x = make_matrix(a.rows, a.cols); \
    for_matrix(x, i, j) { \
      x[i][j] = a[i][j] type b; \
    } \
    return x; \
  } \
  Matrix operator type(double b, Matrix a) { \
    Matrix x = make_matrix(a.rows, a.cols); \
    for_matrix(x, i, j) { \
      x[i][j] = a[i][j] type b; \
    } \
    return x; \
  }

OPERATOR_VV(+);
OPERATOR_VV(-);
OPERATOR_VS(*);
OPERATOR_VS(/);
OPERATOR_MM(+);
OPERATOR_MM(-);
OPERATOR_MS(*);
OPERATOR_MS(/);
// 

/*
Matrix expand_matrix(Matrix a, size_t d) {
	Matrix r = make_matrix(a.rows, a.cols + d);
	return copy_matrix_at_least(a, r);
}*/

Matrix tensor_product(Vector_View a, Vector_View b) {
  Matrix m = make_matrix(a.N, b.N);
  for_matrix(m, i, j) {
    m[i][j] = a[i]*b[j];
  }
  return m;
}

bool is_square_matrix(Matrix a) {
  return a.rows == a.cols;
}


bool is_symmetric_matrix(Matrix a) {
  if(!is_square_matrix(a)) {
    return false;
  }
  for_matrix(a, i, j) {
    if(a[i][j] != a[j][i]) {
      return false;
    }
  }
  return true;
}


Matrix matrix_apply(Matrix a, Matrix b) {
  assert(a.cols == b.rows && is_square_matrix(a) && is_square_matrix(b) && "matrices are supposed to be square in order to return one of them as reference!");

  Matrix tmp = make_matrix(a.rows, b.cols);
	for_matrix(tmp, i, j) {
		double c = 0;
    for_cols(a, k) {
			c += a[i][k] * b[k][j];
    }
		tmp[i][j] = c;
	}
  return tmp;
}

Matrix transpose_inplace(Matrix a) {
  assert(is_square_matrix(a));
  for_rows(a, i) {
    for(size_t j = i+1; j < a.cols; j++) {
      double tmp = a[i][j];
      a[i][j] = a[j][i];
      a[j][i] = tmp;
    }
  }
  return a;
}

Matrix transpose(Matrix a) {
  if(is_square_matrix(a)) {
    return transpose_inplace(a);
  } else {
    Matrix x = make_matrix(a.cols, a.rows);
    for_matrix(x, i, j) {
      x[i][j] = a[j][i];
    }
    return x;
  }
}

double trace(Matrix a) {
	assert(a.rows == a.cols);
	double r = 0;
	for_rows(a, i) { r += a[i][i]; }
	return r;
}

double det(Matrix a) {
	assert(is_square_matrix(a) && "want a sqaure matrix to compute determinant");

	size_t max_permutations = factorial(a.rows);

  Vector_View indices = make_temp_vector(a.rows); // @Incomplete: 

	for_vector(indices, i) { indices[i] = i; }
	
	double result = 0;

	size_t k = 0;
	while (1) {
		double c = permutation_sign(indices);
		for_vector(indices, i) {
			c *= a[i][(uint32_t)indices[i]];
		}
		result += c;

		k++;
		if (k == max_permutations) break;
		next_permutation(indices);
	}
	return result;
}

double matrix_minor(Matrix a, size_t i, size_t j) {
	assert(is_square_matrix(a));
  Matrix subm;
  // temporary_matrix(subm, a.rows-1, a.rows-1); // @Incomplete: 
	for_matrix(subm, k, l) {
		size_t m = (k >= i) ? k+1 : k;
		size_t n = (l >= j) ? l+1 : l;
		subm[k][l] = a[m][n];
	}
	return det(subm);
}

Matrix matrix_adjugate(Matrix a) {
	assert(is_square_matrix(a));
  Matrix x = make_matrix(a.rows, a.cols);
	for_matrix(x, i, j) {
		int sign = (i+j) % 2 == 0 ? 1 : -1;
		x[i][j] = sign * matrix_minor(a, i, j);
	}
	return transpose_inplace(x);
}

Matrix matrix_multiply(Matrix a, double v) {
  Matrix x = make_matrix(a.rows, a.cols);
  for_matrix(a, i, j) {
    x[i][j] = a[i][j] * v;
  }
  return x;
}


Matrix matrix_inverse(Matrix a) {
	return 1/det(a) * matrix_adjugate(a);
}

bool matrix_match(Matrix a, Matrix b, const double epsilon = 1e-10) {
  if(a.rows != b.rows || a.cols != b.cols) {
    return false;
  }
  for_matrix(a, i, j) {
    if(abs(a[i][j] - b[i][j]) > epsilon) return false;
  }
  return true;
}

// 
// 	Gauss elimination method.
// 
static void find_and_swap(Matrix m, uint32_t k) {
  const uint32 n = m.rows;
  const uint32 l = m.cols;

	double max_ = m[k][k];
  uint32 pos  = k;

	for (uint32 i = k; i < n; i++) {
    const double    el = m[i][k];
    const bool new_max = fabs(max_) < fabs(el);

    max_ = new_max ? el : max_;
    pos  = new_max ? i  : pos ;
	}

	if (pos == k) {
		// matrix[k][k] is already maximal element for k-th row.
	} else {
    Vector_View tmp = m[k];
    m[pos] = m[k];
    m[k]   = tmp;
	}
}

Vector linear_solve(Matrix m, Vector_View f) {
  const uint32 n = m.rows;
  const uint32 l = m.cols;

  assert(n == f.size && "dimensions do not match!");
	assert(n <= l);
  assert(n && l && "matrix is expected to have some dimensions.");

  // forward pass.
  for_rows(m, k) {
    find_and_swap(m, k);

    const double diag = m[k][k];
    m[k] /= diag;
    f[k] /= diag;

		for (uint32_t i = k+1; i < n; i++) {
			const double el = m[i][k];
      m[i] -= m[k] * el;
      f[i] -= f[k] * el;
		}
  }

  Vector x = make_vector(l);
  vector_zero_inplace(x);

  // back substituition.
  for(int64 i = n-1; i >= 0; i--) {
    x[i] = f[i] - dot(x, m[i]);
  }

  return x;
}

double infinity_norm(Vector_View a) {
  double max = fabs(a[0]);
  for_vector(a, i) {
    double v = fabs(a[i]);
    max = max < v ? v : max;
  }
  return max;
}

double infinity_norm(Matrix a) {
  Vector_View rows_sums = make_temp_vector(a.rows);

  for_rows(a, i) {
    double sum = 0.0;
    for_cols(a, j) { sum += fabs(a[i][j]); }
    rows_sums[i] = sum;
  }
  return infinity_norm(rows_sums);
}

#if 0
Matrix make_householder_matrix(Vector u) {
  Matrix E  = make_identity(u.size);
  Matrix Ut = tensor_product(u, u) * (2.0 / dot_product(u, u));
  return E - Ut;
}
}
#endif
