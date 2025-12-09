#include "coptmex.h"
#include <stdlib.h>

/* Initialize MEX-style version */
static void COPTMEX_initVersion(coptmex_mversion* version)
{
  version->major = NULL;
  version->minor = NULL;
  version->technical = NULL;
  return;
}

/* Initialize C-style problem */
static void COPTMEX_initCProb(coptmex_cprob* cprob)
{
  // The main part of problem
  cprob->nRow = 0;
  cprob->nCol = 0;
  cprob->nElem = 0;
  cprob->nObjSen = COPT_MINIMIZE;
  cprob->dObjConst = 0.0;

  cprob->colMatBeg = NULL;
  cprob->colMatIdx = NULL;
  cprob->colMatElem = NULL;

  cprob->colCost = NULL;
  cprob->colLower = NULL;
  cprob->colUpper = NULL;
  cprob->rowLower = NULL;
  cprob->rowUpper = NULL;

  cprob->colType = NULL;
  cprob->rowSense = NULL;
  cprob->colNames = NULL;
  cprob->rowNames = NULL;

  // The optional SOS part
  cprob->nSos = 0;
  cprob->nSosSize = 0;
  cprob->sosType = NULL;
  cprob->sosMatBeg = NULL;
  cprob->sosMatCnt = NULL;
  cprob->sosMatIdx = NULL;
  cprob->sosMatWt = NULL;

  // The optional indicator part
  cprob->nIndicator = 0;

  // The optional cone part
  cprob->nCone = 0;
  cprob->nConeSize = 0;
  cprob->coneType = NULL;
  cprob->coneBeg = NULL;
  cprob->coneCnt = NULL;
  cprob->coneIdx = NULL;

  // The optional exponential cone part
  cprob->nExpCone = 0;
  cprob->nExpConeSize = 0;
  cprob->expConeType = NULL;
  cprob->expConeIdx = NULL;

  // The optional affine cone part
  cprob->nAffCone = 0;
  cprob->affMatBeg = NULL;
  cprob->affMatCnt = NULL;
  cprob->affMatIdx = NULL;
  cprob->affMatElem = NULL;
  cprob->affConst = NULL;

  // The optional Q objective part
  cprob->nQElem = 0;

  // The optional quadratic constraint part
  cprob->nQConstr = 0;

  // The optional advanced information
  cprob->hasBasis = 0;
  cprob->colBasis = NULL;
  cprob->rowBasis = NULL;
  return;
}

/* Initialize MEX-style problem */
static void COPTMEX_initMProb(coptmex_mprob* mprob)
{
  // The main part of problem
  mprob->objsen = NULL;
  mprob->objcon = NULL;
  mprob->A = NULL;
  mprob->obj = NULL;
  mprob->lb = NULL;
  mprob->ub = NULL;
  mprob->vtype = NULL;
  mprob->varnames = NULL;
  mprob->sense = NULL;
  mprob->lhs = NULL;
  mprob->rhs = NULL;
  mprob->constrnames = NULL;

  // The optional SOS part
  mprob->sos = NULL;

  // The optional indicator part
  mprob->indicator = NULL;

  // The optional cone part
  mprob->cone = NULL;

  // The optional exponential cone part
  mprob->expcone = NULL;

  // The optional affine cone part
  mprob->affcone = NULL;

  // The optional Q objective part
  mprob->qobj = NULL;

  // The optional quadratic constraint part
  mprob->quadcon = NULL;

  // The optional advanced information
  mprob->varbasis = NULL;
  mprob->constrbasis = NULL;

  mprob->value = NULL;
  mprob->slack = NULL;
  mprob->dual = NULL;
  mprob->redcost = NULL;

  mprob->mipstart = NULL;
  return;
}

/* Initialize C-style cone problem */
static void COPTMEX_initCConeProb(coptmex_cconeprob* cconeprob)
{
  cconeprob->nCol = 0;
  cconeprob->nRow = 0;

  cconeprob->nObjSense = COPT_MINIMIZE;
  cconeprob->dObjConst = 0.0;

  cconeprob->nFree = 0;
  cconeprob->nPositive = 0;
  cconeprob->nCone = 0;
  cconeprob->nRotateCone = 0;
  cconeprob->nPrimalExpCone = 0;
  cconeprob->nDualExpCone = 0;
  cconeprob->nPSD = 0;

  cconeprob->coneDim = NULL;
  cconeprob->rotateConeDim = NULL;
  cconeprob->psdDim = NULL;

  cconeprob->colObj = NULL;

  cconeprob->nScalarCol = 0;
  cconeprob->colType = NULL;

  cconeprob->nQObjElem = 0;
  cconeprob->qObjRow = NULL;
  cconeprob->qObjCol = NULL;
  cconeprob->qObjElem = NULL;

  cconeprob->colMatBeg = NULL;
  cconeprob->colMatIdx = NULL;
  cconeprob->colMatElem = NULL;

  cconeprob->rowRhs = NULL;
}

