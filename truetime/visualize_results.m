function [] = visualize_results()
%VISUALIZE_RESULTS will plot the results of the simulation
%   It requires SR.mat obtained through the execution of the simulink
%   model and produces plots.

    load('SR.mat');
    number_steps = length(SR.idle);

    figure(1);
    plot(SR.idle); hold on
    plot(1:number_steps, zeros(number_steps,1),'k:');
    plot(1:number_steps, ones(number_steps,1),'k:');
    axis([1, number_steps, -0.05, 1.05]);
    xlabel('Simulation Step');
    ylabel('Virtual processor assigned to the idle task');

    figure(2);
    plot(SR.bandwidths); hold on
    plot(1:number_steps, zeros(number_steps,1),'k:');
    plot(1:number_steps, ones(number_steps,1),'k:');
    axis([1, number_steps, -0.05, 1.05]);
    xlabel('Simulation Step');
    ylabel('Virtual processor assigned to the applications');
 
    figure(3);
    plot(SR.performance_functions); hold on
    plot(1:number_steps, zeros(number_steps,1),'k:');
    xlim([1, number_steps]);
    xlabel('Simulation Step');
    ylabel('Performance function (fi) of the applications');

    figure(4);
    plot(SR.serviceLevels); hold on
    plot(1:number_steps, zeros(number_steps,1),'k:');
    xlim([1, number_steps]);
    xlabel('Simulation Step');
    ylabel('Service levels of the applications');

    figure(5);
    plot(SR.performance_multipliers); hold on
    plot(1:number_steps, zeros(number_steps,1),'k:');
    xlim([1, number_steps]);
    xlabel('Simulation Step');
    ylabel('Performance multipliers set by the resource manager');

end

