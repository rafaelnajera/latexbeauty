/*

latexbeauty.c
 
Beautifier for latex files

Takes a latex file in stdin and performs several formatting changes:
   - makes all lines at most 80 characters long 
   - respects current indentation of paragraphs
 
20 Dec 2010

*/
 
#define MAXFILESIZE  512000
#define LINESIZE   80
#define MINLINESIZE 20

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "beautify.h"





int main(int argc, char *argv[]){


   int c;
   int ip;
   char *theInput;
   char *theOutput;
   beautifyInfo info;
   beautifyOptions options;
    int linesize = LINESIZE;
    int aux;

    // Read linesize from command line
    
    if (argc != 1) {
        aux = atoi(argv[1]);
        if (aux >= MINLINESIZE)
            linesize = aux;
    } 
   
   
   theInput = (char *) malloc(MAXFILESIZE*sizeof(char));
   theOutput = (char *) malloc (MAXFILESIZE*sizeof(char));
   
   // Read input file
   c = fgetc(stdin);
   ip = 0;
   while (c != EOF) {
      theInput[ip] = c;
      ip++;
      if (ip == MAXFILESIZE){
         fprintf(stderr, "Max file size reached (%d)\n", MAXFILESIZE);
         return -1;
      }
      c = fgetc (stdin);
   };
   theInput[ip]='\0'; 
   
   beautifyOptionsConstruct(&options);
   beautifyOptionsDefault(&options);
   options.lineSize = linesize;
   
  if (beautify( theInput, theOutput, &options, &info)){
      fputs(theOutput,stdout);
      return 0;
   }
   else {
      fprintf(stderr, "Error:  %d\n", errorNumber());
      return errorNumber();    
  }
}  
                
            
           
            
           
            
            
   
   
   
   
   
   
