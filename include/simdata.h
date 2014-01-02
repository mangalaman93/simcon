/*
This class handles the input data. It reads from standard
input stream and provides API to access the data
*/

#ifndef SIMDATA_H
#define SIMDATA_H

#include <iostream>
#include <cstdlib>
#include "utils.h"
using namespace std;

class SimData
{
    int num_vms;
    int num_phases;
    Matrix<float> *workload;
    float *vm_revenue;
    float *vm_penalty;

  public:
    SimData(int num_vms, int num_phases);
    ~SimData();
    void readInput();
    int getNumVM();
    int getNumPhases();
    float getWorkload(int phase, int vm);
    float getVmRevenue(int vm);
    float getVmPenalty(int vm);
    void setWorkload(int phase, int vm, float val);
    void setVmRevenue(int vm, float val);
    void setVmPenalty(int vm, float val);
};

#endif
