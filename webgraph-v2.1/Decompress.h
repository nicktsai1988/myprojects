#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include "utils.h"
#include "bitvector.h"
#include "DecodeTree.h"
#include <vector>

class DeCompress
{
public:
	DeCompress(const char* comp_graph_file);
	void init(const char* degree_file,const char* node_file,const char* postion_file);
	void setparameter(unsigned int _cachesize=100,double _limit=0.5);
	unsigned int size()const;//节点id从1开始
	int getlinks(unsigned int pageid,std::vector<unsigned int>& result);//result to store the links of pageid node
private:
	struct DeCode
	{
		unsigned int nodeid;
		unsigned int outdegree;
		unsigned int reference;
		BitVector bitvector;
		std::vector<unsigned int> differences;
		std::vector<unsigned int> links;
	};
	/*
	struct CacheNode
	{
		unsigned int nodeid;
		std::vector<unsigned int> links;
	}
	*/
	int decodenode(unsigned int pageid,DeCode& result);
	void insertcache(const DeCode& newnode);
	int page_existed(unsigned int pageid);//不存在返回-1,存在则返回在cache中的index
	BitVector graph_vector;//compressed graph vector
	DecodeTree degree_tree;
	DecodeTree node_tree;
	std::vector<RefPosition> position;
	std::vector<CacheNode> cache;
	unsigned int nodecount;
	unsigned int cachesize;
	double limit;
//	int cacheindex;
};
#endif
