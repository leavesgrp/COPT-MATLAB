function demo_feasrelax(filename)

% Read LP/MPS format file
problem = copt_read(filename);

% Build penalties
penalties.lbpen = ones(length(problem.lb), 1);
penalties.ubpen = ones(length(problem.ub), 1);
penalties.rhspen = ones(length(problem.rhs), 1);

% Set feasibility relaxation mode
params.feasrelaxmode = 0;

% Do the feasibility relaxation
relaxinfo = copt_feasrelax(problem, penalties, params);

% Display relaxation results
display(relaxinfo);

end
