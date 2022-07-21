#include "coptmex.h"

extern int COPT_SearchParamAttr(copt_prob *prob, const char *name, int *p_type);

extern int COPT_GetPSDSolution(copt_prob* prob,
                               double* psdColValue,
                               double* psdRowSlack,
                               double* psdRowDual,
                               double* psdColDual);

extern int COPT_LoadConeProb(copt_prob* prob,
  int nCol,
  int nRow,
  int nFree,
  int nPositive,
  int nBox,
  int nCone,
  int nRotateCone,
  int nPrimalExp,
  int nDualExp,
  int nPrimalPow,
  int nDualPow,
  int nPSD,
  int nQObjElem,
  int iObjSense,
  double dObjConst,
  const double* colObj,
  const int* qObjRow,
  const int* qObjCol,
  const double* qObjElem,
  const int* colMatBeg,
  const int* colMatCnt,
  const int* colMatIdx,
  const double* colMatElem,
  const double* rowRhs,
  const double* boxLower,
  const double* boxUpper,
  const int* coneDim,
  const int* rotateConeDim,
  const int* primalPowDim,
  const int* dualPowDim,
  const double* primalPowAlpha,
  const double* dualPowAlpha,
  const int* psdDim,
  const char* colType,
  char const* const* colNames,
  char const* const* rowNames,
  char const* const* psdColNames,
  int* outRowMap);

/* Convert status code from integer to string */
static const char *COPTMEX_statusInt2Str(int status) {
  switch (status) {
  case 0:  // unstarted
    return COPTMEX_STATUS_UNSTARTED;
  case 1:  // optimal
    return COPTMEX_STATUS_OPTIMAL;
  case 2:  // infeasible
    return COPTMEX_STATUS_INFEASIBLE;
  case 3:  // unbounded
    return COPTMEX_STATUS_UNBOUNDED;
  case 4:  // inf_or_unb
    return COPTMEX_STATUS_INF_OF_UNB;
  case 5:  // numerical
    return COPTMEX_STATUS_NUMERICAL;
  case 6:  // nodelimit
    return COPTMEX_STATUS_NODELIMIT;
  case 8:  // timeout
    return COPTMEX_STATUS_TIMEOUT;
  case 9:  // unfinished
    return COPTMEX_STATUS_UNFINISHED;
  case 10: // interrupted
    return COPTMEX_STATUS_INTERRUPTED;
  default: // unknown
    return "unknown";
  }
}

/* Convert objective sense from string to integer */
static int COPTMEX_objsenStr2Int(char *objsen) {
  if (mystrcmp(objsen, "max") == 0 || mystrcmp(objsen, "maximize") == 0) {
    return COPT_MAXIMIZE;
  }
  return COPT_MINIMIZE;
}

/* Convert objective sense from integer to string */
static const char *COPTMEX_objsenInt2Str(int objsen) {
  if (objsen == COPT_MAXIMIZE) {
    return "max";
  }
  return "min";
}

/* Extract string from MEX */
static int COPTMEX_getString(const mxArray *in_array, char **out_str) {
  int retcode = 0;

  int bufflen = mxGetNumberOfElements(in_array) + 1;
  char *buffer = (char *) mxCalloc(bufflen, sizeof(char));
  if (!buffer) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }
  
  mxGetString(in_array, buffer, bufflen);
  *out_str = buffer;

exit_cleanup:
  return retcode;
}

/* Free string allocated by MEX */
static void COPTMEX_freeString(char **in_str) {
  if (in_str != NULL && *in_str != NULL) {
    mxFree(*in_str);
  }
  return;
}

/* Display error message */
void COPTMEX_errorMsg(int errcode, const char *errinfo) {
  char *errid = NULL;
  char *errtxt = NULL;

  switch (errcode) {
  case COPTMEX_ERROR_BAD_TYPE:
    errid  = "coptmex:BadType";
    errtxt = "Invalid type of '%s'.";
    break;
  case COPTMEX_ERROR_BAD_NAME:
    errid  = "coptmex:BadName";
    errtxt = "Invalid name of '%s'.";
    break;
  case COPTMEX_ERROR_BAD_DATA:
    errid  = "coptmex:BadData";
    errtxt = "Invalid data of '%s'.";
    break;
  case COPTMEX_ERROR_BAD_NUM:
    errid  = "coptmex:BadNum";
    errtxt = "Invalid number of elemtents in '%s'.";
    break;
  case COPTMEX_ERROR_BAD_API:
    errid  = "coptmex:BadAPI";
    errtxt = "%s.";
    break;
  default:
    return;
  }

  mexErrMsgIdAndTxt(errid, errtxt, errinfo);
}

/* Convert CSC matrix to COO matrix */
void COPTMEX_csc2coo(mxArray *q, int *qMatRow, int *qMatCol, double *qMatElem) {
  int ncol = mxGetN(q);
  mwIndex *jc = mxGetJc(q);
  mwIndex *ir = mxGetIr(q);
  double *val = mxGetDoubles(q);

  for (int i = 0; i < ncol; ++i) {
    int nColElem = (int) jc[i];
    int nColLast = (int) jc[i + 1];
    for (; nColElem < nColLast; ++nColElem) {
      qMatRow[nColElem] = (int) ir[nColElem];
      qMatCol[nColElem] = i;
      qMatElem[nColElem] = val[nColElem];
    }
  }

  return;
}

/* Convert COO matrix to CSC matrix */
int COPTMEX_coo2csc(int nQElem, int *qMatRow, int *qMatCol, double *qMatElem, mxArray *q) {
  int ncol = mxGetN(q);
  mwIndex *jc = mxGetJc(q);
  mwIndex *ir = mxGetIr(q);
  double *val = mxGetDoubles(q);

  int *colMatCnt = (int *) mxCalloc(ncol, sizeof(int));
  if (!colMatCnt) {
    return COPT_RETCODE_MEMORY;
  }

  for (int i = 0; i < nQElem; ++i) {
    colMatCnt[qMatCol[i]]++;
  }

  jc[0] = 0;
  for (int i = 1; i <= ncol; ++i) {
    jc[i] = jc[i - 1] + colMatCnt[i - 1];
  }

  for (int i = 0; i < nQElem; ++i) {
    int iCol = qMatCol[i];
    int iElem = (int) jc[iCol];

    ir[iElem] = qMatRow[i];
    val[iElem] = qMatElem[i];

    jc[iCol]++;
  }

  for (int i = 0, last = 0; i <= ncol; ++i) {
    int tmp = jc[i];
    jc[i] = last;
    last = tmp;
  }

  mxFree(colMatCnt);
  return COPT_RETCODE_OK;
}

#include "coptinit.c"

/* Display banner */
int COPTMEX_dispBanner(void) {
  int retcode = 0;
  char msgbuf[COPT_BUFFSIZE];

  COPTMEX_CALL(COPT_GetBanner(msgbuf, COPT_BUFFSIZE));
  mexPrintf("%s", msgbuf);

exit_cleanup:
  return retcode;
}

/* Extract version of COPT */
int COPTMEX_getVersion(mxArray **out_version) {
  int retcode = COPT_RETCODE_OK;
  mxArray *retver = NULL;
  coptmex_mversion coptver;

  COPTMEX_initVersion(&coptver);

  coptver.major = mxCreateDoubleMatrix(1, 1, mxREAL);
  coptver.minor = mxCreateDoubleMatrix(1, 1, mxREAL);
  coptver.technical = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!coptver.major || !coptver.minor || !coptver.technical) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  *mxGetDoubles(coptver.major) = COPT_VERSION_MAJOR;
  *mxGetDoubles(coptver.minor) = COPT_VERSION_MINOR;
  *mxGetDoubles(coptver.technical) = COPT_VERSION_TECHNICAL;

  retver = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!retver) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'major'
  mxAddField(retver, COPTMEX_VERSION_MAJOR);
  mxSetField(retver, 0, COPTMEX_VERSION_MAJOR, coptver.major);
  // 'minor'
  mxAddField(retver, COPTMEX_VERSION_MINOR);
  mxSetField(retver, 0, COPTMEX_VERSION_MINOR, coptver.minor);
  // 'technical'
  mxAddField(retver, COPTMEX_VERSION_TECHNICAL);
  mxSetField(retver, 0, COPTMEX_VERSION_TECHNICAL, coptver.technical);

  *out_version = retver;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_version = NULL;
  }

  return retcode;
}

/* Extract objective sense */
int COPTMEX_getObjsen(const mxArray *in_objsen, int *out_objsen) {
  int retcode = COPT_RETCODE_OK;
  char *objsen = NULL;

  COPTMEX_CALL(COPTMEX_getString(in_objsen, &objsen));
  *out_objsen = COPTMEX_objsenStr2Int(objsen);

exit_cleanup:
  COPTMEX_freeString(&objsen);
  return retcode;
}

