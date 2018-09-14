#ifndef _ZSERVER_H
#define _ZSERVER_H

#if __cplusplus
extern "C" {
#endif 

void zserver_start(int port);
void zserver_stop();

void zserver_commit(int timestep, struct ZC_DataProperty*, struct ZC_CompareData *d);
void zserver_commit_field_data(int tiemstep, int W, int H, double *p, double *q);

#if __cplusplus
}
#endif

#endif
