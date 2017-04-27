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
	ZC_CompareData* value;
	struct entry_t *next;
} entry_t;

typedef struct hashtable_t {
	int capacity;
	int count;
	struct entry_t **table;	
} hashtable_t;

extern hashtable_t *ecPropertyTable;
extern hashtable_t *ecCompareDataTable;

int checkStartsWith(char* str, char* key);
hashtable_t *ht_create( int capacity );
int ht_hash( hashtable_t *hashtable, char *key );
entry_t *ht_newpair( char *key, void *value );
void ht_set( hashtable_t *hashtable, char *key, void *value );
void *ht_get( hashtable_t *hashtable, char *key );
void ht_freeTable( hashtable_t *hashtable);

char** ht_getAllKeys(hashtable_t *hashtable);
int ht_getElemCount(hashtable_t *hashtable);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Hashtable_H  ----- */
