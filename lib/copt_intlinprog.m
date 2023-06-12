function [x,fval,exitflag,output] = copt_intlinprog(f,intcon,A,b,Aeq,beq,lb,ub,x0,options)
% copt_intlinprog Solve MIP problem using the Cardinal Optimizer
%
% This function implements the 'intlinprog' interface of MATLAB Optimization Toolbox.
%
% x = copt_intlinprog(f, intcon, A, b) solve the problem below:
%
%                   minimize:       f' * x
%                   subject to:
%                                 A * x <= b
%                   where x(intcon) are integers.
%
% x = copt_intlinprog(f, intcon, A, b, Aeq, beq) solve the problem below:
%
%                   minimize:       f' * x
%                   subject to:
%                                 A * x <= b
%                                Aeq * x = beq
%                   where x(intcon) are integers.
%
% Set A = [], b = [] if no inequalities exist.
%
% x = copt_intlinprog(f, intcon, A, b, Aeq, beq, lb, ub) solve the problem below:
%
%                   minimize:       f' * x
%                   subject to:
%                                 A * x <= b
%                                Aeq * x = beq
%                                lb <= x <= ub
%                   where x(intcon) are integers.
%
% Set Aeq = [], beq = [] if no equalities exist.
%
% x = copt_intlinprog(f, intcon, A, b, Aeq, beq, lb, ub, x0) solve the problem above,
% with initial values. Set x0 = [] if no initial values exist.
%
% x = copt_intlinprog(f, intcon, A, b, Aeq, beq, lb, ub, x0, options) solve the
% problem above, with initial values. Set x0 = [] if no initial values exist.
%
% Supported options are:
%
%   options.Display                     Logging
%   options.CutGeneration               Cut generation level
%   options.Heuristics                  Heuristic generation level
%   options.IntegerPreprocess           MIP presolve
%   options.MaxNodes                    Node limit
%   options.MaxTime                     Time limit
%   options.RelativeGapTolerance        Relative MIP gap
%   options.AbsoluteGapTolerance        Absolute MIP gap
%   options.ConstraintTolerance         Tolerance of constraints
%   options.IntegerTolerance            Tolerance of integer variables
%
% x = copt_intlinprog(problem) solve problem specified by argument 'problem'.
%
% [x, fval, exitflag, output] = copt_intlinprog(__) solve the problem above,
% and return variable solution, objective value, exit condition and information
% of the optimization process.
%
% Exit conditions are:
%
%    2                                  Stopped with feasible solution
%    1                                  Optimal
%    0                                  Stopped with no feasible solution
%   -2                                  No feasible solution found
%   -3                                  Problem is unbounded
%
% Output information is:
%
%    output.relativegap                 Relative MIP gap
%    output.absolutegap                 Absolute MIP gap
%    output.numfeaspoints               Number of solutions
%    output.numnodes                    Number of explored nodes
%    output.constrviolation             Maximal viloation of constraints and bounds
%    output.message                     COPT status
%

%% Check input arguments
if nargin == 1
  if isstruct(f) && isfield(f, 'solver') && strcmpi(f.solver, 'copt_intlinprog')
    isprobonly = 1;

    fcost   = coptmexgetfield(f, 'f');
    intcon  = coptmexgetfield(f, 'intcon');
    A       = coptmexgetfield(f, 'Aineq');
    b       = coptmexgetfield(f, 'bineq');
    Aeq     = coptmexgetfield(f, 'Aeq');
    beq     = coptmexgetfield(f, 'beq');
    lb      = coptmexgetfield(f, 'lb');
    ub      = coptmexgetfield(f, 'ub');
    x0      = coptmexgetfield(f, 'x0');
    options = coptmexgetfield(f, 'options');

    if isempty(fcost) || isempty(intcon) || isempty(A) || isempty(b)
      error('copt_intlinprog: Invalid data of "problem".');
    end
  else
    error('copt_intlinprog: Invalid type of "problem".');
  end
