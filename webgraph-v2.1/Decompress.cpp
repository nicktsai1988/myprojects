#include "Decompress.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>
using std::cerr;
using std::endl;
using std::vector;
DeCompress::DeCompress(const char* comp_graph_file)
{
	std::ifstream fin;
	fin.open(comp_graph_file,std::ios_base::in|std::ios_base::binary);
	if(!fin.is_open())
	{
		std::cerr<<"open compressed graph file "<<comp_graph_file<<" error."<<std::endl;
		exit(1);
	}
	unsigned int count,bitlength;
	fin.read((char*)&count,sizeof(count));
	nodecount=count;
	fin.read((char*)&bitlength,sizeof(bitlength));
	graph_vector.fill(fin,bitlength);
	cachesize=100;
	limit=0.5;
}


void DeCompress::init(const char* degree_file,const char* node_file,const char* postion_file)
{
	std::ifstream fin;
	vector<HuffNode> degrees;
	vector<HuffNode> nodes;
	HuffNode huffnode;
	unsigned int info;
	unsigned short bit_length;
	unsigned int size;
	fin.open(degree_file,std::ios_base::in|std::ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"open degree file "<<degree_file<<" error"<<endl;
		exit(1);
	}
	fin.read((char*)&size,sizeof(size));//记录个数
	for(unsigned int i=0;i<size;++i)
	{
		fin.read((char*)&info,sizeof(info));
		fin.read((char*)&bit_length,sizeof(bit_length));
		huffnode.info=info;
		if(huffnode.vector.fill(fin,bit_length) != bit_length)
		{
			cerr<<"Reading "<<degree_file<<",BitVector fill error"<<endl;
			exit(1);
		}
		degrees.push_back(huffnode);
	}
	degree_tree.init(degrees);//init degree tree
	fin.close();
	fin.open(node_file,std::ios_base::in|std::ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"open node file "<<node_file<<" error"<<endl;
		exit(1);
	}
	fin.read((char*)&size,sizeof(size));//记录个数
	for(unsigned int i=0;i<size;++i)
	{
		fin.read((char*)&info,sizeof(info));
		fin.read((char*)&bit_length,sizeof(bit_length));
		//char_length=(unsigned int)ceil(bit_length*1.0/8);
		huffnode.info=info;
		if(huffnode.vector.fill(fin,bit_length) != bit_length)
		{
			cerr<<"Reading "<<node_file<<",BitVector fill() error"<<endl;
			exit(1);
		}
		if( info == 700 )
			cerr<<"info= "<<info<<" vector.length= "<<huffnode.vector.length()<<endl;
		nodes.push_back(huffnode);
	}
	node_tree.init(nodes);//init node_tree
	fin.close();
	RefPosition pos;
	//填充一个位置便于，访问
	pos.id=0;
	pos.position=0;
	position.push_back(pos);
	fin.open(postion_file,std::ios_base::in|std::ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"open position file "<<postion_file<<" error"<<endl;
		exit(1);
	}
	fin.read((char*)&size,sizeof(size));//记录个数
	for(unsigned int i=0;i<size;++i)
	{
		fin.read((char*)&pos.id,sizeof(unsigned int));
		fin.read((char*)&pos.position,sizeof(unsigned int));
		position.push_back(pos); //add to position
	}
	fin.close();
}
void DeCompress::setparameter(unsigned int _cachesize,double _limit)
{
	cachesize=_cachesize;
	limit=_limit;
}
unsigned int DeCompress::size()const
{
	return nodecount;
}
int DeCompress::getlinks(unsigned int pageid,std::vector<unsigned int>& result)
{
	/* 
	cerr<<"page id= "<<pageid<<endl;
	cerr<<"position[pageid].id= "<<position[pageid].id<<endl;
	unsigned int myposition=position[pageid].position;
	cerr<<pageid<<" node's position is "<<myposition<<endl;
	unsigned int outdegree=degree_tree.decode(graph_vector,&myposition);
	cerr<<"myposition= "<<myposition<<endl;
	unsigned int reference=node_tree.decode(graph_vector,&myposition);
	cerr<<"myposition= "<<myposition<<endl;
	cerr<<"outdegree= "<<outdegree<<",reference= "<<reference<<endl;
	
	unsigned int ref_position=position[reference].position;
	unsigned int ref_outdegree=degree_tree.decode(graph_vector,&ref_position);
	BitVector bitvector=graph_vector.get_sub(myposition,ref_outdegree);
	cerr<<"reference outdegree= "<<ref_outdegree<<endl;
	unsigned int count=0;
	for(unsigned int i=0;i<bitvector.length();++i)
	{
		if(bitvector.test(i))
			++count;
	}
	cerr<<"the number of 1 in bitvector= "<<count<<endl;
	myposition+=ref_outdegree;
	cerr<<"the difference is: "<<endl;
	/*
	for(unsigned int i=0;i<(outdegree-count);++i)
	{
		cerr<<node_tree.decode(graph_vector,&myposition)<<" ";
	}
	cerr<<endl;
	*/
	int cacheindex;
	vector<DeCode> decodelist;
	DeCode onenode;
	unsigned int position,length=0;
	do
	{
		cacheindex=page_existed(pageid);
		if(cacheindex != -1)
			{
				onenode.nodeid=pageid;
				onenode.outdegree=cache[cacheindex].links.size();
				onenode.reference=0;
				onenode.differences.clear();
				onenode.links=cache[cacheindex].links;
				decodelist.push_back(onenode);
				break;
			}
		else
		{
			decodenode(pageid,onenode);
			decodelist.push_back(onenode);
			pageid=onenode.reference;
			length++;
		}
	}
	while(pageid != 0);
	//TODO::
//	cerr<<"decodelist.size= "<<decodelist.size()<<endl;	
	position=(length+1)/2;
	for(int i=decodelist.size()-1;i>=0;--i)
	{
		if(decodelist[i].reference != 0)
		{
			for(unsigned int j=0;j<decodelist[i].bitvector.length();++j)
			{
				if(decodelist[i].bitvector.test(j))
					decodelist[i].links.push_back(decodelist[i+1].links[j]);
			}
		}
		for(unsigned int j=0;j<decodelist[i].differences.size();++j)
			decodelist[i].links.push_back(decodelist[i].differences[j]);
		//decodelist[i].links.sort();
		std::sort(decodelist[i].links.begin(),decodelist[i].links.end());
		if(i == position)
			insertcache(decodelist[i]);//讲解码长度中点的节点插入cache中
	}
	result.clear();
	for(unsigned int j=0;j<decodelist[0].links.size();++j)
		result.push_back(decodelist[0].links[j]);
//	cerr<<"result size= "<<result.size()<<endl;

	return 0;
}
int DeCompress::decodenode(unsigned int pageid,DeCode& result)
{
	result.nodeid=pageid;
	unsigned myposition=position[pageid].position;
	result.outdegree=degree_tree.decode(graph_vector,&myposition);
	result.reference=node_tree.decode(graph_vector,&myposition);
	unsigned ref_position=position[result.reference].position;
	unsigned int ref_outdegree;
	if(result.reference == 0)
		ref_outdegree=0;
	else
	ref_outdegree=degree_tree.decode(graph_vector,&ref_position);

	result.bitvector=graph_vector.get_sub(myposition,ref_outdegree);
	myposition+=ref_outdegree;

	unsigned int i,count=0;
	for(i=0;i < result.bitvector.length();++i)
	{
		if(result.bitvector.test(i))
				++count;
	}
	result.differences.clear();
	for(i=0;i < (result.outdegree-count);++i)
		result.differences.push_back(node_tree.decode(graph_vector,&myposition));
	return 0;
}
		
void DeCompress::insertcache(const DeCode& newnode)
{
	CacheNode node;
	node.nodeid=newnode.nodeid;
	node.links=newnode.links;
	if(cache.size()<cachesize)//cache中的内容小于cachesize
	{
		cache.push_back(node);
		return;
	}
	unsigned int i,index=0;
	double value=0,temp;
	for(i=0;i<cachesize;++i)
	{
		temp=similarity(node,cache[i]);
		if(temp<limit)
		{
			if(temp>value)
			{
				value=temp;
				index=i;
			}//保存相似度小于limit,但是相似度又最大的点，用于替换
		}
		else//存在相似度>=limit的节点存在，则直接终止循环,
			return;
	}
	if(i == cachesize)//不存在相似度>=limit的点存在
	{
		cache[index]=node;
	}
}

int DeCompress::page_existed(unsigned int pageid)//此处可以使用二分查找，或者哈系表
{
	for(unsigned int i=0;i<cache.size();++i)
	{
		if(cache[i].nodeid == pageid)
			return i;
	}
	return -1;
}

