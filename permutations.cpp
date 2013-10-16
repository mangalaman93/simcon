#include <iostream>
using namespace std;

void getNext(int *value, int N)
{
    int i = N - 1;
    while(value[i-1] >= value[i]) 
        i = i-1;

    int j = N;
    while(value[j-1] <= value[i-1]) 
        j = j-1;

    int temp = value[i-1];
    value[i-1] = value[j-1];
    value[j-1] = temp;

    i++;
    j = N;
    while(i < j)
    {
        temp = value[i-1];
        value[i-1] = value[j-1];
        value[j-1] = temp;
        i++;
        j--;
    }
}

double factorial(int n)
{
    if(n == 0)
        return 1;
    else
        return (n*factorial(n-1));
}

void print(int *value, int N)
{
    for(int i=0; i<N; i++)
    {
        cout<<value[i];
    }
    cout<<endl;
}

int main()
{
    int N;
    cin>>N;

    int* value = new int[N];
    for(int i=0; i<N; i++)
    {
        value[i] = i;
    }
    print(value, N);

    for(int i=1; i<factorial(N); i++)
    {
        getNext(value, N);
        print(value, N);
    }
}
