#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>

// Node for trees
class Node
{
private:
	unsigned int* keys;							// Array of keys(student_id)
	std::size_t* rids;							// Index of StudentArray(pointer to record) or NULL(in B+-Tree)
	Node** children;							// Array of pointers to children
	std::size_t n;								// Number of keys
public:
	Node(std::size_t m, bool has_rids = true);	// Constructor for maximum m keys
	~Node();
	unsigned int& Key(std::size_t idx);			// Return the key
	std::size_t& RID(std::size_t idx);			// Return the RID
	Node*& Child(std::size_t idx);				// Return the child
	std::size_t GetN() const;					// Return n
	// Search index of the key
	// If search succeed, return value indicates index of the exact key.
	// If search failed, return value indicates index of the at least larger key.
	// If the key is greater than all the keys in the node, it indicates n(the last index + 1).
	// Optional variable found indicates whether the key exists in the node.
	std::size_t Search(unsigned int key) const;
	std::size_t Search(unsigned int key, bool& found) const;
	// Insert a single key with two child pointers to the node
	void Insert(std::size_t idx, unsigned int key, std::size_t rid, Node* left = NULL, Node* right = NULL);
	// Delete the key and pointer(right child or left child) from the node
	void Delete(std::size_t idx, bool del_left = false);
	// Print the node
	friend std::ostream& operator<<(std::ostream& os,  const Node& node);
};

#endif