#ifndef SIMDATA_H
#define SIMDATA_H

#include <iostream>
#include <cstdlib>
using namespace std;

class SimData
{
    int num_vms;
    int num_pms;
    int num_phases;
    double *workload;
    float *vm_revenue;
    float *vm_penalty;

public:
    SimData(int num_pms, int num_vms, int num_phases);
    ~SimData();
    void readInput();
    int getNumPM();
    int getNumVM();
    int getNumPhases();
    double getWorkload(int phase, int vm);
    float getVmRevenue(int vm);
    float getVmPenalty(int vm);
};

#endif
