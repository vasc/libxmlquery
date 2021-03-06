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

%{
#include <stdio.h>
#include <string.h>
#include "../include/node.h"
#include "../include/stack.h"
#include "../include/query_parser.h"
#include "../include/macros.h"

#define LXQ_RELATION_TYPE 0
#define LXQ_SELECTOR_TYPE 1

extern int yylex(void);
extern int yyparse(void);
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

doc* lxq_document;

char* lxq_parser_dot_query_operator = "class";
char* lxq_parser_pound_query_operator = "id";
list* lxq_selected_elements;

void yyerror(const char *str)
{
  fprintf(stderr,"error at:%d: %s at '%s'\n", yylineno, str, yytext);
  lxq_document = NULL;
  lxq_selected_elements = NULL;
}

int yywrap()
{
        return 1;
}

void parse_file(char* filename){
  char* address;
  FILE* holder;
  struct yy_buffer_state* bs;

  if(!strcmp(filename, "-")){
      holder = yyin = 0;
  }
  else{
      holder = yyin = fopen(filename, "r");
      if(yyin == NULL){
        log(F, "Unable to open file %s for reading.", filename);
        exit(1);
      }
  }

  yylineno = 1;
  yyparse();
  yylex_destroy();
  if(holder) fclose(holder);
}

void parse_string(const char* str){
  int len = strlen(str);
  char* internal_cpy = alloc(char, len + 2);

  memcpy(internal_cpy, str, len);

  internal_cpy[len] = '\0';
  internal_cpy[len + 1] = '\0';

  if(!yy_scan_buffer(internal_cpy, len + 2)){
    log(F, "Flex could not allocate a new buffer to parse the string.");
    exit(1);
  }
  yylineno = 1;
  yyparse();
  yylex_destroy();
  free(internal_cpy);
}


%}

%union{
  char * string;
  struct snode* dn;
  int digits;
  struct attr_selector_s* attrselector;
  struct step_s* s;
  int token;
  struct filter_selector_s* fa;
  struct generic_list_s* q;
  struct selector_s* sel;
  struct match_value_s* mv;
 }

%token START_EL END_EL SLASH SCUSTOM_FILTER
%token <string> WORD TEXT CDATA_TOK REGEX CUSTOM_FILTER CUSTOM_OPERATOR CUSTOM_RELATION_OPERATOR
%token ALL SPACE  END_REGEXI NO_OP EQUAL_OP WSSV_OP STARTSW_OP ENDSW_OP CONTAINS_OP REGEX_OP REGEXI_OP DSV_OP NOTEQUAL_OP EVEN ODD
%token NTH_CHILD_FILTER NTH_LAST_CHILD_FILTER FIRST_CHILD_FILTER LAST_CHILD_FILTER ONLY_CHILD_FILTER EMPTY_FILTER NOT_FILTER
%type <dn> attr node prop inner attrs declaration start_tag end_tag namespace doctype words_and_values
%token <digits> DIGITS

%type <attrselector> attr_filter attrsel;
%type <s> step parameters;
%type <digits> offset;
%type <token> operator pseudo_op nth_pseudo_op relation_operator end_regex;
%type <fa> pseudo_filter;
%type <string> value
%type <q> selector_group pseudo_filters attrsels regex_stack words declarations
%type <mv> regex id
%type <sel> selector

%start choose

%%

choose: '@' selector_group
      | document
      ;

document: prop inner                                        { lxq_document = new_document(NULL);
                                                              if($2->children == NULL && $1->type == ELEMENT){
                                                                set_doc_root(lxq_document, $1);
                                                              }
                                                              else{
                                                                  set_name($2, "root");
                                                                  prepend_child($2, $1);
                                                                  set_doc_root(lxq_document, $2);
                                                              }
                                                            }
        | declarations node                                 { lxq_document = new_document($1); set_doc_root(lxq_document, $2);}
        ;

namespace: WORD                                             { $$ = new_element_node($1); free($1);}
         | WORD ':' WORD                                    { $$ = new_element_node($3);
                                                              free($3);
                                                              char* old = $$->namespace;
                                                              $$->namespace = $1;
                                                              free($1);
                                                              if(old)
                                                                free(old);
                                                            }
         ;


declarations: declaration                                   { $$ = new_generic_list(1); add_element($$, $1); }
            | doctype                                       { $$ = new_generic_list(1); add_element($$, $1); }
            | declarations declaration                      { $$ = $1; add_element($$, $2); }
            | declarations doctype                          { $$ = $1; add_element($$, $2); }

doctype: START_EL '!' WORD words_and_values END_EL          { $$ = $4; set_name($$, $3); free($3); }

declaration: START_EL '?' namespace attrs '?' END_EL        {
                                                              //if(strcmp(get_name($3), "xml") != 0){
                                                              //  yyerror("Declaration does not begin with xml");
                                                              //  exit(-1);
                                                              //}
                                                              $$ = $4;
                                                              char* old = set_name($$, get_name($3));
                                                              if(old)
                                                                free(old);
                                                              old = set_namespace($$, get_namespace($3));
							                                  if(old)
								                                free(old);
                                                              destroy_dom_node($3);
                                                            }
           ;

