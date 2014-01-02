/*
This is an iterator class which allows you to iterate over
all possible states (new representation) in a lexicographical
order. A standard use case is as follows-

StateIterator sitr;
for(sitr.begin(); sitr.end(), ++sitr)
{
    // write your code here
}
*/

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
