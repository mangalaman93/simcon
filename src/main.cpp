#include <iostream>
#include "simdata.h"
#include "mdp.h"

int main()
{
	int num_vms, num_phases;
    cin>>num_vms;
    cin>>num_phases;

    SimData *sdata = new SimData(num_vms, num_phases);
    sdata->readInput();

    cout<<"******* mdp *******"<<endl;
    Mdp mdp(sdata);
    mdp.run(num_phases);
    mdp.printPolicy();

    delete sdata;
	return 0;
}
