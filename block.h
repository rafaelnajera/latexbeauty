/*

block.h

*/

#ifndef __BLOCK
#define __BLOCK


#define MAX_WORDS 2500
#define MAX_WORD_SIZE  512

enum blocktypes { BLOCK_TEXT = 1, BLOCK_IGNOREDLINE };


typedef struct {
   int type;
   int indent;
   int nWords;
   char words[MAX_WORDS][MAX_WORD_SIZE];
   char ignoredLine[512];
   
} block;


int blockReset(block *theBlock, int type);
int blockOut(block *theBlock, char *outputString, int linesize);
int blockAddWord(block *theBlock, char *theWord);
int blockSetIndent(block *theBlock, int indent);



#endif
