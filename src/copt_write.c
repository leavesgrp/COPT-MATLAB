#include "coptmex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  int retcode = COPT_RETCODE_OK;
  copt_env *env = NULL;
  copt_prob *prob = NULL;

  // Check if arguments are valid
  if (nlhs != 0) {
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, "outputs");
    goto exit_cleanup;
  }

  if (nrhs == 2) {
    if (!mxIsStruct(prhs[0])) {
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "problem");
      goto exit_cleanup;
    }
    if (!mxIsChar(prhs[1])) {
      COPTMEX_errorMsg(COPTMEX_ERROR_BAD_TYPE, "probfile");
      goto exit_cleanup;
    }
  } else {
    COPTMEX_errorMsg(COPTMEX_ERROR_BAD_NUM, "inputs");
    goto exit_cleanup;
  }

  // Create COPT environment and problem
  COPTMEX_CALL(COPT_CreateEnv(&env));
  COPTMEX_CALL(COPT_CreateProb(env, &prob));

  // Extract and load problem data
  COPTMEX_CALL(COPTMEX_loadModel(prob, prhs[0]));
  // Write problem to file
  COPTMEX_CALL(COPTMEX_writeModel(prob, prhs[1]));

exit_cleanup:
  if (retcode != COPT_RETCODE_OK) {
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
