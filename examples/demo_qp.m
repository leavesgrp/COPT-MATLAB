function demo_qp()
%
% Minimize
%  OBJ.FUNC: [ 2 X0 ^2 + 4 X0 * X1 + 4 X1 ^2
%            + 4 X1 * X2 + 4 X2 ^2
%            + 4 X2 * X3 + 4 X3 ^2
%            + 4 X3 * X4 + 4 X4 ^2
%            + 4 X4 * X5 + 4 X5 ^2
%            + 4 X5 * X6 + 4 X6 ^2
%            + 4 X6 * X7 + 4 X7 ^2
%            + 4 X7 * X8 + 4 X8 ^2
%            + 4 X8 * X9 + 2 X9 ^2 ] / 2
% Subject To
%  ROW0: X0 + 2 X1 + 3 X2  = 1
%  ROW1: X1 + 2 X2 + 3 X3  = 1
%  ROW2: X2 + 2 X3 + 3 X4  = 1
%  ROW3: X3 + 2 X4 + 3 X5  = 1
%  ROW4: X4 + 2 X5 + 3 X6  = 1
%  ROW5: X5 + 2 X6 + 3 X7  = 1
%  ROW6: X6 + 2 X7 + 3 X8  = 1
%  ROW7: X7 + 2 X8 + 3 X9  = 1
% Bounds
%       X0 Free
%       X1 Free
%       X2 Free
%       X3 Free
%       X4 Free
%       X5 Free
%       X6 Free
%       X7 Free
%       X8 Free
%       X9 Free
% End
%

% Build problem
problem.objsen      = 'Minimize';
problem.Q           = sparse([1, 2, 0, 0, 0, 0, 0, 0, 0, 0;
                              0, 2, 2, 0, 0, 0, 0, 0, 0, 0;
                              0, 0, 2, 2, 0, 0, 0, 0, 0, 0;
                              0, 0, 0, 2, 2, 0, 0, 0, 0, 0;
                              0, 0, 0, 0, 2, 2, 0, 0, 0, 0;
                              0, 0, 0, 0, 0, 2, 2, 0, 0, 0;
                              0, 0, 0, 0, 0, 0, 2, 2, 0, 0;
                              0, 0, 0, 0, 0, 0, 0, 2, 2, 0;
                              0, 0, 0, 0, 0, 0, 0, 0, 2, 2;
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 1]);
problem.A           = sparse([1, 2, 3, 0, 0, 0, 0, 0, 0, 0;
                              0, 1, 2, 3, 0, 0, 0, 0, 0, 0;
                              0, 0, 1, 2, 3, 0, 0, 0, 0, 0;
                              0, 0, 0, 1, 2, 3, 0, 0, 0, 0;
                              0, 0, 0, 0, 1, 2, 3, 0, 0, 0;
                              0, 0, 0, 0, 0, 1, 2, 3, 0, 0;
                              0, 0, 0, 0, 0, 0, 1, 2, 3, 0;
                              0, 0, 0, 0, 0, 0, 0, 1, 2, 3]);
problem.sense       = repmat('E', 8, 1);
problem.rhs         = ones(8, 1);
problem.lb          = repmat(-inf, 10, 1);
problem.ub          = repmat(+inf, 10, 1);
problem.varnames    = {'X0', 'X1', 'X2', 'X3', 'X4', 'X5', 'X6', 'X7', 'X8', 'X9'};
problem.constrnames = {'ROW0', 'ROW1', 'ROW2', 'ROW3', 'ROW4', 'ROW5', 'ROW6', 'ROW7'};

% Write problem to file
copt_write(problem, 'demo_qp.lp');

% Set parameter
parameter.TimeLimit = 60;

% Solve the problem
solution = copt_solve(problem, parameter);

% Print solution
if strcmpi(solution.status, 'optimal')
    fprintf('\nObjective value: %f\n', solution.objval)

    fprintf('Variable solution:\n')
    for i = 1:size(problem.A, 2)
        fprintf('  %s = %f\n', problem.varnames{i}, solution.x(i))
    end
end
end
