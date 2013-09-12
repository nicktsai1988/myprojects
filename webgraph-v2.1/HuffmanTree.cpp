#include "HuffmanTree.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
using std::cerr;
using std::endl;

HuffmanTree::HuffmanTree(const InfoNode *nodes,size_t size):_result(size)
{
	size_t i,start;
	char *buf;
	unsigned int s1,s2;
	if(size<=1)
	{
		cerr<<"HuffmanTree size error"<<endl;
		exit(1);
	}
	nodesize=size;
	treesize=2*nodesize-1;
	if((data=new HTNode[treesize+1])==NULL)   //0号单元不用
	{
		cerr<<"HuffmanTree construct error"<<endl;
		exit(1);
	}
	for(i=1;i<=nodesize;i++)
	{
		data[i].id=nodes[i-1].id;
		data[i].weight=nodes[i-1].weight;
		data[i].parent=data[i].lchild=data[i].rchild=0;
	}
	for(i=nodesize+1;i<=treesize;i++)
	{
		data[i].id=0;
		data[i].weight=0;
		data[i].parent=data[i].lchild=data[i].rchild=0;
	}
	for(i=nodesize+1;i<=treesize;i++)
	{
		select(i,&s1,&s2); //区间为【1，i)
		data[s1].parent=i;
		data[s2].parent=i;
		data[i].lchild=s1;
		data[i].rchild=s2;
		data[i].weight=data[s1].weight+data[s2].weight;
	}
	//::TODO
	if((buf=new char[nodesize])==NULL)
	{
		cerr<<"HuffmanTree new error"<<endl;
		exit(1);
	}
	buf[nodesize-1]='\0';
	size_t cur,father;
	for(i=1;i<=nodesize;i++)
	{
		start=nodesize-1;
		for(cur=i,father=data[i].parent;father!=0;cur=father,father=data[father].parent)
		{
			if (data[father].lchild==cur)
			{
				buf[--start]='0';
			} 
			else
			{
				buf[--start]='1';
			}
		}
		_result[i-1].info=data[i].id;
		_result[i-1].vector=BitVector(&buf[start],strlen(&buf[start]));
	}
	delete[] buf;
}

HuffmanTree::HuffmanTree(const vector<InfoNode>& nodes):_result(nodes.size())
{
	size_t i,start;
	char *buf;
	unsigned int s1,s2;
	if(nodes.size()<= 1)
	{
		cerr<<"HuffmanTree size error"<<endl;
		exit(1);
	}
	nodesize=nodes.size();
	treesize=2*nodesize-1;
	if((data=new HTNode[treesize+1])==NULL)   //0号单元不用
	{
		cerr<<"HuffmanTree construct error"<<endl;
		exit(1);
	}
	for(i=1;i<=nodesize;i++)
	{
		data[i].id=nodes[i-1].id;
		data[i].weight=nodes[i-1].weight;
		data[i].parent=data[i].lchild=data[i].rchild=0;
	}
	for(i=nodesize+1;i<=treesize;i++)
	{
		data[i].id=0;
		data[i].weight=0;
		data[i].parent=data[i].lchild=data[i].rchild=0;
	}
	for(i=nodesize+1;i<=treesize;i++)
	{
		select(i,&s1,&s2); //区间为【1，i)
		data[s1].parent=i;
		data[s2].parent=i;
		data[i].lchild=s1;
		data[i].rchild=s2;
		data[i].weight=data[s1].weight+data[s2].weight;
	}
	//::TODO
	if((buf=new char[nodesize])==NULL)
	{
		cerr<<"HuffmanTree new error"<<endl;
		exit(1);
	}
	buf[nodesize-1]='\0';
	size_t cur,father;
	for(i=1;i<=nodesize;i++)
	{
		start=nodesize-1;
		for(cur=i,father=data[i].parent;father!=0;cur=father,father=data[father].parent)
		{
			if (data[father].lchild==cur)
			{
				buf[--start]='0';
			} 
			else
			{
				buf[--start]='1';
			}
		}
		_result[i-1].info=data[i].id;
		_result[i-1].vector=BitVector(&buf[start],strlen(&buf[start]));
	}
	delete[] buf;
}
HuffmanTree::~HuffmanTree(void)
{
	delete[] data;
}
bool HuffmanTree::getcode(unsigned int id,BitVector& code) //id从0开始,code保存结果
{
	if(id>=nodesize)
		return false;
	else
	{
		code=_result[id].vector;
		return true;
	}
}

size_t HuffmanTree::saveresult(vector<HuffNode>& result)
{
	result.clear();
	result.resize(_result.size());
	for(unsigned int i=0;i<_result.size();i++)
	{
		result[i].info=_result[i].info;
		result[i].vector=_result[i].vector;
	}
	return result.size();
}
void HuffmanTree::select(unsigned int upbound,unsigned int *s1,unsigned int *s2)
{
	unsigned int i;
	unsigned int minimum[2]={0,0};
	for(i=1;i<upbound;i++)
	{
		if(data[i].parent==0)
		{
			minimum[0]=i;
			break;
		}
	}
	for(i++;i<upbound;i++)
	{
		if(data[i].parent==0)
		{
			minimum[1]=i;
			break;
		}
	}
	for(i++;i<upbound;i++)//选择parent为0且weight为最小的两个
	{
		if(data[i].parent==0&&(data[i].weight<max(data[minimum[0]].weight,data[minimum[1]].weight)))
			minimum[max_index(minimum,2)]=i;
	}
	*s1=minimum[0];
	*s2=minimum[1];
}

unsigned int HuffmanTree::max(unsigned int x,unsigned int y)
{
	return (x>y)?x:y;
}
unsigned int HuffmanTree::max_index(unsigned int array[],size_t size)
{
	unsigned int result=0;
	for(unsigned int i=1;i<size;i++)
	{
		if(data[array[i]].weight>data[array[result]].weight)
			result=i;
	}
	return result;
}