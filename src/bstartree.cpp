#include "bstartree.h"

void BStarTree::_Redistribute(Node* node, std::size_t idx, std::size_t rid, Node* left, Node* right)
{
	Node temp(2 * d);
	
	// Copy the left node to the temporary node
	Node* left_node = node->Child(idx);
	for (std::size_t i = 0; i < left_node->GetN(); i++)
		temp.Insert(i, left_node->Key(i), left_node->RID(i), left_node->Child(i), left_node->Child(i + 1));

	// Copy the separater to the temporary node
	temp.Insert(temp.GetN(), node->Key(idx), node->RID(idx), temp.Child(temp.GetN()));

	// Copy the right node to the temporary node
	Node* right_node = node->Child(idx + 1);
	for (std::size_t i = 0; i < right_node->GetN(); i++)
		temp.Insert(temp.GetN(), right_node->Key(i), right_node->RID(i), right_node->Child(i), right_node->Child(i + 1));

	// Insert the element to the temporary node
	temp.Insert(temp.Search(records[rid].student_id), records[rid].student_id, rid, left, right);

	// Delete the original nodes
	delete left_node;
	delete right_node;

	// Redistribute the temporary node
	std::size_t mid = temp.GetN() / 2;
	Node* new_left = new Node(d - 1);
	Node* new_right = new Node(d - 1);
	for (std::size_t i = 0; i < mid; i++)
		new_left->Insert(i, temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));
	node->Key(idx) = temp.Key(mid);
	node->RID(idx) = temp.RID(mid);
	node->Child(idx) = new_left;
	node->Child(idx + 1) = new_right;
	for (std::size_t i = mid + 1; i < temp.GetN(); i++)
		new_right->Insert(new_right->GetN(), temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));
}

void BStarTree::_InsertAndRebalance(Node** nodes, std::size_t* indices, std::size_t level,
	std::size_t rid, Node* left, Node* right)
{
	// If root, do same as B-Tree
	if (level == 0)
	{
		BTree::_InsertAndSplit(nodes, indices, level, rid, left, right);
		return;
	}

	Node* pnode = nodes[level - 1];			// parent node
	std::size_t pidx = indices[level - 1];	// parent index
	Node* target = nodes[level];			// target node
	std::size_t idx = indices[level];		// target index

	// If the node is not full, just insert the element
	if (target->GetN() + 1 < d)
		target->Insert(idx, records[rid].student_id, rid, left, right);

	// If the right sibling is not full, redistribute with the inserting element
	else if (pidx < pnode->GetN() && pnode->Child(pidx + 1)->GetN() + 1 < d)
		_Redistribute(pnode, pidx, rid, left, right);

	// If the left sibling is not full, redistribute with the inserting element
	else if (pidx > 0 && pnode->Child(pidx - 1)->GetN() + 1 < d)
		_Redistribute(pnode, pidx - 1, rid, left, right);

	// If the both siblings are full, 2-to-3 split
	else
	{
		split_count++;

		// If the right sibling exists, split with it. Otherwise, split with the left
		if (pidx == pnode->GetN())
			pidx = --indices[level - 1];

		// Make temporary size 2n + 2 node to insert the element
		Node temp(2*d);

		// Copy the left node to the temporary node
		target = pnode->Child(pidx);
		for (std::size_t i = 0; i < target->GetN(); i++)
			temp.Insert(i, target->Key(i), target->RID(i), target->Child(i), target->Child(i + 1));

		// Copy the separater to the temporary node
		temp.Insert(temp.GetN(), pnode->Key(pidx), pnode->RID(pidx), temp.Child(temp.GetN()));

		// Copy the right node to the temporary node
		target = pnode->Child(pidx + 1);
		for (std::size_t i = 0; i < target->GetN(); i++)
			temp.Insert(temp.GetN(), target->Key(i), target->RID(i), target->Child(i), target->Child(i + 1));

		// Insert the new element to the temporary node
		idx = temp.Search(records[rid].student_id);
		temp.Insert(idx, records[rid].student_id, rid, left, temp.Child(idx));
		
		// Make new nodes(left, mid and right) and split the temporary node
		Node* new_left = new Node(d - 1);
		Node* new_mid = new Node(d - 1);
		Node* new_right = new Node(d - 1);
		std::size_t sep1 = (2 * d) / 3, sep2 = (4 * d) / 3;
		for (std::size_t i = 0; i < sep1; i++)
			new_left->Insert(i, temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));
		for (std::size_t i = sep1 + 1; i < sep2; i++)
			new_mid->Insert(i - (sep1 + 1), temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));
		for (std::size_t i = sep2 + 1; i < 2*d; i++)
			new_right->Insert(i - (sep2 + 1), temp.Key(i), temp.RID(i), temp.Child(i), temp.Child(i + 1));

		// Delete the original node
		delete pnode->Child(pidx);
		delete pnode->Child(pidx + 1);

		// Update the parent with sep2
		pnode->Key(pidx) = temp.Key(sep2);
		pnode->RID(pidx) = temp.RID(sep2);
		pnode->Child(pidx) = new_mid;
		pnode->Child(pidx + 1) = new_right;
		
		// Recursively insert the sep1 to the parent node
		_InsertAndRebalance(nodes, indices, level - 1, temp.RID(sep1), new_left, new_mid);
	}
}

BStarTree::BStarTree(const StudentArray& records, std::size_t d) : BTree(records, d) {}

BStarTree::~BStarTree() {}

void BStarTree::Insert(std::size_t rid)
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
	_InsertAndRebalance(nodes, indices, level, rid);

	delete[]nodes;
	delete[]indices;
}