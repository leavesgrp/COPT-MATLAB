function demo_lp()
%
% The problem to solve:
%
%  Maximize:
%    1.2 x + 1.8 y + 2.1 z
%
%  Subject to:
%    1.5 x + 1.2 y + 1.8 z <= 2.6
%    0.8 x + 0.6 y + 0.9 z >= 1.2
%
%  where:
%    0.1 <= x <= 0.6
%    0.2 <= y <= 1.5
%    0.3 <= z <= 2.8
%

% Build problem
problem.objsen      = 'Maximize';
problem.A           = sparse([1.5, 1.2, 1.8; 0.8, 0.6, 0.9]);
problem.obj         = [1.2; 1.8; 2.1];
problem.lb          = [0.1; 0.2; 0.3];
problem.ub          = [0.6; 1.5; 2.8];
problem.sense       = ['L'; 'G'];
problem.rhs         = [2.6; 1.2];
problem.varnames    = {'x'; 'y'; 'z'};
problem.constrnames = {'r0'; 'r1'};

% Write problem to file
copt_write(problem, 'demo_lp.mps')

% Set parameter
parameter.TimeLimit = 10;

% Solve the problem
solution = copt_solve(problem, parameter);

% Print solution
if strcmpi(solution.status, 'optimal')
    fprintf('\nObjective value: %f\n', solution.objval)

    fprintf('Variable solution:\n')
    for i = 1:size(problem.obj, 1)
        fprintf('  %s = %f\n', problem.varnames{i}, solution.x(i))
    end

    fprintf('Variable basis status:\n')
    for i = 1:size(problem.obj, 1)
        fprintf('  %s = %d\n', problem.varnames{i}, solution.varbasis(i))
    end

    fprintf('Constraint basis status:\n')
    for i = 1:size(problem.rhs, 1)
        fprintf('  %s = %d\n', problem.constrnames{i}, solution.constrbasis(i))
    end
end
end