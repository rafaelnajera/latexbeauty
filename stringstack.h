/*

 stringstack.h


 
*/
 
 
#ifndef __STRINGSTACK
#define __STRINGSTACK

typedef struct {

   char **theStrings;
   int nStrings;
   
} stringStack;


int stringStackPush(stringStack *ss, char *str);
int stringStackPop(stringStack *ss, char *str);
int stringStackDestruct(stringStack *ss);
int stringStackConstruct(stringStack *ss);
int stringStackMatchBeginning(stringStack *ss, char *str);


#endif
