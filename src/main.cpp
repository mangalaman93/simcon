#include <iostream>
#include <sys/time.h>
#include "simdata.h"
#include "khanna.h"
#include "mdp.h"
#include "astar.h"

int main()
{
	int num_vms, num_phases;
    cin>>num_vms;
    cin>>num_phases;

    SimData *sdata = new SimData(num_vms, num_phases);
    sdata->readInput();

    struct timeval t_begin, t_end;
    gettimeofday(&t_begin, NULL);
    cout<<"############## mdp ##############"<<endl;
    Mdp mdp(sdata);
    mdp.run(num_phases);
    mdp.printPolicy();
    mdp.dumpPolicy();
    gettimeofday(&t_end, NULL);
    cout<<"Time Taken: "<<(t_end.tv_sec-t_begin.tv_sec)*1000000 + t_end.tv_usec-t_begin.tv_usec<<endl;

    gettimeofday(&t_begin, NULL);
    cout<<"############## dijkstra ##############"<<endl;
    Astar dijkstra(sdata, "dijkstra", false);
    dijkstra.run(num_phases);
    dijkstra.printPolicy();
    dijkstra.dumpPolicy();
    gettimeofday(&t_end, NULL);
    cout<<"Time Taken: "<<(t_end.tv_sec-t_begin.tv_sec)*1000000 + t_end.tv_usec-t_begin.tv_usec<<endl;

    gettimeofday(&t_begin, NULL);
    cout<<"############## astar ##############"<<endl;
    Astar astar(sdata, "astar", true);
    astar.run(num_phases);
    astar.printPolicy();
    astar.dumpPolicy();
    gettimeofday(&t_end, NULL);
    cout<<"Time Taken: "<<(t_end.tv_sec-t_begin.tv_sec)*1000000 + t_end.tv_usec-t_begin.tv_usec<<endl;

    gettimeofday(&t_begin, NULL);
    cout<<endl;
    cout<<"############## khanna ##############"<<endl;
    Khanna kh(sdata);
    kh.run(num_phases);
    kh.printPolicy();
    kh.dumpPolicy();
    gettimeofday(&t_end, NULL);
    cout<<"Time Taken: "<<(t_end.tv_sec-t_begin.tv_sec)*1000000 + t_end.tv_usec-t_begin.tv_usec<<endl;

    delete sdata;
	return 0;
}
