function demo_mip(filename)
%
% Read and solve MIP problem from file
%

if nargin == 0
    filename = 'misc07.mps.gz';
end

% Set parameter
parameter.TimeLimit = 10;

% Read problem
problem = copt_read(filename);

% Solve the problem
solution = copt_solve(problem, parameter);

% Print solution
if strcmpi(solution.status, 'optimal') || isfield(solution, 'x')
    fprintf('\nBest objective value: %f', solution.objval)
    fprintf('\nBest bound value: %f', solution.bestbnd)

    fprintf('\nVariable solution:\n');
    for i = 1:size(solution.x, 1)
        if abs(solution.x(i)) > 1e-6
            fprintf('  %s = %f\n', problem.varnames{i}, solution.x(i))
        end
    end
end
end
