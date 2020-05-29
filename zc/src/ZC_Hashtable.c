#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "ZC_Hashtable.h"
#include "zc.h"

hashtable_t *ecPropertyTable = NULL;
hashtable_t *ecCompareDataTable = NULL;
hashtable_t *ecVisDecDataTables = NULL; 
//data structure of ecVisDecDataTables: 
//<1:VarCount> varName(string) --> compressorCRMap;
//	for each compressorCRMap: 
//		compressorName(string) --> compressorCRVisElement (char* compressorName, ZC_CompareData *compressionResults[32], ht_table* CRVisDataMap);
//		for each CRVisDataMap:
//			CR(string) --> ZCVisDecDataElement(char* compressorName, double errorSetting, ZC_CompareData* compressionResult); see ZC_CompareData.h for details; 

/*char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
        strcpy(dup, str);
    return dup;
}*/

int checkStartsWith(char* str, const char* key)
{
	int n = strlen(key);
	int r = strncmp(str, key, n);
	if(r==0)
		return 1;
	else
		return 0;
}

/* Create a new hashtable. */
hashtable_t *ht_create( int capacity ) {

	hashtable_t *hashtable = NULL;
	int i;

	if( capacity < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashtable = (hashtable_t *)malloc( sizeof( hashtable_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = (entry_t**)malloc( sizeof( entry_t * ) * capacity ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < capacity; i++ ) {
		hashtable->table[i] = NULL;
	}

	hashtable->count = 0;
	hashtable->capacity = capacity;

	return hashtable;	
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, const char *key ) {

	unsigned long int hashval = 0;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) 
	{
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % hashtable->capacity;
}

/* Create a key-value pair. */
entry_t *ht_newpair( const char *key, void *value ) {
	entry_t *newpair;

	if( ( newpair = (entry_t*)malloc( sizeof( entry_t ) ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) {
		return NULL;
	}
	
	newpair->value = value;

	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( hashtable_t *hashtable, const char *key, void *value ) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	//deal with possible conflict
	while( next != NULL && next->key != NULL && strcmp( key, next->key ) != 0 ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

		free( next->value );
		next->value = value;

	/* Nope, could't find it.  Time to grow a pair. */
	} else {
		newpair = ht_newpair( key, value );

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
	
		/* We're at the end of the linked list in this bin. */
		} else if ( next == NULL ) {
			last->next = newpair;
	
		/* We're in the middle of the list. */
		} else  {
			newpair->next = next;
			last->next = newpair;
		}
		hashtable->count ++;
	}
}

/* Retrieve a key-value pair from a hash table. */
void *ht_get( hashtable_t *hashtable, const char *key ) {
	int bin = 0;
	entry_t *pair;

	bin = ht_hash( hashtable, key );

	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) != 0 ) {
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
		return NULL;

	} else {
		return pair->value;
	}	
}

void ht_freeTable( hashtable_t *hashtable)
{
	if(hashtable==NULL)
		return;
	int i;
	struct entry_t * pairEntry, * tmp;
	for(i = 0; i < hashtable->capacity; i++ ) {
		pairEntry = hashtable->table[i];
		while(pairEntry != NULL)
		{	
			//TODO: free contents of hashtable->table[i]
			/*The -->value should be freed in a specific way, like free_Property().*/
			if(pairEntry->key!=NULL)
				free(pairEntry->key);
			tmp = pairEntry->next;	
			free(pairEntry);
			pairEntry = tmp;
		}
	}	
	free(hashtable->table);
	free(hashtable);
}

/**
 * Note that either ht_freePairEntry() or ht_freeTable() does not free the value actually. 
 * 
 * return: non-zero means found it and remove it; NULL means missing (didn't found the key)
 * */
void* ht_freePairEntry( hashtable_t *hashtable, const char* key)
{
	int i, flag = 0;
	if(key==NULL)
	 return NULL;
	 
	void* found = NULL;
	struct entry_t * pairEntry, * prePair;
	for(i=0;i<HASHTABLE_SIZE;i++)
	{
		pairEntry = hashtable->table[i];
		if(pairEntry == NULL)
			continue;
		prePair = NULL;
		while(pairEntry != NULL)
		{	
			if(strcmp(pairEntry->key, key)==0)
			{//free it and break;
				if(prePair==NULL)
				{
					hashtable->table[i] = pairEntry->next;
					if(pairEntry->key!=NULL)
						free(pairEntry->key);
					if(pairEntry->value!=NULL)
						found = pairEntry->value;
					free(pairEntry);
				}
				else
				{
					prePair->next = pairEntry->next;
					if(pairEntry->key!=NULL)
						free(pairEntry->key);
					if(pairEntry->value!=NULL)
						found = pairEntry->value;	
					free(pairEntry);					
				}
				flag = 1;
				break;
			}
			prePair = pairEntry;
			pairEntry = pairEntry->next;	
		}		
		if(flag)
			break;
	}
	if(flag)
		hashtable->count--;
	return found;
}

char** ht_getAllKeys(hashtable_t *hashtable)
{
	int i, j = 0;
	char** result = (char**)malloc(hashtable->count*sizeof(char*));
	for(i=0;i<hashtable->capacity&&j<hashtable->count;i++)
	{
		entry_t* t = hashtable->table[i];
		while(t!=NULL)
		{
			result[j++] = t->key;
			t = t->next;
		}
	}
	return result;
}

int ht_getElemCount(hashtable_t *hashtable)
{
	return hashtable->count;
}