/* Initialize MEX-style cone problem */
static void COPTMEX_initMConeProb(coptmex_mconeprob* mconeprob)
{
  mconeprob->c = NULL;
  mconeprob->A = NULL;
  mconeprob->b = NULL;

  mconeprob->K = NULL;
  mconeprob->f = NULL;
  mconeprob->l = NULL;
  mconeprob->q = NULL;
  mconeprob->r = NULL;
  mconeprob->ep = NULL;
  mconeprob->ed = NULL;
  mconeprob->s = NULL;

  mconeprob->objsen = NULL;
  mconeprob->objcon = NULL;
  mconeprob->vtype = NULL;
  mconeprob->Q = NULL;
}

/* Initialize C-style LP solution */
static void COPTMEX_initCLpSol(coptmex_clpsol* clpsol)
{
  clpsol->nRow = 0;
  clpsol->nCol = 0;
  clpsol->nPSD = 0;
  clpsol->nPSDLen = 0;
  clpsol->nPSDConstr = 0;
  clpsol->nQConstr = 0;
  clpsol->hasBasis = 0;
  clpsol->hasLpSol = 0;

  clpsol->nStatus = COPT_LPSTATUS_UNSTARTED;
  clpsol->nSimplexIter = 0;
  clpsol->nBarrierIter = 0;
  clpsol->dSolvingTime = 0.0;
  clpsol->dObjVal = COPT_INFINITY;

  clpsol->colBasis = NULL;
  clpsol->rowBasis = NULL;
  clpsol->colValue = NULL;
  clpsol->colDual = NULL;
  clpsol->rowSlack = NULL;
  clpsol->rowDual = NULL;
  clpsol->primalRay = NULL;
  clpsol->dualFarkas = NULL;

  clpsol->qRowSlack = NULL;

  clpsol->psdColValue = NULL;
  clpsol->psdColDual = NULL;
  clpsol->psdRowSlack = NULL;
  clpsol->psdRowDual = NULL;
  return;
}

/* Initialize C-style MIP solution */
static void COPTMEX_initCMipSol(coptmex_cmipsol* cmipsol)
{
  cmipsol->nRow = 0;
  cmipsol->nCol = 0;
  cmipsol->hasMipSol = 0;

  cmipsol->nStatus = COPT_MIPSTATUS_UNSTARTED;
  cmipsol->nSimplexIter = 0;
  cmipsol->nNodeCnt = 0;
  cmipsol->dBestGap = COPT_INFINITY;
  cmipsol->dSolvingTime = 0.0;
  cmipsol->dObjVal = COPT_INFINITY;
  cmipsol->dBestBnd = -COPT_INFINITY;

  cmipsol->colValue = NULL;

  cmipsol->nSolPool = 0;
  return;
}

/* Initialize C-style IIS information */
static void COPTMEX_initCIISInfo(coptmex_ciisinfo* ciisinfo)
{
  ciisinfo->isMinIIS = 0;
  ciisinfo->colLowerIIS = NULL;
  ciisinfo->colUpperIIS = NULL;
  ciisinfo->rowLowerIIS = NULL;
  ciisinfo->rowUpperIIS = NULL;
  ciisinfo->sosIIS = NULL;
  ciisinfo->indicatorIIS = NULL;
  return;
}

/* Initialize C-style feasibility relaxation information */
static void COPTMEX_initCRelaxInfo(coptmex_crelaxinfo* crelaxinfo)
{
  crelaxinfo->dObjVal = 0.0;
  crelaxinfo->colValue = NULL;
  crelaxinfo->colLowRlx = NULL;
  crelaxinfo->colUppRlx = NULL;
  crelaxinfo->rowLowRlx = NULL;
  crelaxinfo->rowUppRlx = NULL;
  return;
}

