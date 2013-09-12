#include "creategraph.h"
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime>
using std::cerr;
using std::endl;
using std::ofstream;
using std::ios_base;
using std::ifstream;
CreateGraph::CreateGraph(unsigned int nodes,unsigned int exist)
{
    graph.nodecount=nodes+1; //所有的节点加上根节点,节点0为根节点
    if(exist==0)
        this->exist=100+nodes/200;
    else
        this->exist=exist;
	range = (300 < this->exist)?300:(this->exist);
    srand(time(0));  //设置随机数种子
    init();
    create();
    sort();
}//指定节点个数和开始时存在的页面个数，默认开始存在的为节点个数的1/10

CreateGraph::~CreateGraph()
{

}
//初始化WEB图

CreateGraph::CreateGraph(const char *filename,Format format)
{
	if(format == ORIGIN)
	{
		load_origin_graph(filename);
		return;
	}
	if(format == CONVERT)
	{
		load_convert_graph(filename);
		return;
	}
	else
	{
		cerr<<"format argument error"<<endl;
		exit(1);
	}
}

void CreateGraph::load_origin_graph(const char* filename)
{
	ifstream fin;
	unsigned int nodecount,i;
	fin.open(filename,ios_base::in|ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"CreateGraph() error,cannot open "<<filename<<endl;
		exit(1);
	}
	fin.read((char*)&nodecount,sizeof(nodecount));
	graph.nodecount=nodecount+1;
	if((graph.nodes=new WebNode[graph.nodecount])==NULL)
	{
		cerr<<"new webnode error\n";
		exit(1);
	}
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
	graph.nodes[0].links=NULL;
	for(i=1;i<=(graph.nodecount-1);i++)
	{
		unsigned int linkscount;
		graph.nodes[i].pageid=i;
		fin.read((char*)&linkscount,sizeof(unsigned int));
		graph.nodes[i].linkscount=linkscount;
		if((graph.nodes[i].links=new unsigned int[linkscount])==NULL)
		{
			cerr<<"new links error\n";
			exit(1);
		}
		fin.read((char*)(graph.nodes[i].links),linkscount*sizeof(unsigned int));
	}
	fin.close();
}
void CreateGraph::load_convert_graph(const char* filename)
{
	ifstream fin;
	unsigned int from,to;
	unsigned int nodecount,edgecount;
	char buf[256];
	NodeLink *nodelink;
	fin.open(filename);
	if(!fin.is_open())
	{
		cerr<<"CreateGraph::load_convert_graph() error,cannot open "<<filename<<endl;
		exit(1);
	}
	fin.getline(buf,256);//# Directed graph (each unordered pair of nodes is saved once): web-Stanford.txt 
	fin.getline(buf,256);//# Stanford web graph from 2002
	fin>>buf;//#
	fin>>buf;//Nodes
	fin>>nodecount;//nodecount
	fin>>buf;//Edges
	fin>>edgecount;//edgecount
	fin.getline(buf,256);//'\n'
	fin.getline(buf,256);//# FromNodeId	ToNodeId
	if((nodelink = new NodeLink[nodecount+1]) == NULL)
	{
		cerr<<"new NodeLink error"<<endl;
		exit(1);
	}
	unsigned int temp=edgecount;
	while(temp--)
	{
		fin>>from>>to;
		nodelink[from].links.push_back(to);
	}
	//cerr<<"form= "<<from<<" to="<<to<<endl;
	fin.close();
	cerr<<"read text graph completed"<<endl;
	graph.nodecount=nodecount+1;
	if((graph.nodes=new WebNode[graph.nodecount])==NULL)
	{
		cerr<<"new webnode error\n";
		exit(1);
	}
	//unsigned int count=0;
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
	graph.nodes[0].links=NULL;
	for(unsigned int i=1;i<graph.nodecount;++i)
	{
		graph.nodes[i].pageid=i;
		graph.nodes[i].linkscount=nodelink[i].links.size();
		//count+=nodelink[i].links.size();
		if((graph.nodes[i].links = new unsigned int[graph.nodes[i].linkscount]) == NULL)
		{
			cerr<<"new links error"<<endl;
			exit(1);
		}
		for(unsigned int j=0;j<graph.nodes[i].linkscount;++j)
		{
			graph.nodes[i].links[j]=nodelink[i].links[j];
		}
	}
	delete[] nodelink;
	sort();
	std::cerr<<"nodecount= "<<nodecount<<" edgecount= "<<edgecount<<std::endl;
	//cerr<<"count= "<<count<<endl;
}

