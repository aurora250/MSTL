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
    size_t time_;
    __adj_head_node* next_;

    __adj_head_node(std::string name, size_t time) 
        : name_(name), next_(nullptr), time_(time) {}
};

struct __adj_node {
    std::string name_;
    __adj_head_node* head_;
    bool visited_;
    size_t time_;

    __adj_node(std::string name, size_t time) 
        : name_(name), head_(nullptr), visited_(false), time_(time) {}
};

inline bool comp(const __adj_node* x, const __adj_node* y) {
    return x->time_ > y->time_;
}

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

    void add_node(const std::string& nodeName, size_t time) {
        if (graph_.find(nodeName) == graph_.end()) {
            graph_[nodeName] = new __adj_node(nodeName, time);
        }
    }

    void add_edge(const std::string& source, const std::string& dest) {
        if (graph_.find(source) != graph_.end() && graph_.find(dest) != graph_.end()) {
            auto head = graph_.find(dest);
            __adj_head_node* new_node = new __adj_head_node(head->second->name_, head->second->time_);
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

    vector<__adj_node*> get_zero_indegree_nodes(size_t max_count = 2) {
        vector<__adj_node*> target;
        for (auto node = graph_.begin(); node != graph_.end(); ++node) {
            bool have_in_edge = false;
            if (!node->second->visited_) {
                for (auto adj = graph_.begin(); adj != graph_.end(); ++adj) {
                    if (!adj->second->visited_) {
                        for (auto current = adj->second->head_; current; current = current->next_) {
                            if (current->name_ == node->first) {
                                have_in_edge = true;
                                break;
                            }
                        }
                    }
                    if (have_in_edge) break;
                }
            }
            if (!(have_in_edge || node->second->visited_)) {
                target.push_back(node->second);
            }
        }
        if (target.size() > max_count) {
            sort(target.begin(), target.end(), comp);
            target.resize(max_count);
        }
        for (auto it = target.begin(); it != target.end(); ++it) {
            (*it)->visited_ = true;
        }
        return target;
    }

private:
    unordered_map<std::string, __adj_node*> graph_;
};

MSTL_END_NAMESPACE__
#endif // MSTL_ADJACENCY_LIST_H__
