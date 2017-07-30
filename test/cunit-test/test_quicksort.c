#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"

#include "zc.h"

#include <stdio.h>  // for printf
#include <string.h>
#include <stdlib.h>

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

void test_ZC_quick_sort2(void)
{
	int i = 0; 
	StringElem selectedErrBounds[10];
	for(i=0;i<10;i++)
		selectedErrBounds[i] = (StringElem)malloc(sizeof(StringElem));

	selectedErrBounds[0]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[0]->str, "v1");
	selectedErrBounds[0]->value = 1;	
	
	selectedErrBounds[1]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[1]->str, "v3");	
	selectedErrBounds[1]->value = 3;	
	
	selectedErrBounds[2]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[2]->str, "v2");	
	selectedErrBounds[2]->value = 2;	

	selectedErrBounds[3]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[3]->str, "v9");	
	selectedErrBounds[3]->value = 9;	

	selectedErrBounds[4]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[4]->str, "v4");	
	selectedErrBounds[4]->value = 4;	

	selectedErrBounds[5]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[5]->str, "v8");	
	selectedErrBounds[5]->value = 8;	

	selectedErrBounds[6]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[6]->str, "v0");	
	selectedErrBounds[6]->value = 0;	

	selectedErrBounds[7]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[7]->str, "v7");	
	selectedErrBounds[7]->value = 7;	

	selectedErrBounds[8]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[8]->str, "v6");	
	selectedErrBounds[8]->value = 6;	

	selectedErrBounds[9]->str = (char*)malloc(10);
	strcpy(selectedErrBounds[9]->str, "v5");	
	selectedErrBounds[9]->value = 5;	
	
	ZC_quick_sort2(selectedErrBounds, 0, 9);
	
	StringElem expectedErrBounds[10];
	for(i=0;i<10;i++)
		expectedErrBounds[i] = (StringElem)malloc(sizeof(StringElem));
			
	for(i=0;i<10;i++)
	{
		expectedErrBounds[i]->str = (char*)malloc(10);
		sprintf(expectedErrBounds[i]->str, "v%d", 9 - i);
		expectedErrBounds[i]->value = 9 - i;
	}
	
	//verify
	printf("\n");
	for(i=0;i<10;i++)
	{
		printf("selectedErrBounds[%d]->str=%s : %f\n", i, selectedErrBounds[i]->str, selectedErrBounds[i]->value);
		CU_ASSERT_STRING_EQUAL(selectedErrBounds[i]->str, expectedErrBounds[i]->str);
		CU_ASSERT_EQUAL(selectedErrBounds[i]->value, expectedErrBounds[i]->value);
	}
	
	for(i=0;i<10;i++)
	{
		free(selectedErrBounds[i]->str);
		free(selectedErrBounds[i]);
	
		free(expectedErrBounds[i]->str);
		free(expectedErrBounds[i]);
	}
}


void test_checkAndAddStringToList(void)
{
	//TODO
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "test_quicksort_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "test_ZC_quick_sort2", test_ZC_quick_sort2)) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
/*
   // Run all tests using the automated interface
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // Run all tests using the console interface
   CU_console_run_tests();
*/
   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
}
