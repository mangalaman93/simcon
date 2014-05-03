#include "astarnodeheap.h"
#include <iostream>
using namespace std;

int main()
{
	AstarNodeHeap anh;
	anh.push(AstarNode(0, 0, 6));
	anh.push(AstarNode(0, 0, 2));
	anh.push(AstarNode(0, 0, 5));
	anh.push(AstarNode(1, 1, 4));
	anh.push(AstarNode(0, 0, 3));
	anh.push(AstarNode(1, 2, 1));
	cout<<"top should be 6 and actually is: "<<anh.top().g_plus_h<<endl;
	anh.pop();
	cout<<"top should be 5 and actually is: "<<anh.top().g_plus_h<<endl;
	anh.push(AstarNode(0, 0, 8));
	cout<<"top should be 8 and actually is: "<<anh.top().g_plus_h<<endl;
	anh.modify(AstarNode(1, 2, 18));
	cout<<"top should be 18 and actually is: "<<anh.top().g_plus_h<<endl;
	return 0;
}