/* Initialize MEX-style LP solution */
static void COPTMEX_initMLpSol(coptmex_mlpsol* mlpsol)
{
  mlpsol->status = NULL;
  mlpsol->simplexiter = NULL;
  mlpsol->barrieriter = NULL;
  mlpsol->solvingtime = NULL;
  mlpsol->objval = NULL;
  mlpsol->varbasis = NULL;
  mlpsol->constrbasis = NULL;
  mlpsol->value = NULL;
  mlpsol->redcost = NULL;
  mlpsol->slack = NULL;
  mlpsol->dual = NULL;
  mlpsol->ray = NULL;
  mlpsol->farkas = NULL;
  mlpsol->qcslack = NULL;
  mlpsol->psdcolvalue = NULL;
  mlpsol->psdcoldual = NULL;
  mlpsol->psdrowslack = NULL;
  mlpsol->psdrowdual = NULL;
  return;
}

/* Initialize MEX-style MIP solution */
static void COPTMEX_initMMipSol(coptmex_mmipsol* mmipsol)
{
  mmipsol->status = NULL;
  mmipsol->simplexiter = NULL;
  mmipsol->nodecnt = NULL;
  mmipsol->bestgap = NULL;
  mmipsol->solvingtime = NULL;
  mmipsol->objval = NULL;
  mmipsol->bestbnd = NULL;
  mmipsol->value = NULL;

  mmipsol->solpool = NULL;
  return;
}

/* Initialize MEX-style IIS information */
static void COPTMEX_initMIISInfo(coptmex_miisinfo* miisinfo)
{
  miisinfo->isminiis = NULL;
  miisinfo->varlb = NULL;
  miisinfo->varub = NULL;
  miisinfo->constrlb = NULL;
  miisinfo->construb = NULL;
  miisinfo->sos = NULL;
  miisinfo->indicator = NULL;
  return;
}

/* Initialize MEX-style feasibility relaxation information */
static void COPTMEX_initMRelaxInfo(coptmex_mrelaxinfo* mrelaxinfo)
{
  mrelaxinfo->relaxobj = NULL;
  mrelaxinfo->relaxvalue = NULL;
  mrelaxinfo->relaxlb = NULL;
  mrelaxinfo->relaxub = NULL;
  mrelaxinfo->relaxlhs = NULL;
  mrelaxinfo->relaxrhs = NULL;
  return;
}

/* Check parts of penalty */
static int COPTMEX_checkPenalty(copt_prob* prob, const mxArray* penalty)
{
  int isvalid = 1;
  char msgbuf[COPT_BUFFSIZE];

  mxArray* lbpen = NULL;
  mxArray* ubpen = NULL;
  mxArray* rhspen = NULL;
  mxArray* upppen = NULL;

  int nCol = 0, nRow = 0;

  COPT_GetIntAttr(prob, COPT_INTATTR_COLS, &nCol);
  COPT_GetIntAttr(prob, COPT_INTATTR_ROWS, &nRow);

  lbpen = mxGetField(penalty, 0, COPTMEX_PENALTY_LBPEN);
  ubpen = mxGetField(penalty, 0, COPTMEX_PENALTY_UBPEN);
  rhspen = mxGetField(penalty, 0, COPTMEX_PENALTY_RHSPEN);
  upppen = mxGetField(penalty, 0, COPTMEX_PENALTY_UPPPEN);

  if (lbpen != NULL)
  {
    if (!mxIsDouble(lbpen))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_LBPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(lbpen) != nCol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_LBPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }

  if (ubpen != NULL)
  {
    if (!mxIsDouble(ubpen))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_UBPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(ubpen) != nCol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_UBPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }

  if (rhspen != NULL)
  {
    if (!mxIsDouble(rhspen))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_RHSPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(rhspen) != nRow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_RHSPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }

  if (upppen != NULL)
  {
    if (!mxIsDouble(upppen))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_UPPPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(upppen) != nRow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "penalty.%s", COPTMEX_PENALTY_UPPPEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }

exit_cleanup:
  return isvalid;
}

