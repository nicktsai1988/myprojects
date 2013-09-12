#include "utils.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "graph.h"
#include <climits>
bool encode_bitvector(const WebNode& node,const WebNode& reference_node,EnCode & code)
	//以reference为参考节点编码node，结果存于code之中
{
	if(node.pageid!=code.nodeid)
		return false;
	else
	{
		unsigned int i,j,k;
		code.differences.clear();
		BitVector bitvector(reference_node.linkscount);
		code.reference=reference_node.pageid;
		code.nodeid=node.pageid;

		for(i=0,j=0;i<node.linkscount;i++)
		{
			while((j<reference_node.linkscount)&&(node.links[i]>reference_node.links[j]))
				bitvector.clr(j++);
			if(j==reference_node.linkscount)//参考节点游标已经到末尾
			{
				for(k=i;k<node.linkscount;k++)//将node中多余的连接加到difference部分
					code.differences.push_back(node.links[k]);
				break;
			}
			if(node.links[i]==reference_node.links[j]) //如果相等
				bitvector.set(j++);						//则将该位置1
			else
				code.differences.push_back(node.links[i]); //小于，则加入difference
		}
		for(k=j;k<reference_node.linkscount;k++)//参考节点还未到末尾
			bitvector.clr(k);
		code.bitvector=bitvector;
		return true;
	}	
}

const BitVector* find(const std::vector<HuffNode> &huffcode,unsigned int key)//可以修改提高效率
{
	for(unsigned int i=0;i<huffcode.size();i++)
	{
		if(huffcode[i].info == key)
			return &(huffcode[i].vector);
	}
		return NULL;
}

void encode_diffvector(const std::vector<HuffNode>& huffcode,EnCode &code)
{
	unsigned int i;
	code.diffvector.reset();
	for(i=0;i<code.differences.size();i++)
	{
		const BitVector* bits=find(huffcode,code.differences[i]);
		if(bits != NULL)
			code.diffvector.append(*bits);
		else
		{
			std::cerr<<"encode_diffvector error"<<std::endl;
			exit(1);
		}
	}
}

