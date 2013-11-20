#include "policy.h"

Policy::Policy(SimData *s_data)
{
	run_for_phases = 0;
    max_profit = 0;
	sdata = s_data;
	num_vms = sdata->getNumVM();
    num_phases = sdata->getNumPhases();
}

void Policy::printPolicy()
{
	for(int p=0; p<run_for_phases; p++)
    {
    	printState(p);
    	cout<<"\t==>\t";
    	printMigrationList(p);
    	cout<<endl;
    }
    cout<<"overall_profit: "<<max_profit<<endl;
}

void Policy::printState(int phase_number)
{
	vector<int>* vm_to_pm_map = new vector<int>(num_vms, -1);
    getMapping(phase_number, vm_to_pm_map);
	list<int>** pm_to_vm_map = new list<int>*[num_vms];
    for(int i=0; i<num_vms; i++) { pm_to_vm_map[i] = new list<int>;}
    for(int i=0; i<num_vms; i++) { pm_to_vm_map[(*vm_to_pm_map)[i]]->push_back(i);}

    cout<<"[";
    for(int i=0; i<num_vms; i++)
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

    for(int i=0; i<num_vms; i++) { delete pm_to_vm_map[i];}
    delete [] pm_to_vm_map;
    delete vm_to_pm_map;
}

void Policy::printMigrationList(int phase_number)
{
	vector<int>* vm_mig_list = new vector<int>(num_vms, -1);
    getMigrationList(phase_number, vm_mig_list);
	cout<<"migrate vms: ";

    for(int j=0; j<num_vms; j++)
        if((*vm_mig_list)[j] != -1)
            cout<<j<<", ";

    delete vm_mig_list;
}

float Policy::getOverallProfit()
{
    return max_profit;
}

Policy::~Policy() {}
