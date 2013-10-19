#include "simdata.h"

SimData::SimData(int num_vms, int num_phases)
{
	this->num_vms = num_vms;
	this->num_phases = num_phases;
	this->workload = new double[num_phases*num_vms];
	this->vm_revenue = new float[num_vms];
	this->vm_penalty = new float[num_vms];
}

void SimData::readInput()
{
	for(int i=0; i<num_phases; i++)
	{
		for(int j=0; j<num_vms; j++) { cin>>workload[i*num_vms + j];}
	}
	for (int i=0; i<num_vms; i++) { cin>>vm_revenue[i];}
	for (int i=0; i<num_vms; i++) { cin>>vm_penalty[i];}
}

int SimData::getNumVM() { return num_vms;}
int SimData::getNumPhases() { return num_phases;}
double SimData::getWorkload(int phase, int vm) { return workload[phase*num_vms + vm];}
float SimData::getVmRevenue(int vm) { return vm_revenue[vm];}
float SimData::getVmPenalty(int vm) { return vm_penalty[vm];}

SimData::~SimData()
{
	delete [] workload;
	delete [] vm_revenue;
	delete [] vm_penalty;
}
