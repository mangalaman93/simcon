class pm_info
{
public:
	list<vm_info> vm_l;
	float total_util;
	pm_info(const pm_info &obj)
	{
		total_util = obj.total_util;
		vm_l(obj.vm_l);
	}
};



class State
{
public:
	int phase_num;
	pm_info vm_on_pm_tup[NUM_PM]; 
    State();
    ~State();
    getNextState();
    ifVmAllowedOnPm(float vm_util, int pm_index);
};
