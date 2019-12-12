/**
 *  @file ByteToolkit.c
 *  @author Sheng Di
 *  @date April, 2016
 *  @brief Byte Toolkit
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
 
#include <stdlib.h>
#include "ZC_ByteToolkit.h"
#include "zc.h" 
	
int ZC_bytesToInt_bigEndian(unsigned char* bytes)
{
	int temp = 0;
	int res = 0;
	
	res <<= 8;
	temp = bytes[0] & 0xff;
	res |= temp;	

	res <<= 8;
	temp = bytes[1] & 0xff;
	res |= temp;

	res <<= 8;
	temp = bytes[2] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = bytes[3] & 0xff;
	res |= temp;		
	
	return res;
}

/**
 * @unsigned char *b the variable to store the converted bytes (length=4)
 * @unsigned int num
 * */
void ZC_intToBytes_bigEndian(unsigned char *b, unsigned int num)
{
	b[0] = (unsigned char)(num >> 24);	
	b[1] = (unsigned char)(num >> 16);	
	b[2] = (unsigned char)(num >> 8);	
	b[3] = (unsigned char)(num);	
	
	//note: num >> xxx already considered endian_type...
//if(dataEndianType==LITTLE_ENDIAN_DATA)
//		ZC_symTransform_4bytes(*b); //change to BIG_ENDIAN_DATA
}

/**
 * @endianType: refers to the endian_type of unsigned char* b.
 * */
long ZC_bytesToLong_bigEndian(unsigned char* b) {
	long temp = 0;
	long res = 0;
	//int i;

	res <<= 8;
	temp = b[0] & 0xff;
	res |= temp;

	res <<= 8;
	temp = b[1] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[2] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[3] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[4] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[5] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[6] & 0xff;
	res |= temp;
	
	res <<= 8;
	temp = b[7] & 0xff;
	res |= temp;						
	
	return res;
}

void ZC_longToBytes_bigEndian(unsigned char *b, unsigned long num) 
{
	b[0] = (unsigned char)(num>>56);
	b[1] = (unsigned char)(num>>48);
	b[2] = (unsigned char)(num>>40);
	b[3] = (unsigned char)(num>>32);
	b[4] = (unsigned char)(num>>24);
	b[5] = (unsigned char)(num>>16);
	b[6] = (unsigned char)(num>>8);
	b[7] = (unsigned char)(num);
//	if(dataEndianType==LITTLE_ENDIAN_DATA)
//		ZC_symTransform_8bytes(*b);
}


long ZC_doubleToOSEndianLong(double value)
{
	ecldouble buf;
	buf.value = value;
	return buf.lvalue;
}

int ZC_floatToOSEndianInt(float value)
{
	eclfloat buf;
	buf.value = value;
	return buf.ivalue;
}

/**
 * By default, the endian type is OS endian type.
 * */
short ZC_bytesToShort(unsigned char* bytes)
{
	eclint16 buf;
	memcpy(buf.byte, bytes, 2);
	
	return buf.svalue;
}

int ZC_bytesToInt(unsigned char* bytes)
{
	eclfloat buf;
	memcpy(buf.byte, bytes, 4);
	return buf.ivalue;
}

long ZC_bytesToLong(unsigned char* bytes)
{
	ecldouble buf;
	memcpy(buf.byte, bytes, 8);
	return buf.lvalue;
}

//the byte to input is in the big-endian format
float ZC_bytesToFloat(unsigned char* bytes)
{
	eclfloat buf;
	memcpy(buf.byte, bytes, 4);
	if(sysEndianType==LITTLE_ENDIAN_SYSTEM)
		ZC_symTransform_4bytes(buf.byte);	
	return buf.value;
}

void ZC_floatToBytes(unsigned char *b, float num)
{
	eclfloat buf;
	buf.value = num;
	memcpy(b, buf.byte, 4);
	if(sysEndianType==LITTLE_ENDIAN_SYSTEM)
		ZC_symTransform_4bytes(b);		
}

//the byte to input is in the big-endian format
double ZC_bytesToDouble(unsigned char* bytes)
{
	ecldouble buf;
	memcpy(buf.byte, bytes, 8);
	if(sysEndianType==LITTLE_ENDIAN_SYSTEM)
		ZC_symTransform_8bytes(buf.byte);
	return buf.value;
}

void ZC_doubleToBytes(unsigned char *b, double num)
{
	ecldouble buf;
	buf.value = num;
	memcpy(b, buf.byte, 8);
	if(sysEndianType==LITTLE_ENDIAN_SYSTEM)
		ZC_symTransform_8bytes(b);
}
