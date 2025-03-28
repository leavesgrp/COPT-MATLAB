function demo_affcone()
%
% Minimize
%   -6*x1 - 4*x2 - 5*x3
% Subject to
%   16*x1 - 14*x2 + 5*x3 <= -3
%   7*x1 + 2*x2 <= 5
%
%   (-24*x1 -  7*x2 + 15*x3 + 12)
%   (  8*x1 + 13*x2 - 12*x3 - 2 ) \in cone
%   ( -8*x1 + 18*x2 +  6*x3 - 14)
%   (    x1 -  3*x2 - 17*x3 - 13)
%
%   (0*x1 + 0*x2 + 0*x3 + 10)
%   (  x1 + 0*x2 + 0*x3     ) \in cone
%   (0*x1 +   x2 + 0*x3     )
%   (0*x1 + 0*x2 +   x3     )
%

%% Build problem
% Objective
problem.objsen = 'Minimize';
problem.obj = [-6, -4, -5];

% Linear part
problem.A = sparse([16, -14, 5; 7, 2, 0]);
problem.sense = 'LL';
problem.rhs = [-3, 5];
problem.lb = repmat(-inf, 3, 1);
problem.ub = repmat(+inf, 3, 1);
problem.varnames = {'x1', 'x2', 'x3'};
problem.constrnames = {'r1', 'r2'};

% Affine cone part (Ax + b) \in cone
problem.affcone(1).type = 1;
problem.affcone(1).A = sparse([-24, -7, 15; 8, 13, -12; -8, 18, 6; 1, -3, -17]);
problem.affcone(1).b = [12, -2, -14, -13];
problem.affcone(1).name = 'aff1';

problem.affcone(2).type = 1;
problem.affcone(2).A = sparse([0, 0, 0; 1, 0, 0; 0, 1, 0; 0, 0, 1]);
problem.affcone(2).b = [10, 0, 0, 0];
problem.affcone(2).name = 'aff2';

%% Write problem (Affine cone problem supported by CBF and Bin format)
copt_write(problem, 'demo_affcone.cbf');

%% Set parameter
parameter.TimeLimit = 60;
parameter.LogFile = 'demo_affcone.log';

%% Solve the problem
solution = copt_solve(problem, parameter);

%% Print solution
if strcmpi(solution.status, 'optimal')
    fprintf('\nObjective value: %f\n', solution.objval)
    
    fprintf('Variable solution:\n')
    for i = 1:size(problem.A, 2)
        fprintf('  %s = %f\n', problem.varnames{i}, solution.x(i))
    end
end
end
