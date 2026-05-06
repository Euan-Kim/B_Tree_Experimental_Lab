#ifndef __BSTARTREE_H__
#define __BSTARTREE_H__

#include "btree.h"

class BStarTree : public BTree
{
private:
	// Insert element and redistribute or 2-to-3 split the node when it is full
	void _Redistribute(Node* node, std::size_t idx, std::size_t rid, Node* left, Node* right);
	void _InsertAndRebalance(Node** nodes, std::size_t* indices, std::size_t level,
		std::size_t rid, Node* left = NULL, Node* right = NULL);
public:
	BStarTree(const StudentArray& records, std::size_t d);
	~BStarTree() override;
	void Insert(std::size_t rid) override;
};

#endif