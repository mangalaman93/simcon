// Utilization threshold of PM(both on CPU front and memory front)
#define UTIL_THRESHOLD 0.8 

// Lower utilization threshold used to trigger remapping
#define LOWERUTIL_THRESHOLD 0.25

// State transition time as a fraction of total phase duration. PHASEDURATION * TOTALMIGRATIONTIME = State Transition Time.
#define TOTALMIGRATIONTIME 0.05

//When all VMs are of same priority(because they are earning same amount and have same SLA) NOTE : don't use this once VMs earn different amounts
#define IMMREWARD 10000 //Reward of a satisfied VM per workload phase(VM Revenue per Workload phase - VMR)
#define IMMPENALTY 150000 //Penalty of a dissatisfied VM per workload phase(VM Penalty per Workload phase - VMP)
#define OPERATIONALCOST 1 //PM operational cost per workload phase(NOTE : not to be used once a Power Model is implemented)

#define INFTY -999999999999.99 //Custom INFINITY VALUE



//Utilization based power model 
//Linear Power Model Constants( P = C_0 + C_1 * CPUUtil )
#define STATICPOWERCONSTANT	0.40        			//C_0
#define DYNAMICPOWERCONSTANT	( 1 - STATICPOWERCONSTANT )	//C_1
#define MAXPOWER		250				//Wattage of PC(in watts, and how many joules per hour not per sec). 250 watts means 250 watthour joules per hour
#define COSTPERKWH		12				//Average cost of one kWh in RS

#define PHASEDURATION		6				//Single workload phase duration in hours. 6 hours 

#define NUMRESOURCETYPES	2				//Number of resource types we're working with(e.g. CPU, Memory, and N/W bandwidth etc)
#define CPU			0				//CPU resource type index in workload matrix
#define MEM			1				//Memory resource type index in workload matrix

//VM Types : Memory Intensive or CPU Intensive 
//Used to impose different migration overhead factors for different types of VMs
#define MEMINTENSIVE		1				//Memory Intensive workload VM
#define CPUINTENSIVE		2				//CPU Intensive workload VM
#define MOHMEMINTENSIVE		0.40				//Migration overhead of Memory Intensive workload VM
#define MOHCPUINTENSIVE		0.40				//Migration overhead of CPU Intensive workload VM
#define MCMEMINTENSIVE		0.70				//Migration cost(downtime fraction) for MEMINTENSIVE VMs: downtime = MCMEMINTENSIVE * MIGRATIONTIME
#define MCCPUINTENSIVE		0.70				//Migration cost(downtime fraction) for CPUINTENSIVE VMs: downtime = MCCPUINTENSIVE * MIGRATIONTIME