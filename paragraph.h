/* 
 
paragraph.h

*/


#ifndef __PARAGRAPH
#define __PARAGRAPH

#include "block.h"

#define MAX_BLOCKS 200


typedef struct {
   int nBlocks;
   block *theBlocks[MAX_BLOCKS];
   block *lastBlock; 
} paragraph;

int paragraphNew(paragraph *theP);
int paragraphReset(paragraph *theP);
int paragraphNewBlock(paragraph *theP, int type);
int paragraphOut(paragraph *theP, char *outputString, int linesize);

#endif
