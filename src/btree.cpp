#include <iostream>
#include <cstdlib>
#include "btree.h"

void BTree::_DeleteNode(Node* node)
{
	std::size_t n = node->GetN();
	for (std::size_t i = 0; i <= n; i++)
	{
		if (node->Child(i) != NULL)
			_DeleteNode(node->Child(i));
	}
	delete node;
}

bool BTree::_Search(unsigned int key, Node** nodes, std::size_t* indices, std::size_t& level) const
{
	Node* cur = root;
	level = 0;
	bool found = false;
	while (true)
	{
		std::size_t idx = cur->Search(key, found);
		nodes[level] = cur;
		indices[level] = idx;
		if (found) break;
		cur = cur->Child(idx);
		if (cur == NULL) break;
		level++;
	}
	return found;
}

void BTree::_InsertAndSplit(Node** nodes, std::size_t* indices, std::size_t level,
	std::size_t rid, Node* left, Node* right)
{
	Node* target = nodes[level];
	std::size_t idx = indices[level];
	std::size_t n = target->GetN();

	// If the node is not full, just insert the element
	if (n + 1 < d)
	{
		target->Insert(idx, records[rid].student_id, rid, left, right);
		return;
	}

	split_count++;

	// Make temporary size n + 1 node to insert the element
	Node temp(d);

	// Copy target node to the temporary node
	for (std::size_t i = 0; i < n; i++)
		temp.Insert(i, target->Key(i), target->RID(i), target->Child(i), target->Child(i + 1));

	// Insert the new element to the temporary node
	temp.Insert(idx, records[rid].student_id, rid, left, right);

	// Make new nodes(left and right) and split the temporary node
	Node* new_left = new Node(d - 1);
	Node* new_right = new Node(d - 1);
	std::size_t mid = d / 2;
	for (std::size_t i = 0; i < mid; i++)
		new_left->Insert(i, temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));
	for (std::size_t i = mid + 1; i < d; i++)
		new_right->Insert(i - (mid + 1), temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));

	// Delete the original node
	delete target;

	// Recursively insert the middle key to the parent node
	if (level > 0)
		_InsertAndSplit(nodes, indices, level - 1, temp.RID(mid), new_left, new_right);

	// If level is 0, update the root node
	else
	{
		Node* new_root = new Node(d - 1);
		new_root->Insert(0, temp.Key(mid), temp.RID(mid), new_left, new_right);
		root = new_root;
		depth++;
	}
}

void BTree::_Rotate(Node* node, std::size_t idx, bool rotate_left)
{
	Node* left = node->Child(idx);
	Node* right = node->Child(idx + 1);
	Node* subtree;
	// Rotate left
	if (rotate_left)
	{
		subtree = right->Child(0);

		// Modify the left node
		left->Insert(left->GetN(), node->Key(idx), node->RID(idx), left->Child(left->GetN()), subtree);

		// Modify the parent node
		node->Key(idx) = right->Key(0);
		node->RID(idx) = right->RID(0);

		// Modify the right node
		right->Delete(0, true);
	}
	// Rotate Right
	else
	{
		subtree = left->Child(left->GetN());

		// Modify the right node
		right->Insert(0, node->Key(idx), node->RID(idx), subtree, right->Child(0));

		// Modify the parent node
		node->Key(idx) = left->Key(left->GetN() - 1);
		node->RID(idx) = left->RID(left->GetN() - 1);

		// Modify the left node
		left->Delete(left->GetN() - 1);
	}
}

void BTree::_Merge(Node* node, std::size_t idx)
{
	Node* left = node->Child(idx);
	Node* right = node->Child(idx + 1);

	// Insert parent's element to the left node
	left->Insert(left->GetN(), node->Key(idx), node->RID(idx), left->Child(left->GetN()));

	// Insert right node's elements to the left node
	for (size_t i = 0; i < right->GetN(); i++)
		left->Insert(left->GetN(), right->Key(i), right->RID(i), right->Child(i));
	left->Child(left->GetN()) = right->Child(right->GetN());

	// Delete the right node
	delete right;
}

void BTree::_DeleteAndRebalance(Node** nodes, std::size_t* indices, std::size_t level)
{
	// Deletion for the root node
	if (level == 0)
	{
		// If the root has more than one elements, just delete it
		if (root->GetN() > 1)
			root->Delete(indices[0]);

		// If the root has only one element, delete the root
		else
		{
			Node* temp = root->Child(0);

			// If the root is also a leaf, just make it empty
			if (temp == NULL)
				root->Delete(0);

			// If the root is not a leaf, modify the root pointer to the child
			else
			{
				delete root;
				root = temp;
				depth--;
			}
		}
		return;
	}

	std::size_t min_n = d % 2 == 0 ? d / 2 - 1 : d / 2;
	Node* pnode = nodes[level - 1];			// parent node
	std::size_t pidx = indices[level - 1];	// parent index
	Node* target = nodes[level];			// target node
	std::size_t idx = indices[level];		// target index

	// Delete the element
	target->Delete(idx);

	// If the node has enough elements, do nothing
	if (target->GetN() >= min_n)
		return;

	// If the right sibling has more than minimum number of elements, rotate left
	else if (pidx < pnode->GetN() && pnode->Child(pidx + 1)->GetN() > min_n)
		_Rotate(pnode, pidx);

	// If the left sibling has more than minimum number of elements, rotate right
	else if (pidx > 0 && pnode->Child(pidx - 1)->GetN() > min_n)
		_Rotate(pnode, pidx - 1, false);

	// If none of the siblings have more than minimum number of elements, merge with siblings
	else
	{	
		// Determine the left node and right node to merge
		// If the target has no right sibling, merge with left. Else, merge wth right
		if (pidx == pnode->GetN())
			pidx = --indices[level - 1];
		
		// Merge two nodes
		_Merge(pnode, pidx);
		
		// Recursively rebalance the parent node
		_DeleteAndRebalance(nodes, indices, level - 1);
	}
}

