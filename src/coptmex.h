#ifndef __COPTMEX_H__
#define __COPTMEX_H__

#include "copt.h"
#include "mex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define mystrcmp _stricmp
#else
#include <strings.h>
#define mystrcmp strcasecmp
#endif

#ifndef NDEBUG
#define COPTMEX_CALL(func)                                                                                             \
  do                                                                                                                   \
  {                                                                                                                    \
    if ((retcode = (func)) != COPT_RETCODE_OK)                                                                         \
    {                                                                                                                  \
      mexPrintf("ERROR %d: %s:%d\n", retcode, __FILE__, __LINE__);                                                     \
      goto exit_cleanup;                                                                                               \
    }                                                                                                                  \
  } while (0)
#else
#define COPTMEX_CALL(func)                                                                                             \
  do                                                                                                                   \
  {                                                                                                                    \
    if ((retcode = (func)) != COPT_RETCODE_OK)                                                                         \
      goto exit_cleanup;                                                                                               \
  } while (0)
#endif

#define COPTMEX_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define COPTMEX_MIN(a, b) (((a) < (b)) ? (a) : (b))

/* The solution status in string format */
#define COPTMEX_STATUS_UNSTARTED   "unstarted"
#define COPTMEX_STATUS_OPTIMAL     "optimal"
#define COPTMEX_STATUS_INFEASIBLE  "infeasible"
#define COPTMEX_STATUS_UNBOUNDED   "unbounded"
#define COPTMEX_STATUS_INF_OF_UNB  "inf_or_unb"
#define COPTMEX_STATUS_NUMERICAL   "numerical"
#define COPTMEX_STATUS_NODELIMIT   "nodelimit"
#define COPTMEX_STATUS_IMPRECISE   "imprecise"
#define COPTMEX_STATUS_TIMEOUT     "timeout"
#define COPTMEX_STATUS_UNFINISHED  "unfinished"
#define COPTMEX_STATUS_INTERRUPTED "interrupted"

/* The main part of model struct fields */
#define COPTMEX_MODEL_OBJSEN  "objsen"
#define COPTMEX_MODEL_OBJCON  "objcon"
#define COPTMEX_MODEL_A       "A"
#define COPTMEX_MODEL_OBJ     "obj"
#define COPTMEX_MODEL_LB      "lb"
#define COPTMEX_MODEL_UB      "ub"
#define COPTMEX_MODEL_VTYPE   "vtype"
#define COPTMEX_MODEL_VARNAME "varnames"
#define COPTMEX_MODEL_SENSE   "sense"
#define COPTMEX_MODEL_LHS     "lhs"
#define COPTMEX_MODEL_RHS     "rhs"
#define COPTMEX_MODEL_CONNAME "constrnames"

/* The optional part of model struct fields */
#define COPTMEX_MODEL_SOS       "sos"
#define COPTMEX_MODEL_SOSTYPE   "type"
#define COPTMEX_MODEL_SOSVARS   "vars"
#define COPTMEX_MODEL_SOSWEIGHT "weights"

#define COPTMEX_MODEL_INDICATOR   "indicator"
#define COPTMEX_MODEL_INDICBINVAR "binvar"
#define COPTMEX_MODEL_INDICBINVAL "binval"
#define COPTMEX_MODEL_INDICROW    "a"
#define COPTMEX_MODEL_INDICSENSE  "sense"
#define COPTMEX_MODEL_INDICRHS    "rhs"

#define COPTMEX_MODEL_QUADOBJ "Q"

#define COPTMEX_MODEL_QUADCON  "quadcon"
#define COPTMEX_MODEL_QCSPMAT  "Qc"
#define COPTMEX_MODEL_QCROW    "Qrow"
#define COPTMEX_MODEL_QCCOL    "Qcol"
#define COPTMEX_MODEL_QCVAL    "Qval"
#define COPTMEX_MODEL_QCLINEAR "q"
#define COPTMEX_MODEL_QCSENSE  "sense"
#define COPTMEX_MODEL_QCRHS    "rhs"
#define COPTMEX_MODEL_QCNAME   "name"

#define COPTMEX_MODEL_CONE     "cone"
#define COPTMEX_MODEL_CONETYPE "type"
#define COPTMEX_MODEL_CONEVARS "vars"

#define COPTMEX_MODEL_EXPCONE     "expcone"
#define COPTMEX_MODEL_EXPCONETYPE "type"
#define COPTMEX_MODEL_EXPCONEVARS "vars"

#define COPTMEX_MODEL_AFFCONE     "affcone"
#define COPTMEX_MODEL_AFFCONETYPE "type"
#define COPTMEX_MODEL_AFFCONEA    "A"
#define COPTMEX_MODEL_AFFCONEB    "b"
#define COPTMEX_MODEL_AFFCONENAME "name"

