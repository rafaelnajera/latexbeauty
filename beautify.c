/*

beautify.c

the function that does all the magic

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "beautify.h"
#include "paragraph.h"
#include "block.h"
#include "error.h"
#include "stringstack.h"


void beautifyInfoReset(beautifyInfo *bi){

   bi->nLinesIn = 0;
   bi->nParagraphs = 0;
   bi->nLinesOut = 0;
   
   
};

int beautifyOptionsConstruct(beautifyOptions *bo){

   stringStackConstruct(&bo->linesToIgnore);
   return 1;
}

int beautifyOptionsDefault(beautifyOptions *bo){

   int i;
   
   if (bo == NULL) 
      return 0;

   bo->lineSize = 80;
   bo->tabSize = 3;
   bo->verbosityLevel = 0;
   bo->doNotIgnoreAnyLine = 0;
   
   if (!stringStackPush(&bo->linesToIgnore, "%-")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\item ")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\usepackage{")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\chapter")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\section")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\subsection")) return 0;  
   if (!stringStackPush(&bo->linesToIgnore, "\\newcommand")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\renewcommand")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\begin{document")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\end{document")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\begin{quotation")) return 0; 
   if (!stringStackPush(&bo->linesToIgnore, "\\end{quotation")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\begin{enumerate")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\end{enumerate")) return 0;  
   if (!stringStackPush(&bo->linesToIgnore, "\\begin{itemize")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\end{itemize")) return 0;    
   if (!stringStackPush(&bo->linesToIgnore, "\\title{")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\author{")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\date{")) return 0;
   if (!stringStackPush(&bo->linesToIgnore, "\\set")) return 0;

   return 1;
}


int addToStrArray(char **strArray, int *nItems, char *str){
   
   strArray[*nItems] = malloc(strlen(str)+1);
   
   if (strArray[*nItems] == NULL){
         errorSetErrorNumber(BEAUTIFY_NOMEMORY);
         return 0;
   }
   *nItems++;
   
   return 1;

}

int beautify(char *input, char *output, beautifyOptions *bo, beautifyInfo *bi) {

   int ip;
   paragraph p;
   int itl, itw, indent;
   int allParagraphsOut;
   char tempLine[512], tempWord[256];
   int linesize;
  
   linesize = bo->lineSize;
  
   beautifyInfoReset(bi);
   if (!paragraphNew(&p)) return 0;
   output[0]='\0';
   ip = 0;
   allParagraphsOut = 1;
   indent = 0;
   while (input[ip] != '\0'){
      switch(input[ip]){
         case '\n':
            bi->nLinesIn++;
            if (!paragraphOut(&p, output, linesize)) return 0;
            bi->nParagraphs++;
            //   fprintf(stderr, "Paragraph %d\n", bi->nParagraphs);
            if (!paragraphReset(&p)) return 0;
            allParagraphsOut = 1;
            // output all newlines that follow but without increasing
            // the paragraph count
            ip++;
            while ( input[ip] == '\n'){
               strcat(output, "\n");   
               bi->nLinesIn++;
               ip++;
            } 
            ip--;
            break;

         case '\t':
         case ' ':
            indent=0;
            while ( (input[ip] == ' ') || (input[ip]=='\t')){
               if (input[ip] == ' ')
                  indent++;
               else
                  indent+=bo->tabSize;
               ip++;
            }
    
            
         default:
            // take care of ignored lines, if necessary             
            if (!bo->doNotIgnoreAnyLine){
               if (stringStackMatchBeginning(&bo->linesToIgnore, input + ip)){
                  itl = 0;
                  while ( (input[ip] != '\0') && (input[ip] != '\n')){
                     tempLine[itl] = input[ip];
                     itl++;
                     ip++;
                  }
                  if (input[ip]== '\0')
                     ip--;
                  tempLine[itl]='\0';
                  if (allParagraphsOut){
                     if (!paragraphReset(&p)) return 0;
                  }       
                  if (!paragraphNewBlock(&p, BLOCK_IGNOREDLINE)) return 0;
                  if (!blockSetIgnoredLine(p.lastBlock, tempLine)) return 0;
                  if (!blockSetIndent(p.lastBlock, indent)) return 0;
                  bi->nLinesIn++;
                  allParagraphsOut = 0;
                  indent = 0;
                  break;
               }
            }
            // regular lines then
            if (allParagraphsOut ){
                if (!paragraphReset(&p)) return 0;
                if (!paragraphNewBlock(&p, BLOCK_TEXT)) return 0;
                if (!blockSetIndent(p.lastBlock, indent)) return 0;
                allParagraphsOut = 0;
            } 
            else if ( (p.lastBlock->type != BLOCK_TEXT) ||  
                 (p.lastBlock->indent != indent ) ){
               if (!paragraphNewBlock(&p, BLOCK_TEXT)) return 0;
               if (!blockSetIndent(p.lastBlock, indent)) return 0;
            }
            
            itw = 0;
            while ( (input[ip] != '\0') && (input [ip] != '\n')){
               if ( (input[ip] == ' ') || (input[ip]=='\t')){
                  tempWord[itw]='\0';
                  if (!blockAddWord(p.lastBlock, tempWord)) return 0;
                  itw = 0;
                  tempWord[itw]='\0';
                  while ( (input[ip]==' ') || (input[ip]=='\t')){
                     ip++;
                  }
                  ip--;
               }
               else {
                  tempWord[itw] = input[ip];
                  itw++;
               }
               ip++;
            }
            bi->nLinesIn++;
            tempWord[itw]='\0';
            if (itw != 0){
               if (!blockAddWord(p.lastBlock, tempWord)) return 0;   
            }

            if (input[ip]== '\0')
               ip--;
            indent = 0;
            break;
      } // switch input...
      ip++;
   } // while input[ip]
   
   if (!allParagraphsOut){
      if (!paragraphOut(&p, output, linesize)) return 0;
      bi->nParagraphs++;
   }
 
   return 1;
}
