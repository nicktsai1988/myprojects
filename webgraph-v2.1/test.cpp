#include "utils.h"
#include <iostream>
#include <vector>
using std::cout;
using std::endl;
void print_bitvector(const BitVector& vector)
{
	unsigned int i;
	for(i=0;i<vector.length();++i)
	{
		if(vector.test(i))
			cout<<1;
		else
			cout<<0;
	}
	cout<<endl;
}
void print_vector_pair(const vector<Pair>& vector)
{
	unsigned int i;
	for(i=0;i<vector.size();++i)
	{
		cout<<pair_to_number(vector[i]);
	}
	cout<<endl;
}
void print_vector_uint(const vector<unsigned int>& vector)
{
	unsigned int i;
	for(i=0;i<vector.size();++i)
	{
		cout<<vector[i]<<" ";
	}
	cout<<endl;
}
void print_node_encoding(const NodeEncoding& encoding)
{
	cout<<"interval cout= "<<encoding.intervals.size()<<endl;
	cout<<"start_flag= ";
	if(encoding.start_flag)
		cout<<1<<endl;
	else
		cout<<0<<endl;
	for(unsigned int i=0;i<encoding.intervals.size();++i)
	{
		cout<<"start= "<<pair_to_number(encoding.intervals[i].start);
		cout<<",length= "<<encoding.intervals[i].length;
		cout<<endl;
	}
	for(unsigned int i=0;i<encoding.rest_links.size();++i)
		cout<<(encoding.rest_links[i])<<" ";
	cout<<endl;
}
void test_gamma_encoding()
{
	print_bitvector(gamma_encoding(13));
	print_bitvector(gamma_encoding(1));
	print_bitvector(gamma_encoding(16));
}
void test_block_encodind()
{
	NodeEncoding result;
	block_encoding(gamma_encoding(13),result);
	print_vector_uint(result.blocks);
	block_encoding(gamma_encoding(1),result);
	print_vector_uint(result.blocks);
	block_encoding(gamma_encoding(16),result);
	print_vector_uint(result.blocks);
}
void test_interval_encoding()
{
	std::vector<unsigned int> diffs;
	NodeEncoding encoding;
	diffs.push_back(13);
	//diffs.push_back(14);
	diffs.push_back(15);
	//diffs.push_back(16);
	diffs.push_back(17);
	//diffs.push_back(18);
	diffs.push_back(19);
	
	diffs.push_back(23);
	//diffs.push_back(24);
	diffs.push_back(203);
	diffs.push_back(315);
	//diffs.push_back(316);
	
	encoding.nodeid=15;
	interval_encoding(diffs,encoding);
	print_node_encoding(encoding);

	std::vector<unsigned int> diffs2;
	interval_encoding(diffs2,encoding);
	print_node_encoding(encoding);
}

void print_pair(const Pair& p)
{
	cout<<"multiplier="<<p.multiplier<<",remainer="<<p.remainer<<endl;
}

void test_number_to_pair()
{
	Pair p;
	p=number_to_pair(65537);
	print_pair(p);
	p=number_to_pair(128);
	print_pair(p);
	p=number_to_pair(65536);
	print_pair(p);
	p=number_to_pair(0);
	print_pair(p);
}
void test_pair_to_number()
{
	cout<<pair_to_number(number_to_pair(65537))<<endl;
	cout<<pair_to_number(number_to_pair(65536))<<endl;
	cout<<pair_to_number(number_to_pair(128))<<endl;
	cout<<pair_to_number(number_to_pair(0))<<endl;
}
void test_big_to_little_encoding()
{
	unsigned int array1[10]={5,3,2,10,16,65537,2,5,6};
	std::vector<unsigned int> lists,result,length;
	lists.assign(&array1[0],&array1[9]);
	big_to_little_encoding(result,length,lists);
	print_vector_uint(lists);
	print_vector_uint(length);
	print_vector_uint(result);
}
