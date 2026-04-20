#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t count;
  size_t capacity;
} Header;
#define ARR_INIT_CAP 256
#define arr_push(arr, x)                                                       \
  do {                                                                         \
    if (arr == NULL) {                                                         \
      Header *h = malloc(sizeof(*arr) * ARR_INIT_CAP + sizeof(Header));        \
      h->count = 0;                                                            \
      h->capacity = ARR_INIT_CAP;                                              \
      arr = (void *)(h + 1);                                                   \
    }                                                                          \
    Header *h = (Header *)(arr) - 1;                                           \
    if (h->count >= h->capacity) {                                             \
      h->capacity *= 2;                                                        \
      h = realloc(h, sizeof(Header) + h->capacity * sizeof(int));              \
    }                                                                          \
    (arr)[h->count++] = (x);                                                   \
  } while (0)
#define arr_pop(arr)                                                           \
  do {                                                                         \
    Header *h = (Header *)(arr) - 1;                                           \
    if (h->count == 0)                                                         \
      return;                                                                  \
    h->count--;                                                                \
    if (h->count <= h->capacity / 4) {                                         \
      h->capacity /= 2;                                                        \
      h = realloc(h, sizeof(Header) + h->capacity * sizeof(int));              \
      arr = (int *)h + 1;                                                      \
    }                                                                          \
  } while (0)
#define arr_len(arr) ((Header *)arr - 1)->count
#define arr_free(arr) free((Header *)arr - 1)
