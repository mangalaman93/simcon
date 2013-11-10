#include "policy.h"

Policy::Policy(Simdata sdata)
{
	phases_run_for = 0;
	this.sdata = sdata;
	num_vms = sdata->getNumVM();
}

void Policy::printPolicy()
{
	for(int p=0; p<phases_run_for; p++)
    {
    	printState(p);
    	cout<<"\t==>\t";
    	printMigrationList(p);
    	cout<<endl;
    }
}

Policy::printState(int phase_number)
{
	int* vm_to_pm_map = getMapping(phase_number);
	list<int>** pm_to_vm_map = new list<int>*[num_vms];
    for(int i=0; i<num_vms; i++) { pm_to_vm_map[i] = new list<int>;}
    for(int i=0; i<num_vms; i++) { pm_to_vm_map[vm_to_pm_map[i]]->push_back(i);}

    cout<<"[";
    for(int i=0; i<num_vms; i++)
    {
        if(pm_to_vm_map[i]->num_vms() != 0)
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

    for(int i=0; i<num_vms; i++) { delete pm_to_vm_map[i];}
    delete [] pm_to_vm_map;
}

void Policy::printMigrationList(int phase_number)
{
	int* vm_mig_list = getMigrationList(phase_number);
	cout<<"migrate vms: ";

    for(int j=0; j<num_vms; j++)
        if(vm_mig_list[j] == 1)
            cout<<j<<", ";
}

Policy::~Policy() {}
