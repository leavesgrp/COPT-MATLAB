function demo_qcp()
%
% Minimize
%  obj: 2.1 x1 - 1.2 x2 + 3.2 x3 + x4 + x5 + x6 + 2 x7 + [ x2^2 ] / 2
% Subject To
%  r1: x1 + 2 x2 = 6
%  r2: 2 x1 + x3 >= 5
%  r3: x6 + 2 x7 <= 7
%  r4: -x1 + 1.2 x7 >= -2.3
%  q1: [ -1.8 x1^2 + x2^2 ] <= 0
%  q2: [ 4.25 x3^2 - 2 x3 * x4 + 4.25 x4^2 - 2 x4 * x5 + 4 x5^2  ] + 2 x1 + 3 x3 <= 9.9
%  q3: [ x6^2 - 2.2 x7^2 ] >= 5
% Bounds
%  0.2 <= x1 <= 3.8
%  x2 Free
%  0.1 <= x3 <= 0.7
%  x4 Free
%  x5 Free
%  x7 Free
% End
%

% Build problem
problem.objsen = 'Minimize';
problem.Q      = sparse([0 0   0 0 0 0 0;
                         0 0.5 0 0 0 0 0;
                         0 0   0 0 0 0 0;
                         0 0   0 0 0 0 0;
                         0 0   0 0 0 0 0;
                         0 0   0 0 0 0 0;
                         0 0   0 0 0 0 0;]);
problem.obj    = [2.1, -1.2, 3.2, 1, 1, 1, 2];
problem.A      = sparse([ 1 2 0 0 0 0   0;
                          2 0 1 0 0 0   0;
                          0 0 0 0 0 1   2;
                         -1 0 0 0 0 0 1.2]);
problem.sense  = ['E', 'G', 'L', 'G'];
problem.rhs    = [6, 5, 7, -2.3];
problem.lb     = [0.2, -inf, 0.1, -inf, -inf,    0, -inf];
problem.ub     = [3.8, +inf, 0.7, +inf, +inf, +inf, +inf];
problem.varnames = {'x1', 'x2', 'x3', 'x4', 'x5', 'x6', 'x7'};
problem.constrnames = {'r1', 'r2', 'r3', 'r4'};

problem.quadcon(1).Qrow = [1, 2];
problem.quadcon(1).Qcol = [1, 2];
problem.quadcon(1).Qval = [-1.8, 1];
problem.quadcon(1).sense = 'L';
problem.quadcon(1).rhs = 0;
problem.quadcon(1).name = 'q1';

problem.quadcon(2).Qrow = [3, 3, 4, 4, 5];
problem.quadcon(2).Qcol = [3, 4, 4, 5, 5];
problem.quadcon(2).Qval = [4.25, -2, 4.25, -2, 4];
problem.quadcon(2).q = sparse([2; 0; 3; 0; 0; 0; 0]);
problem.quadcon(2).sense = 'L';
problem.quadcon(2).rhs = 9.9;
problem.quadcon(2).name = 'q2';

problem.quadcon(3).Qrow = [6, 7];
problem.quadcon(3).Qcol = [6, 7];
problem.quadcon(3).Qval = [1, -2.2];
problem.quadcon(3).sense = 'G';
problem.quadcon(3).rhs = 5;
problem.quadcon(3).name = 'q3';

% Write problem to file
copt_write(problem, 'demo_qcp.lp');

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
