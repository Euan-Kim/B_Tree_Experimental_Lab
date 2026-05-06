#include <cstdlib>
#include "node.h"

Node::Node(std::size_t m, bool has_rids) : rids(NULL), n(0)
{
	if (m < 2)
	{
		std::cout << "Invalid order!" << std::endl;
		std::exit(1);
	}
	keys = new unsigned int[m];
	if (has_rids)
		rids = new std::size_t[m];
	children = new Node * [m + 1];
	for (std::size_t i = 0; i < m + 1; i++)
		children[i] = NULL;
}

Node::~Node()
{
	delete[]keys;
	if (rids != NULL)
		delete[]rids;
	delete[]children;
}

unsigned int& Node::Key(std::size_t idx)
{
	return keys[idx];
}

std::size_t& Node::RID(std::size_t idx)
{
	return rids[idx];
}

Node*& Node::Child(std::size_t idx)
{
	return children[idx];
}

std::size_t Node::GetN() const
{
	return n;
}

std::size_t Node::Search(unsigned int key) const
{
	// Binary search for the key
	long lo = 0, hi = static_cast<long>(n) - 1;
	while (lo <= hi)
	{
		long mid = lo + (hi - lo) / 2;
		if (keys[mid] == key)
			return static_cast<std::size_t>(mid);
		else if (keys[mid] < key)
			lo = mid + 1;
		else
			hi = mid - 1;
	}
	return static_cast<std::size_t>(lo);
}

std::size_t Node::Search(unsigned int key, bool& found) const
{
	// Binary search for the key
	long lo = 0, hi = static_cast<long>(n) - 1;
	while (lo <= hi)
	{
		long mid = lo + (hi - lo) / 2;
		if (keys[mid] == key)
		{
			found = true;
			return static_cast<std::size_t>(mid);
		}
		else if (keys[mid] < key)
			lo = mid + 1;
		else
			hi = mid - 1;
	}
	found = false;
	return static_cast<std::size_t>(lo);
}

void Node::Insert(std::size_t idx, unsigned int key, std::size_t rid, Node* left, Node* right)
{
	if (n != 0)
	{
		children[n + 1] = children[n];
		for (std::size_t i = n; i-- > idx;)
		{
			keys[i + 1] = keys[i];
			if (rids != NULL)
				rids[i + 1] = rids[i];
			children[i + 1] = children[i];
		}
	}
	keys[idx] = key;
	if (rids != NULL)
		rids[idx] = rid;
	children[idx] = left;
	children[idx + 1] = right;
	n++;
}

void Node::Delete(std::size_t idx, bool del_left)
{
	if (del_left)
		children[idx] = children[idx + 1];
	for (std::size_t i = idx + 1; i < n; i++)
	{
		keys[i - 1] = keys[i];
		if (rids != NULL)
			rids[i - 1] = rids[i];
		children[i] = children[i + 1];
	}
	n--;
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
	if (node.rids != NULL)
	{
		for (std::size_t i = 0; i < node.n; i++)
			os << "(" << node.keys[i] << ", " << node.rids[i] << ") ";
	}
	else
	{
		for (std::size_t i = 0; i < node.n; i++)
			os << node.keys[i] << " ";
	}
	return os;
}