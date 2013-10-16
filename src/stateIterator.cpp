#include "stateIterator.h"

StateIterator::StateIterator(int n)
{
    size = n;
    vm_to_pm_map = new int[size];
    auxillary = new int[size];
}

void StateIterator::begin()
{
    state_number = 0;
    for(int i=0; i<size; i++)
    {
        vm_to_pm_map[i] = 0;
        auxillary[i] = 1;
    }
    loop_end = true;
}

bool StateIterator::end() const
{
    return loop_end;
}

void StateIterator::operator++()
{
    if(!loop_end)
    {
        cout<<"Error occurred, out of bounds!"<<endl;
        exit(1);
    }

    state_number++;
    if(vm_to_pm_map[size-1] == auxillary[size-1])
    {
        int j = (size-1) - 1;
        while(vm_to_pm_map[j] == auxillary[j]) {j = j - 1;}
        if(j == 0) { loop_end = false;}
        else { vm_to_pm_map[j] = vm_to_pm_map[j] + 1;}
        auxillary[size-1] = auxillary[j] + (int)(vm_to_pm_map[j] == auxillary[j]);
        j = j + 1;
        while(j < (size-1))
        {
            vm_to_pm_map[j] = 0;
            auxillary[j] = auxillary[size-1];
            j = j + 1;
        }
        vm_to_pm_map[size-1] = 0;
    } else
    {
        vm_to_pm_map[size-1] = vm_to_pm_map[size-1] + 1;
    }
}

int* & StateIterator::operator*()
{
    return vm_to_pm_map;
}

StateIterator::operator int() const
{
    return state_number;
}

void StateIterator::print()
{
    for(int i=0; i<size; i++)
        cout<<vm_to_pm_map[i];
    cout<<endl;
}

StateIterator::~StateIterator()
{
    delete vm_to_pm_map;
    delete auxillary;
}