/* Extract LP solution */
static int COPTMEX_getLpResult(copt_prob *prob, mxArray **out_lpresult) {
  int retcode = COPT_RETCODE_OK;
  mxArray *lpResult = NULL;
  coptmex_clpsol csol;
  coptmex_mlpsol msol;

  COPTMEX_initCLpSol(&csol);
  COPTMEX_initMLpSol(&msol);

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &csol.nRow));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &csol.nCol));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_PSDCOLS, &csol.nPSD));
  COPTMEX_CALL(COPT_GetIntAttr(prob, "PSDLens", &csol.nPSDLen));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_PSDCONSTRS, &csol.nPSDConstr));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_QCONSTRS, &csol.nQConstr));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_LPSTATUS, &csol.nStatus));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASBASIS, &csol.hasBasis));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASLPSOL, &csol.hasLpSol));

  msol.status      = mxCreateString(COPTMEX_statusInt2Str(csol.nStatus));
  msol.simplexiter = mxCreateDoubleMatrix(1, 1, mxREAL);
  msol.barrieriter = mxCreateDoubleMatrix(1, 1, mxREAL);
  msol.solvingtime = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!msol.status || !msol.simplexiter || !msol.barrieriter || !msol.solvingtime) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  if (csol.hasLpSol) {
    msol.objval  = mxCreateDoubleMatrix(1, 1, mxREAL);
    msol.value   = mxCreateDoubleMatrix(csol.nCol, 1, mxREAL);
    msol.redcost = mxCreateDoubleMatrix(csol.nCol, 1, mxREAL);
    msol.slack   = mxCreateDoubleMatrix(csol.nRow, 1, mxREAL);
    msol.dual    = mxCreateDoubleMatrix(csol.nRow, 1, mxREAL);
    if (!msol.objval || !msol.value || !msol.redcost || !msol.slack || !msol.dual) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    csol.colValue = mxGetDoubles(msol.value);
    csol.colDual  = mxGetDoubles(msol.redcost);
    csol.rowSlack = mxGetDoubles(msol.slack);
    csol.rowDual  = mxGetDoubles(msol.dual);

    if (csol.nQConstr > 0) {
      msol.qcslack = mxCreateDoubleMatrix(csol.nQConstr, 1, mxREAL);
      if (!msol.qcslack) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      csol.qRowSlack = mxGetDoubles(msol.qcslack);
    }

    if (csol.nPSD > 0) {
      msol.psdcolvalue = mxCreateDoubleMatrix(csol.nPSDLen, 1, mxREAL);
      msol.psdcoldual = mxCreateDoubleMatrix(csol.nPSDLen, 1, mxREAL);
      if (!msol.psdcolvalue || !msol.psdcoldual) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      csol.psdColValue = mxGetDoubles(msol.psdcolvalue);
      csol.psdColDual = mxGetDoubles(msol.psdcoldual);
    }

    if (csol.nPSDConstr > 0) {
      msol.psdrowslack = mxCreateDoubleMatrix(csol.nPSDConstr, 1, mxREAL);
      msol.psdrowdual = mxCreateDoubleMatrix(csol.nPSDConstr, 1, mxREAL);
      if (!msol.psdrowslack || !msol.psdrowdual) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      csol.psdRowSlack = mxGetDoubles(msol.psdrowslack);
      csol.psdRowDual = mxGetDoubles(msol.psdrowdual);
    }
  }

  if (csol.hasBasis) {
    msol.varbasis    = mxCreateDoubleMatrix(csol.nCol, 1, mxREAL);
    msol.constrbasis = mxCreateDoubleMatrix(csol.nRow, 1, mxREAL);
    if (!msol.varbasis || !msol.constrbasis) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    csol.colBasis = (int *) mxCalloc(csol.nCol, sizeof(int));
    csol.rowBasis = (int *) mxCalloc(csol.nRow, sizeof(int));
    if (!csol.colBasis || !csol.rowBasis) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_SIMPLEXITER, &csol.nSimplexIter));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_BARRIERITER, &csol.nBarrierIter));
  COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_SOLVINGTIME, &csol.dSolvingTime));

  if (csol.hasLpSol) {
    COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_LPOBJVAL, &csol.dObjVal));
    COPTMEX_CALL(COPT_GetLpSolution(prob, csol.colValue, csol.rowSlack,
                 csol.rowDual, csol.colDual));

    if (csol.nQConstr > 0) {
      COPTMEX_CALL(COPT_GetQConstrInfo(prob, COPT_DBLINFO_SLACK, csol.nQConstr, NULL, csol.qRowSlack));
    }

    if (csol.nPSD > 0) {
      COPTMEX_CALL(COPT_GetPSDSolution(prob, csol.psdColValue, NULL, NULL, csol.psdColDual));
    }

    if (csol.nPSDConstr > 0) {
      COPTMEX_CALL(COPT_GetPSDSolution(prob, NULL, csol.psdRowSlack, csol.psdRowDual, NULL));
    }
  }

  if (csol.hasBasis) {
    COPTMEX_CALL(COPT_GetBasis(prob, csol.colBasis, csol.rowBasis));
  }

  *mxGetDoubles(msol.simplexiter) = csol.nSimplexIter;
  *mxGetDoubles(msol.barrieriter) = csol.nBarrierIter;
  *mxGetDoubles(msol.solvingtime) = csol.dSolvingTime;

  if (csol.hasLpSol) {
    *mxGetDoubles(msol.objval) = csol.dObjVal;
  }

  if (csol.hasBasis) {
    double *colBasis_data = mxGetDoubles(msol.varbasis);
    double *rowBasis_data = mxGetDoubles(msol.constrbasis);
    for (int i = 0; i < csol.nCol; ++i) {
      colBasis_data[i] = csol.colBasis[i];
    }
    for (int i = 0; i < csol.nRow; ++i) {
      rowBasis_data[i] = csol.rowBasis[i];
    }

    mxFree(csol.colBasis);
    mxFree(csol.rowBasis);
  }

  lpResult = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!lpResult) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'status'
  mxAddField(lpResult, COPTMEX_RESULT_STATUS);
  mxSetField(lpResult, 0, COPTMEX_RESULT_STATUS, msol.status);
  // 'simplexiter'
  mxAddField(lpResult, COPTMEX_RESULT_SIMITER);
  mxSetField(lpResult, 0, COPTMEX_RESULT_SIMITER, msol.simplexiter);
  // 'barrieriter'
  mxAddField(lpResult, COPTMEX_RESULT_BARITER);
  mxSetField(lpResult, 0, COPTMEX_RESULT_BARITER, msol.barrieriter);
  // 'solvingtime'
  mxAddField(lpResult, COPTMEX_RESULT_SOLVETIME);
  mxSetField(lpResult, 0, COPTMEX_RESULT_SOLVETIME, msol.solvingtime);

  if (csol.hasLpSol) {
    // 'objval'
    mxAddField(lpResult, COPTMEX_RESULT_OBJVAL);
    mxSetField(lpResult, 0, COPTMEX_RESULT_OBJVAL, msol.objval);
    // 'x'
    mxAddField(lpResult, COPTMEX_RESULT_VALUE);
    mxSetField(lpResult, 0, COPTMEX_RESULT_VALUE, msol.value);
    // 'rc'
    mxAddField(lpResult, COPTMEX_RESULT_REDCOST);
    mxSetField(lpResult, 0, COPTMEX_RESULT_REDCOST, msol.redcost);
    // 'slack;
    mxAddField(lpResult, COPTMEX_RESULT_SLACK);
    mxSetField(lpResult, 0, COPTMEX_RESULT_SLACK, msol.slack);
    // 'pi'
    mxAddField(lpResult, COPTMEX_RESULT_DUAL);
    mxSetField(lpResult, 0, COPTMEX_RESULT_DUAL, msol.dual);

    // 'qcslack'
    if (csol.nQConstr > 0) {
      mxAddField(lpResult, COPTMEX_RESULT_QCSLACK);
      mxSetField(lpResult, 0, COPTMEX_RESULT_QCSLACK, msol.qcslack);
    }

    if (csol.nPSD > 0) {
      // 'psdx'
      mxAddField(lpResult, COPTMEX_RESULT_PSDX);
      mxSetField(lpResult, 0, COPTMEX_RESULT_PSDX, msol.psdcolvalue);

      // 'psdrc'
      mxAddField(lpResult, COPTMEX_RESULT_PSDRC);
      mxSetField(lpResult, 0, COPTMEX_RESULT_PSDRC, msol.psdcoldual);
    }

    if (csol.nPSDConstr > 0) {
      // 'psdslack'
      mxAddField(lpResult, COPTMEX_RESULT_PSDSLACK);
      mxSetField(lpResult, 0, COPTMEX_RESULT_PSDSLACK, msol.psdrowslack);

      // 'psdpi'
      mxAddField(lpResult, COPTMEX_RESULT_PSDPI);
      mxSetField(lpResult, 0, COPTMEX_RESULT_PSDPI, msol.psdrowdual);
    }
  }

  if (csol.hasBasis) {
    // 'varbasis'
    mxAddField(lpResult, COPTMEX_RESULT_VARBASIS);
    mxSetField(lpResult, 0, COPTMEX_RESULT_VARBASIS, msol.varbasis);
    // 'constrbasis'
    mxAddField(lpResult, COPTMEX_RESULT_CONBASIS);
    mxSetField(lpResult, 0, COPTMEX_RESULT_CONBASIS, msol.constrbasis);
  }

  *out_lpresult = lpResult;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_lpresult = NULL;
  }

  return retcode;
}

/* Extract MIP solution */
static int COPTMEX_getMipResult(copt_prob *prob, mxArray **out_mipresult) {
  int retcode = COPT_RETCODE_OK;
  mxArray *mipResult = NULL;
  coptmex_cmipsol csol;
  coptmex_mmipsol msol;

  COPTMEX_initCMipSol(&csol);
  COPTMEX_initMMipSol(&msol);

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &csol.nRow));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &csol.nCol));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASMIPSOL, &csol.hasMipSol));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_POOLSOLS, &csol.nSolPool));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_MIPSTATUS, &csol.nStatus));

  msol.status      = mxCreateString(COPTMEX_statusInt2Str(csol.nStatus));
  msol.simplexiter = mxCreateDoubleMatrix(1, 1, mxREAL);
  msol.nodecnt     = mxCreateDoubleMatrix(1, 1, mxREAL);
  msol.solvingtime = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!msol.status || !msol.simplexiter || !msol.nodecnt || !msol.solvingtime) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  if (csol.hasMipSol) {
    msol.bestgap = mxCreateDoubleMatrix(1, 1, mxREAL);
    msol.objval  = mxCreateDoubleMatrix(1, 1, mxREAL);
    msol.bestbnd = mxCreateDoubleMatrix(1, 1, mxREAL);
    msol.value   = mxCreateDoubleMatrix(csol.nCol, 1, mxREAL);
    if (!msol.bestgap || !msol.objval || !msol.bestbnd || !msol.value) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    csol.colValue = mxGetDoubles(msol.value);
  }

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_SIMPLEXITER, &csol.nSimplexIter));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_NODECNT, &csol.nNodeCnt));
  COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_SOLVINGTIME, &csol.dSolvingTime));

  if (csol.hasMipSol) {
    COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_BESTGAP, &csol.dBestGap));
    COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_BESTOBJ, &csol.dObjVal));
    COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_BESTBND, &csol.dBestBnd));
    COPTMEX_CALL(COPT_GetSolution(prob, csol.colValue));
  }

  if (csol.nSolPool > 0) {
    const char *solpoolfields[] = {COPTMEX_RESULT_POOLOBJ,
                                   COPTMEX_RESULT_POOLXN};
    msol.solpool = mxCreateStructMatrix(csol.nSolPool, 1, 2, solpoolfields);
    if (!msol.solpool) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    for (int i = 0; i < csol.nSolPool; ++i) {
      mxArray *poolobjval = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *poolxn = mxCreateDoubleMatrix(csol.nCol, 1, mxREAL);
      if (!poolobjval || !poolxn) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      double dPoolObjVal = +COPT_INFINITY;
      double *dPoolColValue = mxGetDoubles(poolxn);

      COPTMEX_CALL(COPT_GetPoolObjVal(prob, i, &dPoolObjVal));
      COPTMEX_CALL(COPT_GetPoolSolution(prob, i, csol.nCol, NULL, dPoolColValue));

      *mxGetDoubles(poolobjval) = dPoolObjVal;

      mxSetField(msol.solpool, i, COPTMEX_RESULT_POOLOBJ, poolobjval);
      mxSetField(msol.solpool, i, COPTMEX_RESULT_POOLXN, poolxn);
    }
  }

  *mxGetDoubles(msol.simplexiter) = csol.nSimplexIter;
  *mxGetDoubles(msol.nodecnt)     = csol.nNodeCnt;
  *mxGetDoubles(msol.solvingtime) = csol.dSolvingTime;

  if (csol.hasMipSol) {
    *mxGetDoubles(msol.bestgap) = csol.dBestGap;
    *mxGetDoubles(msol.objval)  = csol.dObjVal; 
    *mxGetDoubles(msol.bestbnd) = csol.dBestBnd;
  }

  mipResult = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!mipResult) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'status'
  mxAddField(mipResult, COPTMEX_RESULT_STATUS);
  mxSetField(mipResult, 0, COPTMEX_RESULT_STATUS, msol.status);
  // 'simplexiter'
  mxAddField(mipResult, COPTMEX_RESULT_SIMITER);
  mxSetField(mipResult, 0, COPTMEX_RESULT_SIMITER, msol.simplexiter);
  // 'nodecnt'
  mxAddField(mipResult, COPTMEX_RESULT_NODECNT);
  mxSetField(mipResult, 0, COPTMEX_RESULT_NODECNT, msol.nodecnt);
  // 'solvingtime'
  mxAddField(mipResult, COPTMEX_RESULT_SOLVETIME);
  mxSetField(mipResult, 0, COPTMEX_RESULT_SOLVETIME, msol.solvingtime);

  if (csol.hasMipSol) {
    // 'bestgap'
    mxAddField(mipResult, COPTMEX_RESULT_BESTGAP);
    mxSetField(mipResult, 0, COPTMEX_RESULT_BESTGAP, msol.bestgap);
    // 'objval'
    mxAddField(mipResult, COPTMEX_RESULT_OBJVAL);
    mxSetField(mipResult, 0, COPTMEX_RESULT_OBJVAL, msol.objval);
    // 'bestbnd'
    mxAddField(mipResult, COPTMEX_RESULT_BESTBND);
    mxSetField(mipResult, 0, COPTMEX_RESULT_BESTBND, msol.bestbnd);
    // 'x'
    mxAddField(mipResult, COPTMEX_RESULT_VALUE);
    mxSetField(mipResult, 0, COPTMEX_RESULT_VALUE, msol.value);
  }

  if (csol.nSolPool > 0) {
    // 'pool'
    mxAddField(mipResult, COPTMEX_RESULT_POOL);
    mxSetField(mipResult, 0, COPTMEX_RESULT_POOL, msol.solpool);
  }

  *out_mipresult = mipResult;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_mipresult = NULL;
  }

  return retcode;
}

/* Extract and save result */
int COPTMEX_getResult(copt_prob *prob, mxArray **out_result) {
  int retcode = 0;
  int isMip = 0;

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ISMIP, &isMip));
  if (isMip) {
    COPTMEX_CALL(COPTMEX_getMipResult(prob, out_result));
  } else {
    COPTMEX_CALL(COPTMEX_getLpResult(prob, out_result));
  }

exit_cleanup:
  return retcode;
}

