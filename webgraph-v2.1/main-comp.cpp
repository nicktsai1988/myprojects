#include <iostream>
#include <cstdlib>
#include <climits>
#include <ctime>
#include "bitvector.h"
//#include "DecodeTree.h"
//#include "Decompress.h"
#include "graph.h"
#include "HuffmanTree.h"
#include "Compress.h"
#include "creategraph.h"
#include "test.h"
using namespace std;
void print_node_info(const WebNode& node)
{
	cerr<<"print_node_info():nodeid= "<<node.pageid<<" linkscount= "<<node.linkscount<<endl;
}
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
	clock_t begin,end;
	// test function
	test_gamma_encoding();
	test_block_encodind();
	test_interval_encoding();
	test_number_to_pair();
	test_pair_to_number();
	test_big_to_little_encoding();
	// test function end
	if(argc != 3)
	{
		cerr<<argv[0]<<" graphfile out-name"<<endl;
	//	cin.get();
		return 1;
	}
	cerr<<endl<<argv[0]<<" "<<argv[1]<<" "<<argv[2]<<endl;
	CreateGraph creator(argv[1],CreateGraph::ORIGIN);
	const WebGraph& graph=creator.instance();
	print_graph_info(graph);	
	begin=clock();
	Compress test(graph);
	end=clock();
	cerr<<"time of compress graph= "<<(end-begin)<<endl;
	unsigned int bitcount=test.write_to_file(argv[2]);
	unsigned int linkscount=0;
	for(unsigned int i=0;i<graph.nodecount;i++)
	{
		linkscount+=graph.nodes[i].linkscount;
	}
	cerr<<"per edge "<<bitcount*1.0/linkscount<<endl;
	cout<<"per edge "<<bitcount*1.0/linkscount<<endl;
	cout<<"completed"<<endl;
	cerr<<"completed"<<endl;
	cerr<<endl;
	return 0;
}

