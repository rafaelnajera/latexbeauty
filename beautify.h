/*
 
 beautify.h
 
 
 */
 

#ifndef __BEAUTIFY
#define __BEAUTIFY

#include "stringstack.h"

typedef struct {

   int nLinesIn;
   int nParagraphs;
   int nLinesOut;

} beautifyInfo;

typedef struct {
   int lineSize;
   int tabSize;
   int verbosityLevel;
   int doNotIgnoreAnyLine;
   stringStack linesToIgnore;
} beautifyOptions;

int beautifyOptionsDefault(beautifyOptions *bo);
int beautifyOptionsConstruct(beautifyOptions *bo);
int beautifyOptionsAddDefaultLinesToIgnore(beautifyOptions *bo);
int beautify(char *input, char *output, beautifyOptions *bo, beautifyInfo *bi);


#endif
