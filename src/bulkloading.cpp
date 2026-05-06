#include "btree.h"
#include "bplustree.h"
#include <vector>

// ==========================================
// BTree::BulkLoading Implementation
// ==========================================
void BTree::BulkLoading(const std::size_t* rids_array, std::size_t count)
{
    // Clear the existing tree if it exists
    if (root != nullptr) {
        _DeleteNode(root);
        root = nullptr;
    }
    depth = 0;

    if (count == 0) return;

    // Structure used to construct the next level
    struct NextLevelData {
        std::vector<unsigned int> keys;
        std::vector<std::size_t> rids;
        std::vector<Node*> children;
    };

    std::size_t max_keys = d - 1;
    std::size_t min_keys = (max_keys + 1) / 2;

    NextLevelData current_data;
    current_data.keys.reserve(count);
    current_data.rids.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        std::size_t rid = rids_array[i];
        current_data.keys.push_back(records[rid].student_id);
        current_data.rids.push_back(rid);
    }
    // Fill with nullptr since leaf nodes have no children
    current_data.children.assign(count + 1, nullptr);

    std::size_t current_depth = 0;

    // Repeat to build up levels until a single root node can contain all keys
    while (current_data.keys.size() > max_keys) {
        std::size_t N = current_data.keys.size();

        // Calculate the number of nodes M to create (Optimize distribution to prevent underflow)
        std::size_t M = (N + max_keys) / (max_keys + 1);
        if (M == 0) M = 1;
        while (M > 1) {
            std::size_t S = N - M + 1; // Actual number of keys to be inserted into nodes (excluding the promoted key)
            if (S / M >= min_keys) {
                break;
            }
            M--;
        }

        std::size_t S = N - M + 1;
        std::size_t base_keys = S / M;
        std::size_t remainder = S % M;

        NextLevelData next_data;
        std::size_t data_idx = 0;
        std::size_t child_idx = 0;

        // Create nodes for the current level
        for (std::size_t i = 0; i < M; ++i) {
            std::size_t keys_in_node = base_keys + (i < remainder ? 1 : 0);
            Node* node = new Node(d, true); // B-Tree nodes all contain data (rid)

            Node* prev_child = current_data.children[child_idx++];
            for (std::size_t j = 0; j < keys_in_node; ++j) {
                Node* curr_child = current_data.children[child_idx++];
                node->Insert(j, current_data.keys[data_idx], current_data.rids[data_idx], prev_child, curr_child);
                prev_child = curr_child;
                data_idx++;
            }
            next_data.children.push_back(node);

            // Promote one key to the upper level if it's not the last node
            if (i < M - 1) {
                next_data.keys.push_back(current_data.keys[data_idx]);
                next_data.rids.push_back(current_data.rids[data_idx]);
                data_idx++;
            }
        }
        current_data = next_data;
        current_depth++;
    }

    // Create the topmost root node
    Node* root_node = new Node(d, true);
    std::size_t N = current_data.keys.size();
    Node* prev_child = current_data.children.empty() ? nullptr : current_data.children[0];
    std::size_t child_idx = 1;

    for (std::size_t j = 0; j < N; ++j) {
        Node* curr_child = current_data.children.empty() ? nullptr : current_data.children[child_idx++];
        root_node->Insert(j, current_data.keys[j], current_data.rids[j], prev_child, curr_child);
        prev_child = curr_child;
    }

    root = root_node;
    depth = current_depth;
}

// ==========================================
// BPlusTree::BulkLoading Implementation
// ==========================================
void BPlusTree::BulkLoading(const std::size_t* rids_array, std::size_t count)
{
    // Clear the existing tree if it exists
    if (root != nullptr) {
        _DeleteNode(root, 0);
        root = nullptr;
    }
    depth = 0;

    if (count == 0) return;

    std::size_t max_keys = d - 1;
    std::size_t min_keys = (max_keys + 1) / 2;
    std::vector<Node*> current_level_nodes;

    // 1. Construct the Leaf Level
    // B+Tree stores all data only in the leaf nodes
    std::size_t M = (count + max_keys - 1) / max_keys; // Number of required leaf nodes
    if (M == 0) M = 1;
    while (M > 1) {
        if (count / M >= min_keys) {
            break;
        }
        M--;
    }

    std::size_t base_keys = count / M;
    std::size_t remainder = count % M;
    std::size_t data_idx = 0;

    for (std::size_t i = 0; i < M; ++i) {
        std::size_t keys_in_node = base_keys + (i < remainder ? 1 : 0);
        Node* node = new Node(d, true); // Leaf node (holds data)
        for (std::size_t j = 0; j < keys_in_node; ++j) {
            std::size_t rid = rids_array[data_idx++];
            node->Insert(j, records[rid].student_id, rid, nullptr, nullptr);
        }
        current_level_nodes.push_back(node);
    }

    std::size_t current_depth = 0;

    // 2. Construct the Internal Level (Bottom-Up)
    // Starting from the leaf level, extract routing keys and build upper nodes level by level
    std::size_t min_children = (d + 1) / 2;

    while (current_level_nodes.size() > 1) {
        std::vector<Node*> next_level_nodes;
        std::size_t L = current_level_nodes.size(); // Number of nodes (children) at the current level

        std::size_t P = (L + d - 1) / d; // Number of parent nodes to construct
        while (P > 1) {
            if (L / P >= min_children) {
                break;
            }
            P--;
        }

        std::size_t base_children = L / P;
        std::size_t remainder_children = L % P;
        std::size_t child_idx = 0;

        for (std::size_t i = 0; i < P; ++i) {
            std::size_t children_in_node = base_children + (i < remainder_children ? 1 : 0);
            Node* node = new Node(d, false); // Internal node (does not hold data, used for routing)

            Node* prev_child = current_level_nodes[child_idx++];
            for (std::size_t j = 0; j < children_in_node - 1; ++j) {
                Node* curr_child = current_level_nodes[child_idx++];

                // Extract the smallest key from the right child's subtree to use as a separator key
                Node* temp = curr_child;
                while (temp && temp->GetN() > 0) {
                    if (temp->Child(0) != nullptr) {
                        temp = temp->Child(0);
                    }
                    else {
                        break;
                    }
                }
                unsigned int min_key = temp->Key(0);

                // The rid of an internal node is meaningless, so insert 0
                node->Insert(j, min_key, 0, prev_child, curr_child);
                prev_child = curr_child;
            }
            next_level_nodes.push_back(node);
        }
        current_level_nodes = next_level_nodes;
        current_depth++;
    }

    // The single remaining node at the top becomes the root of the entire tree
    root = current_level_nodes.empty() ? nullptr : current_level_nodes[0];
    depth = current_depth;
}