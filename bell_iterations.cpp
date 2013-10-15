#include <iostream>
using namespace std;

static int count = 0;

void visit(int *array, int len)
{
    for(int i=0; i<len; i++)
    {
        cout<<array[i];
    }

    cout<<endl;
    count++;
}

int main()
{
    int n;
    cin>>n;

    int *a = new int[n];
    int *b = new int[n];

    for(int i=0; i<n; i++)
    {
        a[i] = 0;
        b[i] = 1;
    }

    while(true)
    {
        visit(a, n);
        if(a[n-1] == b[n-1])
        {
            int j = (n-1) - 1;
            while(a[j] == b[j]) {j = j - 1;}
            if(j == 0) { break;}
            else { a[j] = a[j] + 1;}
            b[n-1] = b[j] + (int)(a[j] == b[j]);
            j = j + 1;
            while(j < (n-1))
            {
                a[j] = 0;
                b[j] = b[n-1];
                j = j + 1;
            }
            a[n-1] = 0;
        } else
        {
            a[n-1] = a[n-1] + 1;
        }
    }

    cout<<count<<endl;

    delete a;
    delete b;
}
