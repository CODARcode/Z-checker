/**
 *  @file ZC_ByteToolkit.h
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief Header file for the whole ByteToolkit.c.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_ByteToolkit_H
#define _ZC_ByteToolkit_H

#include <stdio.h>

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ZC_bytesToInt_bigEndian(unsigned char* bytes);
void ZC_intToBytes_bigEndian(unsigned char *b, unsigned int num);
long ZC_bytesToLong_bigEndian(unsigned char* b);
void ZC_longToBytes_bigEndian(unsigned char *b, unsigned long num);
long ZC_doubleToOSEndianLong(double value);
int ZC_floatToOSEndianInt(float value);

short ZC_bytesToShort(unsigned char* bytes);
int ZC_bytesToInt(unsigned char* bytes);
long ZC_bytesToLong(unsigned char* bytes);
float ZC_bytesToFloat(unsigned char* bytes);
void ZC_floatToBytes(unsigned char *b, float num);
double ZC_bytesToDouble(unsigned char* bytes);
void ZC_doubleToBytes(unsigned char *b, double num);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_ByteToolkit_H  ----- */
