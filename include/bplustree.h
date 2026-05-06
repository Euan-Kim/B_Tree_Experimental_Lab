#ifndef __BPLUSTREE_H__
#define __BPLUSTREE_H__

#include <cstdint>
#include "btree.h"
#include "student.h"
#include "node.h"

class BPlusTree : public Tree
{
private:
	const StudentArray& records;	// Pointer to the records
	const std::size_t d;			// Order of the tree
	std::size_t depth;				// Depth of the tree (0 if root is a leaf)
	Node* root;						// Pointer to the root node
	long long split_count;
	// Delete node recursively for destructor
	void _DeleteNode(Node* node, std::size_t level);
	// Search for the key and store the path nodes, path indices and level(always reaches the leaf node)
	bool _Search(unsigned int key, Node** nodes, std::size_t* indices, std::size_t& level) const;
	// Insert element, split the node when full, and promote the key upward(handles internal/leaf separately)
	void _InsertAndSplit(Node** nodes, std::size_t* indices, std::size_t level,
		unsigned int key, std::size_t rid, Node* left = NULL, Node* right = NULL);
	// Borrow a key from a sibling node to rebalance (rotation)
	void _Rotate(Node* node, std::size_t idx, bool rotate_left, bool is_leaf);
	// Merge with a sibling node
	void _Merge(Node* node, std::size_t idx, bool is_leaf);
	// Delete and rebalance the tree when underflow occurs
	void _DeleteAndRebalance(Node** nodes, std::size_t* indices, std::size_t level);
	// Plot the node and children(only internal nodes traverse children in B+ tree)
	void _PlotNode(Node* node, std::size_t level, std::size_t max_depth) const;
	void _AverageNodeDensity(Node* node, std::size_t level, long long& sum, long long& count) const;
public:
	BPlusTree(const StudentArray& records, std::size_t d);
	~BPlusTree() override;
	std::size_t GetDepth() const;
	long long GetSplitCount() const;
	void Insert(std::size_t rid) override;
	void Delete(unsigned int key) override;
	std::size_t Search(unsigned int key) const override;
	// Range query returning dynamically allocated array of RIDs
	std::size_t* RangeQuery(unsigned int key1, unsigned int key2, std::size_t& count) const override;
	void Plot(std::size_t max_depth = SIZE_MAX) const;
	double AverageNodeDensity() const;
	// Bulk Loading: Construct a tree in a bottom-up manner based on the rids array sorted in ascending order
	void BulkLoading(const std::size_t* rids_array, std::size_t count) override;
};

#endif