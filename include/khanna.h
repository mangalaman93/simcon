#ifndef KHANNA_H
#define KHANNA_H

#include <iostream>
#include <queue>
#include <fstream>
#include "simdata.h"
#include "state.h"
#include "policy.h"
using namespace std;

class Khanna : public Policy
{
	State **policy;

  public:
	Khanna(SimData *s_data);
	~Khanna();
	void run(int phases);
	vector<int> getMapping(int phase_number);
	vector<int> getMigrationList(int phase_number);
};

#endif
