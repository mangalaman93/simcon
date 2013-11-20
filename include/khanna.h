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
	void getMapping(int phase_number, vector<int>* mapping);
	void getMigrationList(int phase_number, vector<int>* mapping);
};

#endif
