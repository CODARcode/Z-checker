#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "CUnit_Array.h"

#include "zc.h"

#include <stdio.h>  // for printf

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

void test_ZC_bytesToInt_bigEndian(void)
{
	unsigned char bytes[4] = {0,0,0,1};
	CU_ASSERT_EQUAL(ZC_bytesToInt_bigEndian(bytes), 1);
	unsigned char bytes2[4] = {1,2,3,4};
	CU_ASSERT_EQUAL(ZC_bytesToInt_bigEndian(bytes2), 16909060);
}

void test_ZC_intToBytes_bigEndian(void)
{
	int value[2] = {1,16909060};
	unsigned char bytes[4] = {0,0,0,1};
	unsigned char bytes2[4] = {1,2,3,4};
	unsigned char bytesBuf[4];
	ZC_intToBytes_bigEndian(bytesBuf, value[0]);
	CU_ASSERT_EQUAL_ARRAY_BYTE(bytesBuf, bytes, 4);
	ZC_intToBytes_bigEndian(bytesBuf, value[1]);
	CU_ASSERT_EQUAL_ARRAY_BYTE(bytesBuf, bytes2, 4);
}

void test_ZC_intToBytes_bytesToInt_bigEndian(void)
{
	int i = 0, value;
	unsigned char bytesBuf[4];

	for(i=0;i<1000000;i+=100000)
	{
		ZC_intToBytes_bigEndian(bytesBuf, i);
		value = ZC_bytesToInt_bigEndian(bytesBuf);	
		CU_ASSERT_EQUAL(value, i);
	}
}

void test_ZC_bytesToLong_bigEndian(void)
{
	unsigned char bytes[8] = {0, 0, 0, 0, 0, 0, 0, 1};
	CU_ASSERT_EQUAL(ZC_bytesToLong_bigEndian(bytes), 1);
	unsigned char bytes2[8] = {0, 0, 0, 0, 1, 2, 3, 4};
	CU_ASSERT_EQUAL(ZC_bytesToLong_bigEndian(bytes2), 16909060);
}

void test_ZC_longToBytes_bigEndian(void)
{
	long value[2] = {1,16909060};
	unsigned char bytes[8] = {0,0,0,0,0,0,0,1};
	unsigned char bytes2[8] = {0,0,0,0,1,2,3,4};
	unsigned char bytesBuf[8];
	ZC_longToBytes_bigEndian(bytesBuf, value[0]);
	CU_ASSERT_EQUAL_ARRAY_BYTE(bytesBuf, bytes, 8);
	ZC_longToBytes_bigEndian(bytesBuf, value[1]);
	CU_ASSERT_EQUAL_ARRAY_BYTE(bytesBuf, bytes2, 8);	
}

void test_ZC_longToBytes_bytesToLong_bigEndian(void)
{
	long i = 0, value;
	unsigned char bytesBuf[8];

	for(i=1;i<2000000000;i+=500000000)
	{
		ZC_longToBytes_bigEndian(bytesBuf, i);
		value = ZC_bytesToLong_bigEndian(bytesBuf);	
		CU_ASSERT_EQUAL(value, i);
	}
}

void test_ZC_doubleToOSEndianLong(void)
{
	//deprecated
}

void test_ZC_floatToOSEndianInt(void)
{
	//deprecated
}

void test_ZC_bytesToShort(void)
{
	if(dataEndianType == LITTLE_ENDIAN_DATA)
	{	
		unsigned char bytes[2] = {1,3};
		CU_ASSERT_EQUAL(ZC_bytesToShort(bytes), 769);
	}
}

void test_ZC_bytesToInt(void)
{
	if(dataEndianType == LITTLE_ENDIAN_DATA)
	{	
		unsigned char bytes[4] = {1,2,3,4};
		CU_ASSERT_EQUAL(ZC_bytesToInt(bytes), 67305985);
		unsigned char bytes2[4] = {100,50,25,12};
		CU_ASSERT_EQUAL(ZC_bytesToInt(bytes2), 202977892);
	}
}

void test_ZC_bytesToLong(void)
{
	if(dataEndianType == LITTLE_ENDIAN_DATA)
	{
		long expected = 578437695752307201;
		unsigned char bytes[8] = {1,2,3,4,5,6,7,8};
		CU_ASSERT_EQUAL(ZC_bytesToLong(bytes), expected);		
	}
}

void test_ZC_bytesToFloat(void)
{
	//see test_floatToBytes_bytesToFloat
}

void test_ZC_floatToBytes(void)
{
	//see test_floatToBytes_bytesToFloat
}

void test_ZC_floatToBytes_bytesToFloat()
{
	float value = 123.456;
	unsigned char bytes[4];
	ZC_floatToBytes(bytes, value);
	float newValue = ZC_bytesToFloat(bytes);
	CU_ASSERT_DOUBLE_EQUAL(value, newValue, 1E-4);
}

void test_ZC_bytesToDouble(void)
{
	//see test_doubleToBytes_bytesToDouble
}

void test_ZC_doubleToBytes(void)
{
	//see test_doubleToBytes_bytesToDouble
}

void test_ZC_doubleToBytes_bytesToDouble()
{
	double value = 123.456;
	unsigned char bytes[4];
	ZC_doubleToBytes(bytes, value);
	double newValue = ZC_bytesToDouble(bytes);
	CU_ASSERT_DOUBLE_EQUAL(value, newValue, 1E-10);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "test_ByteToolkit_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "test_ZC_bytesToInt_bigEndian", test_ZC_bytesToInt_bigEndian)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_intToBytes_bigEndian", test_ZC_intToBytes_bigEndian)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_intToBytes_bytesToInt_bigEndian", test_ZC_intToBytes_bytesToInt_bigEndian)) ||
		(NULL == CU_add_test(pSuite, "test_ZC_bytesToLong_bigEndian", test_ZC_bytesToLong_bigEndian)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_longToBytes_bigEndian", test_ZC_longToBytes_bigEndian)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_longToBytes_bytesToLong_bigEndian", test_ZC_longToBytes_bytesToLong_bigEndian)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_bytesToShort", test_ZC_bytesToShort)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_bytesToInt", test_ZC_bytesToInt)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_bytesToLong", test_ZC_bytesToLong)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_floatToBytes_bytesToFloat", test_ZC_floatToBytes_bytesToFloat)) ||
        (NULL == CU_add_test(pSuite, "test_ZC_doubleToBytes_bytesToDouble", test_ZC_doubleToBytes_bytesToDouble))
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
