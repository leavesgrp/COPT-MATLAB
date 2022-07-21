% copt_feasrelax
%
% relaxinfo = copt_feasrelax(problem, penalties)
% relaxinfo = copt_feasrelax(problem, penalties, parameter)
%
% This function computes feasibility relaxation for an infeasible problem. 
%
% Input arguments:
% ------------------
% problem:
%   A MATLAB struct that specify a valid COPT problem.
%
% penalties:
%   A MATLAB struct that specify penalties.
%
% parameter (optional):
%   A MATLAB struct that specify customized parameters.
%
% Output arguments:
% -------------------
% relaxinfo (optional):
%   A MATLAB struct that represent feasibility relaxation information.
%
% Examples usages:
% ------------------
%   problem = copt_read('inf_lp.mps');
%   penalties.lbpen = ones(length(problem.lb), 1);
%   penalties.ubpen = ones(length(problem.ub), 1);
%   relaxinfo = copt_feasrelax(problem, penalties);
%
%   problem = copt_read('inf_lp.mps')
%   penalties.lbpen = ones(length(problem.lb), 1);
%   penalties.ubpen = ones(length(problem.ub), 1);
%   penalties.rhspen = ones(length(problem.rhs), 1);
%   parameter.feasrelaxmode = 1;
%   relaxinfo = copt_feasrelax(problem, penalties, parameter);
%