else
  isprobonly = 0;

  if nargin < 4 || nargin > 10
    error('copt_intlinprog: Invalid number of inputs.');
  end
  if nargin < 10
    options = struct();
  end
  if nargin < 9
    x0 = [];
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
problem.vtype = repmat('C', ncol, 1);
problem.vtype(intcon) = 'I';
problem.sense = [repmat('L', size(A, 1), 1); repmat('E', size(Aeq, 1), 1)];
problem.rhs = full([b(:); beq(:)]);
if ~isempty(x0)
  problem.start = x0;
end

%% Constuct optimization parameters
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
% 'CutGeneration'
if isfield(options, 'CutGeneration') || ismexoptim == 1
  if strcmp(options.CutGeneration, 'none')
    parameter.CutLevel = 0;
  elseif strcmp(options.CutGeneration, 'basic')
    parameter.CutLevel = 1;
  elseif strcmp(options.CutGeneration, 'intermediate')
    parameter.CutLevel = 2;
  elseif strcmp(options.CutGeneration, 'advanced')
    parameter.CutLevel = 3;
  end
end
% 'Heuristics'
if isfield(options, 'Heuristics') || ismexoptim == 1
  if strcmp(options.Heuristics, 'none')
    parameter.HeurLevel = 0;
  elseif strcmp(options.Heuristics, 'basic')
    parameter.HeurLevel = 1;
  elseif strcmp(options.Heuristics, 'intermediate')
    parameter.HeurLevel = 2;
  elseif strcmp(options.Heuristics, 'advanced')
    parameter.HeurLevel = 3;
  end
end
% 'IntegerPreprocess'
if isfield(options, 'IntegerPreprocess') || ismexoptim == 1
  if strcmp(options.IntegerPreprocess, 'none')
    parameter.Presolve = 0;
  elseif strcmp(options.IntegerPreprocess, 'basic')
    parameter.Presolve = 1;
  elseif strcmp(options.IntegerPreprocess, 'advanced')
    parameter.Presolve = 3;
  end
end
% 'MaxNodes'
if isfield(options, 'MaxNodes') || ismexoptim == 1
  parameter.NodeLimit = options.MaxNodes;
end
% 'MaxTime'
if isfield(options, 'MaxTime') || ismexoptim == 1
  parameter.TimeLimit = options.MaxTime;
end
% 'RelativeGapTolerance'
if isfield(options, 'RelativeGapTolerance') || ismexoptim == 1
  parameter.RelGap = options.RelativeGapTolerance;
end
% 'AbsoluteGapTolerance'
if isfield(options, 'AbsoluteGapTolerance') || ismexoptim == 1
  parameter.AbsGap = options.AbsoluteGapTolerance;
end
% 'ConstraintTolerance'
if isfield(options, 'ConstraintTolerance') || ismexoptim == 1
  parameter.FeasTol = options.ConstraintTolerance;
end
% 'IntegerTolerance'
if isfield(options, 'IntegerTolerance') || ismexoptim == 1
  parameter.IntTol = options.IntegerTolerance;
end

%% Solve the problem
solution = copt_solve(problem, parameter);

%% Construct the MIP solution
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
  exitflag = 1;   % Converged
elseif strcmp(solution.status, 'infeasible')
  exitflag = -2;  % No feasible solution
elseif strcmp(solution.status, 'unbounded')
  exitflag = -3;  % Root LP is unbounded
elseif isfield(solution, 'x')
  exitflag = 2;   % Stopped with integer feasible solution
else
  exitflag = 0;   % Stopped with no feasible solution
end
% 'output'
if isfield(solution, 'objval') && isfield(solution, 'bestbnd')
  % In MATLAB:
  %   relativegap = 100 * (U - L) / (abs(U) + 1)
  objval = solution.objval;
  bestbnd = solution.bestbnd;
  output.relativegap = 100.0 * (objval - bestbnd) / (abs(objval) + 1.0);
  output.absolutegap = solution.objval - solution.bestbnd;
else
  output.relativegap = [];
  output.absolutegap = [];
end
if isfield(solution, 'x')
  output.numfeaspoints = 1;
else
  output.numfeaspoints = 0;
end
output.numnodes = solution.nodecnt;
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
else
  output.constrviolation = [];
end
output.message = solution.status;

function val = coptmexgetfield(s, field)
if isfield(s, field)
  val = getfield(s, field);
else
  val = [];
end

%% End of copt_intlinprog()
