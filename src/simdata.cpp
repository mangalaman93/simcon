#include "simdata.h"

SimData::SimData()
{
}

simdata::readInputFile() 
{
	for(int i = 1;i < NUMPHASES + 1;i++){
		for(int j = 1;j < NUMVMS + 1;j++){
			scanf("%lf",&(workload[i][j][CPU]));
			scanf("%lf",&(workload[i][j][MEM]));
		}
	}

	for(int i = 1; i < NUMVMS + 1; i++){
		scanf("%hd",VMtype + i);
	}

	for(int i = 1; i < NUMVMS + 1; i++){
		scanf("%f",VMR + i);
	}

	for(int i = 1; i < NUMVMS + 1; i++){
		scanf("%f",VMP + i);
	}
}

SimData::~SimData()
{
}
