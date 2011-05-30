/*

error.c


*/

#include <stdio.h>

int theErrorNumber;

int errorNumber(void) {

   return theErrorNumber;
   
}


int errorSetErrorNumber(int e){
 
   theErrorNumber = e;
   
   //fprintf(stderr, "Error %d set", e);
}