/* Extract model data */
int COPTMEX_getModel(copt_prob *prob, int nfiles, const mxArray **in_files,
                     mxArray **out_model) {
  int retcode = COPT_RETCODE_OK;
  int hasInfoFile = 0;
  mxArray *retmodel = NULL;
  coptmex_cprob cprob;
  coptmex_mprob mprob;

  // Read model from file
  if (nfiles == 1 || nfiles == 2) {
    COPTMEX_CALL(COPTMEX_readModel(prob, in_files[0]));
    if (nfiles == 2) {
      COPTMEX_CALL(COPTMEX_readInfo(prob, in_files[1]));
      hasInfoFile = 1;
    }
  }

  COPTMEX_initCProb(&cprob);
  COPTMEX_initMProb(&mprob);

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &cprob.nRow));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &cprob.nCol));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ELEMS, &cprob.nElem));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_QELEMS, &cprob.nQElem));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_SOSS, &cprob.nSos));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_INDICATORS, &cprob.nIndicator));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_CONES, &cprob.nCone));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_QCONSTRS, &cprob.nQConstr));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_OBJSENSE, &cprob.nObjSen));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASBASIS, &cprob.hasBasis));

  mprob.objsen = mxCreateString(COPTMEX_objsenInt2Str(cprob.nObjSen));
  mprob.objcon = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!mprob.objsen || !mprob.objcon) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_OBJCONST, &cprob.dObjConst));
  *mxGetDoubles(mprob.objcon) = cprob.dObjConst;

  if (cprob.nRow > 0 && cprob.nCol > 0) {
    int nRealElem = 0;
    if (cprob.nElem == 0) {
      nRealElem = 1;
    } else {
      nRealElem = cprob.nElem;
    }
    
    mprob.A = mxCreateSparse(cprob.nRow, cprob.nCol, nRealElem, mxREAL);
    if (!mprob.A) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    cprob.colMatBeg = (int *) mxCalloc(cprob.nCol + 1, sizeof(int));
    cprob.colMatIdx = (int *) mxCalloc(nRealElem, sizeof(int));
    if (!cprob.colMatBeg || !cprob.colMatIdx) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
    cprob.colMatElem = mxGetDoubles(mprob.A);

    if (cprob.nElem > 0) {
      COPTMEX_CALL(COPT_GetCols(prob, cprob.nCol, NULL, cprob.colMatBeg, NULL,
                   cprob.colMatIdx, cprob.colMatElem, cprob.nElem, NULL));
    }
    
    mwIndex *colMatBeg_data = mxGetJc(mprob.A);
    mwIndex *colMatIdx_data = mxGetIr(mprob.A);

    for (int i = 0; i < cprob.nCol + 1; ++i) {
      colMatBeg_data[i] = cprob.colMatBeg[i];
    }
    for (int i = 0; i < nRealElem; ++i) {
      colMatIdx_data[i] = cprob.colMatIdx[i];
    }

    mxFree(cprob.colMatBeg);
    mxFree(cprob.colMatIdx);
  }

  if (cprob.nCol > 0) {
    mprob.obj = mxCreateDoubleMatrix(cprob.nCol, 1, mxREAL);
    mprob.lb  = mxCreateDoubleMatrix(cprob.nCol, 1, mxREAL);
    mprob.ub  = mxCreateDoubleMatrix(cprob.nCol, 1, mxREAL);
    mprob.varnames = mxCreateCellMatrix(cprob.nCol, 1);
    if (!mprob.obj || !mprob.lb || !mprob.ub || !mprob.varnames) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    cprob.colCost  = mxGetDoubles(mprob.obj);
    cprob.colLower = mxGetDoubles(mprob.lb);
    cprob.colUpper = mxGetDoubles(mprob.ub);

    COPTMEX_CALL(COPT_GetColInfo(prob, COPT_DBLINFO_OBJ, cprob.nCol, NULL, cprob.colCost));
    COPTMEX_CALL(COPT_GetColInfo(prob, COPT_DBLINFO_LB, cprob.nCol, NULL, cprob.colLower));
    COPTMEX_CALL(COPT_GetColInfo(prob, COPT_DBLINFO_UB, cprob.nCol, NULL, cprob.colUpper));

    char *colType_c  = (char *) mxCalloc(2 * cprob.nCol, sizeof(char));
    char **colType_s = (char **) mxCalloc(cprob.nCol, sizeof(char *));
    if (!colType_c || !colType_s) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetColType(prob, cprob.nCol, NULL, colType_c));

    for (int i = cprob.nCol - 1; i >= 0; --i) {
      colType_c[2 * i] = colType_c[i];
      colType_c[2 * i + 1] = 0;
    }

    for (int i = 0; i < cprob.nCol; ++i) {
      colType_s[i] = colType_c + 2 * i;
    }

    mprob.vtype = mxCreateCharMatrixFromStrings(cprob.nCol, (const char **) colType_s);

    int colNameLen = 0;
    char **colNames = (char **) mxCalloc(cprob.nCol, sizeof(char *));
    if (!colNames) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    for (int i = 0; i < cprob.nCol; ++i) {
      COPTMEX_CALL(COPT_GetColName(prob, i, NULL, 0, &colNameLen));
      colNames[i] = (char *) mxCalloc(colNameLen, sizeof(char));
      if (!colNames[i]) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }
      COPTMEX_CALL(COPT_GetColName(prob, i, colNames[i], colNameLen, NULL));

      mxSetCell(mprob.varnames, i, mxCreateString(colNames[i]));
    }
  }

  if (cprob.nRow > 0) {
    mprob.lhs = mxCreateDoubleMatrix(cprob.nRow, 1, mxREAL);
    mprob.rhs = mxCreateDoubleMatrix(cprob.nRow, 1, mxREAL);
    mprob.constrnames = mxCreateCellMatrix(cprob.nRow, 1);
    if (!mprob.lhs || !mprob.rhs || !mprob.constrnames) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    cprob.rowLower = mxGetDoubles(mprob.lhs);
    cprob.rowUpper = mxGetDoubles(mprob.rhs);

    COPTMEX_CALL(COPT_GetRowInfo(prob, COPT_DBLINFO_LB, cprob.nRow, NULL, cprob.rowLower));
    COPTMEX_CALL(COPT_GetRowInfo(prob, COPT_DBLINFO_UB, cprob.nRow, NULL, cprob.rowUpper));
    
    int rowNameLen = 0;
    //TODO: rowSense
    char **rowNames = (char **) mxCalloc(cprob.nRow, sizeof(char *));
    if (!rowNames) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    for (int i = 0; i < cprob.nRow; ++i) {
      COPTMEX_CALL(COPT_GetRowName(prob, i, NULL, 0, &rowNameLen));
      rowNames[i] = (char *) mxCalloc(rowNameLen, sizeof(char));
      if (!rowNames[i]) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }
      COPTMEX_CALL(COPT_GetRowName(prob, i, rowNames[i], rowNameLen, NULL));

      mxSetCell(mprob.constrnames, i, mxCreateString(rowNames[i]));
    }
  }

  if (cprob.nSos > 0) {
    const char *sosfields[] = {COPTMEX_MODEL_SOSTYPE,
                               COPTMEX_MODEL_SOSVARS,
                               COPTMEX_MODEL_SOSWEIGHT};
    mprob.sos = mxCreateStructMatrix(cprob.nSos, 1, 3, sosfields);
    if (!mprob.sos) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetSOSs(prob, cprob.nSos, NULL, NULL, NULL, NULL, NULL,
                 NULL, 0, &cprob.nSosSize));
    
    cprob.sosType   = (int *) mxCalloc(cprob.nSos, sizeof(int));
    cprob.sosMatBeg = (int *) mxCalloc(cprob.nSos, sizeof(int));
    cprob.sosMatCnt = (int *) mxCalloc(cprob.nSos, sizeof(int));
    cprob.sosMatIdx = (int *) mxCalloc(cprob.nSosSize, sizeof(int));
    cprob.sosMatWt  = (double *) mxCalloc(cprob.nSosSize, sizeof(double));
    if (!cprob.sosType || !cprob.sosMatBeg || !cprob.sosMatCnt ||
        !cprob.sosMatIdx || !cprob.sosMatWt) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetSOSs(prob, cprob.nSos, NULL, cprob.sosType,
                 cprob.sosMatBeg, cprob.sosMatCnt, cprob.sosMatIdx,
                 cprob.sosMatWt, cprob.nSosSize, NULL));
    
    for (int i = 0; i < cprob.nSos; ++i) {
      mxArray *sosType = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *sosIdx  = mxCreateDoubleMatrix(cprob.sosMatCnt[i], 1, mxREAL);
      mxArray *sosWts  = mxCreateDoubleMatrix(cprob.sosMatCnt[i], 1, mxREAL);
      if (!sosType || !sosIdx || !sosWts) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      *mxGetDoubles(sosType) = cprob.sosType[i];

      int iSosElem = cprob.sosMatBeg[i];
      int iSosLast = cprob.sosMatCnt[i] + iSosElem;
      double *sosIdx_data = mxGetDoubles(sosIdx);
      double *sosWts_data = mxGetDoubles(sosWts);
      for (int iElem = 0; iElem < cprob.sosMatCnt[i]; ++iElem) {
        sosIdx_data[iElem] = cprob.sosMatIdx[iSosElem] + 1;
        sosWts_data[iElem] = cprob.sosMatWt[iSosElem];
        iSosElem++;
      }

      mxSetField(mprob.sos, i, COPTMEX_MODEL_SOSTYPE, sosType);
      mxSetField(mprob.sos, i, COPTMEX_MODEL_SOSVARS, sosIdx);
      mxSetField(mprob.sos, i, COPTMEX_MODEL_SOSWEIGHT, sosWts);
    }
  }

  if (cprob.nIndicator > 0) {
    const char *indicfields[] = {COPTMEX_MODEL_INDICBINVAR,
                                 COPTMEX_MODEL_INDICBINVAL,
                                 COPTMEX_MODEL_INDICROW,
                                 COPTMEX_MODEL_INDICSENSE,
                                 COPTMEX_MODEL_INDICRHS};
    mprob.indicator = mxCreateStructMatrix(cprob.nIndicator, 1, 5, indicfields);
    if (!mprob.indicator) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    for (int i = 0; i < cprob.nIndicator; ++i) {
      int rowElemCnt = 0;
      COPTMEX_CALL(COPT_GetIndicator(prob, i, NULL, NULL, NULL, NULL, NULL, 
                   NULL, NULL, 0, &rowElemCnt));

      mxArray *binVar = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *binVal = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *indicA = mxCreateSparse(cprob.nCol, 1, rowElemCnt, mxREAL);
      mxArray *indicSense = NULL;
      mxArray *indicRhs = mxCreateDoubleMatrix(1, 1, mxREAL);

      int binColIdx = 0;
      int binColVal = 0;
      int nRowMatCnt = 0;
      int *rowMatIdx = (int *) mxCalloc(rowElemCnt, sizeof(int));
      double *rowMatElem = (double *) mxCalloc(rowElemCnt, sizeof(double));
      char cRowSense[2] = {0};
      double dRowBound = 0;

      COPTMEX_CALL(COPT_GetIndicator(prob, i, &binColIdx, &binColVal,
                   &nRowMatCnt, rowMatIdx, rowMatElem, &cRowSense[0],
                   &dRowBound, rowElemCnt, NULL));

      *mxGetDoubles(binVar) = binColIdx + 1;
      *mxGetDoubles(binVal) = binColVal;

      mwIndex *jc_data = mxGetJc(indicA);
      mwIndex *ir_data = mxGetIr(indicA);
      double *val_data = mxGetDoubles(indicA);

      jc_data[0] = 0;
      jc_data[1] = rowElemCnt;
      for (int i = 0; i < rowElemCnt; ++i) {
        ir_data[i] = rowMatIdx[i];
        val_data[i] = rowMatElem[i];
      }

      indicSense = mxCreateString(cRowSense);
      *mxGetDoubles(indicRhs) = dRowBound;

      mxFree(rowMatIdx);
      mxFree(rowMatElem);

      mxSetField(mprob.indicator, i, COPTMEX_MODEL_INDICBINVAR, binVar);
      mxSetField(mprob.indicator, i, COPTMEX_MODEL_INDICBINVAL, binVal);
      mxSetField(mprob.indicator, i, COPTMEX_MODEL_INDICROW, indicA);
      mxSetField(mprob.indicator, i, COPTMEX_MODEL_INDICSENSE, indicSense);
      mxSetField(mprob.indicator, i, COPTMEX_MODEL_INDICRHS, indicRhs);
    }
  }

  if (cprob.nCone > 0) {
    const char *conefields[] = {COPTMEX_MODEL_CONETYPE,
                                COPTMEX_MODEL_CONEVARS};
    mprob.cone = mxCreateStructMatrix(cprob.nCone, 1, 2, conefields);
    if (!mprob.cone) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetCones(prob, cprob.nCone, NULL, NULL, NULL, NULL, NULL, 
                 0, &cprob.nConeSize));

    cprob.coneType = (int *) mxCalloc(cprob.nCone, sizeof(int));
    cprob.coneBeg  = (int *) mxCalloc(cprob.nCone, sizeof(int));
    cprob.coneCnt  = (int *) mxCalloc(cprob.nCone, sizeof(int));
    cprob.coneIdx  = (int *) mxCalloc(cprob.nConeSize, sizeof(int));
    if (!cprob.coneType || !cprob.coneBeg || !cprob.coneCnt || !cprob.coneIdx) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetCones(prob, cprob.nCone, NULL, cprob.coneType, 
                 cprob.coneBeg, cprob.coneCnt, cprob.coneIdx, cprob.nConeSize, 
                 NULL));

    for (int i = 0; i < cprob.nCone; ++i) {
      mxArray *coneType = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *coneIdx = mxCreateDoubleMatrix(cprob.coneCnt[i], 1, mxREAL);
      if (!coneType || !coneIdx) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      *mxGetDoubles(coneType) = cprob.coneType[i];

      int iConeElem = cprob.coneBeg[i];
      int iConeLast = cprob.coneCnt[i] + iConeElem;
      double *coneIdx_data = mxGetDoubles(coneIdx);
      for (int iElem = 0; iElem < cprob.coneCnt[i]; ++iElem) {
        coneIdx_data[iElem] = cprob.coneIdx[iConeElem] + 1;
        iConeElem++;
      }

      mxSetField(mprob.cone, i, COPTMEX_MODEL_CONETYPE, coneType);
      mxSetField(mprob.cone, i, COPTMEX_MODEL_CONEVARS, coneIdx);
    }
  }

  if (cprob.nQElem > 0) {
    mprob.qobj = mxCreateSparse(cprob.nCol, cprob.nCol, cprob.nQElem, mxREAL);
    if (!mprob.qobj) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    int *qObjRow = (int *) mxCalloc(cprob.nQElem, sizeof(int));
    int *qObjCol = (int *) mxCalloc(cprob.nQElem, sizeof(int));
    double *qObjElem = (double *) mxCalloc(cprob.nQElem, sizeof(double));
    if (!qObjRow || !qObjCol || !qObjElem) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetQuadObj(prob, NULL, qObjRow, qObjCol, qObjElem));
    COPTMEX_CALL(COPTMEX_coo2csc(cprob.nQElem, qObjRow, qObjCol, qObjElem, mprob.qobj));

    mxFree(qObjRow);
    mxFree(qObjCol);
    mxFree(qObjElem);
  }

  if (cprob.nQConstr > 0) {
    const char *qconstrfields[] = {COPTMEX_MODEL_QCROW,
                                   COPTMEX_MODEL_QCCOL,
                                   COPTMEX_MODEL_QCVAL,
                                   COPTMEX_MODEL_QCLINEAR,
                                   COPTMEX_MODEL_QCSENSE,
                                   COPTMEX_MODEL_QCRHS,
                                   COPTMEX_MODEL_QCNAME};
    mprob.quadcon = mxCreateStructMatrix(cprob.nQConstr, 1, 7, qconstrfields);
    if (!mprob.quadcon) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    for (int i = 0; i < cprob.nQConstr; ++i) {
      int nQMatElem = 0;
      int nQRowElem = 0;
      COPTMEX_CALL(COPT_GetQConstr(prob, i, NULL, NULL, NULL, 0, &nQMatElem, NULL,
                   NULL, NULL, NULL, 0, &nQRowElem));

      mxArray *QcRow = mxCreateDoubleMatrix(nQMatElem, 1, mxREAL);
      mxArray *QcCol = mxCreateDoubleMatrix(nQMatElem, 1, mxREAL);
      mxArray *QcVal = mxCreateDoubleMatrix(nQMatElem, 1, mxREAL);
      mxArray *QcLinear = mxCreateSparse(cprob.nCol, 1, nQRowElem, mxREAL);
      mxArray *QcSense = NULL;
      mxArray *QcRhs = mxCreateDoubleMatrix(1, 1, mxREAL);
      mxArray *QcName = NULL;

      int *qMatRow = (int *) mxCalloc(nQMatElem, sizeof(int));
      int *qMatCol = (int *) mxCalloc(nQMatElem, sizeof(int));
      double *qMatElem = mxGetDoubles(QcVal);
      int *qRowMatIdx = (int *) mxCalloc(nQRowElem, sizeof(int));
      double *qRowMatElem = mxGetDoubles(QcLinear);
      char qRowSense[2];
      double qRowBound = 0.0;
      char *qRowName = NULL;

      COPTMEX_CALL(COPT_GetQConstr(prob, i, qMatRow, qMatCol, qMatElem, nQMatElem, NULL,
                   qRowMatIdx, qRowMatElem, qRowSense, &qRowBound, nQRowElem, NULL));

      double *qMatRow_data = mxGetDoubles(QcRow);
      double *qMatCol_data = mxGetDoubles(QcCol);
      for (int i = 0; i < nQMatElem; ++i) {
        qMatRow_data[i] = qMatRow[i];
        qMatCol_data[i] = qMatCol[i];
      }

      mwIndex *jc_data = mxGetJc(QcLinear);
      mwIndex *ir_data = mxGetIr(QcLinear);

      jc_data[0] = 0;
      jc_data[1] = nQRowElem;
      for (int i = 0; i < nQRowElem; ++i) {
        ir_data[i] = qRowMatIdx[i];
      }

      QcSense = mxCreateString(qRowSense);
      *mxGetDoubles(QcRhs) = qRowBound;

      int nQcNameSize = 0;
      COPTMEX_CALL(COPT_GetQConstrName(prob, i, NULL, 0, &nQcNameSize));

      qRowName = (char *) mxCalloc(nQcNameSize + 1, sizeof(char));
      if (!qRowName) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      COPTMEX_CALL(COPT_GetQConstrName(prob, i, qRowName, nQcNameSize, NULL));
      QcName = mxCreateString(qRowName);

      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCROW, QcRow);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCCOL, QcCol);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCVAL, QcVal);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCLINEAR, QcLinear);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCSENSE, QcSense);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCRHS, QcRhs);
      mxSetField(mprob.quadcon, i, COPTMEX_MODEL_QCNAME, QcName);

      mxFree(qMatRow);
      mxFree(qMatCol);
      mxFree(qRowMatIdx);
      mxFree(qRowName);
    }
  }

  if (hasInfoFile && cprob.hasBasis) {
    mprob.varbasis = mxCreateDoubleMatrix(cprob.nCol, 1, mxREAL);
    mprob.constrbasis = mxCreateDoubleMatrix(cprob.nRow, 1, mxREAL);
    if (!mprob.varbasis || !mprob.constrbasis) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    cprob.colBasis = (int *) mxCalloc(cprob.nCol, sizeof(int));
    cprob.rowBasis = (int *) mxCalloc(cprob.nRow, sizeof(int));
    if (!cprob.colBasis || !cprob.rowBasis) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_CALL(COPT_GetColBasis(prob, cprob.nCol, NULL, cprob.colBasis));
    COPTMEX_CALL(COPT_GetRowBasis(prob, cprob.nRow, NULL, cprob.rowBasis));

    double *colBasis_data = mxGetDoubles(mprob.varbasis);
    double *rowBasis_data = mxGetDoubles(mprob.constrbasis);
    for (int i = 0; i < cprob.nCol; ++i) {
      colBasis_data[i] = cprob.colBasis[i];
    }
    for (int i = 0; i < cprob.nRow; ++i) {
      rowBasis_data[i] = cprob.rowBasis[i];
    }

    mxFree(cprob.colBasis);
    mxFree(cprob.rowBasis);
  }

  retmodel = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!retmodel) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'objsen'
  mxAddField(retmodel, COPTMEX_MODEL_OBJSEN);
  mxSetField(retmodel, 0, COPTMEX_MODEL_OBJSEN, mprob.objsen);
  // 'objcon'
  mxAddField(retmodel, COPTMEX_MODEL_OBJCON);
  mxSetField(retmodel, 0, COPTMEX_MODEL_OBJCON, mprob.objcon);

  if (mprob.A != NULL) {
    // 'A'
    mxAddField(retmodel, COPTMEX_MODEL_A);
    mxSetField(retmodel, 0, COPTMEX_MODEL_A, mprob.A);
  }

  if (cprob.nCol > 0) {
    // 'obj'
    mxAddField(retmodel, COPTMEX_MODEL_OBJ);
    mxSetField(retmodel, 0, COPTMEX_MODEL_OBJ, mprob.obj);
    // 'lb'
    mxAddField(retmodel, COPTMEX_MODEL_LB);
    mxSetField(retmodel, 0, COPTMEX_MODEL_LB, mprob.lb);
    // 'ub'
    mxAddField(retmodel, COPTMEX_MODEL_UB);
    mxSetField(retmodel, 0, COPTMEX_MODEL_UB, mprob.ub);
    // 'vtype'
    mxAddField(retmodel, COPTMEX_MODEL_VTYPE);
    mxSetField(retmodel, 0, COPTMEX_MODEL_VTYPE, mprob.vtype);
    // 'varnames'
    mxAddField(retmodel, COPTMEX_MODEL_VARNAME);
    mxSetField(retmodel, 0, COPTMEX_MODEL_VARNAME, mprob.varnames);
  }

  if (cprob.nRow > 0) {
    //TODO: 'sense'
    // 'lhs'
    mxAddField(retmodel, COPTMEX_MODEL_LHS);
    mxSetField(retmodel, 0, COPTMEX_MODEL_LHS, mprob.lhs);
    // 'rhs'
    mxAddField(retmodel, COPTMEX_MODEL_RHS);
    mxSetField(retmodel, 0, COPTMEX_MODEL_RHS, mprob.rhs);
    // 'constrnames'
    mxAddField(retmodel, COPTMEX_MODEL_CONNAME);
    mxSetField(retmodel, 0, COPTMEX_MODEL_CONNAME, mprob.constrnames);
  }

  // 'sos'
  if (cprob.nSos > 0) {
    mxAddField(retmodel, COPTMEX_MODEL_SOS);
    mxSetField(retmodel, 0, COPTMEX_MODEL_SOS, mprob.sos);
  }

  // 'indicator'
  if (cprob.nIndicator > 0) {
    mxAddField(retmodel, COPTMEX_MODEL_INDICATOR);
    mxSetField(retmodel, 0, COPTMEX_MODEL_INDICATOR, mprob.indicator);
  }

  // 'cone'
  if (cprob.nCone > 0) {
    mxAddField(retmodel, COPTMEX_MODEL_CONE);
    mxSetField(retmodel, 0, COPTMEX_MODEL_CONE, mprob.cone);
  }

  // 'Q'
  if (cprob.nQElem > 0) {
    mxAddField(retmodel, COPTMEX_MODEL_QUADOBJ);
    mxSetField(retmodel, 0, COPTMEX_MODEL_QUADOBJ, mprob.qobj);
  }

  // 'quadcon'
  if (cprob.nQConstr > 0) {
    mxAddField(retmodel, COPTMEX_MODEL_QUADCON);
    mxSetField(retmodel, 0, COPTMEX_MODEL_QUADCON, mprob.quadcon);
  }

  if (hasInfoFile && cprob.hasBasis) {
    // 'varbasis'
    mxAddField(retmodel, COPTMEX_RESULT_VARBASIS);
    mxSetField(retmodel, 0, COPTMEX_RESULT_VARBASIS, mprob.varbasis);
    // 'constrbasis'
    mxAddField(retmodel, COPTMEX_RESULT_CONBASIS);
    mxSetField(retmodel, 0, COPTMEX_RESULT_CONBASIS, mprob.constrbasis);
  }

  *out_model = retmodel;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_model = NULL;
  }

  return retcode;
}