unsigned int difference(const WebNode& node1,const WebNode& node2)//存在于node1却不存在于node2中的链接个数
{
	unsigned int count=0;
	unsigned int i,j;
	for(i=0,j=0;i<node1.linkscount;i++)
	{
		while((j<node2.linkscount)&&(node1.links[i]>node2.links[j]))
			j++;
		if(j==node2.linkscount) //node2长度比较短，已到尾部
		{
			count+=(node1.linkscount-i);
			break;
		}
		if(node1.links[i]<node2.links[j])
			count++;
	}
	return count;
}
/*  
static double log2(double x)
{
	return log(x)/log(2.0);
}
*/
unsigned int cost(const WebNode& node1,const WebNode& node2,unsigned int nodecount)
{
	 return node2.linkscount+((unsigned int)(ceil(log2((double)nodecount))))*(difference(node1,node2)+1);
}//node2作为参考编码编码node1的开销
void write_graph_to_file(const char* filename,const WebGraph& graph)
{
	unsigned int i;
	std::ofstream fout;
	unsigned int count=graph.nodecount-1;//剔除根节点,节点ID从1开始到count
    fout.open(filename,std::ios_base::out|std::ios_base::binary);
    if(!fout.is_open())
    {
		std::cerr<<"open file "<<filename<<" error\n";
        exit(1);
    }
    fout.write((char*)&(count),sizeof(unsigned int));  //保存顶点个数
    for(i=1;i<graph.nodecount;++i)
    {
        //按照顺序保存
        fout.write((char*)&(graph.nodes[i].linkscount),sizeof(unsigned int)); //保存顶点的链接个数
        fout.write((char*)(graph.nodes[i].links),sizeof(unsigned int)*(graph.nodes[i].linkscount));  //保存顶点的链接
    }
    fout.close();
}
double similarity(const CacheNode& node,const CacheNode& reference)
{//node与reference中同时存在的节点个数除以reference中的节点总数
	unsigned int i,j;
	double count=0;
	for(i=0,j=0;i<node.links.size();++i)
	{
		while(j<reference.links.size() && node.links[i] > reference.links[j])
		{
			j++;
		}
		if(j == reference.links.size())
		{
			break;
		}
		if(node.links[i] == reference.links[j])//等于
		{
			j++;
			count++;
		}
	}
	return count/(double)reference.links.size();
}
void print_graph_info(const WebGraph &graph)
{
	using namespace std;
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
	cerr<<"per-edge count= "<<(double)edgecount/nodecount<<endl;
}
/*  
static unsigned int conv(int x)
{
	return (x<0)?(abs(x)*2-1):(abs(x)*2);
}
*/
int gap_encoding(EnCode& encode)
{
	unsigned int i,temp,mid;
	unsigned int size=encode.differences.size();
	
	encode.reference = abs(int(encode.reference-encode.nodeid));	
	if(size == 0)
	{
		return 0;
	}

	mid=size/2;
	temp=encode.differences[mid];

	for(i=size-1;i>mid;--i)
	{
		encode.differences[i] = encode.differences[i]-encode.differences[i-1];
	}

	for(i=0;i<mid;++i)
	{
		encode.differences[i] = encode.differences[i+1]-encode.differences[i];
	}
	encode.differences[mid]=abs(int(encode.differences[mid]-encode.nodeid));
	return temp < encode.nodeid; 
}
int find_ref(const WebGraph& graph,const unsigned int win,std::vector<RefLink>& result)
{
	std::cerr<<"window="<<win<<std::endl;
	unsigned int i,j,k,w;
	unsigned int limit,min,index;
	result.clear();
	result.resize(graph.nodecount);
	result[0].source=0;
	result[0].target=0;
	unsigned int *weight=new unsigned int[win];
	for(i=1;i<graph.nodecount;++i)
	{
		result[i].target=i;
		w=(i>win)?win:i;
		limit=cost(graph.nodes[i],graph.nodes[0],graph.nodecount);
		for(j=0;j<w;++j)
		{
			weight[j]=cost(graph.nodes[i],graph.nodes[i-w+j],graph.nodecount);
		}
		min=weight[0];
		index=0;
		for(k=1;k<w;++k)
		{
			if(weight[k]<min)
			{
				min=weight[k];
				index=k;
			}
		}
		if(min >= limit)
		{
			result[i].source=0;
		}
		else
		{
			result[i].source=i+index-w;
		}
	}
	delete[] weight;
	return 0;
}
BitVector gamma_encoding(unsigned int x)
{
	unsigned int len=floor(log2(x));
	unsigned int i;
	BitVector vector(len*2+1);
	for(i=0;i<len;++i)
		vector.clr(i);
	for(i=len;i<=len*2;++i)
	{
		(x&(1<<(len*2-i)))?vector.set(i):vector.clr(i);
	}
	return vector;
}
int block_encoding(const BitVector& vector,NodeEncoding &result)
{
	unsigned int i,length=vector.length(),count=0;
	result.blocks.clear();
	if(!length)
		return -1;
	bool pre=vector.test(0);
	count=1;
	for(i=1;i<length;++i)
	{
		if(vector.test(i) == pre)
			count++;
		else
		{
			pre=vector.test(i);
			result.blocks.push_back(count);
			count=1;
		}
	}
	result.block_flag=(vector.test(0)?true:false);
	return 0;
}
int interval_encoding(const std::vector<unsigned int>& lists,NodeEncoding& result)
{
	unsigned int i,length;
	unsigned int start,pre,pre_end;
	bool first=true;
	result.start_flag=true;
	result.interval_flag=false;
	result.intervals.clear();
	result.rest_links.clear();
	if(!lists.size())
	{
		return -1;
	}
	start=pre=lists[0];
	pre_end=result.nodeid;
	length=1;
	for(i=1;i<lists.size();++i)
	{
		if(lists[i] ==  (pre+1))
		{
			length++;
			pre=lists[i];
		}
		else
		{
			if(length > 1)
			{
				result.interval_flag=true;
				Interval intv;
				intv.length=length-1;
				if(first)
				{
					intv.start=number_to_pair((start>=pre_end)?(start-pre_end):(pre_end-start));
					result.start_flag=(start>=pre_end)?true:false;
					first=false;
				}
				else
					intv.start=number_to_pair(start-pre_end-1);
				result.intervals.push_back(intv);
				pre_end=lists[i-1];
			}
			else
			{
				result.temp_rest_links.push_back(start);
			}
			length=1;
			start=pre=lists[i];
		}
	}
	if(length  == 1)
		result.temp_rest_links.push_back(start);
	else
	{
		result.interval_flag=true;
		Interval intv;
		intv.length=length-1;
		if(first)
		{
			intv.start=number_to_pair((start>=pre_end)?(start-pre_end):(pre_end-start));
			result.start_flag=(start>=pre_end)?true:false;
			first=false;
		}
		else
			intv.start=number_to_pair(start-pre_end-1);
		result.intervals.push_back(intv);
	}
	return 0;
}
 int rest_links_encoding(NodeEncoding& encoding)
 {

	 unsigned int nodeid=encoding.nodeid;
	 if(!encoding.temp_rest_links.size())
		 return -1;
	 for(unsigned int i=encoding.temp_rest_links.size()-1;i>0;--i)
	 {
		 encoding.temp_rest_links[i]=encoding.temp_rest_links[i]-encoding.temp_rest_links[i-1]-2;
		 //encoding.rest_links[i]=number_to_pair(encoding.temp_rest_links[i]);
	 }
	 encoding.rest_flag=(encoding.temp_rest_links[0]>=nodeid)?true:false;
	 encoding.temp_rest_links[0]=(encoding.temp_rest_links[0]>=nodeid)?(encoding.temp_rest_links[0]-nodeid):(nodeid-encoding.temp_rest_links[0]);
	 encoding.rest_first_flag=(encoding.temp_rest_links[0]>0xFFFF)?true:false;
	 big_to_little_encoding(encoding.rest_links,encoding.rest_links_length,encoding.temp_rest_links);
	 /* 
	 encoding.rest_links[0]=number_to_pair(encoding.temp_rest_links[0]);
	 unsigned int count=0;
	 for(unsigned int i=0;i<encoding.rest_links.size();++i)
	 {
		 if(encoding.rest_links[i].multiplier > 0)
		 {
			 count++;
		 }
	 }
	 */
	 //encoding.big_rest=count;
	 encoding.temp_rest_links.clear();
	 return 0;
 }

 unsigned int pair_to_number(const Pair &p)
 {
	 unsigned int num;
	 num=p.multiplier<<16;
	 num|=p.remainer;
	 return num;
 }
 Pair number_to_pair(unsigned int x)
 {
	 Pair p;
	 p.multiplier=x>>16;
	 p.remainer=x&0x0000FFFF;
	 return p;
 }
