/*
Copyright (c) 2010 Frederico Gonçalves, Vasco Fernandes

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
 
#ifndef __MACROS_H__
#define __MACROS_H__

#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#define log(level, format, ...)					\
  do{								\
    fprintf(stderr, "%s  %s:%d: ", level, __FILE__, __LINE__);	\
    fprintf(stderr, format, ## __VA_ARGS__);			\
    fprintf(stderr, "\n");					\
  }while(0)
#else
#define log(level, str, ...)
#endif

#define I "\x1B[1;34;34m[INFO]\x1B[0;0;0m"
#define W "\x1B[1;33;33m[WARNNING]\x1B[0;0;0m"
#define E "\x1B[1;31;31m[ERROR]\x1B[0;0;0m"
#define F "\x1B[1;31;31m[FATAL ERROR]\x1B[0;0;0m"

#define alloc(type, how_many)				\
  (type *) __alloc(malloc(how_many * sizeof(type)));	

static inline void* __alloc(void* x){
  if(x)
    return x;
  log(F,"malloc failed.");				
  exit(1);
  return 0;
}

#endif
    