/* Load parameters to problem */
int COPTMEX_setParam(copt_prob *prob, const mxArray *in_param) {
  int retcode = 0;
  char msgbuf[COPT_BUFFSIZE];
  
  int islogging = 1;
  mxArray *logging = NULL;
  for (int i = mxGetNumberOfFields(in_param) - 1; i >= 0; --i) {
    const char *loggingname = mxGetFieldNameByNumber(in_param, i);
    if (mystrcmp(loggingname, COPT_INTPARAM_LOGGING) == 0) {
      logging = mxGetField(in_param, 0, loggingname);
      if (!mxIsScalar(logging) || mxIsChar(logging)) {
        snprintf(msgbuf, COPT_BUFFSIZE, "parameter.%s", COPT_INTPARAM_LOGGING);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }

      islogging = (int) mxGetScalar(logging);
      break;
    }
  }

  if (islogging == 1) {
    COPTMEX_CALL(COPTMEX_dispBanner());
  }
  
  if (logging != NULL) {
    COPTMEX_CALL(COPT_SetIntParam(prob, COPT_INTPARAM_LOGGING, islogging));
  }

  for (int i = 0; i < mxGetNumberOfFields(in_param); ++i) {
    int partype = -1;
    const char *parname = mxGetFieldNameByNumber(in_param, i);
    mxArray *pararray = mxGetField(in_param, 0, parname);

    if (mystrcmp(parname, COPT_INTPARAM_LOGGING) == 0) {
      continue;
    }

    COPTMEX_CALL(COPT_SearchParamAttr(prob, parname, &partype));
    if (partype != 0 && partype != 1) {
      snprintf(msgbuf, COPT_BUFFSIZE, "parameter.%s", parname);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NAME, msgbuf);
      goto exit_cleanup;
    }
    
    if (!mxIsScalar(pararray) || mxIsChar(pararray)) {
      snprintf(msgbuf, COPT_BUFFSIZE, "parameter.%s", parname);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    if (partype == 0) {
      COPTMEX_CALL(COPT_SetDblParam(prob, parname, mxGetScalar(pararray)));
    } else if (partype == 1) {
      COPTMEX_CALL(COPT_SetIntParam(prob, parname, (int) mxGetScalar(pararray)));
    }
  }

exit_cleanup:
  return retcode;
}

