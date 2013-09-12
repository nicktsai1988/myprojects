#ifndef MATRIX_H
#define MATRIX_H
#include<cstdlib>
#include<iostream>
using std::cerr;
using std::endl;
template<class T>
class Matrix
{
public:
	typedef T value_type;
	Matrix(size_t m,size_t n);
	Matrix(size_t m,size_t n,const T& element);
	Matrix(const Matrix& other);
	Matrix& operator=(const Matrix& other);
	~Matrix();
	const size_t row()const;
	const size_t col()const;
	T* operator[](size_t index);
	const T* operator[](size_t index)const;
private:
	void init(const T& element);
	size_t _row;
	size_t _col;
	T* data;
};
template<class T>
Matrix<T>::Matrix(size_t m,size_t n)
{
	_row=m;
	_col=n;
	if((data=new T[_row*_col])==NULL)
	{
		cerr<<"matrix construct error"<<endl;
		exit(1);
	}
}
template<class T>
Matrix<T>::Matrix(size_t m,size_t n,const T& element)
{
	_row=m;
	_col=n;
	if((data=new T[_row*_col])==NULL)
	{
		cerr<<"matrix construct error"<<endl;
		exit(1);
	}
	init(element);
}
template<class T>
Matrix<T>::Matrix(const Matrix<T>& other)
{
	_row=other._row;
	_col=other._col;
	if((data=new T[_row*_col])==NULL)
	{
		cerr<<"matrix copy construct error"<<endl;
		exit(1);
	}
	for(int i=0;i<(_row*_col);i++)
		data[i]=other.data[i];
}
template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other)
{
	if(&other==this)
		return *this;
	if((_row*_col)>=(other._row*other._col))
	{
		_row=other._row;
		_col=other._col;
		for(int i=0;i<(other._row*other._col);i++)
			data[i]=other.data[i];
		return *this;
	}
	delete[] data;
	data=NULL;
	_row=other._row;
	_col=other._col;
	if((data=new T[_row*_col])==NULL)
	{
		cerr<<"matrix operator=() error"<<endl;
		exit(1);
	}
	for(int i=0;i<(_row*_col);i++)
		data[i]=other.data[i];
	return *this;
}
template<class T>
Matrix<T>::~Matrix()
{
	delete[] data;
}
template<class T>
const size_t Matrix<T>::row()const
{
	return _row;
}
template<class T>
const size_t Matrix<T>::col()const
{
	return _col;
}
template<class T>
T* Matrix<T>::operator[](size_t index)
{
	if(index<_row)
	{
		return data+index*_col;
	}
	else
	{
		cerr<<"access overflow"<<endl;
		exit(1);
	}
}
template<class T>
const T* Matrix<T>::operator[](size_t index)const
{
	if(index<_row)
		return data+index*_col;
	else
	{
		cerr<<"const access overflow"<<endl;
		exit(1);
	}
}
template<class T>
void Matrix<T>::init(const T& element)
{
	for(size_t i=0;i<(_row*_col);i++)
		data[i]=element;
}
#endif

