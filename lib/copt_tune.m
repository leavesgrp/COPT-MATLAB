% copt_tune
%
% copt_tune(probfile)
% copt_tune(probfile, parameter)
% copt_tune(problem)
% copt_tune(problem, parameter)
%
% This function tune the performance of a given problem.
%
% Input arguments:
% ------------------
% probfile (optional):
%   Name of problem file to read.
%
% problem (optional):
%   A MATLAB struct that specify a valid COPT problem.
%
% parameter (optional):
%   A MATLAB struct that specify customized parameters.
%
% Examples usages:
% -----------------
%   copt_tune('diet.mps');
%
%   parameter.timelimit = 10;
%   copt_tune('diet.mps', parameter);
%
%   problem  = copt_read('diet.mps');
%   copt_tune(problem);
%
%   parameter.timelimit = 10;
%   problem  = copt_read('diet.mps');
%   copt_tune(problem, parameter);
%
