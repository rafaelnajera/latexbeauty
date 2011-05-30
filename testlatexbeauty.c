/*
 
testlatexbeauty.c

*/

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "stringstack.h"
#include "utf8.h"
#include "error.h"
#include "block.h"
#include "paragraph.h"
#include "beautify.h"


void test_stringStack(void){

   stringStack s;
   char tmp[100];
   
   CU_ASSERT(stringStackConstruct(&s));
   CU_ASSERT(stringStackDestruct(&s));
   
   CU_ASSERT(stringStackConstruct(&s));
   CU_ASSERT(stringStackPush(&s, "Str1"));
   CU_ASSERT(stringStackPush(&s, "Str2"));
   CU_ASSERT(stringStackDestruct(&s));
   CU_ASSERT_EQUAL(s.theStrings, NULL);
   
   CU_ASSERT(stringStackConstruct(&s));
   CU_ASSERT(stringStackPush(&s, "Str1"));
   CU_ASSERT(stringStackPush(&s, "Str2"));
   CU_ASSERT_EQUAL(s.nStrings, 2);
   CU_ASSERT_FALSE(stringStackMatchBeginning(&s, "Another string"));
   CU_ASSERT(stringStackMatchBeginning(&s, "Str2thisisgood"));
   CU_ASSERT(stringStackMatchBeginning(&s, "Str1thisisgood"));
   CU_ASSERT_FALSE(stringStackMatchBeginning(&s, "StrthisisNOTgood"));
   CU_ASSERT_FALSE(stringStackMatchBeginning(&s, "St"));
   CU_ASSERT(stringStackPop(&s,tmp));
   CU_ASSERT(stringStackPop(&s,tmp));
   CU_ASSERT_EQUAL(s.nStrings, 0);
   CU_ASSERT_STRING_EQUAL(tmp, "Str1");
   CU_ASSERT_FALSE(stringStackPop(&s, tmp));
   CU_ASSERT_EQUAL(errorNumber(), SS_POPPINGFROMANEMPTYSTACK);
   CU_ASSERT(stringStackDestruct(&s));
}

void test_utf8(void){

   CU_ASSERT_EQUAL(utf8stringLength("test"), 4);
   CU_ASSERT_EQUAL(utf8stringLength("Nájera"), 6);
   CU_ASSERT_EQUAL(utf8stringLength("Ἒλήνικη"), 7);
   CU_ASSERT_EQUAL(utf8stringLength("日本語"), 3);

}


