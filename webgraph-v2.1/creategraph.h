#ifndef CREATEGRAPH_H
#define CREATEGRAPH_H
#include "graph.h"
class CreateGraph
{
public:
	enum Format{ORIGIN,CONVERT};
    CreateGraph(unsigned int,unsigned int exist=0);
    CreateGraph(const char* filename,Format format);
	~CreateGraph();
    const WebGraph& instance();
private:
	struct NodeLink
	{
		vector<unsigned int> links;
	};
	void load_origin_graph(const char* filename);
	void load_convert_graph(const char* filename);
    void init();
    void create();
    bool have_existed(const unsigned int *links,const unsigned int size,const unsigned int element);
    unsigned int new_link(const WebNode& node,const unsigned int size);
    unsigned int new_link(const WebNode& node,const unsigned int size,unsigned int seed); //根据seed变异
    unsigned int link_count();
	unsigned int copy_id(unsigned int id);
    void copy_links(WebNode& node,const unsigned int limit); //limit为已经存在的页面的个数
    void sort();
    static int cmp(const void* x,const void* y);
    unsigned int exist;
	unsigned int range;
    WebGraph graph;
};

#endif // CREATEGRAPH_H
