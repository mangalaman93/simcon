#include <iostream>
using namespace std;

static int count = 0;

void visit(int *array, int len)
{
    for(int i=1; i<=len; i++)
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

    int *a = new int[n+1];
    int *b = new int[n+1];

    for(int i=1; i<=n; i++)
    {
        a[i] = 0;
        b[i] = 1;
    }

    while(true)
    {
        visit(a, n);
        if(a[n] == b[n])
        {
            int j = n - 1;
            while(a[j] == b[j]) {j = j - 1;}
            if(j == 1) { break;}
            else { a[j] = a[j] + 1;}
            b[n] = b[j] + (int)(a[j] == b[j]);
            j = j + 1;
            while(j < n)
            {
                a[j] = 0;
                b[j] = b[n];
                j = j + 1;
            }
            a[n] = 0;
        } else
        {
            a[n] = a[n] + 1;
        }
    }

    cout<<count<<endl;

    delete a;
    delete b;
}
