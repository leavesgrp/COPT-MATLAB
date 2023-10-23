function coptmex_install()
% coptmex_setup Install script for the MATLAB interface of the Cardinal Optimizer

% Check for operating system
switch computer
  case 'PCWIN64'
    coptmex_os = 'Windows';
  case 'GLNXA64'
    coptmex_os = 'Linux';
  case 'MACI64'
    coptmex_os = 'MacOSX (x86)';
  case 'MACA64'
    coptmex_os = 'MacOSX (arm64)';
  otherwise
    error('Unsupported operating system %s', computer);
end

% Setup coptmex
fprintf('Installing MATLAB interface for Cardinal Optimizer on %s system...\n', coptmex_os);

coptmex_path = fullfile(pwd, 'lib');
addpath(coptmex_path)
savepath;
fprintf('Add and save path %s to MATLAB search path.\n', coptmex_path);

coptmex_ver = copt_solve();
fprintf('MATLAB interface for COPT v%d.%d.%d was installed.\n', coptmex_ver.major, coptmex_ver.minor, coptmex_ver.technical);

end
