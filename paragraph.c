/* 
 
paragraph.c

*/

#include <string.h>
#include <stdlib.h>

#include "paragraph.h"
#include "error.h"


int paragraphNew(paragraph *theP){
   
   theP->nBlocks = 0;
   theP->lastBlock = NULL;
   
   return 1;
    
}

int paragraphReset(paragraph *theP){

   int i;
   
   for (i = 0; i < theP->nBlocks; i++)
      free(theP->theBlocks[i]);
      
   theP->nBlocks = 0;
   theP->lastBlock = NULL;
   
   return 1;
  
}

int paragraphOut(paragraph *theP, char *outputString, int linesize){

   int i;
   for (i=0; i < theP->nBlocks; i++){
      if (!blockOut(theP->theBlocks[i], outputString, linesize))
         return 0;
   }
   strcat(outputString, "\n");
      
   return 1;
      
}

int paragraphNewBlock(paragraph *theP, int type){

   if (theP->nBlocks == MAX_BLOCKS){
      errorSetErrorNumber(PAR_MAXBLOCKSREACHED);
      return 0;
   }
   theP->theBlocks[theP->nBlocks] = (block *)malloc(sizeof(block));
   if (theP->theBlocks[theP->nBlocks] == NULL){
      errorSetErrorNumber(PAR_NOMEMORY);
      return 0;
   }
   
   if (!blockReset(theP->theBlocks[theP->nBlocks], type)) return 0;
   theP->lastBlock = theP->theBlocks[theP->nBlocks];
   theP->nBlocks++;
   return 1;
}
