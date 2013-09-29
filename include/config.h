// Utilization threshold of PM(both on CPU front and memory front)
#define UTIL_THRESHOLD 0.8

// Lower utilization threshold used to trigger remapping
#define LOWERUTIL_THRESHOLD 0.25

// Absolute total migration time in hours
#define MIGRATIONDURATION 0.3

// Power calculation constants
#define STATICPOWERCONSTANT	0.40
#define DYNAMICPOWERCONSTANT (1 - STATICPOWERCONSTANT)
#define COSTPERKWH 5
#define MAXPOWER 250
#define IMREWARD 10000
#define IMPENALTY 150000
