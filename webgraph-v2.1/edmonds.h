#ifndef EDMONDS_H
#define EDMONDS_H
#include <vector>
#include <climits>
#include "matrix.h"
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "./src/edmonds_optimum_branching.hpp"
using std::vector;
#include "utils.h"

bool is_cyclic(const vector<RefLink>& result,unsigned int *root);
class Edmonds
{
public:
	typedef boost::property<boost::edge_weight_t, unsigned int>  EdgeProperty;
	typedef boost::adjacency_list<boost::vecS,
								boost::vecS,
								boost::directedS,
								boost::no_property,
								EdgeProperty>              Graph;
	typedef boost::graph_traits<Graph>::vertex_descriptor       Vertex;
	typedef boost::graph_traits<Graph>::vertex_iterator			Vertex_iterator;
	typedef boost::graph_traits<Graph>::edge_descriptor         Edge;
	
	Edmonds(unsigned int nodes);
	//void init(const vector<vector<unsigned int> >& matrix);  //用邻接矩阵初始化图
	void init(const Matrix<unsigned int>& matrix);   //用矩阵初始化图
	void init(const vector<RefLink>& links);        //用邻接序列初始化图
	void find(unsigned int *root); //单根节点的edmonds算法
	const unsigned int minicount();   //返回最小树形图的权值
	void getresult(std::vector<RefLink>* links);   //用links保存找到的最小树形图
private:
	std::vector<Edge> result;  //最小树形图的结果
	unsigned int nodes; //节点个数
	Graph G;         //图
	boost::property_map<Graph, boost::edge_weight_t>::type weights;
	boost::property_map<Graph, boost::vertex_index_t>::type vertex_indices;
	unsigned int count;
};
#endif
