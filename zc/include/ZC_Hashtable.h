/**
 *  @file ZC_Hashtable.h
 *  @author Sheng Di
 *  @date July, 2016
 *  @brief Header file for the ZC_Hashtable.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_Hashtable_H
#define _ZC_Hashtable_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct entry_t {
	char *key;
	void* value; //previously, it was ZC_CompareData* value, which I think is wrong. So, I changed it to void*. - shdi
	struct entry_t *next;
} entry_t;

typedef struct hashtable_t {
	int capacity;
	int count;
	struct entry_t **table;	
} hashtable_t;

extern hashtable_t *ecPropertyTable; //ecPropertyTable contains the properties.
extern hashtable_t *ecCompareDataTable; //ecCompareDataTable contains all compareData cases.
extern hashtable_t *ecVisDecDataTables; //ecVisDecDataTables contains the required error bound settings for visualization of decompressed data

//char *strdup(const char *str);
int checkStartsWith(char* str, const char* key);
hashtable_t *ht_create( int capacity );
int ht_hash( hashtable_t *hashtable, const char *key );
entry_t *ht_newpair( const char *key, void *value );
void ht_set( hashtable_t *hashtable, const char *key, void *value );
void *ht_get( hashtable_t *hashtable, const char *key );
void ht_freeTable( hashtable_t *hashtable);
void* ht_freePairEntry( hashtable_t *hashtable, const char* key);

char** ht_getAllKeys(hashtable_t *hashtable);
int ht_getElemCount(hashtable_t *hashtable);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Hashtable_H  ----- */
