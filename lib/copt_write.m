% copt_write
%
% copt_write(problem, probfile)
%
% This function writes a valid COPT problem to file.
%
% Input arguments:
% -----------------
% problem:
%   A MATLAB struct that specify a valid COPT problem.
%
% probfile:
%   Name of file to write.
%
% Examples usages:
% -----------------
% problem = copt_read('diet.mps');
% copt_write(problem, 'diet_out.mps');
%
