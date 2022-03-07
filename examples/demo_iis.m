function demo_iis()
%
% Minimize
%  X2 + X3 + X4
% Subject To
%  ROW1: 0.8 X3 + X4 <= 10000
%  ROW2: X1 <= 90000
%  ROW3: 2 X6 - X8 <= 10000
%  ROW4: - X2 + X3 >= 50000
%  ROW5: - X2 + X4 >= 87000
%  ROW6: X3 <= 50000
%  ROW7: - 3 X5 + X7 >= 10000
%  ROW8: 0.5 X5 + 0.6 X6 <= 300000
%  ROW9: X2 - 0.05 X3 = 5000
%  ROW10: X2 - 0.04 X3 - 0.05 X4 = 4500
%  ROW11: X2 >= 80000
% END
%

% Build problem
problem.objsen      = 'Minimize';
problem.A           = sparse([0,  0,   0.8,     1,   0,   0, 0,  0;
                              1,  0,     0,     0,   0,   0, 0,  0;
                              0,  0,     0,     0,   0,   2, 0, -1;
                              0, -1,     1,     0,   0,   0, 0,  0;
                              0, -1,     0,     1,   0,   0, 0,  0;
                              0,  0,     1,     0,   0,   0, 0,  0;
                              0,  0,     0,     0,  -3,   0, 1,  0;
                              0,  0,     0,     0, 0.5, 0.6, 0,  0;
                              0,  1, -0.05,     0,   0,   0, 0,  0;
                              0,  1, -0.04, -0.05,   0,   0, 0,  0;
                              0,  1,     0,     0,   0,   0, 0,  0]);
problem.obj         = [0, 1, 1, 1, 0, 0, 0, 0];
problem.lb          = zeros(8, 1);
problem.ub          = repmat(+inf, 8, 1);
problem.sense       = ['L', 'L', 'L', 'G', 'G', 'L', 'G', 'L', 'E', 'E', 'G'];
problem.rhs         = [10000, 90000, 10000, 50000, 87000, 50000, 10000, 300000, ...
                       5000, 4500, 80000];
problem.varnames    = {'X1', 'X2', 'X3', 'X4', 'X5', 'X6', 'X7', 'X8'};
problem.constrnames = {'ROW1', 'ROW2', 'ROW3', 'ROW4', 'ROW5', 'ROW6', 'ROW7', ...
                       'ROW8', 'ROW9', 'ROW10', 'ROW11'};

% Write problem to file
copt_write(problem, 'demo_iis.lp');

% Set parameter
parameter.TimeLimit = 60;

% Compute IIS for the problem
iis = copt_computeiis(problem, parameter);

% Print IIS result
if ~isempty(iis)
    if iis.isminiis
        fprintf('\nIIS is minimal\n');
    else
        fprintf('\nIIS is not minimal\n');
    end

    fprintf('\nConstraint status:\n');
    for i = 1:size(problem.A, 1)
        fprintf('  %8s: lower(%d), upper(%d)\n', problem.constrnames{i}, ...
                                                 iis.constrlb(i), iis.construb(i));
    end

    fprintf('\nVariable status:\n');
    for i = 1:size(problem.A, 2)
        fprintf('  %8s: lower(%d), upper(%d)\n', problem.varnames{i}, ...
                                                 iis.varlb(i), iis.varub(i));
    end
end
end
