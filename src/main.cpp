#include <iostream>
#include "simdata.h"
#include "khanna.h"
#include "mdp.h"

int main()
{
	int num_vms, num_phases;
    cin>>num_vms;
    cin>>num_phases;

    SimData *sdata = new SimData(num_vms, num_phases);
    sdata->readInput();

    cout<<"############## mdp ##############"<<endl;
    Mdp mdp(sdata);
    mdp.run(num_phases);
    mdp.printPolicy();
    mdp.dumpPolicy();

    cout<<endl;
    cout<<"############## khanna ##############"<<endl;
    Khanna kh(sdata);
    kh.run(num_phases);
    kh.printPolicy();
    kh.dumpPolicy();

    delete sdata;
	return 0;
}