#define COPTMEX_MODEL_CONEDATA    "conedata"
#define COPTMEX_MODEL_CONE_OBJSEN "objsen"
#define COPTMEX_MODEL_CONE_OBJCON "objcon"
#define COPTMEX_MODEL_CONE_VTYPE  "vtype"
#define COPTMEX_MODEL_CONE_C      "c"
#define COPTMEX_MODEL_CONE_A      "A"
#define COPTMEX_MODEL_CONE_B      "b"
#define COPTMEX_MODEL_CONE_K      "K"
#define COPTMEX_MODEL_CONE_Q      "Q"

#define COPTMEX_MODEL_CONEK_F  "f"
#define COPTMEX_MODEL_CONEK_L  "l"
#define COPTMEX_MODEL_CONEK_Q  "q"
#define COPTMEX_MODEL_CONEK_R  "r"
#define COPTMEX_MODEL_CONEK_EP "ep"
#define COPTMEX_MODEL_CONEK_ED "ed"
#define COPTMEX_MODEL_CONEK_S  "s"

/* The penalty struct fields */
#define COPTMEX_PENALTY_LBPEN  "lbpen"
#define COPTMEX_PENALTY_UBPEN  "ubpen"
#define COPTMEX_PENALTY_RHSPEN "rhspen"
#define COPTMEX_PENALTY_UPPPEN "upppen"

/* The result struct fields */
#define COPTMEX_RESULT_STATUS     "status"
#define COPTMEX_RESULT_SIMITER    "simplexiter"
#define COPTMEX_RESULT_BARITER    "barrieriter"
#define COPTMEX_RESULT_NODECNT    "nodecnt"
#define COPTMEX_RESULT_BESTGAP    "bestgap"
#define COPTMEX_RESULT_SOLVETIME  "solvingtime"
#define COPTMEX_RESULT_OBJVAL     "objval"
#define COPTMEX_RESULT_BESTBND    "bestbnd"
#define COPTMEX_RESULT_VARBASIS   "varbasis"
#define COPTMEX_RESULT_CONBASIS   "constrbasis"
#define COPTMEX_RESULT_VALUE      "x"
#define COPTMEX_RESULT_REDCOST    "rc"
#define COPTMEX_RESULT_SLACK      "slack"
#define COPTMEX_RESULT_DUAL       "pi"
#define COPTMEX_RESULT_PRIMALRAY  "primalray"
#define COPTMEX_RESULT_DUALFARKAS "dualfarkas"

#define COPTMEX_RESULT_QCSLACK "qcslack"

#define COPTMEX_RESULT_POOL    "pool"
#define COPTMEX_RESULT_POOLOBJ "objval"
#define COPTMEX_RESULT_POOLXN  "xn"

#define COPTMEX_RESULT_PSDX     "psdx"
#define COPTMEX_RESULT_PSDRC    "psdrc"
#define COPTMEX_RESULT_PSDSLACK "psdslack"
#define COPTMEX_RESULT_PSDPI    "psdpi"

/* The advanced information */
#define COPTMEX_ADVINFO_MIPSTART "start"

/* The IIS result struct fields */
#define COPTMEX_IIS_ISMINIIS  "isminiis"
#define COPTMEX_IIS_VARLB     "varlb"
#define COPTMEX_IIS_VARUB     "varub"
#define COPTMEX_IIS_CONSTRLB  "constrlb"
#define COPTMEX_IIS_CONSTRUB  "construb"
#define COPTMEX_IIS_SOS       "sos"
#define COPTMEX_IIS_INDICATOR "indicator"

/* The feasibility relaxation result fields */
#define COPTMEX_FEASRELAX_OBJ   "relaxobj"
#define COPTMEX_FEASRELAX_VALUE "relaxvalue"
#define COPTMEX_FEASRELAX_LB    "relaxlb"
#define COPTMEX_FEASRELAX_UB    "relaxub"
#define COPTMEX_FEASRELAX_LHS   "relaxlhs"
#define COPTMEX_FEASRELAX_RHS   "relaxrhs"

/* The version fields */
#define COPTMEX_VERSION_MAJOR     "major"
#define COPTMEX_VERSION_MINOR     "minor"
#define COPTMEX_VERSION_TECHNICAL "technical"

/* Error types */
#define COPTMEX_ERROR_BAD_TYPE 0
#define COPTMEX_ERROR_BAD_NAME 1
#define COPTMEX_ERROR_BAD_DATA 2
#define COPTMEX_ERROR_BAD_NUM  3
#define COPTMEX_ERROR_BAD_API  4

