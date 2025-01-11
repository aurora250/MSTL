#ifndef MSTL_ADJACENCY_LIST_H__
#define MSTL_ADJACENCY_LIST_H__
#include <string>
#include <memory>
#include "vector.hpp"
#include "hash_map.hpp"
#include "sort.hpp"
MSTL_BEGIN_NAMESPACE__

struct __adj_head_node {
    std::string name_;
    __adj_head_node* next_;

    __adj_head_node(std::string name) 
        : name_(name), next_(nullptr) {}
};

struct __adj_node {
    std::string name_;
    __adj_head_node* head_;

    __adj_node(std::string name) 
        : name_(name), head_(nullptr) {}
};

class adjacency_list {
private:
    bool dfs(const std::string& node, unordered_map<std::string, bool>& visited,
        unordered_map<std::string, std::string>& parent) {
        visited[node] = true;
        auto current = graph_[node]->head_;
        while (current) {
            std::string neighbor = current->name_;
            if (!visited[neighbor]) {
                parent[neighbor] = node;
                if (dfs(neighbor, visited, parent)) 
                    return true;
            }
            else if (parent[node] != neighbor) 
                return true;
            current = current->next_;
        }
        return false;
    }

public:
    adjacency_list() = default;
    ~adjacency_list() {
        for (auto it = graph_.begin(); it != graph_.end(); ++it) {
            delete it->second;
        }
    }

    void add_node(const std::string& nodeName) {
        if (graph_.find(nodeName) == graph_.end()) {
            graph_[nodeName] = new __adj_node(nodeName);
        }
    }

    void add_edge(const std::string& source, const std::string& dest) {
        if (graph_.find(source) != graph_.end() && graph_.find(dest) != graph_.end()) {
            auto head = graph_.find(dest);
            __adj_head_node* new_node = new __adj_head_node(head->second->name_);
            new_node->next_ = graph_[source]->head_;
            graph_[source]->head_ = new_node;
        }
    }

    void print_graph() const {
        for (auto node = graph_.const_begin(); node != graph_.const_end(); ++node) {
            std::cout << node->first << " -> ";
            auto current = node->second->head_;
            while (current) {
                std::cout << current->name_;
                if (current->next_) std::cout << " -> ";
                else std::cout << " -> NULL";
                current = current->next_;
            }
            if (!node->second->head_) std::cout << "NULL";
            std::cout << std::endl;
        }
    }
    bool have_cycle() {
        unordered_map<std::string, bool> visited;
        unordered_map<std::string, std::string> parent;
        for (auto node = graph_.const_begin(); node != graph_.const_end(); ++node) {
            if (!visited[node->first]) {
                if (dfs(node->first, visited, parent)) return true;
            }
        }
        return false;
    }
private:
    unordered_map<std::string, __adj_node*> graph_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_ADJACENCY_LIST_H__
