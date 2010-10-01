#include <stdio.h>
#include "dom/node.h"
#include "parser/dom_parser.h"

extern doc* document;

int main(){
  parse_dom();

  printf("\n\n\n==========================================================\n\n\n");
  /*  output_xml(document);*/

  destroy_dom_tree(document);

  /*  printf("\n\n\nAGORA DE UMA STRING\n");
  yy_scan_string("<this is=\"a test\">texto</this>");
  yyparse();
  
  printf("==========================================================\n");
  output_xml(document);  */
  return 0;
}
