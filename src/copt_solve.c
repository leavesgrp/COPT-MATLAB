#include "coptmex.h"

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
  int retcode = COPT_RETCODE_OK;
  copt_env* env = NULL;
  copt_prob* prob = NULL;
  int retResult = 1;
  int ifConeData = 0;

  // Check if inputs/outputs are valid
  if (nlhs != 1 && nlhs != 0)
  {
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, "outputs");
    goto exit_cleanup;
  }
  if (nlhs == 0)
  {
    retResult = 0;
  }

  if (nrhs == 0)
  {
    COPTMEX_CALL(COPTMEX_getVersion(&plhs[0]));
    return;
  }
  else if (nrhs == 1 || nrhs == 2)
  {
    if (!mxIsChar(prhs[0]) && !mxIsStruct(prhs[0]))
    {
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "problem/probfile");
      goto exit_cleanup;
    }
    if (nrhs == 2)
    {
      if (!mxIsStruct(prhs[1]))
      {
        COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "parameter");
        goto exit_cleanup;
      }
    }
  }
  else
  {
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, "inputs");
    goto exit_cleanup;
  }

  // Create COPT environment and problem
  COPTMEX_CALL(COPT_CreateEnv(&env));
  COPTMEX_CALL(COPT_CreateProb(env, &prob));

  // Processing the second argument, if exists.
  if (nrhs == 2)
  {
    // Load and set parameters to problem
    COPTMEX_CALL(COPTMEX_setParam(prob, prhs[1]));
  }
  else
  {
    COPTMEX_CALL(COPTMEX_dispBanner());
  }

  // Processing the first argument
  //  1. 'string': a valid problem file;
  //  2. 'struct': a struct that specify the problem data.
  if (mxIsChar(prhs[0]))
  {
    // Read and solve the problem from file
    COPTMEX_CALL(COPTMEX_solveModel(prob, prhs[0], 1, &plhs[0], retResult));
  }
  else if (mxIsStruct(prhs[0]))
  {
    // Extract and load data to problem
    ifConeData = COPTMEX_isConeModel(prhs[0]);
    if (ifConeData)
    {
      COPTMEX_CALL(COPTMEX_solveConeModel(prob, prhs[0], &plhs[0], retResult));
    }
    else
    {
      COPTMEX_CALL(COPTMEX_solveModel(prob, prhs[0], 0, &plhs[0], retResult));
    }
  }

exit_cleanup:
  if (retcode != COPT_RETCODE_OK)
  {
    char errmsg[COPT_BUFFSIZE];
    char msgbuf[COPT_BUFFSIZE * 2];
    COPT_GetRetcodeMsg(retcode, errmsg, COPT_BUFFSIZE);
    snprintf(msgbuf, COPT_BUFFSIZE * 2, "COPT Error %d: %s", retcode, errmsg);
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_API, msgbuf);
  }

  // Delete COPT problem and environment
  COPT_DeleteProb(&prob);
  COPT_DeleteEnv(&env);

  return;
}
