#ifndef COMPRESS_H
#define COMPRESS_H
#include <vector>
#include "utils.h"
#include "graph.h"
class Compress
{
public:
	Compress(const WebGraph& graph);
	unsigned int write_to_file(const char* name); //返回写入的bit数
	~Compress(void);
private:
	void find_reference();
	void encode_nodesvector();
	void encode_outdegree();
	void create_result();
	void write_out_degrees(const char* name);
	void write_node_vector(const char* name);
	void write_position(const char* name);
	unsigned int write_result(const char* name);
	const WebGraph& _graph;
	vector<RefLink> _reference;
	vector<HuffNode> _outdegrees;
	vector<HuffNode> _nodesvector;
	vector<EnCode> _encodes;
	vector<RefPosition> _position;//每个节点在result中的位置
	vector<NodeEncoding> _nodes;
	BitVector result;
};
#endif

