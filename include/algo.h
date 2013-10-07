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
