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
* All migrations are carried out before the next phase begins (in the end of the current phase)

Notations and Abbreviations
---------------------------
- pm = physical machine
- vm = virtual machine
- suv = state utility values
- isuv = intermediate state utility values (while migrations)

Input File
----------
* number of virtual machines
* number of phases
* for each phase, utilization of each vm
* reward for each vm
* penalty for each pm

You can also use simcon Ruby script to generate input file with random workload. The script
also allows you to run simulator and push output to spreadsheet.

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
./bin/main < input
```

Ruby Script simcon
------------------
Run `./script/simcon -h` for help. For help with subcommands, run `./script/simcon subcommand -h`
