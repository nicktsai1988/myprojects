#ifndef DECODETREE_H
#define DECODETREE_H
#include "bitvector.h"
#include "utils.h"
struct TreeNode
{
	unsigned int info;
	TreeNode *left_child;
	TreeNode *right_child;
};
class DecodeTree
{
public:
	DecodeTree();
	DecodeTree(const vector<HuffNode>& nodes);
	void init(const vector<HuffNode>& nodes);
	unsigned int decode(const BitVector& vector,unsigned int *position);//position返回vector下一个数据的起始位置
	~DecodeTree(void);
private:
	//bool free_node(TreeNode **ptr);
	TreeNode *root;
};
#endif
