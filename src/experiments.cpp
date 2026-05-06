#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include "experiments.h"
using namespace std;

void InsertionAndParameterTuning(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree)
{
	cout << "===1. Insertion & Parameter Tuning(Generate Trees)===" << endl;

	// Delete remaining trees
	if (btree != NULL)
		delete btree;
	if (bstartree != NULL)
		delete bstartree;
	if (bplustree != NULL)
		delete bplustree;

	// Input order and random_seed
	cout << "Input order(>2): ";
	cin >> order;
	cout << "Input random_seed(unsigned int): ";
	cin >> random_seed;

	// Generate new trees
	btree = new BTree(records, order);
	bstartree = new BStarTree(records, order);
	bplustree = new BPlusTree(records, order);

	// Make random order of index
	const size_t COUNT = 100000;
	size_t* index = new size_t[COUNT];
	iota(index, index + COUNT, 1);
	mt19937 gen(random_seed);
	shuffle(index, index + COUNT, gen);

	Tree* trees[3] = { btree, bstartree, bplustree };
	long long times[3];
	for (size_t i = 0; i < 3; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		for (size_t j = 0; j < COUNT; j++)
			trees[i]->Insert(index[j]);
		auto end = chrono::high_resolution_clock::now();
		auto us = chrono::duration_cast<std::chrono::microseconds>(end - start);
		times[i] = us.count();
	}
	cout << "Insertion completed." << endl;
	cout << "Depth of the trees:" << endl;
	cout << "BTree: " << btree->GetDepth() << endl;
	cout << "BStarTree: " << bstartree->GetDepth() << endl;
	cout << "BPlusTree: " << bplustree->GetDepth() << endl;
	cout << endl;
	cout << "Total execution time:" << endl;
	cout << "BTree: " << times[0] << "(micro seconds)" << endl;
	cout << "BStarTree: " << times[1] << "(micro seconds)" << endl;
	cout << "BPlusTree: " << times[2] << "(micro seconds)" << endl;
	cout << endl;
	cout << "Final node utilization:" << endl;
	cout << "BTree: " << btree->AverageNodeDensity() << "%" << endl;
	cout << "BStarTree: " << bstartree->AverageNodeDensity() << "%" << endl;
	cout << "BPlusTree: " << bplustree->AverageNodeDensity() << "%" << endl;
	cout << endl;
	cout << "Total number of split:" << endl;
	cout << "BTree: " << btree->GetSplitCount() << endl;
	cout << "BStarTree: " << bstartree->GetSplitCount() << endl;
	cout << "BPlusTree: " << bplustree->GetSplitCount() << endl;

	delete[]index;
}

void PointSearch(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree)
{
	cout << "===2. Point Search===" << endl;

	// If trees are note generated, exit
	if (btree == NULL)
	{
		cout << "Generate trees first!" << endl;
		return;
	}

	// Input subset size
	cout << "Input subset size(<=100000): ";
	size_t subset_size;
	cin >> subset_size;

	// Make random order of index
	const size_t COUNT = 100000;
	size_t* index = new size_t[COUNT];
	iota(index, index + COUNT, 1);
	mt19937 gen(random_seed);
	shuffle(index, index + COUNT, gen);

	Tree* trees[3] = { btree, bstartree, bplustree };
	long long times[3];
	for (size_t i = 0; i < 3; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		for (size_t j = 0; j < subset_size; j++)
			trees[i]->Search(records[index[j]].student_id);
		auto end = chrono::high_resolution_clock::now();
		auto us = chrono::duration_cast<std::chrono::nanoseconds>(end - start);
		times[i] = us.count();
		times[i] /= subset_size;
	}
	cout << "Search completed." << endl;
	cout << endl;
	cout << "Average execution time:" << endl;
	cout << "BTree: " << times[0] << "(nano seconds)" << endl;
	cout << "BStarTree: " << times[1] << "(nano seconds)" << endl;
	cout << "BPlusTree: " << times[2] << "(nano seconds)" << endl;

	delete[]index;
}

void RangeQuery(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree)
{
	cout << "===3. Range Query===" << endl;

	// If trees are note generated, exit
	if (btree == NULL)
	{
		cout << "Generate trees first!" << endl;
		return;
	}

	// Input begin and end
	cout << "Input begin key : ";
	unsigned int begin;
	cin >> begin;
	cout << "Input end key : ";
	unsigned int end;
	cin >> end;

	Tree* trees[3] = { btree, bstartree, bplustree };
	long long times[3];
	double bmis[3];
	for (size_t i = 0; i < 3; i++)
	{
		size_t* rids;
		size_t count;
		double height = 0, weight = 0, bmi = 0;
		auto start = chrono::high_resolution_clock::now();
		rids = trees[i]->RangeQuery(begin, end, count);
		for (size_t i = 0; i < count; i++)
		{
			height += records[rids[i]].height;
			weight += records[rids[i]].weight;
		}
		height /= count;
		weight /= count;
		bmi = weight / (height * height);
		auto end = chrono::high_resolution_clock::now();
		delete[]rids;
		auto us = chrono::duration_cast<std::chrono::microseconds>(end - start);
		times[i] = us.count();
		bmis[i] = bmi;
	}
	cout << "Range Query completed." << endl;
	cout << endl;
	cout << "Total execution time:" << endl;
	cout << "BTree: " << times[0] << "(micro seconds)" << endl;
	cout << "BStarTree: " << times[1] << "(micro seconds)" << endl;
	cout << "BPlusTree: " << times[2] << "(micro seconds)" << endl;
	cout << endl;
	cout << "Average BMI between " << begin << " and " << end << ": " << endl;
	cout << "BTree: " << bmis[0] << endl;
	cout << "BStarTree: " << bmis[1] << endl;
	cout << "BPlusTree: " << bmis[2] << endl;
}

