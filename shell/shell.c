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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/macros.h"
#include "symbol_table.h"
#include "command.h"

#define MAX_OPTS 20
#define LINE_SIZE 128

void print_banner(){
  printf("############################################################################\n");
  printf("#                                                                          #\n");
  printf("#  ll   iii bbb  xx   xx mm   mm ll     qqqq   uu  uu eeeee rrrr  yy   yy  #\n");
  printf("#  ll    i  bb b  xx xx  mmm mmm ll    qq  qq  uu  uu ee    rr rr  yy yy   #\n");
  printf("#  ll    i  bbb    xxx   mmmmmmm ll   qq    qq uu  uu eeee  rrrr    yyy    #\n");
  printf("#  ll    i  bb b  xx xx  mm m mm ll    qq  qq  uu  uu ee    rr rr   yyy    #\n");
  printf("#  llll iii bbb  xx   xx mm   mm llll   qqqq q uuuuuu eeeee rr  rr  yyy    #\n");
  printf("#                                                                          #\n");
  printf("############################################################################\n");
  printf("\n");
}

typedef struct parsed_line_s{
  int n_args;
  char* command;
  char* options[MAX_OPTS];
}parsed_line;

parsed_line* parse_command_line(char* line){
  char *start, *end;
  int option = 0;
  parsed_line* pl = alloc(parsed_line, 1);
  
  memset(pl, 0, sizeof(parsed_line));

  //find command
  start = end = line;
  for(; *end != ' ' && *end != '\n'; end++);
  pl->command = alloc(char, end - start + 1);
  memset(pl->command, 0, end - start + 1);
  strncpy(pl->command, start, end - start);

  //find options treating double quotes
  for(; *end != '\n';){
    for(; *end == ' '; end++);

    if(*end == '"'){
      for(end++, start = end; *end != '"'; end++); 
      (pl->options)[option] = alloc(char, end - start + 1);
      memset((pl->options)[option], 0, end - start + 1);
      strncpy((pl->options)[option++], start, end - start);
      end++; //consume double quote
      start = end;
    }else{
      for(start = end; *end != ' ' && *end != '\n'; end++); 
      (pl->options)[option] = alloc(char, end - start + 1);
      memset((pl->options)[option], 0, end - start + 1);
      strncpy((pl->options)[option++], start, end - start);
      start = end;
    }
  }

  pl->n_args = option;
  return pl;
}

void destroy_parsed_line(parsed_line* pl){
  free(pl->command);
  int i;
  for(i = 0; i < pl->n_args; i++)
    free((pl->options)[i]);
  free(pl);
}

int main(){
  char line[LINE_SIZE] = {0};
  init_command_table();
  init_symbol_table();

  print_banner();
  while(1){
    printf(">>> ");
    fgets(line, LINE_SIZE, stdin);
    parsed_line* pl = parse_command_line(line);
    exec_command(pl->command, pl->n_args, pl->options);
    destroy_parsed_line(pl);
    memset(line, 0, LINE_SIZE);
  }
  return 0;
}