/* Check all parts of a cone problem */
static int COPTMEX_checkConeModel(mxArray* conedata)
{
  int nrow = 0, ncol = 0, nscalarcol = 0;
  int isvalid = 1;
  char msgbuf[COPT_BUFFSIZE];

  // 'conedata'
  if (conedata != NULL)
  {
    if (!mxIsStruct(conedata))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONEDATA);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    // 'A'
    mxArray* A = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_A);
    if (A == NULL)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_A);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
      goto exit_cleanup;
    }
    else
    {
      if (!mxIsSparse(A))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_A);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        nrow = (int)mxGetM(A);
        ncol = (int)mxGetN(A);
      }
    }

    // 'K'
    mxArray* K = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_K);
    if (K == NULL)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_K);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
      goto exit_cleanup;
    }
    else
    {
      if (!mxIsStruct(K))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_K);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
    }

    // 'f'
    mxArray* f = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_F);
    if (f != NULL)
    {
      if (!mxIsScalar(f) || mxIsChar(f))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_F);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      nscalarcol += (int)mxGetScalar(f);
    }

    // 'l'
    mxArray* l = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_L);
    if (l != NULL)
    {
      if (!mxIsScalar(l) || mxIsChar(l))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_L);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      nscalarcol += (int)mxGetScalar(l);
    }

    // 'q'
    mxArray* q = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_Q);
    if (q != NULL)
    {
      if (!mxIsDouble(q))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_Q);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }

      int nCone = mxGetNumberOfElements(q);
      double* coneDim_data = mxGetDoubles(q);
      for (int i = 0; i < nCone; ++i)
      {
        nscalarcol += (int)coneDim_data[i];
      }
    }

    // 'r'
    mxArray* r = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_R);
    if (r != NULL)
    {
      if (!mxIsDouble(r))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_R);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }

      int nRotateCone = mxGetNumberOfElements(r);
      double* rotateConeDim_data = mxGetDoubles(r);
      for (int i = 0; i < nRotateCone; ++i)
      {
        nscalarcol += (int)rotateConeDim_data[i];
      }
    }

    // 'ep'
    mxArray* ep = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_EP);
    if (ep != NULL)
    {
      if (!mxIsScalar(ep))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_EP);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      nscalarcol += 3 * ((int)mxGetScalar(ep));
    }

    // 'ed'
    mxArray* ed = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_ED);
    if (ed != NULL)
    {
      if (!mxIsScalar(ed))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_ED);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      nscalarcol += 3 * ((int)mxGetScalar(ed));
    }

    // 's'
    mxArray* s = mxGetField(conedata, 0, COPTMEX_MODEL_CONEK_S);
    if (s != NULL)
    {
      if (!mxIsDouble(s))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONEK_S);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
    }

    // 'c'
    mxArray* c = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_C);
    if (c != NULL)
    {
      if (!mxIsDouble(c))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_C);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (mxGetNumberOfElements(c) != ncol)
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_C);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }
    }

    // 'b'
    mxArray* b = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_B);
    if (b != NULL)
    {
      if (!mxIsDouble(b))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_B);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (mxGetNumberOfElements(b) != nrow)
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_B);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }
    }

    // 'objsen'
    mxArray* objsen = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_OBJSEN);
    if (objsen != NULL)
    {
      if (!mxIsChar(objsen))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_OBJSEN);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
    }

    // 'objcon'
    mxArray* objcon = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_OBJCON);
    if (objcon != NULL)
    {
      if (!mxIsScalar(objcon) || mxIsChar(objcon))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_OBJCON);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
    }

    // 'vtype'
    mxArray* vtype = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_VTYPE);
    if (vtype != NULL)
    {
      if (!mxIsChar(vtype))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_VTYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
        goto exit_cleanup;
      }
      if (mxGetNumberOfElements(vtype) != nscalarcol && mxGetNumberOfElements(vtype) != 1)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_VTYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
        goto exit_cleanup;
      }
    }

    // 'Q'
    mxArray* Q = mxGetField(conedata, 0, COPTMEX_MODEL_CONE_Q);
    if (Q != NULL)
    {
      if (!mxIsSparse(Q))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_Q);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      if (mxGetM(Q) != nscalarcol || mxGetN(Q) != nscalarcol)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.conedata.%s", COPTMEX_MODEL_CONE_Q);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
        goto exit_cleanup;
      }
    }
  }

exit_cleanup:
  return isvalid;
}

