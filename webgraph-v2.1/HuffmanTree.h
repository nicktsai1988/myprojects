#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H
#include "utils.h"

class HuffmanTree
{
private:
	struct HTNode
	{
		unsigned int id;
		unsigned int weight;
		unsigned int parent,lchild,rchild;
	};
	size_t nodesize;
	size_t treesize;
	HTNode * data;
	vector<HuffNode> _result;
	void select(unsigned int upbound,unsigned int *s1,unsigned int *s2);//在1-upbound中寻找parent为0，且weight最小的两个节点，s1,s1返回结果
	unsigned int max_index(unsigned int array[],size_t size);
	unsigned int max(unsigned int x,unsigned int y);
public:
	HuffmanTree(const InfoNode* nodes,size_t size); //nodes为节点的信息，size为节点个数
	HuffmanTree(const vector<InfoNode>& nodes);
	bool getcode(unsigned int id,BitVector& code); //code保存对应的id的编码，返回取编码的状态
	size_t saveresult(vector<HuffNode>& result);
	~HuffmanTree(void);
};
#endif //HUFFMANTREE_H
