/**
 *  @file ReportGenerator.h
 *  @author Sheng Di
 *  @date April, 2016
 *  @brief Header file for generating report.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ReportGenerator_H
#define _ReportGenerator_H

#include "rw.h"
#include "latex.h"

#ifdef __cplusplus
extern "C" {
#endif

StringLine* ZC_generateCompressionRateFigure();
void ZC_generateCompressionRateReport();

StringLine* ZC_generateDecompressionRateFigure();
void ZC_generateDecompressionRateReport();

StringLine* ZC_generatePSNRFigure();
void ZC_generatePSNRReport();

void ZC_generateErrDistributionReport();

void ZC_generateErrAutoCorrReport();
void ZC_generateRateDistortionReport();

StringLine* ZC_generateCompressionFactorFigure();
void ZC_generateCompressionFactorReport();

void ZC_generateSpectrumDistortionReport();

void ZC_updateZCRootTexFile(char* dataSetName);
void ZC_generateOverallReport(char* dataSetName);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ReportGenerator_H  ----- */
