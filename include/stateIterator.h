#ifndef STATEITERATOR_H
#define STATEITERATOR_H

#include "state.h"
using namespace std;

class StateIterator
{
    int size;
    bool loop_end;
    int *vm_to_pm_map;
    int *auxillary;

public:
    StateIterator(int n);
    ~StateIterator();
    void begin() const;
    bool end() const;
    void operator++();
    int* & operator*();
};

#endif
