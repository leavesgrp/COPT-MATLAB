#include "coptmex.h"
#include <stdlib.h>

/* Initialize MEX-style version */
static void COPTMEX_initVersion(coptmex_mversion *version) {
  version->major     = NULL;
  version->minor     = NULL;
  version->technical = NULL;
  return;
}

/* Initialize C-style problem */
static void COPTMEX_initCProb(coptmex_cprob *cprob) {
  // The main part of problem
  cprob->nRow       = 0;
  cprob->nCol       = 0;
  cprob->nElem      = 0;
  cprob->nObjSen    = COPT_MINIMIZE;
  cprob->dObjConst  = 0.0;

  cprob->colMatBeg  = NULL;
  cprob->colMatIdx  = NULL;
  cprob->colMatElem = NULL;

  cprob->colCost    = NULL;
  cprob->colLower   = NULL;
  cprob->colUpper   = NULL;
  cprob->rowLower   = NULL;
  cprob->rowUpper   = NULL;

  cprob->colType    = NULL;
  cprob->rowSense   = NULL;
  cprob->colNames   = NULL;
  cprob->rowNames   = NULL;

  // The optional SOS-part
  cprob->nSos       = 0;
  cprob->nSosSize   = 0;
  cprob->sosType    = NULL;
  cprob->sosMatBeg  = NULL;
  cprob->sosMatCnt  = NULL;
  cprob->sosMatIdx  = NULL;
  cprob->sosMatWt   = NULL;

  // The optional indicator part
  cprob->nIndicator = 0;

  // The optional advanced information
  cprob->hasBasis   = 0;
  cprob->colBasis   = NULL;
  cprob->rowBasis   = NULL;
  return;
}

/* Initialize MEX-style problem */
static void COPTMEX_initMProb(coptmex_mprob *mprob) {
  // The main part of problem
  mprob->objsen      = NULL;
  mprob->objcon      = NULL;
  mprob->A           = NULL;
  mprob->obj         = NULL;
  mprob->lb          = NULL;
  mprob->ub          = NULL;
  mprob->vtype       = NULL;
  mprob->varnames    = NULL;
  mprob->sense       = NULL;
  mprob->lhs         = NULL;
  mprob->rhs         = NULL;
  mprob->constrnames = NULL;

  // The optional SOS part
  mprob->sos         = NULL;

  // The optional indicator part
  mprob->indicator   = NULL;

  // The optional advanced information
  mprob->varbasis    = NULL;
  mprob->constrbasis = NULL;

  mprob->value       = NULL;
  mprob->slack       = NULL;
  mprob->dual        = NULL;
  mprob->redcost     = NULL;

  mprob->mipstart    = NULL;
  return;
}

/* Initialize C-style LP solution */
static void COPTMEX_initCLpSol(coptmex_clpsol *clpsol) {
  clpsol->nRow         = 0;
  clpsol->nCol         = 0;
  clpsol->hasBasis     = 0;
  clpsol->hasLpSol     = 0;

  clpsol->nStatus      = COPT_LPSTATUS_UNSTARTED;
  clpsol->nSimplexIter = 0;
  clpsol->dSolvingTime = 0.0;
  clpsol->dObjVal      = COPT_INFINITY;

  clpsol->colBasis     = NULL;
  clpsol->rowBasis     = NULL;
  clpsol->colValue     = NULL;
  clpsol->colDual      = NULL;
  clpsol->rowSlack     = NULL;
  clpsol->rowDual      = NULL;
  return;
}

/* Initialize C-style MIP solution */
static void COPTMEX_initCMipSol(coptmex_cmipsol *cmipsol) {
  cmipsol->nRow         = 0;
  cmipsol->nCol         = 0;
  cmipsol->hasMipSol    = 0;

  cmipsol->nStatus      = COPT_MIPSTATUS_UNSTARTED;
  cmipsol->nSimplexIter = 0;
  cmipsol->nNodeCnt     = 0;
  cmipsol->dBestGap     = COPT_INFINITY;
  cmipsol->dSolvingTime = 0.0;
  cmipsol->dObjVal      = COPT_INFINITY;
  cmipsol->dBestBnd     = -COPT_INFINITY;

  cmipsol->colValue     = NULL;

  cmipsol->nSolPool     = 0;
  return;
}