int big_to_little_encoding(std::vector<unsigned int>& result,std::vector<unsigned int>&length,const std::vector<unsigned int>& lists)
{
	unsigned int count=0,size=lists.size();
	unsigned int start,end;
	if(!size)
		return -1;
	result.clear();
	length.clear();
	bool cur,pre=(lists[0]<=0XFFFF)?true:false;
	count=1;
	start=0;
	size++;//加1是为了方便gamma编码
	for(unsigned int i=1;i<lists.size();++i)
	{
		cur=(lists[i]<=0XFFFF)?true:false;
		if(cur == pre)
			count++;
		else
		{
			end=i-1;
			length.push_back(count);
			if(lists[start]<=0XFFFF)
			{
				for(unsigned int j=start;j<=end;++j)
				{
					result.push_back(lists[j]);	
				}
			}
			else
			{
				for(unsigned int j=start;j<=end;++j)
				{
					Pair p;
					p=number_to_pair(lists[j]);
					result.push_back(p.multiplier);
					result.push_back(p.remainer);
				}

			}
			//size-=count;
			pre=cur;
			count = 1;
			start=i;
		}
	}
	end=lists.size()-1;
	length.push_back(count);
	if(lists[start]<=0XFFFF)
	{
		for(unsigned int j=start;j<=end;++j)
		{
			result.push_back(lists[j]);	
		}
	}
	else
	{
		for(unsigned int j=start;j<=end;++j)
		{
			Pair p;
			p=number_to_pair(lists[j]);
			result.push_back(p.multiplier);
			result.push_back(p.remainer);
		}

	}
	return 0;
}