void BTree::_PlotNode(Node* node, std::size_t level, std::size_t max_depth) const
{
	if (level > max_depth) return;
	for (std::size_t i = 0; i < level; i++)
		std::cout << "  ";
	if (level != 0)
		std::cout << "->";
	std::cout << *node << std::endl;
	std::size_t n = node->GetN();
	for (std::size_t i = 0; i <= n; i++)
	{
		if (node->Child(i) != NULL)
			_PlotNode(node->Child(i), level + 1, max_depth);
	}
}

void BTree::_AverageNodeDensity(Node* node, long long& sum, long long& count) const
{
	std::size_t n = node->GetN();
	sum += n;
	count++;
	for (std::size_t i = 0; i <= n; i++)
	{
		if (node->Child(i) != NULL)
			_AverageNodeDensity(node->Child(i), sum, count);
	}
}

BTree::BTree(const StudentArray& records, std::size_t d) : records(records), d(d), depth(0), split_count(0)
{
	root = new Node(d - 1);
}

BTree::~BTree()
{
	_DeleteNode(root);
}

std::size_t BTree::GetDepth() const
{
	return depth;
}

long long BTree::GetSplitCount() const
{
	return split_count;
}

void BTree::Insert(std::size_t rid)
{
	// Find the target leaf node to insert
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;
	if (_Search(records[rid].student_id, nodes, indices, level))
	{
		std::cout << "Insertion failed." << std::endl;
		std::cout << "The key already exists in the tree!" << std::endl;
		std::exit(1);
	}
	Node* target = nodes[level];
	std::size_t idx = indices[level];

	// Insert and rebalance the tree
	_InsertAndSplit(nodes, indices, level, rid);

	delete[]nodes;
	delete[]indices;
}

void BTree::Delete(unsigned int key)
{
	// Find the target node to delete
	Node** nodes = new Node * [depth + 1];
	std::size_t* indices = new std::size_t[depth + 1];
	std::size_t level;
	if (!_Search(key, nodes, indices, level))
	{
		std::cout << "Deletion failed." << std::endl;
		std::cout << "The key does not exist in the tree!" << std::endl;
		std::exit(1);
	}

	// Case 1: Key is in the internal node -> Swap with successor -> Case 2
	if (level < depth)
	{
		Node* target = nodes[level];
		std::size_t idx = indices[level];

		// Find the successor and update the path
		Node* cur = target->Child(idx + 1);
		indices[level++]++;
		nodes[level] = cur;
		indices[level] = 0;
		for (std::size_t i = level + 1; i <= depth; i++)
		{
			cur = cur->Child(0);
			level = i;
			nodes[level] = cur;
			indices[level] = 0;
		}
		Node* leaf = cur;

		// Swap with the successor
		unsigned int temp_key = target->Key(idx);
		std::size_t temp_rid = target->RID(idx);
		target->Key(idx) = leaf->Key(0);
		target->RID(idx) = leaf->RID(0);
		leaf->Key(0) = temp_key;
		leaf->RID(0) = temp_rid;
	}
	
	// Case 2: Key is in the leaf node -> Delete and rebalance the tree
	_DeleteAndRebalance(nodes, indices, level);

	delete[]nodes;
	delete[]indices;
}

std::size_t BTree::Search(unsigned int key) const
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

std::size_t* BTree::RangeQuery(unsigned int key1, unsigned int key2, std::size_t& count) const
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

	// Sequential search using inorder traversal
	bool stop = false;
	while (!stop)
	{
		unsigned int k = cur->Key(idx);
		if (k >= min_k && k <= max_k)
			results[count++] = cur->RID(idx);
		else if (k > max_k)
		{
			stop = true;
			break;
		}

		// If the successor doesn't exist, look for the next element
		if (cur->Child(idx + 1) == NULL)
		{
			if (idx + 1 < cur->GetN())
				idx++;
			else
			{
				bool found_parent = false;
				while (level > 0)
				{
					level--;
					cur = nodes[level];
					idx = indices[level];

					if (idx < cur->GetN())
					{
						found_parent = true;
						break;
					}
				}

				if (!found_parent)
				{
					stop = true;
					break;
				}
			}
		}

		// Find the successor and update the path
		else
		{
			cur = cur->Child(idx + 1);
			indices[level++]++;
			nodes[level] = cur;
			indices[level] = 0;
			for (std::size_t i = level + 1; i <= depth; i++)
			{
				cur = cur->Child(0);
				level = i;
				nodes[level] = cur;
				indices[level] = 0;
			}
			idx = 0;
		}
	}

	delete[] nodes;
	delete[] indices;

	return results;
}

void BTree::Plot(std::size_t max_depth) const
{
	_PlotNode(root, 0, max_depth);
}

double BTree::AverageNodeDensity() const
{
	long long sum = 0;
	long long count = 0;
	_AverageNodeDensity(root, sum, count);
	return (double)sum / count / (d - 1) * 100;
}