function [exectime, data] = rm_code(seg, data)
%RM_CODE executes the resource manager
%   The resource manager reads the performance functions from shared memory
%   and performs the choice of the CBS server bandwidths. 

  global SM % use global shared memory
  global SR % for saving simulation results, RM is the only one saving results
  persistent bandwidth_old
  persistent eps_offset
  
  switch seg
    case 1
      actual_time = ttAnalogIn(1);
      eps_offset = 0;
      available_applications = 1;
            
      c1 = 0.1;
      c2 = 1.0;
      base = data.iteration - eps_offset;
      eps = (c1) / (1.0 + (c2 * base * data.RMperiod));
      data.iteration = data.iteration+1;
      bandwidth_old = SM.bandwidth;
      
      bandwidths = SM.bandwidth .* available_applications;
      bandwidths = bandwidths ./ sum(bandwidths);
      sum_wf = sum((1-SM.weights) .* SM.performance_function .* available_applications);
      
      bandwidths = bandwidths - eps * ...
        ((1-SM.weights) .* SM.performance_function - sum_wf * bandwidths);
      bandwidths = bandwidths .* data.max_bandwidth;
      SM.bandwidth = bandwidths .* available_applications;
      
      SM.bandwidth = max(SM.bandwidth, data.min_bandwidth.*available_applications);
      SM.bandwidth = min(SM.bandwidth, data.max_bandwidth);
      
      % Setting the bandwidth in the server
      for app=1:length(SM.bandwidth)
        ser_name = ['server', num2str(app)];
        ttSetCBSParameters(ser_name, ...
          SM.bandwidth(app)*data.RMperiod, data.RMperiod);
      end
      ttEnterMonitor('mutex');
      exectime = 0;
    case 2
      % Setting the performance multipliers for the applications
      SM.performance_multipliers = ...
        (1+SM.performance_function) .* (SM.bandwidth./bandwidth_old);
      exectime = 0;
    case 3
      ttExitMonitor('mutex');
      exectime = data.RMexecutionTime; % setting execution time
    case 4
      % computing idle time
      idletime = 1-sum(SM.bandwidth)-data.RMexecutionTime;
      % saving the simulation results for future use
      SR.idle = [SR.idle; idletime];
      SR.bandwidths = [SR.bandwidths; SM.bandwidth];
      SR.performance_functions = ...
        [SR.performance_functions; SM.performance_function];
      SR.performance_multipliers = ...
        [SR.performance_multipliers; SM.performance_multipliers];
      SR.serviceLevels = [SR.serviceLevels; SM.serviceLevels];
      SR.weights = [SR.weights; SM.weights];
      SR.latencies = [SR.latencies; SM.latencies];
      save('SR.mat','-mat','SR'); % save to file
      exectime = -1; % termination
  end
end