static char *COPTMEX_getFileExt(const char *filename) {
  char *tmpfilename = NULL;
  char *lastdot = NULL;
  int lenfile = strlen(filename);

  tmpfilename = (char *) mxCalloc(lenfile + 1, sizeof(char));
  memcpy(tmpfilename, filename, lenfile + 1);

  lastdot = strrchr(tmpfilename, '.');
  if (!lastdot || lastdot == tmpfilename) {
    return "";
  }

  if (lastdot != NULL) {
    if (strcmp(lastdot, ".gz") == 0) {
      *lastdot = '\0';

      lastdot = strrchr(tmpfilename, '.');
      if (!lastdot || lastdot == tmpfilename) {
        return "";
      }
    }
  }

  return lastdot + 1;
}

/* Read optional information from file */
int COPTMEX_readInfo(copt_prob *prob, const mxArray *in_info) {
  int retcode = 0;
  char *filename = NULL;
  char *fileext = NULL;

  COPTMEX_CALL(COPTMEX_getString(in_info, &filename));

  fileext = COPTMEX_getFileExt(filename);
  if (strcmp(fileext, "bas") == 0) {
    COPTMEX_CALL(COPT_ReadBasis(prob, filename));
  } else {
    retcode = COPT_RETCODE_INVALID;
  }

exit_cleanup:
  COPTMEX_freeString(&filename);
  return retcode;
}

/* Read model from file */
int COPTMEX_readModel(copt_prob *prob, const mxArray *in_model) {
  int retcode = 0;
  char *filename = NULL;
  char *fileext = NULL;

  COPTMEX_CALL(COPTMEX_getString(in_model, &filename));

  fileext = COPTMEX_getFileExt(filename);
  if (strcmp(fileext, "mps") == 0) {
    COPTMEX_CALL(COPT_ReadMps(prob, filename));
  } else if (strcmp(fileext, "lp") == 0) {
    COPTMEX_CALL(COPT_ReadLp(prob, filename));
  } else if (strcmp(fileext, "bin") == 0) {
    COPTMEX_CALL(COPT_ReadBin(prob, filename));
  } else if (strcmp(fileext, "dat-s") == 0) {
    COPTMEX_CALL(COPT_ReadSDPA(prob, filename));
  } else if (strcmp(fileext, "cbf") == 0) {
    COPTMEX_CALL(COPT_ReadCbf(prob, filename));
  } else {
    retcode = COPT_RETCODE_INVALID;
  }

exit_cleanup:
  COPTMEX_freeString(&filename);
  return retcode;
}

/* Write model to file */
int COPTMEX_writeModel(copt_prob *prob, const mxArray *out_file) {
  int retcode = 0;
  char *filename = NULL;
  char *fileext = NULL;

  COPTMEX_CALL(COPTMEX_getString(out_file, &filename));

  fileext = COPTMEX_getFileExt(filename);
  if (strcmp(fileext, "mps") == 0) {
    COPTMEX_CALL(COPT_WriteMps(prob, filename));
  } else if (strcmp(fileext, "lp") == 0) {
    COPTMEX_CALL(COPT_WriteLp(prob, filename));
  } else if (strcmp(fileext, "bin") == 0) {
    COPTMEX_CALL(COPT_WriteBin(prob, filename));
  } else if (strcmp(fileext, "cbf") == 0) {
    COPTMEX_CALL(COPT_WriteCbf(prob, filename));
  } else {
    retcode = COPT_RETCODE_INVALID;
  }

exit_cleanup:
  COPTMEX_freeString(&filename);
  return retcode;
}

/* Check if solve problem via cone data */
int COPTMEX_isConeModel(const mxArray *in_model) {
  int ifConeData = 0;
  mxArray *conedata = NULL;
  
  conedata = mxGetField(in_model, 0, COPTMEX_MODEL_CONEDATA);
  if (conedata != NULL) {
    if (COPTMEX_checkConeModel(conedata)) {
      ifConeData = 1;
    }
  }

  return ifConeData;
}

/* Solve cone problem with cone data */
int COPTMEX_solveConeModel(copt_prob *prob, const mxArray *in_model, mxArray **out_result, int ifRetResult) {
  int retcode = 0;
  coptmex_cconeprob cconeprob;
  coptmex_mconeprob mconeprob;
  mxArray *conedata = NULL;
  int *outRowMap = NULL;

  COPTMEX_initCConeProb(&cconeprob);
  COPTMEX_initMConeProb(&mconeprob);

  conedata = mxGetField(in_model, 0, COPTMEX_MODEL_CONEDATA);

  mconeprob.c = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_C);
  mconeprob.A = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_A);
  mconeprob.b = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_B);

  mconeprob.K = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_K);
  mconeprob.f = mxGetField(mconeprob.K, 0, COPTMEX_MODEL_CONEK_F);
  mconeprob.l = mxGetField(mconeprob.K, 0, COPTMEX_MODEL_CONEK_L);
  mconeprob.q = mxGetField(mconeprob.K, 0, COPTMEX_MODEL_CONEK_Q);
  mconeprob.r = mxGetField(mconeprob.K, 0, COPTMEX_MODEL_CONEK_R);
  mconeprob.s = mxGetField(mconeprob.K, 0, COPTMEX_MODEL_CONEK_S);

  mconeprob.objsen = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_OBJSEN);
  mconeprob.objcon = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_OBJCON);
  mconeprob.Q      = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_Q);

  // 'objsen'
  if (mconeprob.objsen != NULL) {
    COPTMEX_CALL(COPTMEX_getObjsen(mconeprob.objsen, &cconeprob.nObjSense));
  }
  // 'objcon'
  if (mconeprob.objcon != NULL) {
    cconeprob.dObjConst = mxGetScalar(mconeprob.objcon);
  }
  // 'Q'
  if (mconeprob.Q != NULL) {
    cconeprob.nQObjElem = mxGetNzmax(mconeprob.Q);

    cconeprob.qObjRow = (int *) mxCalloc(cconeprob.nQObjElem, sizeof(int));
    cconeprob.qObjCol = (int *) mxCalloc(cconeprob.nQObjElem, sizeof(int));
    cconeprob.qObjElem = (double *) mxCalloc(cconeprob.nQObjElem, sizeof(double));
    if (!cconeprob.qObjRow || !cconeprob.qObjCol || !cconeprob.qObjElem) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_csc2coo(mconeprob.Q, cconeprob.qObjRow, cconeprob.qObjCol, cconeprob.qObjElem);
  }

  // 'c'
  if (mconeprob.c != NULL) {
    cconeprob.colObj = mxGetDoubles(mconeprob.c);
  }
  // 'A'
  if (mconeprob.A != NULL) {
    cconeprob.nRow       = mxGetM(mconeprob.A);
    cconeprob.nCol       = mxGetN(mconeprob.A);
    cconeprob.nElem      = mxGetNzmax(mconeprob.A);
    cconeprob.colMatBeg  = (int *) mxCalloc(cconeprob.nCol + 1, sizeof(int));
    cconeprob.colMatIdx  = (int *) mxCalloc(cconeprob.nElem, sizeof(int));
    if (!cconeprob.colMatBeg || !cconeprob.colMatIdx) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    mwIndex *colMatBeg_data = mxGetJc(mconeprob.A);
    mwIndex *colMatIdx_data = mxGetIr(mconeprob.A);
    for (int i = 0; i < cconeprob.nCol + 1; ++i) {
      cconeprob.colMatBeg[i] = (int) colMatBeg_data[i];
    }
    for (int i = 0; i < cconeprob.nElem; ++i) {
      cconeprob.colMatIdx[i] = (int) colMatIdx_data[i];
    }

    cconeprob.colMatElem = mxGetDoubles(mconeprob.A);
  }
  // 'b'
  if (mconeprob.b != NULL) {
    cconeprob.rowRhs = mxGetDoubles(mconeprob.b);
  }

  // 'K'
  if (mconeprob.K != NULL) {
    // 'f'
    if (mconeprob.f != NULL) {
      cconeprob.nFree = (int) mxGetScalar(mconeprob.f);
    }
    // 'l'
    if (mconeprob.l != NULL) {
      cconeprob.nPositive = (int) mxGetScalar(mconeprob.l);
    }
    // 'q'
    if (mconeprob.q != NULL) {
      int nCone = mxGetNumberOfElements(mconeprob.q);
      double *coneDim_data = mxGetDoubles(mconeprob.q);

      if (nCone > 1 || (nCone == 1 && coneDim_data[0] > 0)) {
        cconeprob.nCone = nCone;
        cconeprob.coneDim = (int *) mxCalloc(cconeprob.nCone, sizeof(int));

        for (int i = 0; i < cconeprob.nCone; ++i) {
          cconeprob.coneDim[i] = (int) coneDim_data[i];
        }
      }
    }
    // 'r'
    if (mconeprob.r != NULL) {
      int nRotateCone = mxGetNumberOfElements(mconeprob.r);
      double *rotateConeDim_data = mxGetDoubles(mconeprob.r);

      if (nRotateCone > 1 || (nRotateCone == 1 && rotateConeDim_data[0] > 0)) {
        cconeprob.nRotateCone = nRotateCone;
        cconeprob.rotateConeDim = (int *) mxCalloc(cconeprob.nRotateCone, sizeof(int));

        for (int i = 0; i < cconeprob.nRotateCone; ++i) {
          cconeprob.rotateConeDim[i] = (int) rotateConeDim_data[i];
        }
      }
    }
    // 's'
    if (mconeprob.s != NULL) {
      int nPSD = mxGetNumberOfElements(mconeprob.s);
      double *psdDim_data = mxGetDoubles(mconeprob.s);

      if (nPSD > 1 || (nPSD == 1 && psdDim_data[0] > 0)) {
        cconeprob.nPSD = nPSD;
        cconeprob.psdDim = (int *) mxCalloc(cconeprob.nPSD, sizeof(int));

        for (int i = 0; i < cconeprob.nPSD; ++i) {
          cconeprob.psdDim[i] = (int) psdDim_data[i];
        }
      }
    }
  }

  outRowMap = (int *) mxCalloc(cconeprob.nRow, sizeof(int));
  if (!outRowMap) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // Load cone problem data
  COPTMEX_CALL(COPT_LoadConeProb(prob, cconeprob.nCol, cconeprob.nRow,
    cconeprob.nFree, cconeprob.nPositive, 0, cconeprob.nCone, cconeprob.nRotateCone,
    0, 0, 0, 0, cconeprob.nPSD, cconeprob.nQObjElem, cconeprob.nObjSense, cconeprob.dObjConst,
    cconeprob.colObj, cconeprob.qObjRow, cconeprob.qObjCol, cconeprob.qObjElem,
    cconeprob.colMatBeg, NULL, cconeprob.colMatIdx, cconeprob.colMatElem, cconeprob.rowRhs,
    NULL, NULL, cconeprob.coneDim, cconeprob.rotateConeDim, NULL, NULL, NULL, NULL,
    cconeprob.psdDim, NULL, NULL, NULL, NULL, outRowMap));

  COPTMEX_CALL(COPT_Solve(prob));

  if (ifRetResult == 1) {
    COPTMEX_CALL(COPTMEX_getResult(prob, out_result));

    if (*out_result != NULL) {
      mxArray *rowMap = mxCreateDoubleMatrix(cconeprob.nRow, 1, mxREAL);
      if (rowMap == NULL) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      double *rowMap_data = mxGetDoubles(rowMap);
      for (int i = 0; i < cconeprob.nRow; ++i) {
        rowMap_data[i] = outRowMap[i];
      }

      mxAddField(*out_result, "rowmap");
      mxSetField(*out_result, 0, "rowmap", rowMap);
    }
  }

