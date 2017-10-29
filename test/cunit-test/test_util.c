#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"

#include "zc.h"

#include <stdio.h>  // for printf
#include <string.h>

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

void test_ltrim(void)
{
	char str[20] = {0};
	strcpy(str, "\t test123");
	char expected[20];
	strcpy(expected, "test123");
	ltrim(str);
	CU_ASSERT_STRING_EQUAL(str, expected);
}

void test_rtrim(void)
{
	char str[20] = {0};
	strcpy(str, "test123\t ");
	char expected[] = "test123";
	rtrim(str);
	CU_ASSERT_STRING_EQUAL(str, expected);
}

void test_trim(void)
{
	char str[] = "\t test123\t ";
	char expected[] = "test123";
	trim(str);
	CU_ASSERT_STRING_EQUAL(str, expected);
}

void test_rtrim_C(void)
{
	char str[] = "0.001)";
	char expected[] = "0.001";
	char *result = rtrim_C(str, ')');
	CU_ASSERT_STRING_EQUAL(result, expected);
	free(result);
}

void test_ZC_executeCmd_GfloatVector(void)
{
	//deprecated
}

void test_ZC_executeCmd_GdoubleVector(void)
{
	//deprecated
}

void test_ZC_executeCmd_RfloatVector(void)
{
	//deprecated
}

void test_ZC_executeCmd_RdoubleVector(void)
{
	//deprecated
}

void test_ZC_executeCmd_RfloatMatrix(void)
{
	//deprecated
}

void test_ZC_executeCmd_RdoubleMatrix(void)
{
	//deprecated
}

void test_ZC_ReplaceStr2(void)
{
	char str[20];
	strcpy(str, "test_1");
	char expected[] = "test\\\\_1";
	ZC_ReplaceStr2(str, "_", "\\\\_");
	CU_ASSERT_STRING_EQUAL(str, expected);
}

void test_strcat_new(void)
{
	char str1[100] = {0};
	char str2[100] = {0};
	strcpy(str1, "test1");
	strcpy(str2, "test2");
	char *result = strcat_new(str1, str2);
	
	char expected[] = "test1test2";
	CU_ASSERT_STRING_EQUAL(result, expected);
	
	free(result);
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
   pSuite = CU_add_suite( "test_util_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "test_ltrim", test_ltrim)) ||
		(NULL == CU_add_test(pSuite, "test_rtrim", test_rtrim)) ||
		(NULL == CU_add_test(pSuite, "test_trim", test_trim)) ||
		(NULL == CU_add_test(pSuite, "test_rtrim_C", test_rtrim_C)) ||
		(NULL == CU_add_test(pSuite, "test_ZC_ReplaceStr2", test_ZC_ReplaceStr2)) ||
		(NULL == CU_add_test(pSuite, "test_strcat_new", test_strcat_new))
      )
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
