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

#include "zc.h"
#include "ZC_rw.h"
#include "ZC_latex.h"
#include "ZC_quicksort.h"

#ifdef __cplusplus
extern "C" {
#endif

StringLine* ZC_generatePropertyAnalysisTable(char** varCases, int varCaseCount);

void ZC_extractCompressorAndErrorBounds(char** compressionCaseFiles, int caseCount);
void ZC_constructSortedSelectedErrorBounds(CmprsorErrBound* compressor);
void ZC_constructSortedSelectedErrorBounds4CmprsEelments(CmprsorErrBound *allCompressors, int allCompressorCount);

StringLine* ZC_generateDataPropertyAnalysisFigures(char** caseNames, int caseNameCount);
StringLine* ZC_generateDataSliceImages(char** caseNames, int caseNameCount);

void ZC_generateDataPropertyAnalysisReport();

StringLine* ZC_generateCompressionRateFigure();
void ZC_generateCompressionRateReport();

StringLine* ZC_generateDecompressionRateFigure();
void ZC_generateDecompressionRateReport();

StringLine* ZC_generatePSNRFigure();
void ZC_generatePSNRReport();

char* selectCmdTemplate(const char* compressorName, int compressionMode, int dim);
void ZC_executeCompDecomp_basedon_CmprVisE(CompressorCRVisElement* cmprVisE);
void ZC_generateDecVisData();

StringLine* ZC_generateCompressionFactorFigure();

StringLine* ZC_generateStaticAnalysisFigures(const char* metricType, CmprsorErrBound *allCompressors, int allCompressorCount);

StringLine* ZC_generateRateDistortionFigure();
StringLine* ZC_generateOverallRateDistortionFigure();

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ReportGenerator_H  ----- */
