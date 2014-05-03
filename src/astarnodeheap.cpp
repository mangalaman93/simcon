#include "astarnodeheap.h"

AstarNodeHeap::AstarNodeHeap()
{
	root = new vector<AstarNode*>();
	size = 0;
}

void AstarNodeHeap::percolateUp(int index)
{
	int pn = (index - 1)/2;
	while((index != 0) && ((*root)[index]->g_plus_h > (*root)[pn]->g_plus_h))
	{
		AstarNode* temp = (*root)[index];
		(*root)[index] = (*root)[pn];
		(*root)[pn] = temp;

		index = pn; pn = (index-1)/2;
	}
}

void AstarNodeHeap::percolateDown(int index)
{
	if(2*index+1 >= size)
		return;
	else
	{
		int larger_child=-1, smaller_child=-1;
		if(2*index+2 >= size)
			larger_child = 2*index+1;
		else
		{
			larger_child = ((*root)[2*index+2]->g_plus_h > (*root)[index*2+1]->g_plus_h)?2:1;
			smaller_child = 2*index + 3 - larger_child;
			larger_child += 2*index;
		}

		if((*root)[index]->g_plus_h < (*root)[larger_child]->g_plus_h)
		{
			AstarNode* temp = (*root)[index];
			(*root)[index] = (*root)[larger_child];
			(*root)[larger_child] = temp;

			percolateDown(larger_child);
		} else if((smaller_child != -1) && ((*root)[index]->g_plus_h < (*root)[smaller_child]->g_plus_h))
		{
			AstarNode* temp = (*root)[index];
			(*root)[index] = (*root)[smaller_child];
			(*root)[smaller_child] = temp;

			percolateDown(smaller_child);
		}
	}
}

void AstarNodeHeap::pop()
{
	if(size <= 0)
	{
		cout<<"poping from an empty heap, error in file " << __FILE__ << "@line " << __LINE__ << endl;
		exit(-1);
	}

	delete ((*root)[0]);
	(*root)[0] = (*root)[size-1];
	root->pop_back();
	size--;
	percolateDown(0);

	// for(vector<AstarNode*>::iterator iter = root->begin(); iter != root->end(); ++iter)
	// 	cout<<(*iter)->g_plus_h<<", ";
	// cout<<endl;
}

void AstarNodeHeap::push(const AstarNode& elem)
{
	root->push_back(new AstarNode(elem));
	size++;

	percolateUp(size-1);

	// for(vector<AstarNode*>::iterator iter = root->begin(); iter != root->end(); ++iter)
	// 	cout<<(*iter)->g_plus_h<<", ";
	// cout<<endl;
}

AstarNode AstarNodeHeap::top()
{
	if(size <= 0)
	{
		cout<<"getting top from an empty heap, error in file " << __FILE__ << "@line " << __LINE__ << endl;
		exit(-1);
	}

	return *((*root)[0]);
}

void AstarNodeHeap::modify(const AstarNode& node)
{
	int index = 0;
	for(vector<AstarNode*>::iterator iter = root->begin(); iter != root->end(); ++iter)
	{
		if((node.phase_number == (*iter)->phase_number) && (node.state_index == (*iter)->state_index))
			break;
		index++;
	}
	(*root)[index]->g_plus_h = node.g_plus_h;

	if((*root)[(index-1)/2]->g_plus_h > (*root)[index]->g_plus_h)
		percolateDown(index);
	else
		percolateUp(index);

	// for(vector<AstarNode*>::iterator iter = root->begin(); iter != root->end(); ++iter)
	// 	cout<<(*iter)->g_plus_h<<", ";
	// cout<<endl;
}

AstarNodeHeap::~AstarNodeHeap()
{
	for(vector<AstarNode*>::iterator iter = root->begin(); iter != root->end(); ++iter)
		delete (*iter);
	delete root;
}
