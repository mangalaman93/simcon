#include <iostream>
#include "simdata.h"
#include "khanna.h"
#include "mdp.h"

#define DEBUG false

int main()
{
	int num_vms, num_phases;
    cin>>num_vms;
    cin>>num_phases;

    SimData *sdata = new SimData(num_vms, num_phases);
    sdata->readInput();

    cout<<"******* khanna *******"<<endl;
    Khanna kh(sdata);
    kh.run();
    kh.printPolicy();
    cout<<endl;

    cout<<"******* mdp *******"<<endl;
    MDP mdp(sdata);
    mdp.run();
    mdp.printPolicy();
    cout<<endl;

    delete sdata;
	return 0;
}
