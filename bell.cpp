#include <iostream>
#include <cstdio>
using namespace std;

double get_sterling(double *sterling, int N, int n, int k)
{
    if(sterling[n*N+k] == -1)
        if((n==0) && (k==0))
            sterling[0] = 1;
        else if((n==0) || (k==0))
            sterling[n*N+k] = 0;
        else
            sterling[n*N+k] = k*get_sterling(sterling, N, n-1, k) + get_sterling(sterling, N, n-1, k-1);
    
    return sterling[n*N+k];
}

double get_bell(int n)
{
    n = n+1;
    double *sterling = new double[n*n];
    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            sterling[i*n+j] = -1;

    double sum = 0;
    for(int i=0; i<n; i++)
        sum += get_sterling(sterling, n, n-1, i);

    delete sterling;
    return sum;
}

int main()
{
    int n;
    cout<<"Enter n:";
    cin>>n;

    printf("%20f\n", get_bell(n));
}
