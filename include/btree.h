#ifndef __BTREE_H__
#define __BTREE_H__

#include <cstdint>
#include "student.h"
#include "node.h"

// Common base for trees
class Tree
{
public:
	virtual ~Tree() {}
	virtual void Insert(std::size_t rid) = 0;
	virtual void Delete(unsigned int key) = 0;
	virtual std::size_t Search(unsigned int key) const = 0;
	virtual std::size_t* RangeQuery(unsigned int key1, unsigned int key2, std::size_t& count) const = 0;
	virtual void BulkLoading(const std::size_t* rids_array, std::size_t count) = 0;
};

class BTree : public Tree
{
protected:
	const StudentArray& records;	// Pointer to the records
	const std::size_t d;			// Order of the tree
	std::size_t depth;				// Depth of the tree (0 if root is a leaf)
	Node* root;						// Pointer to the root node
	long long split_count;
	// Delete node recursively for destructor
	void _DeleteNode(Node* node);
	// Search for the key and store the path nodes, path indices and level
	bool _Search(unsigned int key, Node** nodes, std::size_t* indices, std::size_t& level) const;
	// Insert element and split the node when it is full
	void _InsertAndSplit(Node** nodes, std::size_t* indices, std::size_t level,
		std::size_t rid, Node* left = NULL, Node* right = NULL);
	// Delete and rebalance the tree by rotation and merging
	void _Rotate(Node* node, std::size_t idx, bool rotate_left = true);
	void _Merge(Node* node, std::size_t idx);
	void _DeleteAndRebalance(Node** nodes, std::size_t* indices, std::size_t level);
	// Plot the node and children recursively
	void _PlotNode(Node* node, std::size_t level, std::size_t max_depth) const;
	void _AverageNodeDensity(Node* node, long long& sum, long long& count) const;
public:
	BTree(const StudentArray& records, std::size_t d);
	~BTree() override;
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