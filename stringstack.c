/*

stringstack.c


*/

#include <string.h>
#include <stdlib.h>


#include "stringstack.h"
#include "error.h"


#define MAX_SS_STRINGS  100

int stringStackPush(stringStack *ss, char *str){
   
   if (ss->theStrings == NULL){
      errorSetErrorNumber(SS_PUSHINGONADESTROYEDSTACK);
      return 0;
   }
   
   if(ss->nStrings == MAX_SS_STRINGS){
      errorSetErrorNumber(SS_MAXSTRINGSREACHED);
      return 0;
   }
   
   ss->theStrings[ss->nStrings] = (char *)malloc(strlen(str)+1);
   if (ss->theStrings[ss->nStrings] == NULL){
      errorSetErrorNumber(SS_NOMEMORY);
      return 0;
   }
   strcpy(ss->theStrings[ss->nStrings], str);
   ss->nStrings++;
   return 1;
}


int stringStackPop(stringStack *ss, char *str){
   
   if (str == NULL){
      errorSetErrorNumber(SS_POPWITHNULLSTRING);
      return 0;
   };
   if (ss->nStrings == 0){
      errorSetErrorNumber(SS_POPPINGFROMANEMPTYSTACK);
      return 0;
   };
   
   strcpy(str, ss->theStrings[ss->nStrings-1]);
   free(ss->theStrings[ss->nStrings-1]);
   ss->nStrings--;
   return 1;
}

int stringStackDestruct(stringStack *ss){

   if (ss->theStrings == NULL)
      return 1;
      
   while (ss->nStrings != 0){
     free(ss->theStrings[ss->nStrings-1]);
      ss->nStrings--;
   }
   free(ss->theStrings);
   ss->theStrings = NULL;
   return 1;
}

int stringStackConstruct(stringStack *ss){
   ss->nStrings = 0;
   ss->theStrings = (char **) malloc(MAX_SS_STRINGS*sizeof(char *));
   if (ss->theStrings == NULL){
      errorSetErrorNumber(SS_NOMEMORY);
      return 0;
   }
   return 1;
}

int matchBeginning(char *str1, char *str2){
   
   int i;
   
   for (i = 0; i < strlen(str1); i++){
      if ((str2[i] == '\0') || (str1[i] != str2[i]))
         return 0;
   }
   return 1;

}

int stringStackMatchBeginning(stringStack *ss, char *str) {
// returns 0 on no match, set error number for more info
   int i;
   
   if (ss->theStrings == NULL){
      errorSetErrorNumber(SS_SEARCHINGONADESTROYEDSTACK);
      return 0;
   };
   
   for (i = 0; i < ss->nStrings; i++){
      if (matchBeginning(ss->theStrings[i], str))
         return 1;
   }
   return 0;
      
}
