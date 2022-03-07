% copt_computeiis
%
% iisinfo = copt_computeiis(probfile)
% iisinfo = copt_computeiis(probfile, parameter)
% iisinfo = copt_computeiis(problem)
% iisinfo = copt_computeiis(problem, parameter)
%
% This function computes Irreducible Inconsistent Subsystem (IIS) for an
% infeasible problem. 
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
% -------------------
% iisinfo (optional):
%   A MATLAB struct that represent IIS information.
%
% Examples usages:
% ------------------
%   iisinfo = copt_computeiis('inf_lp.mps')
%
%   parameter.timelimit = 10;
%   iisinfo = copt_computeiis('inf_lp.mps', parameter);
%
%   problem = copt_read('inf_lp.mps');
%   iisinfo = copt_computeiis(problem);
%
%   parameter.timelimit = 10;
%   problem = copt_read('inf_lp.mps')
%   iisinfo = copt_computeiis(problem, parameter);
%
