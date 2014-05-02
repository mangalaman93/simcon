/*
Custom heap class for Dijkstra's algorithm
may be converted to Fibonaci heap later
*/

#ifndef CUSTOMHEAP_H
#define CUSTOMHEAP_H

#include <iostream>
using namespace std;

template <class T>
class CustomHeap
{
  public:
	CustomHeap();
	~CustomHeap();
	void pop();
	void push(T& elem);
	T pop();
	void modify();
};

#endif
