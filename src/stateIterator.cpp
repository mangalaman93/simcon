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
    list<int>** pm_to_vm_map = new list<int>*[size];
    for(int i=0; i<size; i++) { pm_to_vm_map[i]=new list<int>;}
    for(int i=0; i<size; i++) { pm_to_vm_map[vm_to_pm_map[i]]->push_back(i);}

    cout<<"[";
    for(int i=0; i<size; i++)
    {
        if(pm_to_vm_map[i]->size() != 0)
        {
            cout<<"{";
            for(list<int>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            {
                if(it == pm_to_vm_map[i]->begin()) cout<<*it;
                else cout<<","<<*it;
            }
            cout<<"}";
        }
    }
    cout<<"]";

    for(int i=0; i<size; i++) { delete pm_to_vm_map[i];}
    delete [] pm_to_vm_map;
}

StateIterator::~StateIterator()
{
    delete [] vm_to_pm_map;
    delete [] auxillary;
}
