#include "algo.h"

Sterling::Sterling(int size)
{
    memo = new Matrix<int>(size+1, size+1);
    for(int i=0; i<=size; i++)
        for(int j=0; j<=size; j++)
            (*memo)(i, j) = -1;
}

int Sterling::get(int n, int k)
{
    if((*memo)(n, k) == -1)
    {
        if(n==0 && k==0)
            (*memo)(0, 0) = 1;
        else if(n==0 || k==0)
            (*memo)(n, k) = 0;
        else
            (*memo)(n, k) = k*get(n-1, k) + get(n-1, k-1);
    }
    
    return (*memo)(n, k);
}

Sterling::~Sterling()
{
    delete memo;
}

long int Bell::get(int n)
{
    Sterling sterling(n);

    long int bell = 0;
    for(int i=0; i<=n; i++)
        bell += sterling.get(n, i);

    return bell;
}
