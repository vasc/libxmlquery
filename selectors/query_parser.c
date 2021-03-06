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
 
#include <stdlib.h>
#include <string.h>
#include "../include/query_parser.h"
#include "../include/macros.h"
#include "../include/y.tab.h"
#include "../include/byte_buffer.h"

extern int yyparse();

selector* new_selector(match_value* id){
  selector* r = alloc(selector, 1);
  r->id = id;
  r->filters = NULL;
  r->attrs = NULL;
  return r;
}

match_value* new_match_value(const char* value, int op){
    match_value* r = alloc(match_value, 1);
    r->value = (value == NULL)? NULL : strdup(value);
    r->op = op;
    return r;
}

match_value* new_match_value_no_strdup(char* value, int op){
    match_value* r = alloc(match_value, 1);
    r->value = value;
    r->op = op;
    return r;
}

attr_selector* new_attr_value_selector(match_value* name, match_value* value){
  attr_selector* r = alloc(attr_selector, 1);
  r->name = name;
  r->value = value;
  return r;
}

filter_selector* new_filter(int filter){
  filter_selector* r = alloc(filter_selector, 1);
  r->op = filter;
  r->name = NULL;
  r->args = NULL;
  return r;
}

void destroy_selector(selector* s);

void destroy_filter_selector(filter_selector* fs){
  switch(fs->op){
  /*case NOT_FILTER:
    {
      int i;
      for(i = 0; i < fs->value.selector->count; i++)
	destroy_selector((selector*) get_element_at(fs->value.selector, i));
      destroy_generic_list(fs->value.selector);
      break;
    }*/
  case NTH_CHILD_FILTER:
  case NTH_LAST_CHILD_FILTER:
    free(fs->value.s);
  }
  free(fs);
}

void destroy_attr_selector(attr_selector* as){
  free(as->name->value);
  free(as->name);
  if(as->value && as->value->value) free(as->value->value);
  if(as->value) free(as->value);
  free(as);
}

void destroy_selector(selector* s){
  if(s->id && s->id->value) free(s->id->value);
  if(s->id) free(s->id);

  int i;
  for(i = 0; i < s->attrs->count; i++)
    destroy_attr_selector((attr_selector*) get_element_at(s->attrs, i));
  destroy_generic_list(s->attrs);

  for(i = 0; i < s->filters->count; i++)
    destroy_filter_selector((filter_selector*) get_element_at(s->filters, i));
  destroy_generic_list(s->filters);

  free(s);
}

match_value* make_operators(char* str, int op){
    byte_buffer* bb = NULL;
    match_value* r = NULL;
    switch(op){
        case STARTSW_OP:
            bb = new_byte_buffer(strlen(str)+2);
            append_string_to_buffer("^", bb);
            append_string_to_buffer(str, bb);
            append_bytes_to_buffer("\0", bb, 1);
            r = new_match_value_no_strdup(bb->buffer, REGEX_OP);
            free(str);
            break;
        case ENDSW_OP:
            bb = new_byte_buffer(strlen(str)+2);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("$", bb);
            append_bytes_to_buffer("\0", bb, 1);
            r = new_match_value_no_strdup(bb->buffer, REGEX_OP);
            free(str);
            break;
        case WSSV_OP:
            bb = new_byte_buffer((strlen(str)+5)*4);
            append_string_to_buffer("(^", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("$)|", bb);

            append_string_to_buffer("(^", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer(" )|", bb);

            append_string_to_buffer("( ", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("$)|", bb);

            append_string_to_buffer("( ", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer(" )", bb);

            append_bytes_to_buffer("\0", bb, 1);
            r = new_match_value_no_strdup(bb->buffer, REGEX_OP);
            free(str);
            break;
        case DSV_OP:
            bb = new_byte_buffer((strlen(str)+5)*4);
            append_string_to_buffer("(^", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("$)|", bb);

            append_string_to_buffer("(^", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("-)|", bb);

            append_string_to_buffer("(-", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("$)|", bb);

            append_string_to_buffer("(-", bb);
            append_string_to_buffer(str, bb);
            append_string_to_buffer("-)", bb);

            append_bytes_to_buffer("\0", bb, 1);
            r = new_match_value_no_strdup(bb->buffer, REGEX_OP);
            free(str);
            break;
        case NO_OP:
            r = NULL;
            break;
        default:
            r = new_match_value_no_strdup(str, op);
    }
    free(bb);
    return r;
}

