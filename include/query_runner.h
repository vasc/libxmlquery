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
 
#ifndef __QUERY_RUNNER_H__
#define __QUERY_RUNNER_H__

#include "stack.h"
#include "node.h"

typedef struct custom_filter_s{
    char* name;
    struct{
        int (*simple)(dom_node* node);
        int (*args)(dom_node* node, list* args);
    } function;
} custom_filter;

extern list* query(const char*, dom_node*);
extern void register_simple_custom_filter(const char* name, int (*filter)(dom_node* node));
extern void register_custom_filter(const char* name, int (*filter)(dom_node* node, list* args));
extern void destroy_custom_filters();
#endif

