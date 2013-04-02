gtrm
====

Game Theoretic Resource Manager: depends on jobsignal and SCHED_DEADLINE
[Jobsignal](https://github.com/martinamaggio/jobsignal "Jobsignal")
[SCHED_DEADLINE](https://github.com/jlelli/sched-deadline "SCHED_DEADLINE")

To use the code contained in this repository you shoud have installed and configured SCHED_DEADLINE and installed the library jobsignaler, which is needed to interact with the resource manager. The repository contains the stub of an experiment run, where it is possible to specify the number of applications and their parameters (the test application code is contained in the jobsignal repository).