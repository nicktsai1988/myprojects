/*
 * =====================================================================================
 *
 *       Filename:  main-create.cpp
 *
 *    Description:  create random webgraph
 *
 *        Version:  1.0
 *        Created:  08/15/2013 04:54:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nick Tsai (hi.baidu.com/nicktsai), nicktsai@163.com
 *        Company:  Xidian University
 *
 * =====================================================================================
 */
#include "creategraph.h"
#include "utils.h"
#include <iostream>
#include <cstdlib>
int main(int argc ,char * argv[])
{
	using namespace std;
	if(argc != 3)
	{
		cerr<<argv[0]<<" nodecount filename"<<endl;
		return 1;
	}
	unsigned int nodes=atoi(argv[1]);
	CreateGraph creator(nodes);
	print_graph_info(creator.instance());
	write_graph_to_file(argv[2],creator.instance());
	return 0;
}
