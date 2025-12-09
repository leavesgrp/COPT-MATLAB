% copt_solve
%
% version  = copt_solve()
% solution = copt_solve(probfile)
% solution = copt_solve(probfile, parameter)
% solution = copt_solve(problem)
% solution = copt_solve(problem, parameter)
%
% This function solves a given problem with customized optimization parameters.
% The LP or MIP solver will be called depends on the type of the given problem,
% and returns a MATLAB struct that contains LP or MIP solution respectively
% upon successful finish of solving.
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
% Output arguments:
% ------------------
% solution (optional):
%   A MATLAB struct that represent LP/MIP solution.
%
% version (optional):
%   A MATLAB struct that represent COPT version
%
% Examples usages:
% -----------------
%   version = copt_solve();
%   fprintf("This is %d.%d.%d\n", version.major, version.minor, version.technical)
%
%   solution = copt_solve('diet.mps');
%
%   parameter.timelimit = 10;
%   solution = copt_solve('diet.mps', parameter);
%
%   problem  = copt_read('diet.mps');
%   solution = copt_solve(problem);
%
%   parameter.timelimit = 10;
%   problem  = copt_read('diet.mps');
%   solution = copt_solve(problem, parameter);
%
