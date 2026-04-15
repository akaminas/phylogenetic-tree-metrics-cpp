

```cpp
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

struct Node {
    int left = -1;
    int right = -1;
};

class Tree {
public:
    explicit Tree(const std::string& filename) {
        load_from_edge_list(filename);
        root_ = find_root();
        validate_tree();
    }

    int root() const {
        return root_;
    }

    int tip_count() const {
        int count = 0;
        for (const auto& node : nodes_) {
            if (node.left == -1 && node.right == -1) {
                ++count;
            }
        }
        return count;
    }

    int internal_node_count() const {
        int count = 0;
        for (const auto& node : nodes_) {
            const bool has_left = node.left != -1;
            const bool has_right = node.right != -1;
            if (has_left || has_right) {
                ++count;
            }
        }
        return count;
    }

    int colless_index() const {
        int total = 0;
        for (std::size_t i = 0; i < nodes_.size(); ++i) {
            const auto& node = nodes_[i];
            if (node.left != -1 && node.right != -1) {
                const int left_tips = count_tips(node.left);
                const int right_tips = count_tips(node.right);
                total += std::abs(left_tips - right_tips);
            }
        }
        return total;
    }

private:
    std::vector<Node> nodes_;
    std::vector<bool> appears_as_child_;
    int root_ = -1;

    void ensure_size(int id) {
        if (id < 0) {
            throw std::runtime_error("Node labels must be non-negative integers.");
        }

        const std::size_t needed = static_cast<std::size_t>(id) + 1;
        if (needed > nodes_.size()) {
            nodes_.resize(needed);
            appears_as_child_.resize(needed, false);
        }
    }

    void load_from_edge_list(const std::string& filename) {
        std::ifstream fin(filename);
        if (!fin) {
            throw std::runtime_error("Failed to open input file: " + filename);
        }

        int parent = -1;
        int child = -1;

        while (fin >> parent >> child) {
            ensure_size(parent);
            ensure_size(child);

            Node& p = nodes_[parent];

            if (p.left == -1) {
                p.left = child;
            } else if (p.right == -1) {
                p.right = child;
            } else {
                throw std::runtime_error(
                    "Input is not binary: node " + std::to_string(parent) +
                    " has more than two children."
                );
            }

            if (appears_as_child_[child]) {
                throw std::runtime_error(
                    "Invalid tree: node " + std::to_string(child) +
                    " appears as a child more than once."
                );
            }

            appears_as_child_[child] = true;
        }

        if (nodes_.empty()) {
            throw std::runtime_error("Input tree is empty.");
        }
    }

    int find_root() const {
        int root = -1;

        for (std::size_t i = 0; i < nodes_.size(); ++i) {
            const bool has_descendants =
                (nodes_[i].left != -1 || nodes_[i].right != -1);

            if (has_descendants && !appears_as_child_[i]) {
                if (root != -1) {
                    throw std::runtime_error("Invalid tree: multiple roots detected.");
                }
                root = static_cast<int>(i);
            }
        }

        if (root == -1) {
            throw std::runtime_error("Invalid tree: no root detected.");
        }

        return root;
    }

    void validate_tree() const {
        for (std::size_t i = 0; i < nodes_.size(); ++i) {
            const bool has_left = nodes_[i].left != -1;
            const bool has_right = nodes_[i].right != -1;

            if (has_left != has_right) {
                throw std::runtime_error(
                    "Invalid rooted binary tree: node " + std::to_string(i) +
                    " has only one child."
                );
            }
        }

        std::vector<bool> visited(nodes_.size(), false);
        dfs_check(root_, visited);

        for (std::size_t i = 0; i < nodes_.size(); ++i) {
            const bool exists =
                (nodes_[i].left != -1 || nodes_[i].right != -1 || appears_as_child_[i]);

            if (exists && !visited[i]) {
                throw std::runtime_error(
                    "Invalid tree: disconnected node detected: " + std::to_string(i)
                );
            }
        }
    }

    void dfs_check(int node_id, std::vector<bool>& visited) const {
        if (node_id < 0) {
            return;
        }

        if (visited[node_id]) {
            throw std::runtime_error(
                "Invalid tree: cycle detected at node " + std::to_string(node_id)
            );
        }

        visited[node_id] = true;

        if (nodes_[node_id].left != -1) {
            dfs_check(nodes_[node_id].left, visited);
        }
        if (nodes_[node_id].right != -1) {
            dfs_check(nodes_[node_id].right, visited);
        }
    }

    int count_tips(int node_id) const {
        if (node_id == -1) {
            return 0;
        }

        const Node& node = nodes_[node_id];
        if (node.left == -1 && node.right == -1) {
            return 1;
        }

        return count_tips(node.left) + count_tips(node.right);
    }
};

int main(int argc, char* argv[]) {
    try {
        const std::string input_file =
            (argc > 1) ? argv[1] : "data/edges.txt";

        Tree tree(input_file);

        std::cout << "Root node: " << tree.root() << '\n';
        std::cout << "Number of tips: " << tree.tip_count() << '\n';
        std::cout << "Number of internal nodes: " << tree.internal_node_count() << '\n';
        std::cout << "Colless index: " << tree.colless_index() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
