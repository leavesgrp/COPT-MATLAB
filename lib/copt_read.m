% copt_read
%
% problem = copt_read(probfile)
% problem = copt_read(probfile, infofile)
%
% This function reads a problem from file, and optionally reads a start basis
% for the problem (LP only).
%
% Input arguments:
% -----------------
% probfile:
%   Name of the problem file to read.
%
% infofile (optional):
%   Name of start basis file to read.
%
% Output arguments:
% ------------------
% problem:
%   A MATLAB struct that represent the problem read.
%
% Example usages:
% ----------------
%   problem  = copt_read('diet.mps');
%   solution = copt_solve(problem);
%
%   problem  = copt_read('diet.mps', 'diet.bas');
%   solution = copt_solve(problem);
%
