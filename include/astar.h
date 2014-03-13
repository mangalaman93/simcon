/*
Implementation of MDP algorithm
*/

#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <cmath>
#include <fstream>
#include <pthread.h>
#include <queue>
#include "algo.h"
#include "simdata.h"
#include "config.h"
#include "stateIterator.h"
#include "policy.h"
using namespace std;

struct AstarNode
{
    int phase_number;
    int state_index;
    Matrix<float> *g_plus_h;

    AstarNode(int pn, int sn, Matrix<float>* gph)
    : phase_number(pn), state_index(sn), g_plus_h(gph) {}

    AstarNode(const AstarNode& an)
    {
    	phase_number = an.phase_number;
    	state_index = an.state_index;
    	g_plus_h = an.g_plus_h;
    }
};
typedef struct AstarNode AstarNode;

class CompareAstarNode
{
  public:
    bool operator()(AstarNode& an1, AstarNode& an2)
    {
        return((*an1.g_plus_h)(an1.phase_number, an1.state_index) > (*an2.g_plus_h)(an2.phase_number, an2.state_index));
    }
};

class Astar : public Policy
{
	long int num_states;
	vector< vector<int> > optimal_policy;
	vector< vector<int> > mig_list;

	float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map);
	float getSUV(int phase, int* vm_to_pm_map);

  public:
	Astar(SimData *s_data);
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