typedef struct coptmex_mversion_s
{
  mxArray* major;
  mxArray* minor;
  mxArray* technical;
} coptmex_mversion;

typedef struct coptmex_cprob_s
{
  /* The main part of problem */
  int nCol;
  int nRow;
  int nElem;
  int nObjSen;
  double dObjConst;

  int* colMatBeg;
  int* colMatIdx;
  double* colMatElem;

  double* colCost;
  double* colLower;
  double* colUpper;
  double* rowLower;
  double* rowUpper;

  char* colType;
  char* rowSense;
  char** colNames;
  char** rowNames;

  /* The optional SOS part */
  int nSos;
  int nSosSize;
  int* sosType;
  int* sosMatBeg;
  int* sosMatCnt;
  int* sosMatIdx;
  double* sosMatWt;

  /* The optional indicator part */
  int nIndicator;

  /* The optional cone part */
  int nCone;
  int nConeSize;
  int* coneType;
  int* coneBeg;
  int* coneCnt;
  int* coneIdx;

  /* The optional exponential cone part */
  int nExpCone;
  int nExpConeSize;
  int* expConeType;
  int* expConeIdx;

  /* The optional affine cone part */
  int nAffCone;
  int* affMatBeg;
  int* affMatCnt;
  int* affMatIdx;
  double* affMatElem;
  double* affConst;

  /* The optional Q objective part */
  int nQElem;

  /* The optional quadratic constraint part */
  int nQConstr;

  /* The optional advanced information */
  int hasBasis;
  int* colBasis;
  int* rowBasis;
} coptmex_cprob;

typedef struct coptmex_mprob_s
{
  /* The main part of model */
  mxArray* objsen;
  mxArray* objcon;
  mxArray* A;
  mxArray* obj;
  mxArray* lb;
  mxArray* ub;
  mxArray* vtype;
  mxArray* varnames;
  mxArray* sense;
  mxArray* lhs;
  mxArray* rhs;
  mxArray* constrnames;

  /* The optional sos part of model */
  mxArray* sos;

  /* The optional indicator part of model */
  mxArray* indicator;

  /* The optional cone part of model */
  mxArray* cone;

  /* The optional exponential cone part of model */
  mxArray* expcone;

  /* The optional affine cone part of model */
  mxArray* affcone;

  /* The optional Q objective part of model */
  mxArray* qobj;

  /* The optional quadratic constraint part of model */
  mxArray* quadcon;

  /* The optional advanced information */
  mxArray* varbasis;
  mxArray* constrbasis;

  mxArray* value;
  mxArray* slack;
  mxArray* dual;
  mxArray* redcost;

  mxArray* mipstart;
} coptmex_mprob;

typedef struct coptmex_cconeprob_s
{
  int nCol;
  int nRow;
  int nElem;

  int nObjSense;
  double dObjConst;

  int nFree;
  int nPositive;
  int nCone;
  int nRotateCone;
  int nPrimalExpCone;
  int nDualExpCone;
  int nPSD;

  int* coneDim;
  int* rotateConeDim;
  int* psdDim;

  double* colObj;

  int nScalarCol;
  char* colType;

  int nQObjElem;
  int* qObjRow;
  int* qObjCol;
  double* qObjElem;

  int* colMatBeg;
  int* colMatIdx;
  double* colMatElem;

  double* rowRhs;
} coptmex_cconeprob;

typedef struct coptmex_mconeprob_s
{
  /* Main cone data */
  mxArray* c;
  mxArray* A;
  mxArray* b;

  mxArray* K;
  mxArray* f;
  mxArray* l;
  mxArray* q;
  mxArray* r;
  mxArray* ep;
  mxArray* ed;
  mxArray* s;

  /* Optional parts */
  mxArray* objsen;
  mxArray* objcon;
  mxArray* vtype;
  mxArray* Q;
} coptmex_mconeprob;

typedef struct coptmex_clpsol_s
{
  int nRow;
  int nCol;
  int nPSD;
  int nPSDLen;
  int nPSDConstr;
  int nQConstr;
  int hasBasis;
  int hasLpSol;

  int nStatus;
  int nSimplexIter;
  int nBarrierIter;
  double dSolvingTime;
  double dObjVal;

  int* colBasis;
  int* rowBasis;
  double* colValue;
  double* colDual;
  double* rowSlack;
  double* rowDual;
  double* primalRay;
  double* dualFarkas;

  double* qRowSlack;

  double* psdColValue;
  double* psdColDual;
  double* psdRowSlack;
  double* psdRowDual;
} coptmex_clpsol;

