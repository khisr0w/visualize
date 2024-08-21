/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  8/13/2024 10:47:33 PM                                         |
    |    Last Modified:                                                                |
    |                                                                                  |
    +======================================| Copyright © Sayed Abid Hashimi |==========+  */

#define array_length(array) (sizeof(array)/sizeof(array[0]))

/* NOTE(Abid): Byte Macros */
#define kilobyte(value) ((value)*1024ll)
#define megabyte(value) (kilobyte(value)*1024ll)
#define gigabyte(value) (megabyte(value)*1024ll)
#define terabyte(value) (gigabyte(value)*1024ll)

/* NOTE(Abid): typedef and static define for ease of use */
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t i64;
typedef int32_t i32;
typedef int8_t i8;
typedef double f64;
typedef float f32;
typedef uintptr_t uintptr;
typedef size_t usize;
#define true 1
#define false 0
#define internal static
#define local_persist static
#define global_var static

#define assert(Expr, ErrorStr, ...) \
    if((Expr)) { } \
    else { \
        printf("ASSERTION ERROR (%s:%d): " ErrorStr "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__); \
        *(i32 *)0 = 0; \
    }

typedef struct {
    i32 x;
    i32 y;
} vector2i;

typedef struct {
    Vector2 a;
    Vector2 b;
} range;

#if 0
internal inline f32
abs(f32 value) { return (value < 0) ? -value : value; }
internal inline f32
max(f32 first, f32 second) { return (first < second) ? second : first; }
internal inline i32
round(f32 value) { return (i32)(value + 0.5f); }
#endif
