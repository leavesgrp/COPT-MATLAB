function demo_sdp()
%
%                 [2, 1, 0]   
%  minimize    Tr [1, 2, 1] * X + x0
%                 [0, 1, 2]
% 
%                 [1, 0, 0]
%  subject to  Tr [0, 1, 0] * X <= 0.8
%                 [0, 0, 1]
% 
%                 [1, 1, 1]
%              Tr [1, 1, 1] * X + x1 + x2 = 0.6
%                 [1, 1, 1]
%
%              x0 + x1 + x2 <= 0.9
%              x0 >= (x1^2 + x2^2) ^ (1/2)
% 
%    x0, x1, x2 non-negative, X in PSD
%

%
% linear variables:        x0 - x2
% slack variables:         x3 - x4
% conic variables:         x5 - x7
% flattened SDP variables: x8 - x13
%

% Build problem
conedata.c =         [1; 0; 0; 0; 0;  0;  0;  0; 2; 1; 0; 2; 1; 2];
conedata.A = sparse([[0, 0, 0, 1, 0,  0,  0,  0, 1, 0, 0, 1, 0, 1];
                     [0, 1, 1, 0, 0,  0,  0,  0, 1, 1, 1, 1, 1, 1];
                     [1, 1, 1, 0, 1,  0,  0,  0, 0, 0, 0, 0, 0, 0];
                     [1, 0, 0, 0, 0, -1,  0,  0, 0, 0, 0, 0, 0, 0];
                     [0, 1, 0, 0, 0,  0, -1,  0, 0, 0, 0, 0, 0, 0];
                     [0, 0, 1, 0, 0,  0,  0, -1, 0, 0, 0, 0, 0, 0]]);
conedata.b =         [0.8; 0.6; 0.9; 0; 0; 0];

% Build dimension
K.f = 0;   % Free variables
K.l = 5;   % Positive orthants
K.q = [3]; % Quadratic cones
K.r = [];  % Rotated cones
K.s = [3]; % SDP cones
conedata.K = K;

% Build sense and constant offset
conedata.objsen = 'minimize';
conedata.objcon = 0;
conedata.vtype = 'C';

% Model with conic problem data
problem.conedata = conedata;

% Set parameter
parameter.TimeLimit = 60;

% Solve the problem
solution = copt_solve(problem, parameter);

% Print solution
if strcmpi(solution.status, 'optimal')
    fprintf('\nObjective value: %f\n', solution.objval)
    
    fprintf('\nPSD primal solution: \n')
    disp(solution.psdx)
    
    fprintf('\nPSD dual solution: \n')
    disp(solution.psdrc)
    
    fprintf('\nNon-PSD primal solution: \n')
    disp(solution.x);
    
    fprintf('\nNon-PSD dual solution: \n')
    disp(solution.rc);
end
end
 