/* Initialize MEX-style LP solution */
static void COPTMEX_initMLpSol(coptmex_mlpsol *mlpsol) {
  mlpsol->status      = NULL;
  mlpsol->simplexiter = NULL;
  mlpsol->solvingtime = NULL;
  mlpsol->objval      = NULL;
  mlpsol->varbasis    = NULL;
  mlpsol->constrbasis = NULL;
  mlpsol->value       = NULL;
  mlpsol->redcost     = NULL;
  mlpsol->slack       = NULL;
  mlpsol->dual        = NULL;
  return;
}

/* Initialize MEX-style MIP solution */
static void COPTMEX_initMMipSol(coptmex_mmipsol *mmipsol) {
  mmipsol->status      = NULL;
  mmipsol->simplexiter = NULL;
  mmipsol->nodecnt     = NULL;
  mmipsol->bestgap     = NULL;
  mmipsol->solvingtime = NULL;
  mmipsol->objval      = NULL;
  mmipsol->bestbnd     = NULL;
  mmipsol->value       = NULL;

  mmipsol->solpool     = NULL;
  return;
}

/* Check all parts of a problem */
static int COPTMEX_checkModel(coptmex_mprob *mprob) {
  int nrow = 0, ncol = 0;
  int isvalid = 1;
  char msgbuf[COPT_BUFFSIZE];

  // 'objsen'
  if (mprob->objsen != NULL) {
    if (!mxIsChar(mprob->objsen)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJSEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'objcon'
  if (mprob->objcon != NULL) {
    if (!mxIsScalar(mprob->objcon) || mxIsChar(mprob->objcon)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJCON);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'A'
  if (mprob->A == NULL) {
    isvalid = 0;
    snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_A);
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
    goto exit_cleanup;
  } else {
    if (!mxIsSparse(mprob->A)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_A);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    } else {
      nrow = (int) mxGetM(mprob->A);
      ncol = (int) mxGetN(mprob->A);
    }
  }
  // 'obj'
  if (mprob->obj != NULL) {
    if (!mxIsDouble(mprob->obj)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->obj) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'lb'
  if (mprob->lb != NULL) {
    if (!mxIsDouble(mprob->lb)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->lb) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'ub'
  if (mprob->ub != NULL) {
    if (!mxIsDouble(mprob->ub)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_UB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->ub) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_UB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'vtype'
  if (mprob->vtype != NULL) {
    if (!mxIsChar(mprob->vtype)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VTYPE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->vtype) != ncol &&
        mxGetNumberOfElements(mprob->vtype) != 1) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VTYPE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'sense'
  if (mprob->sense == NULL) {
    // 'lhs'
    if (!mxIsDouble(mprob->lhs)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->lhs) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    // 'rhs'
    if (!mxIsDouble(mprob->rhs)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->rhs) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  } else {
    // 'sense'
    if (!mxIsChar(mprob->sense)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SENSE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->sense) != nrow &&
        mxGetNumberOfElements(mprob->sense) != 1) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SENSE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    // 'rhs'
    if (!mxIsDouble(mprob->rhs)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->rhs) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'varnames'
  if (mprob->varnames != NULL) {
    if (!mxIsCell(mprob->varnames)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VARNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->varnames) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VARNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->varnames); ++i) {
      if (!mxIsChar(mxGetCell(mprob->varnames, i))) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s{%d}", COPTMEX_MODEL_VARNAME, i);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
    }
  }
  // 'constrnames'
  if (mprob->constrnames != NULL) {
    if (!mxIsCell(mprob->constrnames)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->constrnames) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->constrnames); ++i) {
      if (!mxIsChar(mxGetCell(mprob->constrnames, i))) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s{%d}", COPTMEX_MODEL_CONNAME, i);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
    }
  }

  // 'sos'
  if (mprob->sos != NULL) {
    if (!mxIsStruct(mprob->sos)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SOS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->sos); ++i) {
      mxArray *sostype = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSTYPE);
      mxArray *sosvars = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSVARS);
      mxArray *soswght = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSWEIGHT);

      if (sostype == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS,
                 i, COPTMEX_MODEL_SOSTYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsScalar(sostype) || mxIsChar(sostype)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS,
                   i, COPTMEX_MODEL_SOSTYPE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (sosvars == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS,
                 i, COPTMEX_MODEL_SOSVARS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsDouble(sosvars) || mxIsScalar(sosvars) || mxIsSparse(sosvars)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS,
                   i, COPTMEX_MODEL_SOSVARS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (soswght != NULL) {
        if (!mxIsDouble(soswght) || mxIsScalar(soswght) || mxIsSparse(soswght)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS,
                   i, COPTMEX_MODEL_SOSWEIGHT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }

        if (mxGetNumberOfElements(sosvars) != mxGetNumberOfElements(soswght)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s and problem.%s(%d).%s",
                   COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSVARS,
                   COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSWEIGHT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  if (mprob->indicator != NULL) {
    if (!mxIsStruct(mprob->indicator)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_INDICATOR);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->indicator); ++i) {
      mxArray *binvar = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICBINVAR);
      mxArray *binval = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICBINVAL);
      mxArray *indicA = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICROW);
      mxArray *rSense = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICSENSE);
      mxArray *rowBnd = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICRHS);

      if (binvar == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                 i, COPTMEX_MODEL_INDICBINVAR);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsScalar(binvar) || mxIsChar(binvar)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                   i, COPTMEX_MODEL_INDICBINVAR);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (binval == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                 i, COPTMEX_MODEL_INDICBINVAL);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsScalar(binval) || mxIsChar(binval)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                   i, COPTMEX_MODEL_INDICBINVAL);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (indicA == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                 i, COPTMEX_MODEL_INDICROW);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsDouble(indicA)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                   i, COPTMEX_MODEL_INDICROW);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
        if (!mxIsSparse(indicA)) {
          if (mxGetNumberOfElements(indicA) != ncol) {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                     i, COPTMEX_MODEL_INDICROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
        } else {
          if (mxGetN(indicA) != 1) {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                     i, COPTMEX_MODEL_INDICROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
            goto exit_cleanup;
          }
        }
      }

      if (rSense == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                 i, COPTMEX_MODEL_INDICSENSE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsChar(rSense) || !mxIsScalar(rSense)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                   i, COPTMEX_MODEL_INDICSENSE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (rowBnd == NULL) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                 i, COPTMEX_MODEL_INDICRHS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      } else {
        if (!mxIsScalar(rowBnd) || mxIsChar(rowBnd)) {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR,
                   i, COPTMEX_MODEL_INDICRHS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'varbasis'
  if (mprob->varbasis != NULL) {
    if (!mxIsDouble(mprob->varbasis) || mxIsScalar(mprob->varbasis) ||
        mxIsSparse(mprob->varbasis)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VARBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->varbasis) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VARBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'constrbasis'
  if (mprob->constrbasis != NULL) {
    if (!mxIsDouble(mprob->constrbasis) || mxIsScalar(mprob->constrbasis) ||
        mxIsSparse(mprob->constrbasis)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_CONBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->constrbasis) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_CONBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'x'
  if (mprob->value != NULL) {
    if (!mxIsDouble(mprob->value) || mxIsScalar(mprob->value) || mxIsSparse(mprob->value)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VALUE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->value) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VALUE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'rc'
  if (mprob->redcost != NULL) {
    if (!mxIsDouble(mprob->redcost) || mxIsScalar(mprob->redcost) || mxIsSparse(mprob->redcost)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_REDCOST);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->redcost) != ncol) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_REDCOST);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'slack'
  if (mprob->slack != NULL) {
    if (!mxIsDouble(mprob->slack) || mxIsScalar(mprob->slack) || mxIsSparse(mprob->slack)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_SLACK);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->slack) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_SLACK);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'pi'
  if (mprob->dual != NULL) {
    if (!mxIsDouble(mprob->dual) || mxIsScalar(mprob->dual) || mxIsSparse(mprob->dual)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_DUAL);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->dual) != nrow) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_DUAL);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'start'
  if (mprob->mipstart != NULL) {
    if (!mxIsDouble(mprob->mipstart)) {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_ADVINFO_MIPSTART);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (!mxIsSparse(mprob->mipstart)) {
      if (mxGetNumberOfElements(mprob->mipstart) != ncol) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_ADVINFO_MIPSTART);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
        goto exit_cleanup;
      }
    } else {
      if (mxGetN(mprob->mipstart) != 1) {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_ADVINFO_MIPSTART);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
    }
  }

exit_cleanup:
  return isvalid;
}