node: start_tag inner end_tag                               {
                                                              if(strcmp(get_name($1),get_name($3)) != 0){
								                                char* error_line = alloc(char, 41 + strlen(get_name($1)) + strlen(get_name($3)));
								                                sprintf(error_line, "Start tag '%s' does not match end tag '%s' ", get_name($1), get_name($3));
                                                                yyerror(error_line);
                                                                free(error_line);
                                                                exit(1);
                                                              }

                                                              $1->children = $2->children;
                                                              int i;
                                                              if($1->children){
                                                                  for(i = 0; i < $1->children->count; i++){
                                                                      ((dom_node*)get_element_at($1->children, i))->parent = $1;
                                                                  }
                                                              }

                                                              $2->children = NULL;
                                                              destroy_dom_node($2);

                                                              $$ = $1;
                                                              destroy_dom_node($3);
                                                            }
    | START_EL namespace attrs SLASH END_EL                 { $$ = $3;
                                                              char* old = set_name($$, get_name($2));
                                                              if(old)
                                                                free(old);
                                                              old = set_namespace($$, get_namespace($2));
                                                              if(old)
                                                                free(old);
                                                              destroy_dom_node($2);
                                                            }
    ;

inner:                                                      { $$ = new_element_node(NULL);}
     | inner prop                                           { $$ = $1;
                                                              append_child($$, $2);
                                                            }
     ;

prop: CDATA_TOK                                             {$$ = new_cdata($1); free($1);}
    | TEXT                                                  {$$ = new_text_node($1); free($1);}
    | node                                                  {$$ = $1;}
    ;


start_tag: START_EL namespace attrs END_EL                  { $$ = $3;
                                                              char* old = set_name($$, get_name($2));
							                                  if(old)
								                                free(old);
                                                              old = set_namespace($$, get_namespace($2));
							                                  if(old)
								                                free(old);
                                                              destroy_dom_node($2);
                                                            }
         ;

end_tag: START_EL SLASH namespace END_EL                    { $$ = $3;}
       ;

attrs:                                                      { $$ = new_element_node(NULL); }
     | attrs attr                                           {
                                                              $$ = $1;
                                                              add_attribute($$, $2);
                                                            }
     ;

attr:  namespace '=' value                                  {$$ = new_attribute(get_name($1), $3);
                                                             free($3);
                                                             char* old = set_namespace($$, get_namespace($1));
                                                               if(old)
                                                                 free(old);
                                                                 destroy_dom_node($1); }
    | namespace                                             { $$ = new_attribute(get_name($1), NULL);
                                                              char* old = set_namespace($$, get_namespace($1));
                                                              if(old)
                                                                free(old);
                                                                destroy_dom_node($1);
                                                            }
    ;

words_and_values:                                           { $$ = new_element_node(NULL); }
                | words_and_values WORD                     { $$ = $1; add_attribute($$, new_attribute($2, NULL)); free($2);}
                | words_and_values value                    { $$ = $1; append_child($$, new_text_node($2)); free($2);}


value: '"' TEXT '"'                                         {$$ = $2;}
     | '"' '"'                                              {$$ = strdup("");}
     | '\'' TEXT '\''                                       {$$ = $2;}
     | '\'' '\''                                            {$$ = strdup("");}
     ;


selector_group: selector                                    { lxq_selected_elements = $$ = new_generic_list_with_type(4); enqueue_with_type($$, $1, LXQ_SELECTOR_TYPE); }
              | selector_group relation_operator selector   { int* a = alloc(int, 1);
                                                              *a = $2;
                                                              $$ = $1;
                                                              enqueue_with_type($$, a, LXQ_RELATION_TYPE);
                                                              enqueue_with_type($$, $3, LXQ_SELECTOR_TYPE);
                                                            }
              | selector_group CUSTOM_RELATION_OPERATOR selector    { $$ = $1;
                                                                      enqueue_with_type($$, strdup($2+1), CUSTOM_RELATION_OPERATOR);
                                                                      free($2);
                                                                      enqueue_with_type($$, $3, LXQ_SELECTOR_TYPE);
                                                                    }
              ;

selector: id attrsels pseudo_filters                        { $$ = new_selector($1); $$->attrs = $2; $$->filters = $3; }
        ;

attrsels:                                                   { $$ = new_stack(4); }
        | attrsels '[' attrsel ']'                          { $$ = $1; push_stack($$, $3); }
        | attrsels '.' WORD                                 { $$ = $1;
                                                              push_stack($$, new_attr_value_selector(
                                                                                 new_match_value(lxq_parser_dot_query_operator, EQUAL_OP),
                                                                                 make_operators($3, WSSV_OP)));
                                                            }
        | attrsels '#' WORD                                 { $$ = $1;
                                                              push_stack($$, new_attr_value_selector(
                                                                                 new_match_value(lxq_parser_pound_query_operator, EQUAL_OP),
                                                                                 new_match_value_no_strdup($3, EQUAL_OP)));
                                                            }
        ;

attrsel: WORD attr_filter                                   { $$ = $2; $$->name = new_match_value_no_strdup($1, EQUAL_OP); }
       | regex attr_filter                                  { $$ = $2; $$->name = $1; }
       ;