void test_beautify(void){

   char out[1000];
   beautifyInfo info;
   beautifyOptions opt;
   
   CU_ASSERT(beautifyOptionsConstruct(&opt));
   CU_ASSERT(beautifyOptionsDefault(&opt));
   CU_ASSERT_STRING_EQUAL(opt.linesToIgnore.theStrings[0], "%-");

   // Empty input   
   CU_ASSERT(beautify("", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "");
   CU_ASSERT_EQUAL(info.nLinesIn, 0);
   CU_ASSERT_EQUAL(info.nParagraphs, 0);
 

   // Input with just one newline
   CU_ASSERT(beautify("\n", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 1);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);
   
   // Input with just newlines
   CU_ASSERT(beautify("\n\n\n", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "\n\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 3);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);
   
   // Input with lines to ignore
   CU_ASSERT(beautify("\\chapter{test}\n\\section{Ignore)", out, &opt, &info));
   //puts("\n---");
   //puts(out);
   //puts("---");
   CU_ASSERT_STRING_EQUAL(out, "\\chapter{test}\n\\section{Ignore)\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 2);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);

   // Very simple no indent input (with trailing whitespace)
   CU_ASSERT(beautify("Word1 word2 \t", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "Word1 word2\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 1);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);
   
   // Simple no indent input
   CU_ASSERT(beautify("Par 1 Line 1\nPar 1 Line 2\n\nPar 2 Line1", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "Par 1 Line 1 Par 1 Line 2\n\nPar 2 Line1\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 4);
   CU_ASSERT_EQUAL(info.nParagraphs, 2);
   
   // Simple no indent input with lines to ignore
   CU_ASSERT(beautify(
      "\\begin{document}\nThis is a test\n\nTest1\n\\chapter{1}\nTest2", 
      out, &opt, &info));
   //printf("\n--Error Nr. %d\n", errorNumber());
   CU_ASSERT_STRING_EQUAL(out, 
      "\\begin{document}\nThis is a test\n\nTest1\n\\chapter{1}\nTest2\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 6);
   CU_ASSERT_EQUAL(info.nParagraphs, 2);
   
   // Very simple indent input
   CU_ASSERT(beautify("    Word1 word2", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "    Word1 word2\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 1);
   CU_ASSERT_EQUAL(info.nParagraphs, 1); 
  
   // Simple indent input
   CU_ASSERT(beautify(
      "   Word1 word2\n   word3 word4\n   word5", out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, "   Word1 word2 word3 word4 word5\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 3);
   CU_ASSERT_EQUAL(info.nParagraphs, 1); 
   
   // Three blocks of indented text
   CU_ASSERT(beautify(
      "Word1 word2\nword3 word4\n   word5 word6\n   word7\nword8", 
      out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, 
      "Word1 word2 word3 word4\n   word5 word6 word7\nword8\n\n");
   CU_ASSERT_EQUAL(info.nLinesIn, 5);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);
   
   // Two blocks of indented text with a line to ignore in the middle
   CU_ASSERT(beautify(
      "Word1 word2\nword3 word4\n\t\\newcommand{test}\nword5 word6\n   word7\n   word8", 
      out, &opt, &info));
   CU_ASSERT_STRING_EQUAL(out, 
      "Word1 word2 word3 word4\n   \\newcommand{test}\nword5 word6\n   word7 word8\n\n");
   //puts("\n---");
   //puts(out);
   //puts("---");
   CU_ASSERT_EQUAL(info.nLinesIn, 6);
   CU_ASSERT_EQUAL(info.nParagraphs, 1);

}

void test_basicParagraph(void) {
   
   paragraph testP;
   char testOut[1000];
   
   testOut[0]='\0';
   
   // Paragraph new
   CU_ASSERT(paragraphNew(&testP));
   CU_ASSERT_EQUAL(testP.nBlocks, 0);
   
   // Paragraph reset
   paragraphReset(&testP);
   CU_ASSERT(testP.nBlocks == 0);
   // Null paragraph out
   paragraphOut(&testP, testOut,80);
   CU_ASSERT_STRING_EQUAL(testOut, "\n");
   // Simple paragraph
   testOut[0]='\0';
   paragraphReset(&testP);
   CU_ASSERT(paragraphNewBlock(&testP, BLOCK_TEXT));
   CU_ASSERT(blockSetIndent(testP.lastBlock, 4));
   blockAddWord(testP.lastBlock, "Line1:");
   blockAddWord(testP.lastBlock, "Word1.");
   blockAddWord(testP.lastBlock, "Line2:");
   blockAddWord(testP.lastBlock, "Word2.");
   CU_ASSERT(paragraphOut(&testP, testOut, 17));
   CU_ASSERT_STRING_EQUAL(testOut,"    Line1: Word1.\n    Line2: Word2.\n\n");
   // Adding an ignored line
   testOut[0]='\0';
   CU_ASSERT(paragraphNewBlock(&testP, BLOCK_IGNOREDLINE));
   CU_ASSERT(blockSetIgnoredLine(testP.lastBlock,"This line is ignored"));
   CU_ASSERT(paragraphOut(&testP, testOut, 17));
   CU_ASSERT_STRING_EQUAL(testOut,
         "    Line1: Word1.\n    Line2: Word2.\nThis line is ignored\n\n");
   // Same, but with indent
   testOut[0]='\0';      

}



void test_basicBlockOperations(void) {

   block testBlock;
   
   // Tests for ignored-line-type blocks
   CU_ASSERT(blockReset(&testBlock, BLOCK_IGNOREDLINE));

   CU_ASSERT_FALSE(blockAddWord(&testBlock, "Test"));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_WRONGTYPE);

   CU_ASSERT_FALSE(blockSetIgnoredLine(&testBlock, ""));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_EMPTYLINE);  

   CU_ASSERT_FALSE(blockSetIgnoredLine(&testBlock, "Test\nTest"));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_NEWLINE);  

   CU_ASSERT(blockSetIgnoredLine(&testBlock, "Test"));
   CU_ASSERT_STRING_EQUAL(testBlock.ignoredLine, "Test");

   // Test for text-type blocks
   CU_ASSERT(blockReset(&testBlock, BLOCK_TEXT));
   CU_ASSERT_EQUAL(testBlock.indent, 0);

   CU_ASSERT_FALSE(blockSetIndent(&testBlock, -1));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_NEGATIVEINDENT);  

   CU_ASSERT(blockSetIndent(&testBlock, 5));
   CU_ASSERT_EQUAL(testBlock.indent, 5);

   CU_ASSERT_FALSE(blockAddWord(&testBlock, ""));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_EMPTYWORD);  

   CU_ASSERT_FALSE(blockAddWord(&testBlock, "Test Test"));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_WHITESPACE);  

   CU_ASSERT_FALSE(blockAddWord(&testBlock, "Test\nTest"));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_WHITESPACE);  

   CU_ASSERT_FALSE(blockAddWord(&testBlock, "Test\tTest"));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_WHITESPACE);  

   CU_ASSERT(blockAddWord(&testBlock, "Test"));
   CU_ASSERT_STRING_EQUAL(testBlock.words[0], "Test");
  
}


void test_blockOut(void) {
   block testBlock;
   char out[2000];
   
   out[0] = '\0';
   
   // test for ignored lines
   blockReset(&testBlock, BLOCK_IGNOREDLINE);
   blockSetIgnoredLine(&testBlock, "Test");

   CU_ASSERT_FALSE(blockOut(&testBlock, out, 0));
   CU_ASSERT_EQUAL(errorNumber(),BLOCK_BADLINESIZE);
   
   blockOut(&testBlock, out, 1);
   CU_ASSERT_STRING_EQUAL(out, "Test\n");

   blockOut(&testBlock, out, 1);
   CU_ASSERT_STRING_EQUAL(out, "Test\nTest\n");
   
   out[0] = '\0';
   blockReset(&testBlock, BLOCK_IGNOREDLINE);
   blockSetIgnoredLine(&testBlock, "Test");
   CU_ASSERT(blockSetIndent(&testBlock, 3));
   CU_ASSERT(blockOut(&testBlock, out, 1));
   CU_ASSERT_STRING_EQUAL(out, "   Test\n");
   

   // tests for text blocks
   
   // empty block
   out[0] = '\0';
   blockReset(&testBlock, BLOCK_TEXT);
   blockSetIndent(&testBlock, 4);
   CU_ASSERT(blockOut(&testBlock, out, 3));
   CU_ASSERT_STRING_EQUAL(out, "\n");
   // bad indent size
   out[0] = '\0';
   blockSetIndent(&testBlock, 4);
   blockAddWord(&testBlock, "Test");
   CU_ASSERT_FALSE(blockOut(&testBlock, out, 3));
   CU_ASSERT_EQUAL(errorNumber(), BLOCK_INDENTLARGERTHANLINESIZE); 
   CU_ASSERT_STRING_EQUAL(out, "");
   // Simple 1 word case
   CU_ASSERT(blockOut(&testBlock, out, 20)); 
   CU_ASSERT_STRING_EQUAL(out, "    Test\n");
   // A word that goes over the margin
   out[0] = '\0';
   blockReset(&testBlock, BLOCK_TEXT);
   blockSetIndent(&testBlock, 4);
   blockAddWord(&testBlock, "word567890");
   CU_ASSERT(blockOut(&testBlock, out, 10)); 
   CU_ASSERT_STRING_EQUAL(out, "    word567890\n");
   // Simple multiword case
   out[0] = '\0';
   blockReset(&testBlock, BLOCK_TEXT);
   blockSetIndent(&testBlock, 0);
   blockAddWord(&testBlock, "Word1");
   blockAddWord(&testBlock, "Word2");
   blockAddWord(&testBlock, "Word3");
   blockAddWord(&testBlock, "Word4");     
   CU_ASSERT(blockOut(&testBlock, out, 80)); 
   CU_ASSERT_STRING_EQUAL(out, "Word1 Word2 Word3 Word4\n");
   // Multiword with linesize exactly equal to word size
   out[0]='\0';
   CU_ASSERT(blockOut(&testBlock, out, 5)); 
   CU_ASSERT_STRING_EQUAL(out, "Word1\nWord2\nWord3\nWord4\n");
   // Multiword with linesize exactly equal to word size-1
   out[0]='\0';
   CU_ASSERT(blockOut(&testBlock, out, 4)); 
   CU_ASSERT_STRING_EQUAL(out, "Word1\nWord2\nWord3\nWord4\n");
   // Multiword with linesize exactly equal to word size+1
   out[0]='\0';
   CU_ASSERT(blockOut(&testBlock, out, 6)); 
   CU_ASSERT_STRING_EQUAL(out, "Word1\nWord2\nWord3\nWord4\n");
   // Multiword with linesize exactly equal to word size+2
   out[0]='\0';
   CU_ASSERT(blockOut(&testBlock, out, 7));
   CU_ASSERT_STRING_EQUAL(out, "Word1\nWord2\nWord3\nWord4\n");
   // Simple multiline case
   out[0]='\0';
   CU_ASSERT(blockOut(&testBlock, out, 11)); 
   //puts("\n----5----0");
   //puts(out);
   //puts("----5----0"); 
   CU_ASSERT_STRING_EQUAL(out, "Word1 Word2\nWord3 Word4\n");
   // Simple multiline case with indent
   out[0]='\0';
   blockSetIndent(&testBlock, 4);
   CU_ASSERT(blockOut(&testBlock, out, 15)); 
   CU_ASSERT_STRING_EQUAL(out, "    Word1 Word2\n    Word3 Word4\n");
   // Multiword with linesize exactly equal to word size + indent
   out[0]='\0';
   blockSetIndent(&testBlock, 4);
   CU_ASSERT(blockOut(&testBlock, out, 9)); 
   CU_ASSERT_STRING_EQUAL(out, "    Word1\n    Word2\n    Word3\n    Word4\n");
   // Basic UTF-8 test
   out[0] = '\0';
   blockReset(&testBlock, BLOCK_TEXT);
   blockSetIndent(&testBlock, 0);
   blockAddWord(&testBlock, "amāre");
   blockAddWord(&testBlock, "Jardín");
   blockAddWord(&testBlock, "Tazón");
   blockAddWord(&testBlock, "Ὀδυσσεύς");     
   CU_ASSERT(blockOut(&testBlock, out, 12)); 
   CU_ASSERT_STRING_EQUAL(out, "amāre Jardín\nTazón\nὈδυσσεύς\n");
   out[0] = '\0';
   CU_ASSERT(blockOut(&testBlock, out, 14)); 
   CU_ASSERT_STRING_EQUAL(out, "amāre Jardín\nTazón Ὀδυσσεύς\n");  
   //puts(out); 

}

int main(void){

   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("General Tests", NULL, NULL);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   ;
   
   if ( 
      (NULL == CU_add_test(pSuite, "UTF8 Ops", test_utf8))  ||
      (NULL == CU_add_test(pSuite, "stringStack", test_stringStack))  ||
     (NULL == CU_add_test(pSuite, "Basic Block Ops", test_basicBlockOperations))  ||
     (NULL == CU_add_test(pSuite, "Block Out Ops", test_blockOut))  ||
     (NULL == CU_add_test(pSuite, "Basic Paragraph", test_basicParagraph)) ||
      (NULL == CU_add_test(pSuite, "Beautify", test_beautify))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error(); 

   
}


