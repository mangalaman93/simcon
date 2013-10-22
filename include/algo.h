/*
various algorithms are implemented in this class-
1) finding out the sterling number of second kind
2) calculating the bell number
we are using memoization to increase the efficiency of the
algorithm. read more here http://en.wikipedia.org/wiki/Memoization
for details go to http://en.wikipedia.org/wiki/Stirling_numbers_of_the_second_kind
*/

#ifndef ALGO_H
#define ALGO_H

#include <iostream>
#include "utils.h"
using namespace std;

class Sterling
{
    Matrix<int> *memo;

public:
    Sterling(int size);
    ~Sterling();
    int get(int n, int k);
};

class Bell
{
public:
    Bell();
    static long int get(int n);
};

#endif
