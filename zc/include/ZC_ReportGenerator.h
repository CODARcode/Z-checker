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

#ifdef __cplusplus
extern "C" {
#endif

void ZC_generateCompressionRateReport();
void ZC_generateDecompressionRateReport();
void ZC_generatePSNRReport();
void ZC_generateErrDistributionReport();
void ZC_generateErrAutoCorrReport();
void ZC_generateRateDistortionReport();
void ZC_generateCompressionFactorReport();
void ZC_generateSpectrumDistortionReport();
void ZC_generateOverallReport();

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ReportGenerator_H  ----- */
