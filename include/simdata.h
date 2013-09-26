class SimData
{

public:
double workload[NUMPHASES + 1][NUMVMS + 1][NUMRESOURCETYPES]; //Cyclic Workload Matrix
short VMtype[NUMVMS + 1];//Array storing VMTYPE info : MEMINTENSIVE or CPUINTENSIVE
float VMR[NUMVMS + 1]; //VMRevenue matrix storing revenue made by an SLA compliant VM per workload phase
float VMP[NUMVMS + 1];

    SimData();
    ~SimData();
    void readInputFile();

};