typedef struct coptmex_cmipsol_s
{
  int nRow;
  int nCol;
  int hasMipSol;

  int nStatus;
  int nSimplexIter;
  int nNodeCnt;
  double dBestGap;
  double dSolvingTime;
  double dObjVal;
  double dBestBnd;

  double* colValue;

  int nSolPool;
} coptmex_cmipsol;

typedef struct coptmex_ciisinfo_s
{
  int isMinIIS;
  int* colLowerIIS;
  int* colUpperIIS;
  int* rowLowerIIS;
  int* rowUpperIIS;
  int* sosIIS;
  int* indicatorIIS;
} coptmex_ciisinfo;

typedef struct coptmex_crelaxinfo_s
{
  double dObjVal;
  double* colValue;
  double* colLowRlx;
  double* colUppRlx;
  double* rowLowRlx;
  double* rowUppRlx;
} coptmex_crelaxinfo;

typedef struct coptmex_mlpsol_s
{
  mxArray* status;
  mxArray* simplexiter;
  mxArray* barrieriter;
  mxArray* solvingtime;
  mxArray* objval;
  mxArray* varbasis;
  mxArray* constrbasis;
  mxArray* value;
  mxArray* redcost;
  mxArray* slack;
  mxArray* dual;
  mxArray* ray;
  mxArray* farkas;
  mxArray* qcslack;
  mxArray* psdcolvalue;
  mxArray* psdcoldual;
  mxArray* psdrowslack;
  mxArray* psdrowdual;
} coptmex_mlpsol;

typedef struct coptmex_mmipsol_s
{
  mxArray* status;
  mxArray* simplexiter;
  mxArray* nodecnt;
  mxArray* bestgap;
  mxArray* solvingtime;
  mxArray* objval;
  mxArray* bestbnd;
  mxArray* value;

  mxArray* solpool;
} coptmex_mmipsol;

typedef struct coptmex_miisinfo_s
{
  mxArray* isminiis;
  mxArray* varlb;
  mxArray* varub;
  mxArray* constrlb;
  mxArray* construb;
  mxArray* sos;
  mxArray* indicator;
} coptmex_miisinfo;

typedef struct coptmex_mrelaxinfo_s
{
  mxArray* relaxobj;
  mxArray* relaxvalue;
  mxArray* relaxlb;
  mxArray* relaxub;
  mxArray* relaxlhs;
  mxArray* relaxrhs;
} coptmex_mrelaxinfo;

/* Display error message */
void COPTMEX_errorMsg(int errcode, const char* errinfo);
/* Display banner */
int COPTMEX_dispBanner(void);

/* Extract version of COPT */
int COPTMEX_getVersion(mxArray** out_version);
/* Extract objective sense */
int COPTMEX_getObjsen(const mxArray* in_objsen, int* out_objsen);
/* Extract and save result */
int COPTMEX_getResult(copt_prob* prob, mxArray** out_result);
/* Extract model data */
int COPTMEX_getModel(copt_prob* prob, int nfiles, const mxArray** in_files, mxArray** out_model);

/* Load parameters to problem */
int COPTMEX_setParam(copt_prob* prob, const mxArray* in_param);
/* Read optional information from file */
int COPTMEX_readInfo(copt_prob* prob, const mxArray* in_info);
/* Read model from file */
int COPTMEX_readModel(copt_prob* prob, const mxArray* in_model);
/* Write model to file */
int COPTMEX_writeModel(copt_prob* prob, const mxArray* out_file);
/* Extract and load data to model */
int COPTMEX_loadModel(copt_prob* prob, const mxArray* in_model);
/* Solve problem */
int COPTMEX_solveModel(copt_prob* prob, const mxArray* in_model, int ifRead, mxArray** out_result, int ifRetResult);

/* Check if solve problem via cone data */
int COPTMEX_isConeModel(const mxArray* in_model);
/* Load problem with cone data */
int COPTMEX_loadConeModel(copt_prob* prob, const mxArray* in_model, int* p_nRow, int** p_outMap);
/* Solve cone problem with cone data */
int COPTMEX_solveConeModel(copt_prob* prob, const mxArray* in_model, mxArray** out_result, int ifRetResult);

/* Compute IIS for infeasible problem */
int COPTMEX_computeIIS(copt_prob* prob, mxArray** out_iis, int ifRetResult);

/* Feasibility relaxation for infeasible problem */
int COPTMEX_feasRelax(copt_prob* prob, const mxArray* penalty, mxArray** out_relax, int ifRetResult);

/* Get default parameters */
int COPTMEX_getDefaultParams(copt_prob* prob, mxArray** out_param);

#endif
