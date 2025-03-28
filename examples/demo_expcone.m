function demo_expcone(filename)
%
% Read and solve exponential cone problem from CBF file
%

%% Problem file name
if nargin == 0
    filename = 'mra01.cbf.gz';
end

%% Set parameter
parameter.TimeLimit = 60;

%% Read problem
problem = copt_read(filename);

%% Solve the problem
solution = copt_solve(problem, parameter);

%% Print solution
if strcmpi(solution.status, 'optimal') || isfield(solution, 'x')
    fprintf('\nObjective value: %f', solution.objval)
    
    fprintf('\nVariable solution:\n');
    for i = 1:size(solution.x, 1)
        fprintf('  %s = %f\n', problem.varnames{i}, solution.x(i))
    end
end
end
