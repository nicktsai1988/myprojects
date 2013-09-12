#include "graph.h"
#include <cstdlib>
WebGraph::WebGraph()
{
    nodes=NULL;
    nodecount=0;
}
WebGraph::~WebGraph()
{
    unsigned int i;
    if(nodecount!=0)
    {
        for(i=0;i<nodecount;i++)
        {
            if(nodes[i].links!=NULL)
                delete[] nodes[i].links;
        }
        delete[] nodes;
    }
}
