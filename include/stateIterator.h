#ifndef STATEITERATOR_H
#define STATEITERATOR_H

#include <iostream>
#include <cstdlib>
#include <list>
using namespace std;

class StateIterator
{
    int size;
    bool loop_end;
    int *vm_to_pm_map;
    int *auxillary;
    int state_number;

public:
    StateIterator(int n);
    ~StateIterator();
    void begin();
    bool end() const;
    void operator++();
    int* & operator*();
    operator int() const;
    void print();
};

#endif
