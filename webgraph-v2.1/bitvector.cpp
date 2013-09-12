#include "bitvector.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
using std::endl;
using std::cerr;
using std::ofstream;
using std::ios_base;
using std::ostream;
BitVector::BitVector(unsigned int length)
{
	bit_length=length;
	char_length=(length/8+1);
	if((bit_vector=new unsigned char[char_length])==NULL)
	{
		cerr<<"construct BitVectot new[] error"<<endl;
		exit(1);
	}
}
/*
BitVector::BitVector(const char* filename)
{
	std::ifstream fin;
	fin.open(filename,ios_base::in|ios_base::binary);
	if(!fin.is_open())
	{
		cerr<<"construct BitVector open graph file "<<filename<<" error!"<<endl;
		exit(1);
	}
	fin.read((char*)&bit_length,sizeof(unsigned int));
	char_length=(unsigned int)ceil(bit_length*1.0/8);
	if((bit_vector=new unsigned char[char_length]) == NULL)
	{
		cerr<<"construct BitVector new unsigned char[char_length] error"<<endl;
		exit(1);
	}
	fin.read((char*)bit_vector,char_length);
	fin.close();
}

BitVector::BitVector(const unsigned char *data,size_t bitlength)
{


}
*/
BitVector::BitVector(const vector<char> &list)
{
    unsigned int i;
    bit_length=list.size();
    char_length=(bit_length/8+1)*3/2;
    if((bit_vector=new unsigned char[char_length])==NULL)
    {
        cerr<<"construct BitVectot new[] error"<<endl;
        exit(1);
    }
    for(i=0;i<list.size();i++)
    {
        (list[i]=='1')?set(i):clr(i);
    }
}

BitVector::BitVector(const char *list,size_t size)
{
	unsigned int i;
	bit_length=size;
	char_length=(bit_length/8+1)*3/2;
	if((bit_vector=new unsigned char[char_length])==NULL)
	{
		cerr<<"construct BitVectot new[] error"<<endl;
		exit(1);
	}
	for(i=0;i<size;i++)
	{
		(list[i]=='1')?set(i):clr(i);
	}
}
BitVector::BitVector(const BitVector &bits)
{
    bit_length=bits.bit_length;
    char_length=(bits.bit_length/8+1);
    if((bit_vector=new unsigned char[char_length])==NULL)
    {
        cerr<<"copy construct BitVector new[] error"<<endl;
        exit(1);
    }
    memcpy(bit_vector,bits.bit_vector,char_length);
}
BitVector& BitVector::operator=(const BitVector &bits)
{
    if(this==&bits)
        return *this;
    if(bit_vector!=NULL)
        delete[] bit_vector;
    bit_length=bits.bit_length;
    char_length=bits.char_length;
    if((bit_vector=new unsigned char[char_length])==NULL)
    {
        cerr<<"operator= BitVector new[] error"<<endl;
        exit(1);
    }
    memcpy(bit_vector,bits.bit_vector,char_length);
    return *this;
}
BitVector::~BitVector()
{
	if(bit_vector!=NULL)
		delete[] bit_vector;
}

BitVector& BitVector::append(const vector<char> &list)
{
    unsigned int i;
    if((bit_length+list.size())>=(char_length*8))
        adjust_size(bit_length+list.size());
    for(i=0;i<list.size();i++)
    {
        (list[i]=='1')?set(bit_length++):clr(bit_length++);
    }
	return *this;
}
BitVector& BitVector::append(const char* list,size_t size)
{
	unsigned int i;
	if((bit_length+size)>=(char_length*8))
		adjust_size(bit_length+size);
	for(i=0;i<size;i++)
	{
		(list[i]=='1')?set(bit_length++):clr(bit_length++);
	}
	return *this;

}
BitVector& BitVector::append(const BitVector &bits)
{
    unsigned int i;
    if((bit_length+bits.length())>=(char_length*8))
        adjust_size(bit_length+bits.length());
    for(i=0;i<bits.length();++i)
    {
        bits.test(i)?set(bit_length++):clr(bit_length++);
    }
	return *this;
}
BitVector& BitVector::append(bool bit)
{
	if((bit_length+1)>=(char_length*8))
		adjust_size(bit_length+1);
	bit?set(bit_length++):clr(bit_length++);

	return *this;
}
void BitVector::set(unsigned int index) //位序号从0开始
{
    if(index>=bit_length)
    {
        cerr<<"set bit overflow"<<endl;
        return;
    }
    bit_vector[index>>3]|=(1<<(7-(index&0x07)));
}
void BitVector::clr(unsigned int index)
{
    if(index>=bit_length)
    {
        cerr<<"clr bit overflow"<<endl;
        return;
    }
    bit_vector[index>>3]&=~(1<<(7-(index&0x07)));
}

bool BitVector::test(unsigned int index)const
{
    if(index>=bit_length)
    {
        cerr<<"test bit overflow"<<endl;
        return false;
    }
    return bit_vector[index>>3]&(1<<(7-(index&0x07)));
}
void BitVector::reset()
{
	if(bit_length==0)
		return;
	delete[] bit_vector;
	bit_vector=NULL;
	bit_length=0;
	char_length=(bit_length/8+1);
	if((bit_vector=new unsigned char[char_length])==NULL)
	{
		cerr<<"reset BitVectot new[] error"<<endl;
		exit(1);
	}
}

int BitVector::fill(std::ifstream& fin,size_t bitlength)
{
	if(fin.eof())
		return -1;
	if(fin.bad())
		return -1;
	if(bit_vector != NULL)
		delete[] bit_vector;
	bit_length=bitlength;
	char_length=(unsigned int)ceil(bitlength*1.0/8);
	bit_vector=new unsigned char[char_length];
	if(bit_vector == NULL)
	{
		cerr<<"fill() new unsigned char error"<<endl;
		exit(1);
	}
	fin.read((char*)bit_vector,char_length);
	return bitlength;
}

unsigned int BitVector::length() const
{
    return bit_length;
}

void BitVector::adjust_size(const unsigned int size)
{
    if(size<(char_length*8))
        return;
    unsigned int length=(size/8+1)*3/2;
    unsigned char *temp;
    if((temp=new unsigned char[length])==NULL)
    {
        cerr<<"adjust_size error"<<endl;
        exit(1);
    }
    memcpy(temp,bit_vector,char_length);
    char_length=length;
    if(bit_vector!=NULL)
        delete[] bit_vector;
    bit_vector=temp;
    return;
}

BitVector BitVector::get_sub(unsigned int position,unsigned int bitlength)
{
	unsigned int i;
	BitVector vector(bitlength);
	//vector<char> links;
	for(i=0;i<bitlength;++i)
		test(position++)?vector.set(i):vector.clr(i);
	return vector;
}
/*
unsigned int BitVector::write_to_file(const char*filename)//进攻测试
{
	ofstream fout;
	fout.open(filename,ios_base::out|ios_base::binary);
	if(!fout.is_open())
	{
		cerr<<"BitVector write_to_file() error"<<endl;
		exit(1);
	}
	unsigned int length=ceil(bit_length*1.0/8);
	fout.write((char*)bit_vector,length);
	fout.close();
	return bit_length;
}
*/
/*
ostream& BitVector::write(ostream& out)
{
	unsigned int length=ceil(bit_length*1.0/8);
	out.write((char*)bit_vector,length);
	return out;
}
*/
ostream& operator<<(ostream& out,const BitVector& vector)
{
	unsigned int length=(unsigned int)ceil(vector.bit_length*1.0/8);
	//out.write((char*)&length,sizeof(length));
	out.write((char*)vector.bit_vector,length);
	return out;
}

