#include "coptmex.h"

extern int COPT_SearchParamAttr(copt_prob *prob, const char *name, int *p_type);

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
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_LPSTATUS, &csol.nStatus));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASBASIS, &csol.hasBasis));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_HASLPSOL, &csol.hasLpSol));

  msol.status      = mxCreateString(COPTMEX_statusInt2Str(csol.nStatus));
  msol.simplexiter = mxCreateDoubleMatrix(1, 1, mxREAL);
  msol.solvingtime = mxCreateDoubleMatrix(1, 1, mxREAL);
  if (!msol.status || !msol.simplexiter || !msol.solvingtime) {
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
  COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_SOLVINGTIME, &csol.dSolvingTime));

  if (csol.hasLpSol) {
    COPTMEX_CALL(COPT_GetDblAttr(prob, COPT_DBLATTR_LPOBJVAL, &csol.dObjVal));
    COPTMEX_CALL(COPT_GetLpSolution(prob, csol.colValue, csol.rowSlack,
                 csol.rowDual, csol.colDual));
  }

  if (csol.hasBasis) {
    COPTMEX_CALL(COPT_GetBasis(prob, csol.colBasis, csol.rowBasis));
  }

  *mxGetDoubles(msol.simplexiter) = csol.nSimplexIter;
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
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_SOSS, &cprob.nSos));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_INDICATORS, &cprob.nIndicator));
  COPTMEX_CALL(COPT_GetIntAttr(prob, COPT_INTATTR_ELEMS, &cprob.nElem));
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
  } else {
    retcode = COPT_RETCODE_INVALID;
  }

exit_cleanup:
  COPTMEX_freeString(&filename);
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
          rowElem[i] = 2 * COPT_INFINITY;
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
