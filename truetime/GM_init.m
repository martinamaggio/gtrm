function GM_init(args)
%GM_init creates the environment for the simulation
%   This function is needed by the TrueTime GM.mdl model and sets up the
%   environment for the simulation. TrueTime is needed and it is necessary
%   that the server block in GM.mdl points to this function to initialize
%   the simulation.
%   Some parameters can be modified from here: for example the number of
%   applications and their required load. Comments are inlined.  


  % cleaning up the old environment and makes random reproducible
  clear; clc; close all; warning off;
  rand('seed',0);

  % simulation parameters, can be configured
  n_applications = 4;
  service_level_adaptation = 0; % 1 if you want applications to adjust
  randomness_exec_time = 0; % 1 if you want randomness, 0 if not
  randomness_weights = 0;
 
  % TrueTime initialization 
  ttInitKernel('prioEDF');
  ttCreateMonitor('mutex'); % creating mutex for shared memory
  
  % Resource manager parameters
  rm_data.iteration = 0; % starting from zero
  rm_data.RMbandwidth = 0.1; % reserved bandwidth for RM
  rm_data.RMperiod = 0.001; % how often would RM execute
  rm_data.RMexecutionTime = 0.000001; % execution time of RM
  rm_data.min_bandwidth = 0.1; % min bandwidth assigned to an app
  rm_data.max_bandwidth = 1-rm_data.RMbandwidth; % max bandwidth assigned
  
  % Application parameters
  offsets = zeros(1, n_applications);
  
  app_num_means = 1; % number of last jobs considered for performances
  
  % Shared memory data
  global SM
  SM.performance_multipliers = ones(1, n_applications);
  SM.performance_function = ones(1, n_applications) * (-1);
  SM.weights = ones(1, n_applications)*0.5; % Initial equal weights
  SM.weights = SM.weights + randomness_weights*0.5* ...
    (rand(1, n_applications)-0.5);
  SM.weights = max(SM.weights, 0); % ensuring bounds with randomness 
  SM.weights = min(SM.weights, 1);
  SM.jobs = zeros(1, n_applications);
    
  SM.periods = ones(1, n_applications) * 0.100;
  SM.startTime = []; % Start time of each job in each application
  SM.stopTime = []; % Stop time of each job in each application
  SM.bandwidth = ones(1, n_applications)* ...
      (1-rm_data.RMbandwidth)/n_applications; % Save space for RM
  % in the kernel impelentation the two subsequent values are not stored
  % but this helps saving simulation results
  SM.latencies = zeros(1, n_applications);
  SM.serviceLevels = ones(1, n_applications);
  
  % Simulation results
  global SR 
  SR.bandwidths = [];
  SR.performance_functions = [];
  SR.performance_multipliers = [];
  SR.serviceLevels = [];
  SR.weights = [];
  SR.idle = [];
  SR.latencies = [];
  
  % Starting applications and their servers
  for app=1:n_applications
    app_data.index = app;
    app_data.SLadaptation = service_level_adaptation;
    app_data.SL = 1; % Initial service level
    app_data.counter = 0; % Number of jobs executed
    app_data.exectimes = zeros(1, app_num_means);
    app_data.random_performance = randomness_exec_time;
    app_data.acpu = 0;
    app_data.bcpu = 3.0000;
    app_data.amem = 0;
    app_data.bmem = 0;
    app_data.epsilon = 0.03;
    app_data.deadline = SM.periods(app);
    app_name = ['application', num2str(app)];
    ser_name = ['server', num2str(app)];
    ttCreatePeriodicTask(app_name, ...
      offsets(app), SM.periods(app), 'application_code', app_data);
    ttCreateCBS(ser_name, ... % Soft real time
      SM.bandwidth(app)*rm_data.RMperiod, rm_data.RMperiod, 1); 
    ttAttachCBS(app_name, ser_name);
  end
  
  % Starting the resource manager and its server, hard real time
  ttCreatePeriodicTask('applicationRM', ...
    0, rm_data.RMperiod, 'rm_code', rm_data);
  ttCreateCBS('serverRM', rm_data.RMbandwidth, rm_data.RMperiod, 1);
  ttAttachCBS('applicationRM', 'serverRM');

end

