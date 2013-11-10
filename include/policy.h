#ifndef POLICY_H
#define POLICY_H

#include "simdata.h"

class Policy
{
	int run_for_phases;
	int num_vms;
	Simdata *sdata;
	virtual void printState(int phase_number);
	virtual void printMigrationList(int phase_number);

  public:
	Policy(Simdata *sdata);
	virtual ~Policy();
	virtual void run() = 0;
	virtual const int* getMapping(int phase_number) = 0;
	virtual const int* getMigrationList(int phase_number) = 0;
	virtual void printPolicy();
	virtual void dumpStats() = 0;
};

#endif
