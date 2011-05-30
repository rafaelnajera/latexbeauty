/*

block.c

*/

#include <string.h>


#include "block.h"
#include "error.h"
#include "utf8.h"



int checkNewLine(char *string){
   
   int i;
   
   i = 0;
   while(string[i] != '\0'){
      if (string[i] == '\n') 
         return 1;
      i++;
   }
   return 0;

} 

int checkWhiteSpace(char *string){

   int i;
   
   i = 0;
   while(string[i] != '\0'){
      if ( (string[i] == '\n') || (string[i]==' ') || (string[i]=='\t'))
         return 1;
      i++;
   }
   return 0;
}

void addSpaces(char * s, int n){

   int pos, outputPos;
   
   pos = 0;
   outputPos = strlen(s);
   while (pos < n){
      s[outputPos+pos] = ' ';
      pos++;
   }
   s[outputPos+pos] = '\0';
}    

int blockOut(block *theBlock, char *outputString, int linesize) {

  int pos,w;

  if (linesize <= 0){
      errorSetErrorNumber(BLOCK_BADLINESIZE);
      return 0;
   }

   switch (theBlock->type){
      case BLOCK_IGNOREDLINE:
         addSpaces(outputString, theBlock->indent);
         strcat(outputString, theBlock->ignoredLine);
         strcat(outputString, "\n");
         break;
         
      case BLOCK_TEXT:
         if (theBlock->nWords == 0){
            strcat(outputString, "\n");
            return 1;
         }
         if (theBlock->indent >= linesize){
            errorSetErrorNumber(BLOCK_INDENTLARGERTHANLINESIZE);
            return 0;
         }
         
         addSpaces(outputString, theBlock->indent);
         strcat(outputString, theBlock->words[0]);
         pos = utf8stringLength(theBlock->words[0]) + theBlock->indent;
         for (w = 1; w < theBlock->nWords; w++){
            if ( (pos + 1 + utf8stringLength(theBlock->words[w])) <= linesize){
                strcat(outputString," ");
                strcat(outputString, theBlock->words[w]);
                pos += utf8stringLength(theBlock->words[w])+1;                
            } 
            else {
              strcat(outputString, "\n");
              addSpaces(outputString, theBlock->indent);
              strcat(outputString, theBlock->words[w]);
              pos = utf8stringLength(theBlock->words[w]) + theBlock->indent;
            }
         }      
         strcat(outputString, "\n");
            
         break;
         
   }
   return 1;
   
}


int blockReset(block *theBlock, int type) {

   theBlock->nWords = 0;
   theBlock->indent = 0;
   theBlock->type = type;
   theBlock->ignoredLine[0]='\0';
   
   return 1;
}

int blockSetIndent(block *theBlock, int indent) {
   
  /* if (theBlock->type != BLOCK_TEXT){
      errorSetErrorNumber(BLOCK_WRONGTYPE);
      return 0;
   } */

   if (indent < 0){
      errorSetErrorNumber(BLOCK_NEGATIVEINDENT);
      return 0;
   } 
   
   theBlock->indent = indent;
}


int blockAddWord(block *theBlock, char *theWord){

   // Check for block type
   if (theBlock->type != BLOCK_TEXT){
      errorSetErrorNumber(BLOCK_WRONGTYPE);
      return 0;
   }
  
   // Can't allow adding empty words
   if (theWord[0] == '\0'){
      errorSetErrorNumber(BLOCK_EMPTYWORD);
      return 0;
   }
      
   // Can't allow whitespace in words
   if (checkWhiteSpace(theWord)){
      errorSetErrorNumber(BLOCK_WHITESPACE);
      return 0;
   }
   
   if (theBlock->nWords == MAX_WORDS){
      errorSetErrorNumber(BLOCK_MAXWORDSREACHED);
      return 0;
   }
      
   strcpy(theBlock->words[theBlock->nWords], theWord);
   theBlock->nWords++;
   return 1;
}

int blockSetIgnoredLine(block *theBlock, char *theLine){

   // Check for block type
   if (theBlock->type != BLOCK_IGNOREDLINE){
      errorSetErrorNumber(BLOCK_WRONGTYPE);
      return 0;
   }

   // Can't allow empty ignored lines
   if (theLine[0] == '\0'){
      errorSetErrorNumber(BLOCK_EMPTYLINE);
      return 0;
   }
   // Can't allow new lines in the line
   if (checkNewLine(theLine)){
       errorSetErrorNumber(BLOCK_NEWLINE);
      return 0;
   }

   strcpy(theBlock->ignoredLine, theLine);
   return 1;
}