exit_cleanup:
  if (outRowMap != NULL) {
    mxFree(outRowMap);
  }

  if (cconeprob.qObjRow != NULL) {
    mxFree(cconeprob.qObjRow);
  }
  if (cconeprob.qObjCol != NULL) {
    mxFree(cconeprob.qObjCol);
  }
  if (cconeprob.qObjElem != NULL) {
    mxFree(cconeprob.qObjElem);
  }

  if (cconeprob.coneDim != NULL) {
    mxFree(cconeprob.coneDim);
  }
  if (cconeprob.rotateConeDim != NULL) {
    mxFree(cconeprob.rotateConeDim);
  }
  if (cconeprob.psdDim != NULL) {
    mxFree(cconeprob.psdDim);
  }

  if (cconeprob.colMatBeg != NULL) {
    mxFree(cconeprob.colMatBeg);
  }
  if (cconeprob.colMatIdx != NULL) {
    mxFree(cconeprob.colMatIdx);
  }
  return retcode;
}

/* Extract and load data to problem */
int COPTMEX_loadModel(copt_prob *prob, const mxArray *in_model) {
  int retcode = 0;
  coptmex_cprob cprob;
  coptmex_mprob mprob;

  COPTMEX_initCProb(&cprob);
  COPTMEX_initMProb(&mprob);

  mprob.objsen      = mxGetField(in_model, 0, COPTMEX_MODEL_OBJSEN);
  mprob.objcon      = mxGetField(in_model, 0, COPTMEX_MODEL_OBJCON);
  mprob.A           = mxGetField(in_model, 0, COPTMEX_MODEL_A);
  mprob.obj         = mxGetField(in_model, 0, COPTMEX_MODEL_OBJ);
  mprob.lb          = mxGetField(in_model, 0, COPTMEX_MODEL_LB);
  mprob.ub          = mxGetField(in_model, 0, COPTMEX_MODEL_UB);
  mprob.vtype       = mxGetField(in_model, 0, COPTMEX_MODEL_VTYPE);
  mprob.varnames    = mxGetField(in_model, 0, COPTMEX_MODEL_VARNAME);
  mprob.sense       = mxGetField(in_model, 0, COPTMEX_MODEL_SENSE);
  mprob.lhs         = mxGetField(in_model, 0, COPTMEX_MODEL_LHS);
  mprob.rhs         = mxGetField(in_model, 0, COPTMEX_MODEL_RHS);
  mprob.constrnames = mxGetField(in_model, 0, COPTMEX_MODEL_CONNAME);

  mprob.sos         = mxGetField(in_model, 0, COPTMEX_MODEL_SOS);
  mprob.indicator   = mxGetField(in_model, 0, COPTMEX_MODEL_INDICATOR);
  mprob.cone        = mxGetField(in_model, 0, COPTMEX_MODEL_CONE);

  mprob.qobj        = mxGetField(in_model, 0, COPTMEX_MODEL_QUADOBJ);
  mprob.quadcon     = mxGetField(in_model, 0, COPTMEX_MODEL_QUADCON);

  mprob.varbasis    = mxGetField(in_model, 0, COPTMEX_RESULT_VARBASIS);
  mprob.constrbasis = mxGetField(in_model, 0, COPTMEX_RESULT_CONBASIS);

  mprob.value       = mxGetField(in_model, 0, COPTMEX_RESULT_VALUE);
  mprob.slack       = mxGetField(in_model, 0, COPTMEX_RESULT_SLACK);
  mprob.dual        = mxGetField(in_model, 0, COPTMEX_RESULT_DUAL);
  mprob.redcost     = mxGetField(in_model, 0, COPTMEX_RESULT_REDCOST);

  mprob.mipstart    = mxGetField(in_model, 0, COPTMEX_ADVINFO_MIPSTART);

  if (COPTMEX_checkModel(&mprob) == 0) {
    goto exit_cleanup;
  }

  // 'objsen'
  if (mprob.objsen != NULL) {
    COPTMEX_CALL(COPTMEX_getObjsen(mprob.objsen, &cprob.nObjSen));
  }
  // 'objcon'
  if (mprob.objcon != NULL) {
    cprob.dObjConst = mxGetScalar(mprob.objcon);
  }
  // 'A'
  if (mprob.A != NULL) {
    cprob.nRow       = mxGetM(mprob.A);
    cprob.nCol       = mxGetN(mprob.A);
    cprob.nElem      = mxGetNzmax(mprob.A);
    cprob.colMatBeg  = (int *) mxCalloc(cprob.nCol + 1, sizeof(int));
    cprob.colMatIdx  = (int *) mxCalloc(cprob.nElem, sizeof(int));
    if (!cprob.colMatBeg || !cprob.colMatIdx) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    mwIndex *colMatBeg_data = mxGetJc(mprob.A);
    mwIndex *colMatIdx_data = mxGetIr(mprob.A);
    for (int i = 0; i < cprob.nCol + 1; ++i) {
      cprob.colMatBeg[i] = (int) colMatBeg_data[i];
    }
    for (int i = 0; i < cprob.nElem; ++i) {
      cprob.colMatIdx[i] = (int) colMatIdx_data[i];
    }

    cprob.colMatElem = mxGetDoubles(mprob.A);
  }
  // 'obj'
  if (mprob.obj != NULL) {
    cprob.colCost = mxGetDoubles(mprob.obj);
  }
  // 'lb'
  if (mprob.lb != NULL) {
    cprob.colLower = mxGetDoubles(mprob.lb);
  }
  // 'ub'
  if (mprob.ub != NULL) {
    cprob.colUpper = mxGetDoubles(mprob.ub);
  }
  // 'vtype'
  if (mprob.vtype != NULL) {
    if (mxGetNumberOfElements(mprob.vtype) == cprob.nCol) {
      COPTMEX_CALL(COPTMEX_getString(mprob.vtype, &cprob.colType));
    } else {
      char *vtype = NULL;
      COPTMEX_CALL(COPTMEX_getString(mprob.vtype, &vtype));

      cprob.colType = (char *) mxCalloc(cprob.nCol + 1, sizeof(char));
      if (!cprob.colType) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }
      for (int i = 0; i < cprob.nCol; ++i) {
        cprob.colType[i] = vtype[0];
      }
    }
  }
  // 'varnames'
  if (mprob.varnames != NULL) {
    cprob.colNames = (char **) mxCalloc(cprob.nCol, sizeof(char *));
    for (int i = 0; i < cprob.nCol; ++i) {
      mxArray *nameCell = mxGetCell(mprob.varnames, i);
      COPTMEX_CALL(COPTMEX_getString(nameCell, &cprob.colNames[i]));
    }
  }
  // 'sense', 'lhs' and 'rhs'
  if (mprob.sense == NULL) {
    cprob.rowLower = mxGetDoubles(mprob.lhs);
    cprob.rowUpper = mxGetDoubles(mprob.rhs);
  } else {
    if (mxGetNumberOfElements(mprob.sense) == cprob.nRow) {
      COPTMEX_CALL(COPTMEX_getString(mprob.sense, &cprob.rowSense));
    } else {
      char *rsense = NULL;
      COPTMEX_CALL(COPTMEX_getString(mprob.sense, &rsense));

      cprob.rowSense = (char *) mxCalloc(cprob.nRow + 1, sizeof(char));
      if (!cprob.rowSense) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }
      for (int i = 0; i < cprob.nRow; ++i) {
        cprob.rowSense[i] = rsense[0];
      }
    }

    cprob.rowUpper = mxGetDoubles(mprob.rhs);
  }
  // 'constrnames'
  if (mprob.constrnames != NULL) {
    cprob.rowNames = (char **) mxCalloc(cprob.nRow, sizeof(char *));
    for (int i = 0; i < cprob.nRow; ++i) {
      mxArray *namecell = mxGetCell(mprob.constrnames, i);
      COPTMEX_CALL(COPTMEX_getString(namecell, &cprob.rowNames[i]));
    }
  }

  // Load problem data to COPT problem
  if (cprob.rowSense == NULL) {
    COPTMEX_CALL(COPT_LoadProb(prob, cprob.nCol, cprob.nRow,
                 cprob.nObjSen, cprob.dObjConst, cprob.colCost,
                 cprob.colMatBeg, NULL, cprob.colMatIdx, cprob.colMatElem,
                 cprob.colType, cprob.colLower, cprob.colUpper,
                 NULL, cprob.rowLower, cprob.rowUpper,
                 cprob.colNames, cprob.rowNames));
  } else {
    COPTMEX_CALL(COPT_LoadProb(prob, cprob.nCol, cprob.nRow,
                 cprob.nObjSen, cprob.dObjConst, cprob.colCost,
                 cprob.colMatBeg, NULL, cprob.colMatIdx, cprob.colMatElem,
                 cprob.colType, cprob.colLower, cprob.colUpper,
                 cprob.rowSense, cprob.rowUpper, NULL,
                 cprob.colNames, cprob.rowNames));
  }
  
  // Extract and load the optional SOS part
  if (mprob.sos != NULL) {
    for (int i = 0; i < mxGetNumberOfElements(mprob.sos); ++i) {
      mxArray *sostype_m = mxGetField(mprob.sos, i, COPTMEX_MODEL_SOSTYPE);
      mxArray *sosvars_m = mxGetField(mprob.sos, i, COPTMEX_MODEL_SOSVARS);
      mxArray *soswgts_m = mxGetField(mprob.sos, i, COPTMEX_MODEL_SOSWEIGHT);

      int sosType    = (int) mxGetScalar(sostype_m);
      int sosMatBeg  = 0;
      int sosMatCnt  = (int) mxGetNumberOfElements(sosvars_m);
      int *sosMatIdx = (int *) mxCalloc(sosMatCnt, sizeof(int));
      if (!sosMatIdx) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      double *sosvars_data = mxGetDoubles(sosvars_m);
      for (int i = 0; i < sosMatCnt; ++i) {
        sosMatIdx[i] = (int) sosvars_data[i] - 1;
      }

      double *sosMatWt = NULL;
      if (soswgts_m != NULL) {
        sosMatWt = mxGetDoubles(soswgts_m);
      }

      COPTMEX_CALL(COPT_AddSOSs(prob, 1, &sosType, &sosMatBeg, &sosMatCnt,
                   sosMatIdx, sosMatWt));
      
      mxFree(sosMatIdx);
    }
  }

  // Extract and load the optional indicator part
  if (mprob.indicator != NULL) {
    for (int i = 0; i < mxGetNumberOfElements(mprob.indicator); ++i) {
      mxArray *binVar = mxGetField(mprob.indicator, i, COPTMEX_MODEL_INDICBINVAR);
      mxArray *binVal = mxGetField(mprob.indicator, i, COPTMEX_MODEL_INDICBINVAL);
      mxArray *indicA = mxGetField(mprob.indicator, i, COPTMEX_MODEL_INDICROW);
      mxArray *rSense = mxGetField(mprob.indicator, i, COPTMEX_MODEL_INDICSENSE);
      mxArray *rowBnd = mxGetField(mprob.indicator, i, COPTMEX_MODEL_INDICRHS);

      int binColIdx  = (int) mxGetScalar(binVar) - 1;
      int binColVal  = (int) mxGetScalar(binVal);
      int nRowMatCnt = 0;
      int *rowMatIdx = NULL;
      double *rowMatElem = NULL;

      if (mxIsSparse(indicA)) {
        nRowMatCnt = mxGetNzmax(indicA);
        rowMatIdx  = (int *) mxCalloc(nRowMatCnt, sizeof(int));
        if (!rowMatIdx) {
          retcode = COPT_RETCODE_MEMORY;
          goto exit_cleanup;
        }
        mwIndex *rowMatIdx_data = mxGetIr(indicA);
        for (int i = 0; i < nRowMatCnt; ++i) {
          rowMatIdx[i] = rowMatIdx_data[i];
        }
        rowMatElem = mxGetDoubles(indicA);
      } else {
        double *rowMatElem_data = mxGetDoubles(indicA);
        for (int i = 0; i < mxGetNumberOfElements(indicA); ++i) {
          if (rowMatElem_data[i] != 0) {
            ++nRowMatCnt;
          }
        }
        rowMatIdx = (int *) mxCalloc(nRowMatCnt, sizeof(int));
        rowMatElem = (double *) mxCalloc(nRowMatCnt, sizeof(double));
        if (!rowMatIdx || !rowMatElem) {
          retcode = COPT_RETCODE_MEMORY;
          goto exit_cleanup;
        }
        for (int i = 0, iElem = 0; i < mxGetNumberOfElements(indicA); ++i) {
          if (rowMatElem_data[i] != 0) {
            rowMatIdx[iElem] = i;
            rowMatElem[iElem] = rowMatElem_data[i];
            iElem++;
          }
        }
      }

      char cRowSense[2];
      mxGetString(rSense, cRowSense, 2);
      double dRowBound = mxGetScalar(rowBnd);

      COPTMEX_CALL(COPT_AddIndicator(prob, binColIdx, binColVal, nRowMatCnt,
                   rowMatIdx, rowMatElem, cRowSense[0], dRowBound));

      mxFree(rowMatIdx);
      if (!mxIsSparse(indicA)) {
        mxFree(rowMatElem);
      }
    }
  }

  // Extract and load the optional cone part
  if (mprob.cone != NULL) {
    for (int i = 0; i < mxGetNumberOfElements(mprob.cone); ++i) {
      mxArray *conetype_m = mxGetField(mprob.cone, i, COPTMEX_MODEL_CONETYPE);
      mxArray *conevars_m = mxGetField(mprob.cone, i, COPTMEX_MODEL_CONEVARS);

      int coneType = (int) mxGetScalar(conetype_m);
      int coneBeg = 0;
      int coneCnt = (int) mxGetNumberOfElements(conevars_m);
      int *coneIdx = (int *) mxCalloc(coneCnt, sizeof(int));
      if (!coneIdx) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      double *conevars_data = mxGetDoubles(conevars_m);
      for (int i = 0; i < coneCnt; ++i) {
        coneIdx[i] = (int) conevars_data[i] - 1;
      }

      COPTMEX_CALL(COPT_AddCones(prob, 1, &coneType, &coneBeg, &coneCnt, coneIdx));

      mxFree(coneIdx);
    }
  }

  // Extract and load optional Q objective part
  if (mprob.qobj != NULL) {
    cprob.nQElem = mxGetNzmax(mprob.qobj);
    int *qObjRow = (int *) mxCalloc(cprob.nQElem, sizeof(int));
    int *qObjCol = (int *) mxCalloc(cprob.nQElem, sizeof(int));
    double *qObjElem = (double *) mxCalloc(cprob.nQElem, sizeof(double));
    if (!qObjRow || !qObjCol || !qObjElem) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }

    COPTMEX_csc2coo(mprob.qobj, qObjRow, qObjCol, qObjElem);
    COPTMEX_CALL(COPT_SetQuadObj(prob, cprob.nQElem, qObjRow, qObjCol, qObjElem));

    mxFree(qObjRow);
    mxFree(qObjCol);
    mxFree(qObjElem);
  }

  // Extract and load optional quadratic constraint part
  if (mprob.quadcon != NULL) {
    for (int i = 0; i < mxGetNumberOfElements(mprob.quadcon); ++i) {
      mxArray *QcMat = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCSPMAT);
      mxArray *QcRow = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCROW);
      mxArray *QcCol = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCCOL);
      mxArray *QcVal = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCVAL);
      mxArray *QcLinear = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCLINEAR);
      mxArray *QcSense = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCSENSE);
      mxArray *QcRhs = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCRHS);
      mxArray *QcName = mxGetField(mprob.quadcon, i, COPTMEX_MODEL_QCNAME);

      int nQMatElem = 0;
      int *qMatRow = NULL;
      int *qMatCol = NULL;
      double *qMatElem = NULL;
      int nQRowElem = 0;
      int *qRowMatIdx = NULL;
      double *qRowMatElem = NULL;
      char qRowSense[2];
      double qRowBound = 0.0;
      char qRowName[COPT_BUFFSIZE] = {0};

      if (QcMat != NULL) {
        nQMatElem = mxGetNzmax(QcMat);
        qMatRow = (int *) mxCalloc(nQMatElem, sizeof(int));
        qMatCol = (int *) mxCalloc(nQMatElem, sizeof(int));
        qMatElem = (double *) mxCalloc(nQMatElem, sizeof(double));
        if (!qMatRow || !qMatCol || !qMatElem) {
          retcode = COPT_RETCODE_MEMORY;
          goto exit_cleanup;
        }

        COPTMEX_csc2coo(QcMat, qMatRow, qMatCol, qMatElem);
      } else {
        if (QcRow != NULL && QcCol != NULL && QcVal != NULL) {
          nQMatElem = mxGetNumberOfElements(QcRow);
          qMatRow = (int *) mxCalloc(nQMatElem, sizeof(int));
          qMatCol = (int *) mxCalloc(nQMatElem, sizeof(int));
          if (!qMatRow || !qMatCol) {
            retcode = COPT_RETCODE_MEMORY;
            goto exit_cleanup;
          }

          double *qMatRow_data = mxGetDoubles(QcRow);
          double *qMatCol_data = mxGetDoubles(QcCol);
          qMatElem = mxGetDoubles(QcVal);
          for (int i = 0; i < nQMatElem; ++i) {
            qMatRow[i] = (int) qMatRow_data[i] - 1;
            qMatCol[i] = (int) qMatCol_data[i] - 1;
          }
        }
      }

      if (QcLinear != NULL) {
        if (mxIsSparse(QcLinear)) {
          double *qRowMatElem_data = mxGetDoubles(QcLinear);
          for (int i = 0; i < mxGetNzmax(QcLinear); ++i) {
            if (qRowMatElem_data[i] != 0.0) {
              ++nQRowElem;
            }
          }
          if (nQRowElem > 0) {
            qRowMatIdx = (int *) mxCalloc(nQRowElem, sizeof(int));
            qRowMatElem = (double *) mxCalloc(nQRowElem, sizeof(double));
            if (!qRowMatIdx || !qRowMatElem) {
              retcode = COPT_RETCODE_MEMORY;
              goto exit_cleanup;
            }
            mwIndex *qRowMatIdx_data = mxGetIr(QcLinear);
            for (int i = 0, iElem = 0; i < mxGetNzmax(QcLinear); ++i) {
              if (qRowMatElem_data[i] != 0.0) {
                qRowMatIdx[iElem] = (int) qRowMatIdx_data[i];
                qRowMatElem[iElem] = qRowMatElem_data[i];
                iElem++;
              }
            }
          }
        } else {
          double *qRowMatElem_data = mxGetDoubles(QcLinear);
          for (int i = 0; i < mxGetNumberOfElements(QcLinear); ++i) {
            if (qRowMatElem_data[i] != 0.0) {
              ++nQRowElem;
            }
          }
          if (nQRowElem > 0) {
            qRowMatIdx = (int *) mxCalloc(nQRowElem, sizeof(int));
            qRowMatElem = (double *) mxCalloc(nQRowElem, sizeof(double));
            if (!qRowMatIdx || !qRowMatElem) {
              retcode = COPT_RETCODE_MEMORY;
              goto exit_cleanup;
            }
            for (int i = 0, iElem = 0; i < mxGetNumberOfElements(QcLinear); ++i) {
              if (qRowMatElem_data[i] != 0.0) {
                qRowMatIdx[iElem] = i;
                qRowMatElem[iElem] = qRowMatElem_data[i];
                iElem++;
              }
            }
          }
        }
      }

      if (QcSense != NULL) {
        mxGetString(QcSense, qRowSense, 2);
      } else {
        qRowSense[0] = COPT_LESS_EQUAL;
      }
      qRowBound = mxGetScalar(QcRhs);
      if (QcName != NULL) {
        mxGetString(QcName, qRowName, COPT_BUFFSIZE);
      }

      COPTMEX_CALL(COPT_AddQConstr(prob, nQRowElem, qRowMatIdx, qRowMatElem, 
                   nQMatElem, qMatRow, qMatCol, qMatElem, qRowSense[0], qRowBound,
                   qRowName));

      mxFree(qMatRow);
      mxFree(qMatCol);
      if (QcMat != NULL) {
        mxFree(qMatElem);
      }

      if (nQRowElem > 0) {
        mxFree(qRowMatIdx);
        mxFree(qRowMatElem);
      }
    }
  }

  // Extract and load the optional advanced information
  if (mprob.varbasis != NULL && mprob.constrbasis != NULL) {
    cprob.colBasis = (int *) mxCalloc(cprob.nCol, sizeof(int));
    cprob.rowBasis = (int *) mxCalloc(cprob.nRow, sizeof(int));
    if (!cprob.colBasis || !cprob.rowBasis) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
    
    double *colBasis_data = mxGetDoubles(mprob.varbasis);
    double *rowBasis_data = mxGetDoubles(mprob.constrbasis);
    for (int i = 0; i < cprob.nCol; ++i) {
      cprob.colBasis[i] = (int) colBasis_data[i];
    }
    for (int i = 0; i < cprob.nRow; ++i) {
      cprob.rowBasis[i] = (int) rowBasis_data[i];
    }

    COPTMEX_CALL(COPT_SetBasis(prob, cprob.colBasis, cprob.rowBasis));

    mxFree(cprob.colBasis);
    mxFree(cprob.rowBasis);
  }

  if (mprob.value != NULL && mprob.slack != NULL && mprob.dual != NULL && mprob.redcost != NULL) {
    double *colValue = mxGetDoubles(mprob.value);
    double *colDual  = mxGetDoubles(mprob.redcost);
    double *rowSlack = mxGetDoubles(mprob.slack);
    double *rowDual  = mxGetDoubles(mprob.dual);

    COPTMEX_CALL(COPT_SetLpSolution(prob, colValue, rowSlack, rowDual, colDual));
  }

  if (mprob.mipstart != NULL) {
    int nRowCnt = 0;
    int *rowIdx = NULL;
    double *rowElem = NULL;

    if (mxIsSparse(mprob.mipstart)) {
      nRowCnt = mxGetNzmax(mprob.mipstart);
      rowIdx = (int *) mxCalloc(nRowCnt, sizeof(int));
      if (!rowIdx) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }
      mwIndex *rowIdx_data = mxGetIr(mprob.mipstart);
      for (int i = 0; i < nRowCnt; ++i) {
        rowIdx[i] = rowIdx_data[i];
      }
      rowElem = mxGetDoubles(mprob.mipstart);

      COPTMEX_CALL(COPT_AddMipStart(prob, nRowCnt, rowIdx, rowElem));

      mxFree(rowIdx);
    } else {
      double *rowElem_data = mxGetDoubles(mprob.mipstart);

      nRowCnt = mxGetNumberOfElements(mprob.mipstart);
      rowElem = (double *) mxCalloc(nRowCnt, sizeof(double));
      if (!rowElem) {
        retcode = COPT_RETCODE_MEMORY;
        goto exit_cleanup;
      }

      for (int i = 0; i < nRowCnt; ++i) {
        if (mxIsNaN(rowElem_data[i])) {
          rowElem[i] = COPT_UNDEFINED;
        } else {
          rowElem[i] = rowElem_data[i];
        }
      }

      COPTMEX_CALL(COPT_AddMipStart(prob, nRowCnt, NULL, rowElem));

      mxFree(rowElem); 
    }
  }

