/**
 *  @file ZC_manyArgs.c
 *  @author Sheng Di
 *  @date June, 2017
 *  @brief 
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "ZC_manyArgs.h"

inline SEXP lang7(SEXP s, SEXP t, SEXP u, SEXP v, SEXP w, SEXP x, SEXP y)
{
    PROTECT(s);
    s = LCONS(s, list6(t, u, v, w, x, y));
    UNPROTECT(1);
    return s;
}
