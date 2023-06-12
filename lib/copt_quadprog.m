function [x,fval,exitflag,output,lambda] = copt_quadprog(H,f,A,b,Aeq,beq,lb,ub,options)
% copt_quadprog Solve QP problem using the Cardinal Optimizer
%
% This function implements the 'quadprog' interface of MATLAB Optimization Toolbox.
%
% x = copt_quadprog(H, f, A, b) solve the problem below:
%
%              minimize:   1/2 * x' * H * x + f' * x
%              subject to:
%                             A * x <= b
%
% x = copt_quadprog(H, f, A, b, Aeq, beq) solve the problem below:
%
%              minimize:   1/2 * x' * H * x + f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%
% Set A = [], b = [] if no inequalities exist.
%
% x = copt_quadprog(H, f, A, b, Aeq, beq, lb, ub) solve the problem below:
%
%              minimize:   1/2 * x' * H * x + f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%                           lb <= x <= ub
%
% Set Aeq = [], beq = [] if no equalities exist.
%
% x = copt_quadprog(H, f, A, b, Aeq, beq, lb, ub, options) solve the problem below:
%
%              minimize:   1/2 * x' * H * x + f' * x
%              subject to:
%                             A * x <= b
%                           Aeq * x = beq
%                           lb <= x <= ub
%
% Supported options are:
%
%   options.Display                Logging
%   options.MaxIterations          Maximum number of barrier iterations
%   options.MaxTime                Time limit
%   options.ConstraintTolerance    Primal feasilibity tolerance
%   options.OptimalityTolerance    Dual feasibility tolerance
%
% x = copt_quadprog(problem) solve problem specified by argument 'problem'.
%
% [x, fval] = copt_quadprog(__) solve the problem above, and return variable solution
% and objective value.
%
% [x, fval, exitflag, output] = copt_quadprog(__) solve the problem above, and return
% variable solution, objective value, exit condition and information of the
% optimization process.
%
% Exit conditions are:
%
%    1                             Optimal
%    0                             Iteration limit/Timeout
%   -2                             Infeasible
%   -3                             Unbounded
%
% Output information is:
%
%    output.iterations             Simplex iteration counts
%    output.message                COPT status
%    output.constrviolation        Maximal violation for constraints and bounds
%
% [x, fval, exitflag, output, lambda] = copt_quadprog(__) solve the problem above,
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
  if isstruct(f) && isfield(f, 'solver') && strcmpi(f.solver, 'copt_quadprog')
    isprobonly = 1;

    Hcost   = coptmexgetfield(f, 'H');
    fcost   = coptmexgetfield(f, 'f');
    A       = coptmexgetfield(f, 'Aineq');
    b       = coptmexgetfield(f, 'bineq');
    Aeq     = coptmexgetfield(f, 'Aeq');
    beq     = coptmexgetfield(f, 'beq');
    lb      = coptmexgetfield(f, 'lb');
    ub      = coptmexgetfield(f, 'ub');
    options = coptmexgetfield(f, 'options');

    if isempty(Hcost) || isempty(A) || isempty(b)
      error('copt_linprog: Invalid data of "problem".');
    end
  else
    error('copt_quadprog: Invalid type of "problem".');
  end
else
  isprobonly = 0;

  if nargin < 4 || nargin > 9
    error('copt_quadprog: Invalid number of inputs.');
  end
  if nargin < 9
    options = struct();
  end
  if nargin < 8
    ub = [];
  end
  if nargin < 7
    lb = [];
  end
  if nargin < 6
    beq = [];
  end
  if nargin < 5
    Aeq = [];
  end
end

%% Construct COPT problem
problem.A = [sparse(A); sparse(Aeq)];
ncol = size(problem.A, 2);

if isprobonly == 1
  problem.Q = 0.5 * sparse(Hcost);
  problem.obj = fcost;
else
  problem.Q = 0.5 * sparse(H);
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

%% Construct optimization parameters
parameter = struct();
% Options are created by 'optimoptions'
if isa(options, 'optim.options.SolverOptions')
  ismexoptim = 1;
else
  ismexoptim = 0;
end
% 'Display'
if isfield(options, 'Display') || ismexoptim == 1
  if any(strcmp(options.Display, {'off', 'none'}))
    parameter.Logging = 0;
  end
end
% 'MaxIterations' (only available to Barrier Method in COPT)
if isfield(options, 'MaxIterations') || ismexoptim == 1
  parameter.BarIterLimit = options.MaxIterations;
end
% 'MaxTime'
if isfield(options, 'MaxTime') || ismexoptim == 1
  parameter.TimeLimit = options.MaxTime;
end
% 'ConstraintTolerance'
if isfield(options, 'ConstraintTolerance') || ismexoptim == 1
  parameter.FeasTol = options.ConstraintTolerance;
end
% 'OptimalityTolerance'
if isfield(options, 'OptimalityTolerance') || ismexoptim == 1
  parameter.DualTol = options.OptimalityTolerance;
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
if strcmp(solution.status, 'optimal')
  exitflag = 1;     % Converged
elseif strcmp(solution.status, 'infeasible')
  exitflag = -2;    % No feasible solution
elseif strcmp(solution.status, 'unbounded')
  exitflag = -3;    % Problem is unbounded
else
  exitflag = 0;     % Terminated by limits
end
% 'output'
output.iterations = solution.barrieriter;
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
    lambda.eqlin = -solution.pi((size(A, 1) + 1):end);
  end
end

function val = coptmexgetfield(s, field)
if isfield(s, field)
  val = getfield(s, field);
else
  val = [];
end

%% End of copt_quadprog()
