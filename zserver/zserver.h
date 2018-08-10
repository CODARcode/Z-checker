#ifndef _ZSERVER_H
#define _ZSERVER_H

#if __cplusplus
extern "C" {
#endif 

void zserver_start(int port);
void zserver_stop();
void zserver_commit_file(const char *key, const char *filename);

void zserver_commit_val(const char *key, double val);
void zserver_commit_vec(const char *key, int length, double *vals);

void zserver_commit(int timestep, struct ZC_DataProperty*, struct ZC_CompareData *d);

#if __cplusplus
}
#endif

#endif
