#pragma once
#if defined(__UINT8_TYPE__) && defined(__UINT16_TYPE__) && defined(__UINT32_TYPE__) && defined(__UINT64_TYPE__)
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
#else
#error "compiler does not provide __UINTxx_TYPE__"
#endif

#if defined(__INT8_TYPE__) && defined(__INT16_TYPE__) && defined(__INT32_TYPE__) && defined(__INT64_TYPE__)
typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
#else
#error "compiler does not provide __INTxx_TYPE__"
#endif

typedef uint32_t size_t;
