#include "coptmex.h"

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
  int retcode = COPT_RETCODE_OK;
  copt_env* env = NULL;
  copt_prob* prob = NULL;
  int retResult = 1;

  // Check if inputs/outputs are valid
  if (nlhs != 0 && nlhs != 1)
  {
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, "outputs");
    goto exit_cleanup;
  }
  if (nlhs == 0)
  {
    retResult = 0;
  }

  if (nrhs == 2 || nrhs == 3)
  {
    if (!mxIsStruct(prhs[0]))
    {
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "problem");
      goto exit_cleanup;
    }
    if (!mxIsStruct(prhs[1]))
    {
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "penalties");
      goto exit_cleanup;
    }
    if (nrhs == 3)
    {
      if (!mxIsStruct(prhs[2]))
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

  // Processing the third argument, if exists.
  if (nrhs == 3)
  {
    // Load and set parameters to problem
    COPTMEX_CALL(COPTMEX_setParam(prob, prhs[2]));
  }
  else
  {
    COPTMEX_CALL(COPTMEX_dispBanner());
  }

  // Extract and load data to problem
  COPTMEX_CALL(COPTMEX_loadModel(prob, prhs[0]));

  // Compute feasibility relaxation and save result
  COPTMEX_CALL(COPTMEX_feasRelax(prob, prhs[1], &plhs[0], retResult));

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
