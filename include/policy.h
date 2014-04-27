/*
Policy abstract class, this class should be used as base class for the
"algorithm to design policy" class
*/

#ifndef POLICY_H
#define POLICY_H

#include <vector>
#include <list>
#include <fstream>
#include <cstring>
#include "simdata.h"

#define DEBUG true

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
	void dumpPolicyHelper(string algo);

  public:
	Policy(SimData *s_data);
	virtual ~Policy();
	virtual void run(int phases) = 0;
	virtual void getMapping(int phase_number, vector<int>* mapping) = 0;
	virtual void getMigrationList(int phase_number, vector<int>* mapping) = 0;
	virtual void printPolicy();
	virtual void dumpPolicy() = 0;
	float getOverallProfit();
};

#endif