exit_cleanup:
  return retcode;
}

/* Extract IIS information */
static int COPTMEX_getIIS(copt_prob *prob, mxArray **out_iis) {
  int retcode = COPT_RETCODE_OK;
  int nRow = 0, nCol = 0, nSos = 0, nIndicator = 0;
  int hasIIS = 0;
  int isMinIIS = 0;
  mxArray *iisInfo = NULL;
  coptmex_ciisinfo ciisinfo;
  coptmex_miisinfo miisinfo;

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASIIS, &hasIIS));
  if (hasIIS == 0) {
    *out_iis = NULL;
    goto exit_cleanup;
  }

  COPTMEX_initCIISInfo(&ciisinfo);
  COPTMEX_initMIISInfo(&miisinfo);

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &nRow));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &nCol));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_SOSS, &nSos));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_INDICATORS, &nIndicator));

  miisinfo.isminiis = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!miisinfo.isminiis) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  if (nCol > 0) {
    miisinfo.varlb = mxCreateLogicalMatrix(nCol, 1);
    miisinfo.varub = mxCreateLogicalMatrix(nCol, 1);
    if (!miisinfo.varlb || !miisinfo.varub) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nRow > 0) {
    miisinfo.constrlb = mxCreateLogicalMatrix(nRow, 1);
    miisinfo.construb = mxCreateLogicalMatrix(nRow, 1);
    if (!miisinfo.constrlb || !miisinfo.construb) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nSos > 0) {
    miisinfo.sos = mxCreateLogicalMatrix(nSos, 1);
    if (!miisinfo.sos) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nIndicator > 0) {
    miisinfo.indicator = mxCreateLogicalMatrix(nIndicator, 1);
    if (!miisinfo.indicator) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nCol > 0) {
    ciisinfo.colLowerIIS = (int *) mxCalloc(nCol, sizeof(int));
    ciisinfo.colUpperIIS = (int *) mxCalloc(nCol, sizeof(int));
    if (!ciisinfo.colLowerIIS || !ciisinfo.colUpperIIS) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nRow > 0) {
    ciisinfo.rowLowerIIS = (int *) mxCalloc(nRow, sizeof(int));
    ciisinfo.rowUpperIIS = (int *) mxCalloc(nRow, sizeof(int));
    if (!ciisinfo.rowLowerIIS || !ciisinfo.rowUpperIIS) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nSos > 0) {
    ciisinfo.sosIIS = (int *) mxCalloc(nSos, sizeof(int));
    if (!ciisinfo.sosIIS) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nIndicator > 0) {
    ciisinfo.indicatorIIS = (int *) mxCalloc(nIndicator, sizeof(int));
    if (!ciisinfo.indicatorIIS) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ISMINIIS, &isMinIIS));

  if (nCol > 0) {
    COPTMEX_CALL(COPT_GetColLowerIIS(prob, nCol, NULL, ciisinfo.colLowerIIS));
    COPTMEX_CALL(COPT_GetColUpperIIS(prob, nCol, NULL, ciisinfo.colUpperIIS));
  }

  if (nRow > 0) {
    COPTMEX_CALL(COPT_GetRowLowerIIS(prob, nRow, NULL, ciisinfo.rowLowerIIS));
    COPTMEX_CALL(COPT_GetRowUpperIIS(prob, nRow, NULL, ciisinfo.rowUpperIIS));
  }

  if (nSos > 0) {
    COPTMEX_CALL(COPT_GetSOSIIS(prob, nSos, NULL, ciisinfo.sosIIS));
  }

  if (nIndicator > 0) {
    COPTMEX_CALL(COPT_GetIndicatorIIS(prob, nIndicator, NULL, ciisinfo.indicatorIIS));
  }

  *mxGetDoubles(miisinfo.isminiis) = isMinIIS;

  if (nCol > 0) {
    mxLogical *colLowerIIS_data = mxGetLogicals(miisinfo.varlb);
    mxLogical *colUpperIIS_data = mxGetLogicals(miisinfo.varub);
    for (int i = 0; i < nCol; ++i) {
      colLowerIIS_data[i] = ciisinfo.colLowerIIS[i];
      colUpperIIS_data[i] = ciisinfo.colUpperIIS[i];
    }
    mxFree(ciisinfo.colLowerIIS);
    mxFree(ciisinfo.colUpperIIS);
  }

  if (nRow > 0) {
    mxLogical *rowLowerIIS_data = mxGetLogicals(miisinfo.constrlb);
    mxLogical *rowUpperIIS_data = mxGetLogicals(miisinfo.construb);
    for (int i = 0; i < nRow; ++i) {
      rowLowerIIS_data[i] = ciisinfo.rowLowerIIS[i];
      rowUpperIIS_data[i] = ciisinfo.rowUpperIIS[i];
    }
    mxFree(ciisinfo.rowLowerIIS);
    mxFree(ciisinfo.rowUpperIIS);
  }

  if (nSos > 0) {
    mxLogical *sosIIS_data = mxGetLogicals(miisinfo.sos);
    for (int i = 0; i < nSos; ++i) {
      sosIIS_data[i] = ciisinfo.sosIIS[i];
    }
    mxFree(ciisinfo.sosIIS);
  }

  if (nIndicator > 0) {
    mxLogical *indicatorIIS_data = mxGetLogicals(miisinfo.indicator);
    for (int i = 0; i < nIndicator; ++i) {
      indicatorIIS_data[i] = ciisinfo.indicatorIIS[i];
    }
    mxFree(ciisinfo.indicatorIIS);
  }

  iisInfo = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!iisInfo) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'isminiis'
  mxAddField(iisInfo, COPTMEX_IIS_ISMINIIS);
  mxSetField(iisInfo, 0, COPTMEX_IIS_ISMINIIS, miisinfo.isminiis);

  if (nCol > 0) {
    // 'varlb'
    mxAddField(iisInfo, COPTMEX_IIS_VARLB);
    mxSetField(iisInfo, 0, COPTMEX_IIS_VARLB, miisinfo.varlb);
    // 'varub'
    mxAddField(iisInfo, COPTMEX_IIS_VARUB);
    mxSetField(iisInfo, 0, COPTMEX_IIS_VARUB, miisinfo.varub);
  }

  if (nRow > 0) {
    // 'constrlb'
    mxAddField(iisInfo, COPTMEX_IIS_CONSTRLB);
    mxSetField(iisInfo, 0, COPTMEX_IIS_CONSTRLB, miisinfo.constrlb);
    // 'construb'
    mxAddField(iisInfo, COPTMEX_IIS_CONSTRUB);
    mxSetField(iisInfo, 0, COPTMEX_IIS_CONSTRUB, miisinfo.construb);
  }

  if (nSos > 0) {
    // 'sos'
    mxAddField(iisInfo, COPTMEX_IIS_SOS);
    mxSetField(iisInfo, 0, COPTMEX_IIS_SOS, miisinfo.sos);
  }

  if (nIndicator > 0) {
    // 'indicator'
    mxAddField(iisInfo, COPTMEX_IIS_INDICATOR);
    mxSetField(iisInfo, 0, COPTMEX_IIS_INDICATOR, miisinfo.indicator);
  }

  // Write out IIS problem
  COPTMEX_CALL(COPT_WriteIIS(prob, "result.iis"));

  *out_iis = iisInfo;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_iis = NULL;
  }

  return retcode;
}

