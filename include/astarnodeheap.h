/*
Custom heap class for Dijkstra's algorithm
may be converted to Fibonaci heap later
*/

#ifndef ASTARNODEHEAP_H
#define ASTARNODEHEAP_H

#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

struct AstarNode
{
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
};
typedef struct AstarNode AstarNode;

class AstarNodeHeap
{
	vector<AstarNode*>* root;
	int size;
	void percolateUp(int index);
	void percolateDown(int index);

  public:
	AstarNodeHeap();
    AstarNodeHeap(const AstarNodeHeap& obj);
	~AstarNodeHeap();
	void pop();
	void push(const AstarNode& elem);
	void modify(const AstarNode& node);
    bool empty();
    AstarNode top();
};

#endif
