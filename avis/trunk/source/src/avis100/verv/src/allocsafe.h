// halts with error message if allocation fails

#ifndef ALLOCSAFE_H
#define ALLOCSAFE_H
#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

#define malloc(s)   Malloc(s, __FILE__, __LINE__)
#define calloc(n,s) Calloc(n, s, __FILE__, __LINE__)

void* Malloc (size_t size, const char* FILE, unsigned LINE);
void* Calloc (size_t num, size_t size, const char* FILE, unsigned LINE);

#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif
#endif   // ALLOCSAFE_H
