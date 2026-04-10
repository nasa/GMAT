

%  Find the current directory name
current_dir = pwd;

%  Cost and constraint evaluation files
addpath([ current_dir '\FormOpt']);
addpath([ current_dir '\FormOpt\MMS']);
addpath([ current_dir '\HPDC']);
addpath([ current_dir '\HPDC\input']);
addpath([ current_dir '\HPDC\output']);
addpath([ current_dir '\utils']);
addpath([ current_dir '\utils\stateconv']);

%  TrajOpt system files
addpath([ current_dir '\TrajOptv3.0\gui\util']);
addpath([ current_dir '\TrajOptv3.0']);
addpath([ current_dir '\TrajOptv3.0\bin']);
addpath([ current_dir '\TrajOptv3.0\globals']);
addpath([ current_dir '\TrajOptv3.0\Optim']);