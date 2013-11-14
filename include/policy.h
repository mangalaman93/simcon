#ifndef POLICY_H
#define POLICY_H

#include <vector>
#include <list>
#include "simdata.h"

#define DEBUG false

class Policy
{
  protected:
	int run_for_phases;
	int num_vms;
	int num_phases;
	float max_profit;
	SimData *sdata;
	virtual void printState(int phase_number);
	virtual void printMigrationList(int phase_number);

  public:
	Policy(SimData *s_data);
	virtual ~Policy();
	virtual void run(int phases) = 0;
	virtual vector<int> getMapping(int phase_number) = 0;
	virtual vector<int> getMigrationList(int phase_number) = 0;
	virtual void printPolicy();
};

#endif
