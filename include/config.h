// Utilization threshold of PM(both on CPU front and memory front)
#define UTIL_THRESHOLD 0.8

// Lower utilization threshold used to trigger remapping
#define LOWERUTIL_THRESHOLD 0.25

// Power constants
#define STATICPOWERCONSTANT 0.40
#define DYNAMICPOWERCONSTANT (1 - STATICPOWERCONSTANT)

// Wattage of PC (in watt, joule per hour)
#define MAXPOWER 250
#define COSTPERKWH 5

// Migration overhead of CPU Intensive workload VM
#define MOHCPUINTENSIVE 0.55

// Absolute total migration time in hours
#define MIGRATIONDURATION 0.3
