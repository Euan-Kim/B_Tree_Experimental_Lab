#include <iostream>
#include <cstdlib>
#include "bplustree.h"

void BPlusTree::_DeleteNode(Node* node, std::size_t level)
{
	if (node == NULL) return;

	// Check whether it is not a leaf node
	if (level < depth)
	{
		std::size_t n = node->GetN();
		// Recursively traverse and delete children
		for (std::size_t i = 0; i <= n; i++)
		{
			// Check whether it has child
			if (node->Child(i) != NULL)
				_DeleteNode(node->Child(i), level + 1);
		}
	}

	delete node;
}

bool BPlusTree::_Search(unsigned int key, Node** nodes, std::size_t* indices, std::size_t& level) const
{
	Node* cur = root;
	level = 0;
	bool found = false;

	// Route through internal nodes
	while (level < depth)
	{
		bool tmp_found = false;
		std::size_t idx = cur->Search(key, tmp_found);
		// If the key matches in an internal node of a B+ tree, the actual data exists in the right subtree
		if (tmp_found) idx++;
		nodes[level] = cur;
		indices[level] = idx;
		cur = cur->Child(idx);
		level++;
	}

	// Reached the leaf node
	std::size_t idx = cur->Search(key, found);
	nodes[level] = cur;
	indices[level] = idx;
	return found;
}

void BPlusTree::_InsertAndSplit(Node** nodes, std::size_t* indices, std::size_t level,
	unsigned int key, std::size_t rid, Node* left, Node* right)
{
	Node* target = nodes[level];
	std::size_t idx = indices[level];
	std::size_t n = target->GetN();
	bool is_leaf = (level == depth);
	Node* prev_leaf = NULL;
	Node* next_leaf = NULL;
	if (is_leaf)
	{
		prev_leaf = target->Child(0);
		next_leaf = target->Child(d - 1);
	}

	// If the node has free space, just insert the element and return
	if (n + 1 < d)
	{
		target->Insert(idx, key, rid, left, right);
		// Restore the linked list pointers
		if (is_leaf)
		{
			target->Child(0) = prev_leaf;
			target->Child(d - 1) = next_leaf;
		}
		return;
	}

	split_count++;

	// Make temporary node when overflow occurs (guarantees size n + 1)
	Node temp(d, is_leaf);

	// Copy elements from target node to temporary node
	for (std::size_t i = 0; i < n; i++)
		temp.Insert(i, target->Key(i), is_leaf ? target->RID(i) : NULL, target->Child(i), target->Child(i + 1));

	// Insert the new element to the temporary node
	temp.Insert(idx, key, rid, left, right);

	// Make two new nodes to be split
	Node* new_left = new Node(d - 1, is_leaf);
	Node* new_right = new Node(d - 1, is_leaf);
	std::size_t mid = d / 2;
	unsigned int up_key;	// key to be promoted

	// Leaf node split: the middle key is promoted and also becomes the first element of the right node
	if (is_leaf)
	{
		for (std::size_t i = 0; i < mid; i++)
			new_left->Insert(i, temp.Key(i), temp.RID(i));
		for (std::size_t i = mid; i < d; i++)
			new_right->Insert(i - mid, temp.Key(i), temp.RID(i));
		up_key = new_right->Key(0);
		// Link the leaf nodes (Linked List)
		if (prev_leaf != NULL)
			prev_leaf->Child(d - 1) = new_left;
		new_left->Child(0) = prev_leaf;
		new_left->Child(d - 1) = new_right;
		new_right->Child(0) = new_left;
		new_right->Child(d - 1) = next_leaf;
		if (next_leaf != NULL)
			next_leaf->Child(0) = new_right;
	}

	// Internal node split: the middle key is promoted and removed from the node
	else
	{
		for (std::size_t i = 0; i < mid; i++)
			new_left->Insert(i, temp.Key(i), NULL, temp.Child(i), temp.Child(i + 1));
		for (std::size_t i = mid + 1; i < d; i++)
			new_right->Insert(i - (mid + 1), temp.Key(i), NULL, temp.Child(i), temp.Child(i + 1));
		up_key = temp.Key(mid);
	}
	
	// Delete the original node
	delete target;
	
	// Recursively insert the promoted key to the parent node
	if (level > 0)
		_InsertAndSplit(nodes, indices, level - 1, up_key, 0, new_left, new_right);
	// If the root node is split, create a new internal root node
	else
	{
		Node* new_root = new Node(d - 1, false);
		new_root->Insert(0, up_key, 0, new_left, new_right);
		root = new_root;
		depth++;
	}
}

