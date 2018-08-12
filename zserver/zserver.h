#ifndef _ZSERVER_H
#define _ZSERVER_H

#if __cplusplus
extern "C" {
#endif 

void zserver_start(int port);
void zserver_stop();

void zserver_commit(int timestep, struct ZC_DataProperty*, struct ZC_CompareData *d);

#if __cplusplus
}
#endif

#endif
