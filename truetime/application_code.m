function [exectime, data] = application_code(seg, data)
%APPLICATION_CODE executes the application
%   The application is supposed to do some computation and signal job start
%   and job end. It is also able to adjust its service level if asked to.
%   It reads the global shared memory with memory protection over a mutex. 

  % data contains:
  % --------------
  % data.index              -> index of the app in the vector of applications
  % data.SLadaptation       -> 1 means service level adaptation enabled
  % data.SL                 -> current service level
  % data.counter            -> number of jobs executed
  % data.exectimes          -> execution times stored (not all of them)
  % data.random_performance -> 1 means add some randomness in execution time
  % data.acpu
  % data.bcpu
  % data.amem
  % data.bmem
  % data.epsilon
  % data.deadline

  global SM;
  persistent effective_exec_time;
  memory_writing_time = 0;
  min_ET = 0.00001; % minimum execution time

  % the application will adjust the service level after a certain number
  % of jobs:
  adaptation_interval = 10;
 
  % truetime segments, the code on each segment is executed alltogether
  % while different segments can be split among different executions 
  switch seg
    case 1
      data.counter = data.counter+1;
      if (data.SLadaptation == 1) % only if I have to adjust the SLs
        if (mod(data.counter, adaptation_interval) == 0)
          pm = SM.performance_multipliers(data.index);
          data.SL = data.SL + data.epsilon*(pm*data.SL - data.SL); % adaptation
          SM.serviceLevels(data.index) = data.SL; % save level
        end
      end
      ttEnterMonitor('mutex');
      exectime = 0;
    case 2
      SM.startTime(data.index, data.counter) = ttAnalogIn(1); % read time
      exectime = memory_writing_time;
    case 3
      ttExitMonitor('mutex');
      c_et = data.acpu * data.SL + data.bcpu + ... % cpu usage
             data.amem * data.SL + data.bmem;      % mem usage
      c_et = c_et + (data.random_performance)*0.01*c_et*(rand()-0.5);
      c_et = max(c_et, min_ET); % maintain correctness in presence of rand
      exectime = c_et;
    case 4
      ttEnterMonitor('mutex'); % ready to save in memory
      exectime = 0;
    case 5
      SM.stopTime(data.index, data.counter) = ttAnalogIn(1);
      effective_exec_time = SM.stopTime(data.index, data.counter) - ...
        SM.startTime(data.index, data.counter);
      SM.latencies(data.index) = effective_exec_time;
      exectime = memory_writing_time;
    case 6
      ttExitMonitor('mutex');
      
      % save values and compute performance measure
      data.exectimes(1,1:length(data.exectimes)-1) = ...
        data.exectimes(1,2:length(data.exectimes));
      data.exectimes(length(data.exectimes)) = effective_exec_time;

      if SM.jobs > length(data.exectimes)
        components = SM.periods(data.index)./data.exectimes - 1;
        components = max(components, -1);
        components = min(components, 1);
        SM.performance_function(data.index) = mean(components);
      else
        SM.performance_function(data.index) = -1;
      end
      
      SM.jobs(data.index) = SM.jobs(data.index)+1;
      exectime = memory_writing_time;
    case 7
      exectime = -1; % terminate execution of a job
  end
end
