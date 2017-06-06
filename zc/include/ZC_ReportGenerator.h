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

#include "ZC_rw.h"
#include "latex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StringElem_t
{
	char* str;
	double value;
} *StringElem;

StringLine* ZC_generatePropertyAnalysisTable(char** varCases, int varCaseCount);

void ZC_extractCompressorAndErrorBounds(char** compressionCaseFiles, int caseCount);
void ZC_constructSortedSelectedErrorBounds(StringElem* selectedErrorBounds, int *selectedErrorBoundCount);

StringLine* ZC_generateDataPropertyAnalysisFigures(char** caseNames, int caseNameCount);
void ZC_generateDataPropertyAnalysisReport();

StringLine* ZC_generateCompressionRateFigure();
void ZC_generateCompressionRateReport();

StringLine* ZC_generateDecompressionRateFigure();
void ZC_generateDecompressionRateReport();

StringLine* ZC_generatePSNRFigure();
void ZC_generatePSNRReport();

StringLine* ZC_generateCompressionFactorFigure();
void ZC_generateCompressionFactorReport();

void ZC_generateRateDistortionReport();

void ZC_generateRateCorrelationReport();

StringLine* ZC_generateStaticAnalysisFigures(char* metricType, StringElem* selectedErrorBounds, int selectedErrorBoundCount);

void ZC_generateErrDistributionReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount);
void ZC_generateErrAutoCorrReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount);
void ZC_generateSpectrumDistortionReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount);

void ZC_updateZCRootTexFile(char* dataSetName);
void ZC_generateOverallReport(char* dataSetName);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ReportGenerator_H  ----- */
