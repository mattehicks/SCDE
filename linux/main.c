/*
 *  SCDE Linux Main
 */ 


#include <stdio.h>
#include "scde_task.h"
//#include "dukf_utils.h"



int 
main(int argc, char *argv[]) {

  printf("argc = %d\n", argc);

  // argc = 1 .. no names
  int i;
  for (i=1; i<argc; i++) {

      //dukf_addRunAtStart(argv[i]);

  }

  scde_task(NULL);

}
