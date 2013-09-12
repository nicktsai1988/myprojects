#ifndef TEST_H
#define TEST_H
#include "bitvector.h"
void print_bitvector(const BitVector& vector);
void print_vector_pair(const vector<Pair>& vector);
void print_vector_uint(const vector<unsigned int>& vector);
void test_gamma_encoding();
void test_block_encodind();
void test_interval_encoding();
void print_pair(const Pair& p);
void test_number_to_pair();
void test_pair_to_number();
void test_big_to_little_encoding();
#endif
