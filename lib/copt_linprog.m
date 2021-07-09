function [x,fval,exitflag,output,lambda] = copt_linprog(f,A,b,Aeq,beq,lb,ub,options)
% copt_linprog Solve LP problem using the Cardinal Optimizer
%
% This function implements the 'linprog' interface of MATLAB Optimization Toolbox.
%
% x = copt_linprog(f, A, b) solve the problem below:
%
%              minimize:        f' * x
%              subject to:
%                             A * x <= b
%
% x = copt_linprog(f, A, b, Aeq, beq) solve the problem below:
%
%              minimize:        f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%
% Set A = [], b = [] if no inequalities exist.
%
% x = copt_linprog(f, A, b, Aeq, beq, lb, ub) solve the problem below:
%
%              minimize:        f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%                           lb <= x <= ub
%
% Set Aeq = [], beq = [] if no equalities exist.
%
% x = copt_linprog(f, A, b, Aeq, beq, lb, ub, options) solve the problem below:
%
%              minimize:        f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%                           lb <= x <= ub
%
% Supported options are:
%
%   options.ConstraintTolerance    Primal feasilibity tolerance
%   options.Display                Logging
%   options.MaxTime                Time limit
%   options.OptimalityTolerance    Dual feasibility tolerance
%   options.Preprocess             Presolve
%
% x = copt_linprog(problem) solve problem specified by argument 'problem'.
%
% [x, fval] = copt_linprog(__) solve the problem above, and return variable solution
% and objective value.
%
% [x, fval, exitflag, output] = copt_linprog(__) solve the problem above, and return
% variable solution, objective value, exit condition and information of the
% optimization process.
%
% Exit conditions are:
%
%    1                             Optimal
%    0                             Timeout
%   -2                             Infeasible
%   -3                             Unbounded
%
% Output information is:
%
%    output.iterations             Simplex iteration counts
%    output.message                COPT status
%    output.constrviolation        Maximal violation for constraints and bounds
%
% [x, fval, exitflag, output, lambda] = copt_linprog(__) solve the problem above,
% and return variable solution, objective value, exit condition, information of
% the optimization process and Lagrange multipliers of the solution.
%
% Lagrange multipliers are:
%
%   lambda.lower                   Multipliers of: x >= lb
%   lambda.upper                   Multipliers of: x <= ub
%   lambda.ineqlin                 Multipliers of: A * x <= b
%   lambda.eqlin                   Multipliers of: Aeq * x = beq
%

%% Check input arguments
if nargin == 1
  if isstruct(f) && isfield(f, 'solver') && strcmpi(f.solver, 'copt_linprog')
    isprobonly = 1;

    fcost   = coptmexgetfield(f, 'f');
    A       = coptmexgetfield(f, 'Aineq');
    b       = coptmexgetfield(f, 'bineq');
    Aeq     = coptmexgetfield(f, 'Aeq');
    beq     = coptmexgetfield(f, 'beq');
    lb      = coptmexgetfield(f, 'lb');
    ub      = coptmexgetfield(f, 'ub');
    x0      = coptmexgetfield(f, 'x0');
    options = coptmexgetfield(f, 'options');

    if isempty(fcost) || isempty(A) || isempty(b)
      error('copt_linprog: Invalid data of "problem".');
    end
  else
    error('copt_linprog: Invalid type of "problem".');
  end
else
  isprobonly = 0;

  if nargin < 3 || nargin > 8
    error('copt_linprog: Invalid number of inputs.');
  end
  if nargin < 8
    options = struct();
  end
  if nargin < 7
    ub = [];
  end
  if nargin < 6
    lb = [];
  end
  if nargin < 5
    beq = [];
  end
  if nargin < 4
    Aeq = [];
  end
end

%% Construct COPT problem
problem.A = [sparse(A); sparse(Aeq)];
ncol = size(problem.A, 2);

if isprobonly == 1
  problem.obj = fcost;
else
  problem.obj = f;
end
if ~isempty(lb)
  problem.lb = lb;
else
  problem.lb = -inf(ncol, 1);
end
if ~isempty(ub)
  problem.ub = ub;
else
  problem.ub = inf(ncol, 1);
end
problem.sense = [repmat('L', size(A, 1), 1); repmat('E', size(Aeq, 1), 1)];
problem.rhs = full([b(:); beq(:)]);
if ~isempty(x0)
  warning('copt_linprog: LP start feature not supported yet.');
end

%% Construct optimization parameters
parameter = struct();
% Options are created by 'optimoptions'
if isa(options, 'optim.options.SolverOptions')
  ismexoptim = 1;
else
  ismexoptim = 0;
end
% 'ConstraintTolerance'
if isfield(options, 'ConstraintTolerance') || ismexoptim == 1
  parameter.FeasTol = options.ConstraintTolerance;
end
% 'Display'
if isfield(options, 'Display') || ismexoptim == 1
  if any(strcmpi(options.Display, {'off', 'none'}))
    parameter.Logging = 0;
  end
end
% 'MaxTime'
if isfield(options, 'MaxTime') || ismexoptim == 1
  parameter.TimeLimit = options.MaxTime;
end
% 'OptimalityTolerance'
if isfield(options, 'OptimalityTolerance') || ismexoptim == 1
  parameter.DualTol = options.OptimalityTolerance;
end
% 'Preprocess'
if isfield(options, 'Preprocess') || ismexoptim == 1
  if strcmpi(options.Preprocess, 'basic')
    parameter.Presolve = 1;
  elseif strcmpi(options.Preprocess, 'none')
    parameter.Presolve = 0;
  end
end

%% Solve the problem
solution = copt_solve(problem, parameter);

%% Construct the LP solution
% 'x'
if isfield(solution, 'x')
  x = solution.x;
else
  x = [];
end
% 'fval'
if isfield(solution, 'objval')
  fval = solution.objval;
else
  fval = [];
end
% 'exitflag'
if strcmpi(solution.status, 'optimal')
  exitflag = 1;     % Converged
elseif strcmpi(solution.status, 'infeasible')
  exitflag = -2;    % No feasible solution
elseif strcmpi(solution.status, 'unbounded')
  exitflag = -3;    % Problem is unbounded
else
  exitflag = 0;     % Terminated by limits
end
% 'output'
output.iterations = solution.simplexiter;
output.message = solution.status;
output.constrviolation = [];
if isfield(solution, 'x')
  slack = problem.A * solution.x - problem.rhs;
  vio_A = slack(1:size(A, 1));
  vio_Aeq = norm(slack((size(A, 1) + 1):end), inf);
  vio_lb = problem.lb(:) - solution.x;
  if isfield(problem, 'ub')
    vio_ub = solution.x - problem.ub(:);
  else
    vio_ub = 0;
  end
  output.constrviolation = max([0; vio_A; vio_Aeq; vio_lb; vio_ub]);
end
% 'lambda'
lambda.lower   = [];
lambda.upper   = [];
lambda.ineqlin = [];
lambda.eqlin   = [];
if isfield(solution, 'rc')
  lambda.lower = max(0, solution.rc);
  lambda.upper = -min(0, solution.rc);
end
if isfield(solution, 'pi')
  if ~isempty(A)
    lambda.ineqlin = -solution.pi(1:size(A, 1));
  end
  if ~isempty(Aeq)
    lambda.eqlin = -solution.pi((sizeof(A, 1) + 1):end);
  end
end

function val = coptmexgetfield(s, field)
if isfield(s, field)
  val = getfield(s, field);
else
  val = [];
end

%% End of copt_linprog()
