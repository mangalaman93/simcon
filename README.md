simcon
======

Simulator for various schemes of server consolidation

Assumptions and Notations
-------------------------
* All numbering begins from 0, e.g. numbering of vms begins from 0 and goes till (vm_number -1)

Input File
----------
* number of physical machines
* number of virtual machines
* number of phases
* for each phase, utilization of each vm

How to Run
----------
Current Working Dir: simcon/
```
make clean
make
./bin/khanna < input
```