id:                                                         { $$ = NULL; }
  | ALL                                                     { $$ = NULL; }
  | WORD                                                    { $$ = new_match_value_no_strdup($1, EQUAL_OP);}
  | regex                                                   { $$ = $1; }
  ;

pseudo_filters:                                             { $$ = new_stack(4); }
              | pseudo_filters ':' pseudo_filter            { push_stack($$, $3); }
              ;

pseudo_filter: pseudo_op                                    { $$ = new_filter($1); }
             | CUSTOM_FILTER                                { $$ = new_filter(SCUSTOM_FILTER); $$->name = $1; }
             | CUSTOM_FILTER '(' words ')'                  { $$ = new_filter(CUSTOM_FILTER); $$->name = $1; $$->args = $3; }
             | nth_pseudo_op '(' parameters ')'             { $$ = new_filter($1); $$->value.s = $3; }
             | NOT_FILTER '(' selector_group ')'            { $$ = new_filter(NOT_FILTER); $$->value.selector = $3; }
             ;

words:                                                      { $$ = new_stack(2); }
     | words DIGITS                                         { int* i = alloc(int, 1); *i = $2; push_stack($1, i); $$ = $1;}
     | words WORD                                           { push_stack($1, $2); $$ = $1;}

nth_pseudo_op: NTH_CHILD_FILTER                             { $$ = NTH_CHILD_FILTER; }
             | NTH_LAST_CHILD_FILTER                        { $$ = NTH_LAST_CHILD_FILTER; }
             ;

pseudo_op: FIRST_CHILD_FILTER                               { $$ = FIRST_CHILD_FILTER; }
         | LAST_CHILD_FILTER                                { $$ = LAST_CHILD_FILTER; }
         | ONLY_CHILD_FILTER                                { $$ = ONLY_CHILD_FILTER; }
         | EMPTY_FILTER                                     { $$ = EMPTY_FILTER; }
         ;

parameters: step                                            { $$ = $1; }
          | offset                                          { $$ = alloc(struct step_s, 1); $$->multiplier = 0; $$->offset = $1;}
          | EVEN                                            { $$ = alloc(struct step_s, 1); $$->multiplier = 2; $$->offset = 0; }
          | ODD                                             { $$ = alloc(struct step_s, 1); $$->multiplier = 2; $$->offset = 1; }
          ;

step: 'n'                                                   { $$ = alloc(struct step_s, 1); $$->multiplier = 1; $$->offset = 0; }
    | DIGITS 'n'                                            { $$ = alloc(struct step_s, 1); $$->multiplier = $1; $$->offset = 0; }
    | 'n' offset                                            { $$ = alloc(struct step_s, 1); $$->multiplier = 1; $$->offset = $2; }
    | DIGITS 'n' offset                                     { $$ = alloc(struct step_s, 1); $$->multiplier = $1; $$->offset = $3; }
    ;

offset: '+' DIGITS                                          { $$ = $2; }
      | '-' DIGITS                                          { $$ = -$2; }
      | DIGITS                                              { $$ = $1; }
      ;

relation_operator: '>'                                      { $$ = '>'; }
                 | '~'                                      { $$ = '~'; }
                 | '+'                                      { $$ = '+'; }
                 | ','                                      { $$ = ','; }
                 | SPACE                                    { $$ = SPACE; }
                 ;

attr_filter:                                                { $$ = new_attr_value_selector(NULL, NULL); }
           | operator '"' TEXT '"'                          { $$ = new_attr_value_selector(NULL, make_operators($3, $1)); }
           | operator '\'' TEXT '\''                        { $$ = new_attr_value_selector(NULL, make_operators($3, $1)); }
           | EQUAL_OP regex                                 { $$ = new_attr_value_selector(NULL, $2); }
           ;

regex: '/' regex_stack end_regex                            {   char* text = (char*)dequeue($2);
                                                                while($2->count > 0){
                                                                    char* r = (char*)dequeue($2);
                                                                    text = (char*)realloc(text, strlen(text) + strlen(r));
                                                                    strcat(text, r);
                                                                    free(r);
                                                                }
                                                                $$ = new_match_value_no_strdup(text, $3);
                                                                destroy_generic_list($2);
                                                            }

regex_stack: REGEX                                          { $$ = new_queue(4); enqueue($$, $1); }
           | regex_stack REGEX                              { $$ = $1; enqueue($$, $2); }
           ;

end_regex: '/'                                              { $$ = REGEX_OP; }
         | END_REGEXI                                       { $$ = REGEXI_OP; }
         ;

operator: EQUAL_OP                                          { $$ = EQUAL_OP; }
        | WSSV_OP                                           { $$ = WSSV_OP; }
        | STARTSW_OP                                        { $$ = STARTSW_OP; }
        | ENDSW_OP                                          { $$ = ENDSW_OP; }
        | CONTAINS_OP                                       { $$ = CONTAINS_OP; }
        | DSV_OP                                            { $$ = DSV_OP; }
        | NOTEQUAL_OP                                       { $$ = NOTEQUAL_OP; }
        ;