void DeletionAndStructuralIntegrity(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree)
{
	cout << "===4. Deletion & Structural Integrity(Delete Trees)===" << endl;

	// If trees are note generated, exit
	if (btree == NULL)
	{
		cout << "Generate trees first!" << endl;
		return;
	}

	// Input subset size
	cout << "Input subset size(<=100000): ";
	size_t subset_size;
	cin >> subset_size;

	// Make random order of index
	const size_t COUNT = 100000;
	size_t* index = new size_t[COUNT];
	iota(index, index + COUNT, 1);
	mt19937 gen(random_seed);
	shuffle(index, index + COUNT, gen);

	Tree* trees[3] = { btree, bstartree, bplustree };
	long long times[3];
	for (size_t i = 0; i < 3; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		for (size_t j = 0; j < subset_size; j++)
			trees[i]->Delete(records[index[j]].student_id);
		auto end = chrono::high_resolution_clock::now();
		auto us = chrono::duration_cast<std::chrono::microseconds>(end - start);
		times[i] = us.count();
	}
	for (size_t j = subset_size; j < COUNT; j++)
		btree->Search(records[index[j]].student_id);
	cout << "BTree structrue verified." << endl;
	for (size_t j = subset_size; j < COUNT; j++)
		bstartree->Search(records[index[j]].student_id);
	cout << "BStarTree structrue verified." << endl;
	for (size_t j = subset_size; j < COUNT; j++)
		bplustree->Search(records[index[j]].student_id);
	cout << "BPlusTree structrue verified." << endl;
	cout << "Deletion completed." << endl;
	cout << endl;
	cout << "Total execution time:" << endl;
	cout << "BTree: " << times[0] << "(micro seconds)" << endl;
	cout << "BStarTree: " << times[1] << "(micro seconds)" << endl;
	cout << "BPlusTree: " << times[2] << "(micro seconds)" << endl;

	delete[]index;

	delete btree;
	btree = NULL;
	delete bstartree;
	bstartree = NULL;
	delete bplustree;
	bplustree = NULL;
	order = 0;
	random_seed = 0;
}

void AdditionalExperiments(const StudentArray& records, size_t& order, unsigned int& random_seed,
	BTree*& btree, BStarTree*& bstartree, BPlusTree*& bplustree)
{
	cout << "===5. Additional Experiments: Bulk Loading===" << endl;

	// Delete remaining trees
	if (btree != NULL)
		delete btree;
	if (bstartree != NULL)
		delete bstartree;
	if (bplustree != NULL)
		delete bplustree;

	// Input order and random_seed
	cout << "Input order(>2): ";
	cin >> order;
	cout << "Input random_seed(unsigned int, not effect on bulk loading): ";
	cin >> random_seed;

	// Generate new trees
	btree = new BTree(records, order);
	bstartree = new BStarTree(records, order);
	bplustree = new BPlusTree(records, order);

	// Make sorted order of index
	const size_t COUNT = 100000;
	size_t* index = new size_t[COUNT];
	iota(index, index + COUNT, 1);
	std::sort(index, index + COUNT, [&](size_t i, size_t j) {
		return records[i].student_id < records[j].student_id;
	});

	Tree* trees[3] = { btree, bstartree, bplustree };
	long long times[3];
	for (size_t i = 0; i < 3; i++)
	{
		auto start = chrono::high_resolution_clock::now();
		trees[i]->BulkLoading(index, COUNT);
		auto end = chrono::high_resolution_clock::now();
		auto us = chrono::duration_cast<std::chrono::microseconds>(end - start);
		times[i] = us.count();
	}
	cout << "Insertion completed." << endl;
	cout << endl;
	cout << "Total execution time:" << endl;
	cout << "BTree: " << times[0] << "(micro seconds)" << endl;
	cout << "BStarTree: " << times[1] << "(micro seconds)" << endl;
	cout << "BPlusTree: " << times[2] << "(micro seconds)" << endl;
	cout << endl;
	cout << "Final node utilization:" << endl;
	cout << "BTree: " << btree->AverageNodeDensity() << "%" << endl;
	cout << "BStarTree: " << bstartree->AverageNodeDensity() << "%" << endl;
	cout << "BPlusTree: " << bplustree->AverageNodeDensity() << "%" << endl;

	delete[]index;
}