/* Check all parts of a problem */
static int COPTMEX_checkModel(coptmex_mprob* mprob)
{
  int nrow = 0, ncol = 0;
  int isvalid = 1;
  char msgbuf[COPT_BUFFSIZE];

  // 'objsen'
  if (mprob->objsen != NULL)
  {
    if (!mxIsChar(mprob->objsen))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJSEN);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'objcon'
  if (mprob->objcon != NULL)
  {
    if (!mxIsScalar(mprob->objcon) || mxIsChar(mprob->objcon))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJCON);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'A'
  if (mprob->A == NULL)
  {
    isvalid = 0;
    snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_A);
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
    goto exit_cleanup;
  }
  else
  {
    if (!mxIsSparse(mprob->A))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_A);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    else
    {
      nrow = (int)mxGetM(mprob->A);
      ncol = (int)mxGetN(mprob->A);
    }
  }
  // 'obj'
  if (mprob->obj != NULL)
  {
    if (!mxIsDouble(mprob->obj))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->obj) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_OBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'lb'
  if (mprob->lb != NULL)
  {
    if (!mxIsDouble(mprob->lb))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->lb) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'ub'
  if (mprob->ub != NULL)
  {
    if (!mxIsDouble(mprob->ub))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_UB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->ub) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_UB);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'vtype'
  if (mprob->vtype != NULL)
  {
    if (!mxIsChar(mprob->vtype))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VTYPE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->vtype) != ncol && mxGetNumberOfElements(mprob->vtype) != 1)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VTYPE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'sense'
  if (mprob->sense == NULL)
  {
    // 'lhs'
    if (!mxIsDouble(mprob->lhs))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->lhs) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_LHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    // 'rhs'
    if (!mxIsDouble(mprob->rhs))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->rhs) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  else
  {
    // 'sense'
    if (!mxIsChar(mprob->sense))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SENSE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->sense) != nrow && mxGetNumberOfElements(mprob->sense) != 1)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SENSE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    // 'rhs'
    if (!mxIsDouble(mprob->rhs))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->rhs) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_RHS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'varnames'
  if (mprob->varnames != NULL)
  {
    if (!mxIsCell(mprob->varnames))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VARNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->varnames) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_VARNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->varnames); ++i)
    {
      if (!mxIsChar(mxGetCell(mprob->varnames, i)))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s{%d}", COPTMEX_MODEL_VARNAME, i);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
    }
  }
  // 'constrnames'
  if (mprob->constrnames != NULL)
  {
    if (!mxIsCell(mprob->constrnames))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->constrnames) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONNAME);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->constrnames); ++i)
    {
      if (!mxIsChar(mxGetCell(mprob->constrnames, i)))
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s{%d}", COPTMEX_MODEL_CONNAME, i);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
    }
  }

  // 'sos'
  if (mprob->sos != NULL)
  {
    if (!mxIsStruct(mprob->sos))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_SOS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->sos); ++i)
    {
      mxArray* sostype = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSTYPE);
      mxArray* sosvars = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSVARS);
      mxArray* soswght = mxGetField(mprob->sos, i, COPTMEX_MODEL_SOSWEIGHT);

      if (sostype == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSTYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(sostype) || mxIsChar(sostype))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSTYPE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (sosvars == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSVARS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsDouble(sosvars) || mxIsScalar(sosvars) || mxIsSparse(sosvars))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSVARS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (soswght != NULL)
      {
        if (!mxIsDouble(soswght) || mxIsScalar(soswght) || mxIsSparse(soswght))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSWEIGHT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }

        if (mxGetNumberOfElements(sosvars) != mxGetNumberOfElements(soswght))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s and problem.%s(%d).%s", COPTMEX_MODEL_SOS, i,
            COPTMEX_MODEL_SOSVARS, COPTMEX_MODEL_SOS, i, COPTMEX_MODEL_SOSWEIGHT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'indicator'
  if (mprob->indicator != NULL)
  {
    if (!mxIsStruct(mprob->indicator))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_INDICATOR);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->indicator); ++i)
    {
      mxArray* binvar = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICBINVAR);
      mxArray* binval = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICBINVAL);
      mxArray* indicA = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICROW);
      mxArray* rSense = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICSENSE);
      mxArray* rowBnd = mxGetField(mprob->indicator, i, COPTMEX_MODEL_INDICRHS);

      if (binvar == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICBINVAR);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(binvar) || mxIsChar(binvar))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICBINVAR);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (binval == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICBINVAL);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(binval) || mxIsChar(binval))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICBINVAL);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (indicA == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICROW);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsDouble(indicA))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICROW);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
        if (!mxIsSparse(indicA))
        {
          if (mxGetNumberOfElements(indicA) != ncol)
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
        }
        else
        {
          if (mxGetN(indicA) != 1)
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
            goto exit_cleanup;
          }
        }
      }

      if (rSense == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICSENSE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsChar(rSense) || !mxIsScalar(rSense))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICSENSE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (rowBnd == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICRHS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(rowBnd) || mxIsChar(rowBnd))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_INDICATOR, i, COPTMEX_MODEL_INDICRHS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'cone'
  if (mprob->cone != NULL)
  {
    if (!mxIsStruct(mprob->cone))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_CONE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->cone); ++i)
    {
      mxArray* conetype = mxGetField(mprob->cone, i, COPTMEX_MODEL_CONETYPE);
      mxArray* conevars = mxGetField(mprob->cone, i, COPTMEX_MODEL_CONEVARS);

      if (conetype == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_CONE, i, COPTMEX_MODEL_CONETYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(conetype) || mxIsChar(conetype))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_CONE, i, COPTMEX_MODEL_CONETYPE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (conevars == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_CONE, i, COPTMEX_MODEL_CONEVARS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsDouble(conevars) || mxIsScalar(conevars) || mxIsSparse(conevars))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_CONE, i, COPTMEX_MODEL_CONEVARS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'expcone'
  if (mprob->expcone != NULL)
  {
    if (!mxIsStruct(mprob->expcone))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_EXPCONE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->expcone); ++i)
    {
      mxArray* conetype = mxGetField(mprob->expcone, i, COPTMEX_MODEL_EXPCONETYPE);
      mxArray* conevars = mxGetField(mprob->expcone, i, COPTMEX_MODEL_EXPCONEVARS);

      if (conetype == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_EXPCONE, i, COPTMEX_MODEL_EXPCONETYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(conetype) || mxIsChar(conetype))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_EXPCONE, i, COPTMEX_MODEL_EXPCONETYPE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (conevars == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_EXPCONE, i, COPTMEX_MODEL_EXPCONEVARS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsDouble(conevars) || mxIsScalar(conevars) || mxIsSparse(conevars))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_EXPCONE, i, COPTMEX_MODEL_EXPCONEVARS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'affcone'
  if (mprob->affcone != NULL)
  {
    if (!mxIsStruct(mprob->affcone))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_AFFCONE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->affcone); ++i)
    {
      mxArray* coneType = mxGetField(mprob->affcone, i, COPTMEX_MODEL_AFFCONETYPE);
      mxArray* coneA = mxGetField(mprob->affcone, i, COPTMEX_MODEL_AFFCONEA);
      mxArray* coneB = mxGetField(mprob->affcone, i, COPTMEX_MODEL_AFFCONEB);
      mxArray* coneName = mxGetField(mprob->affcone, i, COPTMEX_MODEL_AFFCONENAME);

      if (coneType == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONETYPE);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsScalar(coneType) || mxIsChar(coneType))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONETYPE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (coneA == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONEA);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (!mxIsSparse(coneA))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONEA);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (coneB != NULL)
      {
        if (!mxIsDouble(coneB))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONEB);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
        if (mxGetNumberOfElements(coneB) != mxGetM(coneA))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONEB);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }

      if (coneName != NULL)
      {
        if (!mxIsChar(coneName))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_AFFCONE, i, COPTMEX_MODEL_AFFCONENAME);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'Q'
  if (mprob->qobj != NULL)
  {
    if (!mxIsSparse(mprob->qobj))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_QUADOBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetM(mprob->qobj) != ncol || mxGetN(mprob->qobj) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_QUADOBJ);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }

  // 'quadcon'
  if (mprob->quadcon != NULL)
  {
    if (!mxIsStruct(mprob->quadcon))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_MODEL_QUADCON);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }

    for (int i = 0; i < mxGetNumberOfElements(mprob->quadcon); ++i)
    {
      mxArray* QcMat = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCSPMAT);
      mxArray* QcRow = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCROW);
      mxArray* QcCol = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCCOL);
      mxArray* QcVal = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCVAL);
      mxArray* QcLinear = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCLINEAR);
      mxArray* QcSense = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCSENSE);
      mxArray* QcRhs = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCRHS);
      mxArray* QcName = mxGetField(mprob->quadcon, i, COPTMEX_MODEL_QCNAME);

      if (QcMat != NULL)
      {
        if (!mxIsSparse(QcMat))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCSPMAT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
          goto exit_cleanup;
        }
        if (mxGetM(QcMat) != mxGetN(QcMat))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCSPMAT);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
          goto exit_cleanup;
        }
      }
      else
      {
        if (QcRow != NULL && QcCol != NULL && QcVal != NULL)
        {
          if (!mxIsDouble(QcRow))
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
            goto exit_cleanup;
          }
          if (!mxIsDouble(QcCol))
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCCOL);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
            goto exit_cleanup;
          }
          if (!mxIsDouble(QcVal))
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCVAL);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
            goto exit_cleanup;
          }
          if (mxGetNumberOfElements(QcRow) != mxGetNumberOfElements(QcCol))
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCROW);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCCOL);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
          if (mxGetNumberOfElements(QcRow) != mxGetNumberOfElements(QcVal))
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCVAL);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
        }
      }

      if (QcLinear != NULL)
      {
        if (!mxIsDouble(QcLinear))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCLINEAR);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
        if (!mxIsSparse(QcLinear))
        {
          if (mxGetNumberOfElements(QcLinear) != ncol)
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCLINEAR);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
        }
        else
        {
          if (mxGetN(QcLinear) != 1)
          {
            isvalid = 0;
            snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCLINEAR);
            COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
            goto exit_cleanup;
          }
        }
      }

      if (QcMat == NULL && QcRow == NULL && QcCol == NULL && QcVal == NULL && QcLinear == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d)", COPTMEX_MODEL_QUADCON, i);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }

      if (QcSense != NULL)
      {
        if (!mxIsChar(QcSense) || !mxIsScalar(QcSense))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCSENSE);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (QcRhs == NULL)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCRHS);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_DATA, msgbuf);
        goto exit_cleanup;
      }
      else
      {
        if (mxIsChar(QcRhs) || !mxIsScalar(QcRhs))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCRHS);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }

      if (QcName != NULL)
      {
        if (!mxIsChar(QcName))
        {
          isvalid = 0;
          snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s(%d).%s", COPTMEX_MODEL_QUADCON, i, COPTMEX_MODEL_QCNAME);
          COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
          goto exit_cleanup;
        }
      }
    }
  }

  // 'varbasis'
  if (mprob->varbasis != NULL)
  {
    if (!mxIsDouble(mprob->varbasis) || mxIsScalar(mprob->varbasis) || mxIsSparse(mprob->varbasis))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VARBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->varbasis) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VARBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'constrbasis'
  if (mprob->constrbasis != NULL)
  {
    if (!mxIsDouble(mprob->constrbasis) || mxIsScalar(mprob->constrbasis) || mxIsSparse(mprob->constrbasis))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_CONBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->constrbasis) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_CONBASIS);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'x'
  if (mprob->value != NULL)
  {
    if (!mxIsDouble(mprob->value) || mxIsScalar(mprob->value) || mxIsSparse(mprob->value))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VALUE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->value) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_VALUE);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'rc'
  if (mprob->redcost != NULL)
  {
    if (!mxIsDouble(mprob->redcost) || mxIsScalar(mprob->redcost) || mxIsSparse(mprob->redcost))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_REDCOST);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->redcost) != ncol)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_REDCOST);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'slack'
  if (mprob->slack != NULL)
  {
    if (!mxIsDouble(mprob->slack) || mxIsScalar(mprob->slack) || mxIsSparse(mprob->slack))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_SLACK);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->slack) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_SLACK);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'pi'
  if (mprob->dual != NULL)
  {
    if (!mxIsDouble(mprob->dual) || mxIsScalar(mprob->dual) || mxIsSparse(mprob->dual))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_DUAL);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (mxGetNumberOfElements(mprob->dual) != nrow)
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_RESULT_DUAL);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
      goto exit_cleanup;
    }
  }
  // 'start'
  if (mprob->mipstart != NULL)
  {
    if (!mxIsDouble(mprob->mipstart))
    {
      isvalid = 0;
      snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_ADVINFO_MIPSTART);
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, msgbuf);
      goto exit_cleanup;
    }
    if (!mxIsSparse(mprob->mipstart))
    {
      if (mxGetNumberOfElements(mprob->mipstart) != ncol)
      {
        isvalid = 0;
        snprintf(msgbuf, COPT_BUFFSIZE, "problem.%s", COPTMEX_ADVINFO_MIPSTART);
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, msgbuf);
        goto exit_cleanup;
      }
    }
    else
    {
      if (mxGetN(mprob->mipstart) != 1)
      {
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
