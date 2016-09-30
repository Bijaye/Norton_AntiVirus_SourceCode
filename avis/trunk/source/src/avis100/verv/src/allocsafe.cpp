// halts with error message if allocation fails

#include <stdlib.h>
#include <stdio.h>

#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

void* Malloc (size_t size, const char* FILE, unsigned LINE) {
  void* p = malloc(size);
  if (!p) {
    printf("\nERROR: unable to allocate %lu bytes at line %u in %s\n", size, LINE, FILE);
    exit (8);
  }
  return p;
}

void* Calloc (size_t num, size_t size, const char* FILE, unsigned LINE) {
  void* p = calloc(num, size);
  if (!p) {
    printf("\nERROR: unable to allocate %lu items of %lu bytes each at line %u in %s\n", num, size, LINE, FILE);
    exit (8);
  }
  return p;
}

#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif
