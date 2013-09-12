#include "DecodeTree.h"
#include <iostream>
#include <cstdlib>
#include <climits>

DecodeTree::DecodeTree(void)
{
	/*
	if((root=new TreeNode)==NULL)
	{
		std::cerr<<"DecodeTree construct error"<<std::endl;
		exit(1);
	}
	root->info=UINT_MAX;
	root->left_child=NULL;
	root->right_child=NULL;
	*/
	root=NULL;
}

//x个叶子节点，则总节点个数为2x-1
/*
DecodeTree::DecodeTree(const vector<HuffNode>& nodes)
{
	unsigned int i,j;
	if((root=new TreeNode)==NULL)
	{
		std::cerr<<"DecodeTree construct error"<<std::endl;
		exit(1);
	}
	root->info=UINT_MAX;
	root->left_child=NULL;
	root->right_child=NULL;
	TreeNode *ptr,*pre;

	for(i=0;i<nodes.size();i++)
	{
		ptr=root;
		for(j=0;j<nodes[i].vector.length();j++)
		{
			pre=ptr;
			nodes[i].vector.test(j)?(ptr=ptr->right_child):(ptr=ptr->left_child);
		
			if(ptr==NULL)
			{
				TreeNode *newnode;
				if((newnode=new TreeNode)==NULL)
				{
					std::cerr<<"DecodeTree construct error"<<std::endl;
					exit(1);
				}
				newnode->left_child=NULL;
				newnode->right_child=NULL;
				newnode->info=UINT_MAX;
				//if(j==(nodes[i].vector.bits_length()-1))
					//newnode->info=nodes[i].info;
				nodes[i].vector.test(j)?(pre->right_child=newnode):(pre->left_child=newnode);
				ptr=newnode;
			}
		}
		ptr->info=nodes[i].info;
	}

}
*/
DecodeTree::DecodeTree(const vector<HuffNode>& nodes)
{
	unsigned int i,j,index;
	unsigned int size=2*nodes.size()-1;
	if((root=new TreeNode[size])==NULL)
	{
		std::cerr<<"DecodeTree construct error"<<std::endl;
		exit(1);
	}
	root[0].info=UINT_MAX;
	root[0].left_child=NULL;
	root[0].right_child=NULL;
	TreeNode *ptr,*pre;
	index=1;//从第一个开始
	for(i=0;i<nodes.size();i++)
	{
		ptr=&root[0];
		for(j=0;j<nodes[i].vector.length();j++)
		{
			pre=ptr;
			nodes[i].vector.test(j)?(ptr=ptr->right_child):(ptr=ptr->left_child);

			if(ptr==NULL)
			{
				TreeNode *newnode=&root[index++];
				if(index > size)
				{
					std::cerr<<"error: index > size"<<std::endl;
					exit(1);
				}
				newnode->left_child=NULL;
				newnode->right_child=NULL;
				newnode->info=UINT_MAX;
				//if(j==(nodes[i].vector.bits_length()-1))
				//newnode->info=nodes[i].info;
				nodes[i].vector.test(j)?(pre->right_child=newnode):(pre->left_child=newnode);
				ptr=newnode;
			}
		}
		ptr->info=nodes[i].info;
	}
}
void DecodeTree::init(const vector<HuffNode>& nodes)
{
	unsigned int i,j,index;
	unsigned int size=2*nodes.size()-1;
	if((root=new TreeNode[size])==NULL)
	{
		std::cerr<<"DecodeTree construct error"<<std::endl;
		exit(1);
	}
	root[0].info=UINT_MAX;
	root[0].left_child=NULL;
	root[0].right_child=NULL;
	TreeNode *ptr,*pre;
	index=1;//从第一个开始
	for(i=0;i<nodes.size();i++)
	{
		ptr=&root[0];
		for(j=0;j<nodes[i].vector.length();j++)
		{
			pre=ptr;
			nodes[i].vector.test(j)?(ptr=ptr->right_child):(ptr=ptr->left_child);

			if(ptr==NULL)
			{
				TreeNode *newnode=&root[index++];
				if(index > size)
				{
					std::cerr<<"error: index > size"<<std::endl;
					exit(1);
				}
				newnode->left_child=NULL;
				newnode->right_child=NULL;
				newnode->info=UINT_MAX;
				//if(j==(nodes[i].vector.bits_length()-1))
				//newnode->info=nodes[i].info;
				nodes[i].vector.test(j)?(pre->right_child=newnode):(pre->left_child=newnode);
				ptr=newnode;
			}
		}
		ptr->info=nodes[i].info;
	}
}
unsigned int DecodeTree::decode(const BitVector& vector,unsigned int *position)
{
	if(root == NULL)
	{
		std::cerr<<"DecodeTree need initialize first"<<std::endl;
		exit(1);
	}
	TreeNode *ptr=root;
	unsigned int index=*position;
	//unsigned int i;
	//for(i=0;i<vector.length();i++)
	//{
		//if(ptr!=NULL)
			//vector.test(index)?(ptr=ptr->right_child):(ptr=ptr->left_child);
		//else
			//return UINT_MAX;
	//}
	while(ptr != NULL && ptr->info == UINT_MAX)
	{
		vector.test(index++)?(ptr=ptr->right_child):(ptr=ptr->left_child);
	}
	if(ptr == NULL)
	{
		std::cerr<<"decode error"<<std::endl;
		exit(1);
	}
	*position=index;
	return ptr->info;
}

/*
bool DecodeTree::free_node(TreeNode **ptr)
{
	if(*ptr==NULL)
		return false;
	if(((*ptr)->left_child==NULL)&&((*ptr)->right_child==NULL))
	{
		delete *ptr;
		*ptr=NULL;
		return true;
	}
	if((*ptr)->left_child!=NULL)
		free_node(&((*ptr)->left_child));
	if((*ptr)->right_child!=NULL)
		free_node(&((*ptr)->right_child));
	return free_node(ptr);
}
*/


DecodeTree::~DecodeTree(void)
{
	//free_node(&root);
	if(root != NULL)
		delete[] root;
}