void BPlusTree::_Rotate(Node* pnode, std::size_t pidx, bool rotate_left, bool is_leaf)
{
	Node* left = pnode->Child(pidx);
	Node* right = pnode->Child(pidx + 1);

	// Leaf node rotation
	if (is_leaf)
	{
		Node* left_prev = left->Child(0);
		Node* right_next = right->Child(d - 1);

		if (rotate_left)
		{
			// Move the first element of the right sibling to the last of the left sibling
			left->Insert(left->GetN(), right->Key(0), right->RID(0));
			right->Delete(0);
			// Update the routing key of the parent
			pnode->Key(pidx) = right->Key(0);
		}
		else
		{
			// Move the last element of the left sibling to the first of the right sibling
			std::size_t last = left->GetN() - 1;
			right->Insert(0, left->Key(last), left->RID(last));
			left->Delete(last);
			// Update the routing key of the parent
			pnode->Key(pidx) = right->Key(0);
		}

		// Restore leaf linked list pointers
		left->Child(0) = left_prev;
		left->Child(d - 1) = right;
		right->Child(0) = left;
		right->Child(d - 1) = right_next;
	}

	// Internal node rotation
	else
	{
		Node* subtree;
		if (rotate_left)
		{
			subtree = right->Child(0);
			left->Insert(left->GetN(), pnode->Key(pidx), 0, left->Child(left->GetN()), subtree);
			pnode->Key(pidx) = right->Key(0);
			right->Delete(0, true);
		}
		else
		{
			subtree = left->Child(left->GetN());
			std::size_t last = left->GetN() - 1;
			right->Insert(0, pnode->Key(pidx), 0, subtree, right->Child(0));
			pnode->Key(pidx) = left->Key(last);
			left->Delete(last); // Delete the last key of the left sibling, preserve the right child(del_left = false)
		}
	}
}

void BPlusTree::_Merge(Node* pnode, std::size_t pidx, bool is_leaf)
{
	Node* left = pnode->Child(pidx);
	Node* right = pnode->Child(pidx + 1);

	if (is_leaf)
	{
		Node* left_prev = left->Child(0);
		Node* right_next = right->Child(d - 1);

		// Merge all data from the right leaf to the left
		for (size_t i = 0; i < right->GetN(); i++)
			left->Insert(left->GetN(), right->Key(i), right->RID(i));

		// Update leaf linked list pointers
		left->Child(0) = left_prev;
		left->Child(d - 1) = right_next;
		if (right_next != NULL)
			right_next->Child(0) = left;

		delete right;
	}
	else
	{
		// Bring down the parent's routing key and merge the pointers and keys of the right internal node
		left->Insert(left->GetN(), pnode->Key(pidx), 0, left->Child(left->GetN()));
		for (size_t i = 0; i < right->GetN(); i++)
			left->Insert(left->GetN(), right->Key(i), 0, right->Child(i));
		left->Child(left->GetN()) = right->Child(right->GetN());
		delete right;
	}
}

void BPlusTree::_DeleteAndRebalance(Node** nodes, std::size_t* indices, std::size_t level)
{
	bool is_leaf = (level == depth);

	// Reached the root node
	if (level == 0)
	{
		if (root->GetN() > 0)
		{
			Node* prev_leaf = NULL;
			Node* next_leaf = NULL;
			if (is_leaf)
			{
				prev_leaf = root->Child(0);
				next_leaf = root->Child(d - 1);
			}

			root->Delete(indices[0]);

			if (is_leaf)
			{
				root->Child(0) = prev_leaf;
				root->Child(d - 1) = next_leaf;
			}
		}

		// If the root becomes empty because its only routing key was deleted during merge(tree height decreases)
		if (root->GetN() == 0 && depth > 0)
		{
			Node* temp = root->Child(0);
			delete root;
			root = temp;
			depth--;
		}
		return;
	}

	std::size_t min_n = d % 2 == 0 ? d / 2 - 1 : d / 2;
	Node* pnode = nodes[level - 1];			// parent node
	std::size_t pidx = indices[level - 1];	// parent index
	Node* target = nodes[level];			// target node
	std::size_t idx = indices[level];		// target index

	Node* prev_leaf = NULL;
	Node* next_leaf = NULL;
	if (is_leaf)
	{
		prev_leaf = target->Child(0);
		next_leaf = target->Child(d - 1);
	}

	// Delete the element (from the target node)
	target->Delete(idx);

	if (is_leaf)
	{
		target->Child(0) = prev_leaf;
		target->Child(d - 1) = next_leaf;
	}

	// If underflow doesn't occur, just return
	if (target->GetN() >= min_n)
		return;

	// If redistribution from the right sibling is possible
	else if (pidx < pnode->GetN() && pnode->Child(pidx + 1)->GetN() > min_n)
		_Rotate(pnode, pidx, true, is_leaf);

	// If redistribution from the left sibling is possible
	else if (pidx > 0 && pnode->Child(pidx - 1)->GetN() > min_n)
		_Rotate(pnode, pidx - 1, false, is_leaf);

	// If redistribution is impossible, merge with a sibling
	else
	{
		// If it is the rightmost child, adjust pidx to merge with the left sibling
		if (pidx == pnode->GetN())
			pidx = --indices[level - 1];

		_Merge(pnode, pidx, is_leaf);

		// Recursively check for underflow because the key in the parent node must be deleted after merging
		_DeleteAndRebalance(nodes, indices, level - 1);
	}
}

