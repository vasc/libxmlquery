#ifndef _LXQ_SERIALIZE_H_
#define _LXQ_SERIALIZE_H_

//TODO: There this cross platform definition must be configured through some sort of './configure' script
#define MONGO_USE_LONG_LONG_INT

//#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include "bson.h"
#include "node.h"


typedef enum{
    XML=0,
    JSON=1,
    YAML=2
} serialization_type;

typedef struct bb_s{
    char* buffer;
    int capacity;
    int size;
} byte_buffer;

extern void output_xml(doc* root);
extern char* to_xml(doc* root);
/*extern char* to_json(doc* root);*/

void append_bytes_to_buffer(const char* bytes, byte_buffer* b, size_t size);

byte_buffer* __node_list_to_xml(list* l, int depth);
byte_buffer* __attribute_to_xml(dom_node* n);

#endif

