

/*
utf8.c

*/

#include <stdio.h>

#include "utf8.h"


int utf8charSize(char *input, int i){
// Gets the next encoded Unicode char size

   unsigned char c;
   
   c = (unsigned char) input[i];
   
   if ( c < 0x80)
      return 1;
   else if (c < 0xe0)
      return 2;
   else if (c < 0xf0)
      return 3;
   else if (c < 0xf8)
      return 4;
   else if (c < 0xfc)
      return 5;
   else 
      return 6;
}


int utf8stringLength(char *str){
// returns the number of utf8 characters in a string

   int i,l;
   
   i = 0;
   l = 0;
   
   while (str[i] != '\0'){
      i +=   utf8charSize(str, i);
      l++;
   }
   return l;
}
/*
int main(void)

{

   char input[100];
   int i;
   int s;
   unsigned char c;
      
   while (1){
      fgets(input, 100, stdin);
      
      i = 0;
      while(input[i] != '\0'){
            
        s = getUTF8charsize(input,i);   
         
         fprintf(stdout, "%d : %d bytes = ", i, s);
         fwrite(input+i, sizeof(char), s, stdout);
         fprintf(stdout, "\n");
         i += s; 
        
   
      }
   }
   
} */
