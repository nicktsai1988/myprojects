#include "edmonds.h"
Edmonds::Edmonds(unsigned int nodes):nodes(nodes),G(nodes)
{
	count=0;
}
/*
void Edmonds::init(const vector<vector<unsigned int> >& matrix) //邻接矩阵接口
{
	vector<Edmonds::Vertex> the_vertices;
	BOOST_FOREACH(Vertex v,vertices(G))
	{
		the_vertices.push_back(v);
	}
	for(unsigned int i=0;i<nodes;i++)
		for(unsigned int j=0;j<nodes;j++)
		{
			if(matrix[i][j]!=UINT_MAX)
				add_edge(the_vertices[i],the_vertices[j],matrix[i][j],G);
		}
	weights=get(boost::edge_weight_t(),G);
	vertex_indices=get(boost::vertex_index_t(),G);
}
*/
void Edmonds::init(const Matrix<unsigned int>& matrix)
{
	vector<Edmonds::Vertex> the_vertices;
	BOOST_FOREACH(Vertex v,vertices(G))
	{
		the_vertices.push_back(v);
	}
	for(unsigned int i=0;i<nodes;i++)
		for(unsigned int j=0;j<nodes;j++)
		{
			if(matrix[i][j]!=UINT_MAX)
				add_edge(the_vertices[i],the_vertices[j],matrix[i][j],G);
		}
	weights=get(boost::edge_weight_t(),G);
	vertex_indices=get(boost::vertex_index_t(),G);
}
void Edmonds::init(const vector<RefLink>& links)   //邻接表接口
{
	Edmonds::Vertex_iterator begin=vertices(G).first;
	for(unsigned int i=0;i<links.size();i++)
	{
		Edmonds::Vertex_iterator source=begin+links[i].source;
		Edmonds::Vertex_iterator target=begin+links[i].target;
		add_edge(*source,*target,links[i].weight,G);
	}
}

void Edmonds::find(unsigned int *root)
{
	result.clear();
	edmonds_optimum_branching<false, true, false>(G,
												vertex_indices,
												weights,
												static_cast<Vertex *>(root),
												static_cast<Vertex *>(root+1),
												std::back_inserter(result));
	count=0;
	BOOST_FOREACH (Edge e, result)
	{
		count+=get(weights,e);
	}

}

void Edmonds::getresult(std::vector<RefLink>* links)
{
	links->clear();
	links->resize(nodes);
	RefLink newlink;
	BOOST_FOREACH (Edge e, result)
	{
		newlink.target=boost::target(e,G);
		newlink.source=boost::source(e,G);
		newlink.weight=get(weights,e);
		(*links)[newlink.target]=newlink;
	}
}

const unsigned int Edmonds::minicount()
{
	return count;
}

bool is_cyclic(const vector<RefLink>& result,unsigned int* root)
{
	unsigned size = result.size();
	vector<bool> visited(size, false);

	for (unsigned int i = 0; i < size; ++i)
	{
		if (visited[i])  //环上的一点若已经检查过，则不检查
			continue;

		vector<bool> visited_this_iteration(size, false);
		visited[i] = true;
		visited_this_iteration[i] = true;
		unsigned int v = i;
		while (result[v].source != i)       //要么碰到环，要么碰到已经检查过的节点
		{
			v = result[v].source;              
			if (visited_this_iteration[v])
				return true;
			visited_this_iteration[v] = true;
			if (visited[v]) //环上的一点若已经检查过，则不检查
				break;
		}
		if(result[v].source==i&&result[v].source!=*root)
			return true;
	}
	return false;
}