/* Compute IIS for infeasible problem */
int COPTMEX_computeIIS(copt_prob *prob, mxArray **out_iis, int ifRetResult) {
  int retcode = COPT_RETCODE_OK;
  int modelStatus = 0;
  int isMIP = 0;

  // Try to find IIS for the given problem
  COPTMEX_CALL(COPT_ComputeIIS(prob));

  // Extract IIS information
  if (ifRetResult == 1) {
    COPTMEX_CALL(COPTMEX_getIIS(prob, out_iis));
  }

exit_cleanup:
  return retcode;
}

/* Extract feasibility relaxation information */
static int COPTMEX_getFeasRelax(copt_prob *prob, mxArray **out_relax) {
  int retcode = COPT_RETCODE_OK;
  int nRow = 0, nCol = 0;
  int hasFeasRelax = 0;
  mxArray *relaxInfo = NULL;
  coptmex_crelaxinfo crelaxinfo;
  coptmex_mrelaxinfo mrelaxinfo;

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASFEASRELAXSOL, &hasFeasRelax));
  if (hasFeasRelax == 0) {
    *out_relax = NULL;
    goto exit_cleanup;
  }

  COPTMEX_initCRelaxInfo(&crelaxinfo);
  COPTMEX_initMRelaxInfo(&mrelaxinfo);

  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &nRow));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &nCol));

  mrelaxinfo.relaxobj = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!mrelaxinfo.relaxobj) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  if (nCol > 0) {
    mrelaxinfo.relaxlb = mxCreateDoubleMatrix(nCol, 1, mxREAL);
    mrelaxinfo.relaxub = mxCreateDoubleMatrix(nCol, 1, mxREAL);
    if (!mrelaxinfo.relaxlb || !mrelaxinfo.relaxub) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nRow > 0) {
    mrelaxinfo.relaxlhs = mxCreateDoubleMatrix(nRow, 1, mxREAL);
    mrelaxinfo.relaxrhs = mxCreateDoubleMatrix(nRow, 1, mxREAL);
    if (!mrelaxinfo.relaxlhs || !mrelaxinfo.relaxrhs) {
      retcode = COPT_RETCODE_MEMORY;
      goto exit_cleanup;
    }
  }

  if (nCol > 0) {
    crelaxinfo.colLowRlx = mxGetDoubles(mrelaxinfo.relaxlb);
    crelaxinfo.colUppRlx = mxGetDoubles(mrelaxinfo.relaxub);
  }

  if (nRow > 0) {
    crelaxinfo.rowLowRlx = mxGetDoubles(mrelaxinfo.relaxlhs);
    crelaxinfo.rowUppRlx = mxGetDoubles(mrelaxinfo.relaxrhs);
  }

  COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_FEASRELAXOBJ, &crelaxinfo.dObjVal));
  *mxGetDoubles(mrelaxinfo.relaxobj) = crelaxinfo.dObjVal;

  if (nCol > 0) {
    COPTMEX_CALL(COPT_GetColInfo(prob, COPT_DBLINFO_RELAXLB, nCol, NULL, crelaxinfo.colLowRlx));
    COPTMEX_CALL(COPT_GetColInfo(prob, COPT_DBLINFO_RELAXUB, nCol, NULL, crelaxinfo.colUppRlx));
  }

  if (nRow > 0) {
    COPTMEX_CALL(COPT_GetRowInfo(prob, COPT_DBLINFO_RELAXLB, nRow, NULL, crelaxinfo.rowLowRlx));
    COPTMEX_CALL(COPT_GetRowInfo(prob, COPT_DBLINFO_RELAXUB, nRow, NULL, crelaxinfo.rowUppRlx));
  }

  relaxInfo = mxCreateStructMatrix(1, 1, 0, NULL);
  if (!relaxInfo) {
    retcode = COPT_RETCODE_MEMORY;
    goto exit_cleanup;
  }

  // 'relaxobj'
  mxAddField(relaxInfo, COPTMEX_FEASRELAX_OBJ);
  mxSetField(relaxInfo, 0, COPTMEX_FEASRELAX_OBJ, mrelaxinfo.relaxobj);

  if (nCol > 0) {
    // 'relaxlb'
    mxAddField(relaxInfo, COPTMEX_FEASRELAX_LB);
    mxSetField(relaxInfo, 0, COPTMEX_FEASRELAX_LB, mrelaxinfo.relaxlb);
    // 'relaxub'
    mxAddField(relaxInfo, COPTMEX_FEASRELAX_UB);
    mxSetField(relaxInfo, 0, COPTMEX_FEASRELAX_UB, mrelaxinfo.relaxub);
  }

  if (nRow > 0) {
    // 'relaxlhs'
    mxAddField(relaxInfo, COPTMEX_FEASRELAX_LHS);
    mxSetField(relaxInfo, 0, COPTMEX_FEASRELAX_LHS, mrelaxinfo.relaxlhs);
    // 'relaxrhs'
    mxAddField(relaxInfo, COPTMEX_FEASRELAX_RHS);
    mxSetField(relaxInfo, 0, COPTMEX_FEASRELAX_RHS, mrelaxinfo.relaxrhs);
  }

  // Write out feasibility relaxation problem
  COPTMEX_CALL(COPT_WriteRelax(prob, "result.relax"));

  *out_relax = relaxInfo;

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
    *out_relax = NULL;
  }

  return retcode;
}

int COPTMEX_feasRelax(copt_prob *prob, const mxArray *penalty, mxArray **out_relax, int ifRetResult) {
  int retcode = COPT_RETCODE_OK;

  mxArray *lbpen = NULL;
  mxArray *ubpen = NULL;
  mxArray *rhspen = NULL;
  mxArray *upppen = NULL;

  double *colLowPen = NULL;
  double *colUppPen = NULL;
  double *rowBndPen = NULL;
  double *rowUppPen = NULL;

  if (COPTMEX_checkPenalty(prob, penalty) == 0) {
    goto exit_cleanup;
  }

  lbpen = mxGetField(penalty, 0, COPTMEX_PENALTY_LBPEN);
  ubpen = mxGetField(penalty, 0, COPTMEX_PENALTY_UBPEN);
  rhspen = mxGetField(penalty, 0, COPTMEX_PENALTY_RHSPEN);
  upppen = mxGetField(penalty, 0, COPTMEX_PENALTY_UPPPEN);

  if (lbpen != NULL) {
    colLowPen = mxGetDoubles(lbpen);
  }
  if (ubpen != NULL) {
    colUppPen = mxGetDoubles(ubpen);
  }
  if (rhspen != NULL) {
    rowBndPen = mxGetDoubles(rhspen);
  }
  if (upppen != NULL) {
    rowUppPen = mxGetDoubles(upppen);
  }

  // Compute the feasibility relaxation
  COPTMEX_CALL(COPT_FeasRelax(prob, colLowPen, colUppPen, rowBndPen, rowUppPen));

  // Extract feasibility relaxation information
  if (ifRetResult == 1) {
    COPTMEX_CALL(COPTMEX_getFeasRelax(prob, out_relax));
  }

exit_cleanup:
  return retcode;
}
