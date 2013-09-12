#include"utils.h"
#include"creategraph.h"
#include<iostream>
using namespace std;
int main(int argc,char *argv[])
{
	if(argc == 1)
	{
		cerr<<argv[0]<<" graph.txt graph.graph"<<endl;
		return -1;
	}
	if(argc ==3)
	{
		CreateGraph creator(argv[1],CreateGraph::CONVERT);
		write_graph_to_file(argv[2],creator.instance());
		print_graph_info(creator.instance());
	}
	if(argc ==2)
	{
		CreateGraph creator(argv[1],CreateGraph::ORIGIN);
		print_graph_info(creator.instance());
	}
	return 0;
}

		
