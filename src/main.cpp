#include <iostream>
#include <limits>
#include "student.h"
#include "io.h"
#include "btree.h"
#include "bstartree.h"
#include "bplustree.h"
#include "experiments.h"

int main()
{
	using namespace std;

	// Input records
	cout << "Loading records..." << endl;
	StudentArray records(100000);
	ReadStudentCsv(records);
	size_t n = records.GetN();
	cout << "Records Loading completed." << endl;
	cout << "Number of records: " << n << endl;
	cout << endl;

	// Initialize trees
	size_t order = 0;
	unsigned int random_seed = 0;
	BTree* btree = NULL;
	BStarTree* bstartree = NULL;
	BPlusTree* bplustree = NULL;

	// Select experiment
	while (true)
	{
		cout << "Welcome to B-Tree Experimental Lab v1.0 by Euan Kim." << endl;
		cout << "You must generate trees first to perform other experiments." << endl;
		cout << "Also, you have to generate trees agian after the deletion to perform other experiments." << endl;
		cout << "Current state of the trees: " << "order = " << order << ", random seed = " << random_seed << endl;
		cout << "=======================Menu=======================" << endl;
		cout << "[1] Insertion & Parameter Tuning(Generate Trees)" << endl;
		cout << "[2] Point Search" << endl;
		cout << "[3] Range Query" << endl;
		cout << "[4] Deletion & Structural Integrity(Delete Trees)" << endl;
		cout << "[5] Additional Experiments: Bulk Loading(Generate Trees)" << endl;
		cout << "[6] Exit" << endl;
		cout << "==================================================" << endl;
		cout << "Choose a number: ";

		int input;
		cin >> input;
		if (cin.fail())
		{
			input = 7;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		cout << endl;

		bool terminate = false;
		switch (input)
		{
		case 1:
			InsertionAndParameterTuning(records, order, random_seed, btree, bstartree, bplustree);
			break;
		case 2:
			PointSearch(records, order, random_seed, btree, bstartree, bplustree);
			break;
		case 3:
			RangeQuery(records, order, random_seed, btree, bstartree, bplustree);
			break;
		case 4:
			DeletionAndStructuralIntegrity(records, order, random_seed, btree, bstartree, bplustree);
			break;
		case 5:
			AdditionalExperiments(records, order, random_seed, btree, bstartree, bplustree);
			break;
		case 6:
			terminate = true;
			break;
		default:
			cout << "Choose a valid number." << endl;
		}
		cout << endl;

		if (terminate)
			break;
	}

	if (btree != NULL)
		delete btree;
	if (bstartree != NULL)
		delete bstartree;
	if (bplustree != NULL)
		delete bplustree;
	return 0;
}