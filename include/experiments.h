#ifndef __EXPERIMENTS_H__
#define __EXPERIMENTS_H__

#include "student.h"
#include "btree.h"
#include "bstartree.h"
#include "bplustree.h"

void InsertionAndParameterTuning(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree);
void PointSearch(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree);
void RangeQuery(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree);
void DeletionAndStructuralIntegrity(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree);
void AdditionalExperiments(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree);

#endif