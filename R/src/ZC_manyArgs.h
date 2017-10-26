/**
 *  @author Sheng Di
 *  @date June, 2017
 *  @brief Supporting 6+ arguments
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_ManyArgs_H
#define _ZC_ManyArgs_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Rinternals.h>
#include <Rembedded.h>

SEXP lang7(SEXP s, SEXP t, SEXP u, SEXP v, SEXP w, SEXP x, SEXP y);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_ManyArgs_H  ----- */