void CreateGraph::init() //分配空间和初始化
{
    unsigned int i;
    if((graph.nodes=new WebNode[graph.nodecount])==NULL)
    {
        cerr<<"CreateGraph init() error\n";
        exit(1);
    }
    for(i=0;i<graph.nodecount;i++)
        graph.nodes[i].links=NULL;
	graph.nodes[0].pageid=0;
	graph.nodes[0].linkscount=0;
}
//某条链接是否已经存在

bool CreateGraph::have_existed(const unsigned int *links, const unsigned int size,const unsigned int element)
{
    unsigned int i;
    for(i=0;i<size;i++)
    {
        if(links[i]==element)
            return true;
    }
    return false;
}

void CreateGraph::create()
{

    unsigned int i,j;
	//节点0为root节点
    for(i=1;i<=exist;i++)//先初始化存在的页面，然后再根据存在的页面生成其它页面,i从1开始
    {
        graph.nodes[i].pageid=i;//设置页面ID
        graph.nodes[i].linkscount=link_count();//链接个数为50~150
        if((graph.nodes[i].links=new unsigned int[graph.nodes[i].linkscount])==NULL)
        {
            std::cerr<<"creategraph create() error\n";
            exit(1);
        }//为每个页面的连接分配空间
        for(j=0;j<graph.nodes[i].linkscount;j++)//为每个页面分配链接
        {
            graph.nodes[i].links[j]=new_link(graph.nodes[i],j);
        }
    }
    for(i=exist+1;i <= (graph.nodecount-1);i++)//为其余的页面非配存储空间和链接
    {
        graph.nodes[i].pageid=i;
        graph.nodes[i].linkscount=link_count();
        if((graph.nodes[i].links=new unsigned int[graph.nodes[i].linkscount])==NULL)
        {
            std::cerr<<"creategraph create() error\n";
            exit(1);
        }//为每个页面的连接分配空间
        copy_links(graph.nodes[i],i);
    }
}

unsigned int CreateGraph::link_count()
{
    return (rand()%100+10);
}
unsigned int CreateGraph::new_link(const WebNode& node, const unsigned int size)//为初始节点分配链接
{
    unsigned int temp=(rand()%(graph.nodecount-1))+1; //分配ID为1~(exist-1)给链接
    while(have_existed(node.links,size,temp) || (temp == node.pageid))  //若不存在某个链接则返回，否则继续选择
    {
        temp=(rand()%(graph.nodecount-1))+1;
    }
    return temp;
}
unsigned int CreateGraph::new_link(const WebNode& node, const unsigned int size, unsigned int seed)//为后来的节点分配链接
{

    int temp1=rand()%100; //有80%的机会是原来的链接，20%会产生变化
    if(temp1 < 70)
        return seed;
    else
    {
        unsigned int temp2=node.pageid+(range/2)-(rand()%range); //取出去除节点ID为０的节点
        while(have_existed(node.links,size,temp2) || (temp2 == node.pageid))
        {
            temp2=node.pageid+(range/2)-(rand()%range);
        }
        return temp2;
    }
}

void CreateGraph::copy_links(WebNode &node, const unsigned int limit)
{
    unsigned int pageid,linkscount;
    unsigned int subcount;
    unsigned int count=0;
    unsigned int current;
    while(1)
    {
//        pageid=(rand()%(limit-1))+1; //用已经存在的页面生成新的页面,但是剔除ID为0的页面
		pageid=copy_id(limit);
        linkscount=graph.nodes[pageid].linkscount;
        subcount=(rand()%linkscount+1); //计算需要复制的链接个数
        if(subcount>(node.linkscount-count))
            subcount=(node.linkscount-count);
        if(subcount==0)
        {
            break;
        }
        for(unsigned int i=0;i<subcount;i++)
        {
            unsigned int temp=0;
            while(temp<100)
            {
                current=graph.nodes[pageid].links[rand()%linkscount];
				if(current == node.pageid)
					continue;
				if(have_existed(node.links,count,current))
					temp++;
				else
					break;
            }
            if(temp==100)//如果超过100次则换一个参考的节点
                break;
            else       //否则将该链接添加到新的页面中
            {
                node.links[count]=new_link(node,count,current);
                count++;
            }
        }
    }
}
void CreateGraph::sort()
{
    unsigned int i;
    for(i=1;i<graph.nodecount;i++)
    {
        qsort(graph.nodes[i].links,graph.nodes[i].linkscount,sizeof(unsigned int),cmp);
    }
}

int CreateGraph::cmp(const void* x, const void* y)
{
    return *(int *)x-*(int *)y;
}

const WebGraph& CreateGraph::instance()
{
    return graph;
}
unsigned int CreateGraph::copy_id(unsigned int id)
{
	unsigned ret=id-(rand()%range);
	while((ret == id)|| (ret == 0))
	{
		ret=id-(rand()%range);
	}
	return ret;
}




