#include <stdlib.h>
#include <string.h>
#include "eregex.h"
#include "list.h"

list_keeper* new_list(){
  list_keeper * lk = alloc(list_keeper, 1);
  lk->first = NULL;
  lk->last = NULL;
  lk->count = 0;
  return lk;
}

struct snode* pop(list_keeper* keeper){
  list_node* aux = keeper->first;
  struct snode* res;

  if(aux == NULL)
    return NULL;

  keeper->first = keeper->first->next;
  res = aux->node;
  free(aux);
  return res;
}

void append(list_keeper* keeper, struct snode* node){
  list_node* lnode;
  
  if(keeper == NULL){
    log(W, "Trying to append node to null keeper.");
    return;
  }

  lnode = alloc(list_node, 1);
  lnode->node = node;
  lnode->next = NULL;

  keeper->count++;

  if(keeper->first == NULL){
    keeper->first = lnode;
    keeper->last = lnode;
    return;
  }

  keeper->last->next = lnode;
  keeper->last = lnode;
  return;
}

struct snode* get(list_keeper keeper, int index){
  unsigned int it;
  list_node* lit;

  if(index < 0 || keeper.count <= index){
    log(W, "Index out of bounds.");
    return NULL;
  }

  for(it = 0, lit = keeper.first; it < index; it++, lit = lit->next);

  return lit->node;
}

list_keeper* get_by_name(list_keeper keeper, char* name){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(strcmp(lit->node->name, name) == 0)
      append(list, lit->node);
  }

  return list;
}

void destroy(list_keeper* keeper){
  list_node* lit;

  lit = keeper->first;
  while(lit != NULL){
    keeper->first = lit->next;
    free(lit);
    lit = keeper->first;
  }

  free(keeper);
}

void prepend(list_keeper* keeper, struct snode* node){
  list_node* lnode;
  
  if(keeper == NULL){
    log(W, "Trying to prepend node to null keeper.");
    return;
  }

  lnode = alloc(list_node, 1);
  lnode->node = node;
  lnode->next = NULL;

  keeper->count++;

  if(keeper->first == NULL){
    keeper->first = lnode;
    keeper->last = lnode;
    return;
  }

  lnode->next = keeper->first;
  keeper->first = lnode;
  return;  
}

list_keeper* get_by_namespace(list_keeper keeper, char* namespace){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(strcmp(lit->node->namespace, namespace) == 0)
      append(list, lit->node);
  }

  return list;
}

list_keeper* regex_get_by_name(list_keeper keeper, char* pattern){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(match(lit->node->name, pattern, 0))
      append(list, lit->node);
  }

  return list;
}

list_keeper* regex_get_by_namespace(list_keeper keeper, char* pattern){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(match(lit->node->namespace, pattern, 0))
      append(list, lit->node);
  }

  return list;
}

list_keeper* regex_get_by_name_ignore_case(list_keeper keeper, char* pattern){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(match(lit->node->name, pattern, 1))
      append(list, lit->node);
  }

  return list;
}

list_keeper* regex_get_by_namespace_ignore_case(list_keeper keeper, char* pattern){
  list_keeper* list = new_list();
  list_node* lit;

  for(lit = keeper.first; lit != NULL; lit = lit->next){
    if(match(lit->node->namespace, pattern, 1))
      append(list, lit->node);
  }

  return list;
}
