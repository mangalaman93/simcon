/*
Implementation of A* (Dijkstra's also)
*/

#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <pthread.h>
#include <queue>
#include <limits>
#include <set>
#include "algo.h"
#include "simdata.h"
#include "config.h"
#include "stateIterator.h"
#include "policy.h"
using namespace std;

class AstarNode
{
  public:
    int phase_number;
    int state_index;
    float g_plus_h;

    AstarNode(int pn, int sn, float gph)
    : phase_number(pn), state_index(sn), g_plus_h(gph) {}

    AstarNode(const AstarNode& an)
    {
    	phase_number = an.phase_number;
    	state_index = an.state_index;
    	g_plus_h = an.g_plus_h;
    }

    bool operator<(const AstarNode& rhs) const
    {
    	if(this->g_plus_h < rhs.g_plus_h)
    		return true;
    	else if(this->g_plus_h > rhs.g_plus_h)
    		return false;
    	else if(this->state_index < rhs.state_index)
    		return true;
    	else if(this->state_index > rhs.state_index)
    		return false;
    	else if(this->phase_number < rhs.phase_number)
    		return true;
    	else
    		return false;
    }
};

class Astar : public Policy
{
	long int num_states;
  bool astar_flag;
  string algo;
	vector< vector<int> > optimal_policy;
	vector< vector<int> > mig_list;

	float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map);
	float getSUV(int phase, int* vm_to_pm_map);

  public:
	Astar(SimData *s_data, string a, bool flag);
	float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int *best_perm_map);
	void run(int phases);
	void getMapping(int phase_number, vector<int>* mapping);
	void getMigrationList(int phase_number, vector<int>* mapping);
	void dumpPolicy();
};

struct ThreadDataStar
{
   int num_vms;
   int p;
   Matrix<int*>* mig_table;
   Matrix<float>* trans_table;
   Astar *astar;

   ThreadDataStar(int n, int phaseno, Matrix<int*>* mtable, Matrix<float>* ttable, Astar* a)
   : num_vms(n), p(phaseno), mig_table(mtable), trans_table(ttable), astar(a) {}
};

#endif
