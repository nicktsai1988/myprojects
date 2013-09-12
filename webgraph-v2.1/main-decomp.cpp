#include <iostream>
#include <cstdlib>
#include <climits>
#include <ctime>
#include "bitvector.h"
#include "DecodeTree.h"
#include "Decompress.h"
#include "graph.h"
//#include "HuffmanTree.h"
//#include "Compress.h"
#include "creategraph.h"
using namespace std;
void print_node_info(const WebNode& node)
{
	cerr<<"print_node_info():nodeid= "<<node.pageid<<" linkscount= "<<node.linkscount<<endl;
}
/*
void print_graph_info(const WebGraph &graph)
{
	unsigned int nodecount,edgecount,maxcount,minicount;
	nodecount=graph.nodecount-1;
	maxcount=edgecount=0;
	minicount=UINT_MAX;
	unsigned int i,j;
	for(i=1;i<graph.nodecount;i++)
	{
		edgecount+=graph.nodes[i].linkscount;
		if(graph.nodes[i].linkscount>maxcount)
			maxcount=graph.nodes[i].linkscount;
		if(graph.nodes[i].linkscount<minicount)
			minicount=graph.nodes[i].linkscount;
		for(j=1;j<graph.nodes[i].linkscount;j++)
		{
			if(graph.nodes[i].links[j-1]>graph.nodes[i].links[j])
				cerr<<"link error"<<endl;
		}
	}
	cerr<<"print_graph_info"<<endl;
	cerr<<"nodecout= "<<nodecount<<",edgecount= "<<edgecount<<endl;
	cerr<<"maxcount= "<<maxcount<<",minicount= "<<minicount<<endl;
	cerr<<"per-edge count= "<<edgecount/nodecount<<endl;
}
*/
void check_encode_bitvector(const WebNode& node,const WebNode& reference)
{
	EnCode encode;
	encode.nodeid=node.pageid;
	encode.reference=reference.pageid;
	bool state=encode_bitvector(node,reference,encode);
	if(!state)
	{
		cerr<<"check_encode_bitvector() error"<<endl;
		exit(1);
	}
	unsigned int count=0;
	for(unsigned int i=0;i<encode.bitvector.length();++i)
	{
		if(encode.bitvector.test(i))
			++count;
	}
	cerr<<"count= "<<count<<endl;
	cerr<<"difference size= "<<encode.differences.size()<<endl;
	cout<<"outdegree of node "<<node.pageid<<" = "<<node.linkscount<<endl;
	cout<<"outdegree of reference "<<reference.pageid<<" = "<<reference.linkscount<<endl;
}
int main(int argc,char* argv[])
{
	using namespace std;
	unsigned int nodes;
	clock_t begin,end;
	if(argc==1)
		nodes=1000;
	else
		nodes=atoi(argv[1]);
	
	const char* filename="test.graph";
	//cout<<"nodes= "<<nodes<<endl;
	//CreateGraph creator(nodes);
	//cerr<<"test.graph completed"<<endl;
	//CreateGraph webgraph(filename);
	//webgraph.write_to_file("test2.graph");
	CreateGraph creator(filename,CreateGraph::ORIGIN);
	
	const WebGraph& graph=creator.instance();
	//write_graph_to_file(filename,graph);
	/*
	//cerr<<"write test.graph completed"<<endl;
	begin=clock();
	Compress test(graph);
	end=clock();
	cerr<<"time of compress graph= "<<(end-begin)<<endl;
	unsigned int bitcount=test.write_to_file("test");
	unsigned int linkscount=0;
	for(unsigned int i=0;i<graph.nodecount;i++)
	{
		linkscount+=graph.nodes[i].linkscount;
	}
	cout<<"per edge "<<bitcount*1.0/linkscount<<endl;
*/
	DeCompress decompress("test.comp");
	decompress.init("test.degree","test.node","test.pos");
	WebGraph graph2;
	graph2.nodes=new WebNode[decompress.size()+1];
	graph2.nodecount=decompress.size()+1;
	graph2.nodes[0].pageid=0;
	graph2.nodes[0].links=NULL;
	graph2.nodes[0].linkscount=0;
	std::vector<unsigned int> links;
	
	unsigned int pageid =1;
	begin=clock();
	decompress.setparameter(100,0.2);
	cerr<<"start decode node"<<endl;
	while(pageid<=decompress.size())
	{
		decompress.getlinks(pageid,links);
	//	print_node_info(graph.nodes[pageid]);
		if(links.size() != graph.nodes[pageid].linkscount)
		{
			cerr<<"decode node ="<<pageid<<" error!"<<endl;
			cerr<<"links.size()="<<links.size()<<",node["<<pageid<<"].linkscount=" <<graph.nodes[pageid].linkscount<<endl;;
			exit(1);
		}
		graph2.nodes[pageid].pageid=pageid;
		graph2.nodes[pageid].linkscount=links.size();
		graph2.nodes[pageid].links=new unsigned int[links.size()];
		for(unsigned int i=0;i<links.size();i++)
		{
			if(links[i] != graph.nodes[pageid].links[i])
			{	
				cerr<<"pageid="<<pageid<<",link can't match"<<endl;
				exit(1);
			}
			graph2.nodes[pageid].links[i]=links[i];
		}
		pageid++;
	}
	cerr<<"decompress.nodecount= "<<decompress.size()<<endl;
	cerr<<"decode right"<<endl;
	end=clock();
	cerr<<"time of decompress graph= "<<(end-begin)<<endl;
	write_graph_to_file("test2.graph",graph2);
	cerr<<"write decompressed graph completed"<<endl;
	cout<<"completed"<<endl;
	return 0;
}