void BPlusTree::_PlotNode(Node* node, std::size_t level, std::size_t max_depth) const
{
	if (level > max_depth) return;
	for (std::size_t i = 0; i < level; i++)
		std::cout << "  ";
	if (level != 0)
		std::cout << "->";
	std::cout << *node << std::endl;

	// Check whether it is not a leaf node
	if (level < depth)
	{
		// Recursively plot children
		std::size_t n = node->GetN();
		for (std::size_t i = 0; i <= n; i++)
		{
			// Check whether it has child
			if (node->Child(i) != NULL)
				_PlotNode(node->Child(i), level + 1, max_depth);
		}
	}
}

void BPlusTree::_AverageNodeDensity(Node* node, std::size_t level, long long& sum, long long& count) const
{
	if (node == NULL) return;

	std::size_t n = node->GetN();
	sum += n;
	count++;

	// Check whether it is not a leaf node
	if (level < depth)
	{
		// Recursively traverse
		for (std::size_t i = 0; i <= n; i++)
		{
			// Check whether it has child
			if (node->Child(i) != NULL)
				_AverageNodeDensity(node->Child(i), level + 1, sum, count);
		}
	}
}

BPlusTree::BPlusTree(const StudentArray& records, std::size_t d) : records(records), d(d), depth(0), split_count(0)
{
	// The initial tree has only one leaf root node (has_rids = true)
	root = new Node(d - 1, true);
}

BPlusTree::~BPlusTree()
{
	_DeleteNode(root, 0);
}

std::size_t BPlusTree::GetDepth() const
{
	return depth;
}

long long BPlusTree::GetSplitCount() const
{
	return split_count;
}

void BPlusTree::Insert(std::size_t rid)
{
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;

	if (_Search(records[rid].student_id, nodes, indices, level))
	{
		std::cout << "Insertion failed." << std::endl;
		std::cout << "The key already exists in the tree!" << std::endl;
		std::exit(1);
	}

	_InsertAndSplit(nodes, indices, level, records[rid].student_id, rid);

	delete[]nodes;
	delete[]indices;
}

void BPlusTree::Delete(unsigned int key)
{
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;

	if (!_Search(key, nodes, indices, level))
	{
		std::cout << "Deletion failed." << std::endl;
		std::cout << "The key does not exist in the tree!" << std::endl;
		std::exit(1);
	}

	// Since all data is in the leaves of a B+ tree, deletion always starts at the leaf (depth level)
	_DeleteAndRebalance(nodes, indices, depth);

	delete[]nodes;
	delete[]indices;
}

std::size_t BPlusTree::Search(unsigned int key) const
{
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;

	if (!_Search(key, nodes, indices, level))
	{
		std::cout << "Search failed." << std::endl;
		std::cout << "Failed to find the key: " << key << std::endl;
		std::exit(1);
	}

	std::size_t result = nodes[level]->RID(indices[level]);
	delete[]nodes;
	delete[]indices;

	return result;
}

std::size_t* BPlusTree::RangeQuery(unsigned int key1, unsigned int key2, std::size_t& count) const
{
	unsigned int min_k = key1 < key2 ? key1 : key2;
	unsigned int max_k = key1 > key2 ? key1 : key2;

	// Determine the maximum array size using the difference between the two keys
	std::size_t max_size = static_cast<std::size_t>(max_k - min_k + 1);

	std::size_t* results = new std::size_t[max_size];
	count = 0;

	if (root->GetN() == 0) return results;

	// Find the leaf node where min_k belongs
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;

	_Search(min_k, nodes, indices, level);
	Node* cur = nodes[level];			// The leaf node to start with
	std::size_t idx = indices[level];	// The current leaf node's index to start with

	delete[] nodes;
	delete[] indices;

	// Sequential search using the leaf linked list
	for (std::size_t i = idx; i < cur->GetN(); i++)
		results[count++] = cur->RID(i);
	cur = cur->Child(d - 1);
	while (cur != NULL)
	{
		bool stop = false;
		for (std::size_t i = 0; i < cur->GetN(); i++)
		{
			unsigned int k = cur->Key(i);
			if (k >= min_k && k <= max_k)
				results[count++] = cur->RID(i);
			else if (k > max_k)
			{
				stop = true;
				break;
			}
		}
		if (stop) break;

		// Move to the next leaf node in the linked list
		cur = cur->Child(d - 1);
	}

	return results;
}

void BPlusTree::Plot(std::size_t max_depth) const
{
	_PlotNode(root, 0, max_depth);
}

double BPlusTree::AverageNodeDensity() const
{
	long long sum = 0;
	long long count = 0;
	_AverageNodeDensity(root, 0, sum, count);
	return (double)sum / count / (d - 1) * 100;
}