gtrm
====

Game Theoretic Resource Manager: depends on [Jobsignal](https://github.com/martinamaggio/jobsignal "Jobsignal") and [SCHED_DEADLINE](https://github.com/jlelli/sched-deadline "SCHED_DEADLINE").

To use the code contained in this repository you shoud have installed and configured SCHED_DEADLINE and installed the library jobsignaler, which is needed to interact with the resource manager. The repository contains the stub of an experiment run, where it is possible to specify the number of applications and their parameters (the test application code is contained in the jobsignal repository).

This code is described in a paper submitted to the European Conference on Real-Time Systems (ECRTS), that you can find [here](https://github.com/martinamaggio/gtrm/blob/master/docs/ECRTS2013.pdf "ECRTS 2013"). The theoretical background was previously accepted for presentation at the American Control Conference (ACC), [paper](https://github.com/martinamaggio/gtrm/blob/master/docs/ACC2013.pdf "ACC 2013").
