simcon
======

Simulator for various schemes of server consolidation

Assumptions
-----------
* All Physical Machines are considered to be homogeneous
* CPU frequencies for physical machines are considered to be constant
* Current version of simulator only assumes CPU as resource
* Migrations from one physical machine are done at the same time (live gang migration)
* All numbering begins from 0, e.g. numbering of vms begins from 0 and goes till (vm_number -1)

Notations and Abbreviations
---------------------------
- pm = physical machine
- vm = virtual machine

Input File
----------
* number of virtual machines
* number of phases
* for each phase, utilization of each vm
* reward for each vm
* penalty for each pm

Output
------
Each algorithm prints vm to pm mapping in all the phases with the actions
required to be undertaken to reach from one state to next consecutive state

How to Run
----------
Current Working Dir: **simcon/**
```
make clean
make
./bin/mdp < input
./bin.khanna < input
```
