#ifndef BJR_TREE_HPP
#define BJR_TREE_HPP

#include <iostream>
#include <cmath>
using namespace std;

struct Point{
    int id;
    int dim = 0;
    int* values = new int[dim];
    Point() : id(-1) {}
    Point(int which, int dimention) : id(which), dim(dimention) {}

    bool dominate(Point second) {
        bool res = false;
        for (int i = 0; i < dim; i++) {
            if (values[i] > second.values[i]) return false;
            if (values[i] < second.values[i]) res = true;
        }
        return res;
    }

    bool smart_dominate(Point secondPoint, int* ND_cache){
        if(ND_cache == nullptr){
            cout << "Cache is null" << endl;
            return false;  
        }

        if((ND_cache[id] == ND_cache[secondPoint.id]) && (ND_cache[id] != -1 || ND_cache[secondPoint.id]!=-1)) return false;
        else{
            return this->dominate(secondPoint);
        }
    }

    bool operator==(const Point& other) const {
        return id == other.id;
    }
};

struct Node{
    Node* head_child;
    Node* next;
    Node* parent;
    Point point;
    bool is_root = false;

   void add_child(Node* child){
        if (head_child == nullptr) {
            head_child = child;
        } else {
            Node* current = head_child;
            while (current->next != nullptr) {
                if (current == child) return;
                current = current->next;
            }
            if (current == child) return;

            current->next = child;
        }

        child->next = nullptr;
        child->parent = this;
    }

 

    int children_size(){
        Node* current = head_child;
        int res = 0;
        while(current != nullptr){
            res++;
            current = current->next;
        }
        return res;
    }

    void remove_child(Node* child){
        if(head_child == nullptr || child == nullptr) return;

        if(head_child == child){
            this->head_child = this->head_child->next;
            return;
        }

        Node* current = head_child;
        while(current->next != nullptr){
            if(current->next == child){
                current->next = current->next->next;
                break;
            } 
            current = current->next;
        }
    }


    Node** get_children(){
        int len = this->children_size();
        Node** children = new Node*[len];
        Node* current = this->head_child;
        for(int i=0 ; i<len ; i++){
            children[i] = current;
            current = current->next;
        }
        return children;
    }

    bool operator==(const Node& other) const {
        return point.id == other.point.id;
    }

};

struct BJR_tree{
    Node* root;
    int total_points = 0;
    bool* exists;
    int depth = 0;
    bool lazy = false;
    int* ND_cache = nullptr;
    bool ND_use = false;

    BJR_tree(int num_points) : total_points(num_points) {
        exists = new bool[total_points];
        for(int i=0 ; i<total_points ; i++){
            exists[i] = false;
        }
    }


    bool does_point_exist(int id){
        return exists[id];
    }

    Node* find_node_by_id(int id, Node* root) {
        if (root == nullptr) return nullptr;
        if (root->point.id == id) return root;

        Node* current = root->head_child;
        while (current != nullptr) {
            Node* res = find_node_by_id(id, current);
            if (res != nullptr) return res;
            current = current->next;
        }

        return nullptr;
    }


    int Depth(Node* node){
        int res = 0;
        Node* current = node;
        while(current->parent != nullptr){
            res++;
            current = current->parent;
        }
        return res;
    }

    int Desc(Node* node){
        if(node->head_child == nullptr) return 0;

        int res = 1;
        Node* current = node->head_child;
        while(current != nullptr){
            res++;
            res += Desc(current);
            current = current->next;
        }

        return res;
    }

    void add_to_exists(Node* node){
        int id = node->point.id;
        exists[id] = true;
    }

    void remove_exists(Node* node){
        int id = node->point.id;
        exists[id] = false;
    }

    void inject(Node* root, Node* new_node){
        if (root == new_node) return;

        Node* current = root->head_child;
        while(current != nullptr){
            if(ND_use){
                if (current->point.smart_dominate(new_node->point, ND_cache)) {
                    inject(current, new_node);
                    return;
                }
            } else{
                if (current->point.dominate(new_node->point)) {
                    inject(current, new_node);
                    return;
                }
            }
            current = current->next;
        }

        root->add_child(new_node);
        new_node->parent = root;

        current = root->head_child;
        while(current != nullptr){
            Node* next = current->next;

            if (current == new_node) {
                current = next;
                continue;
            }

            bool should_move = false;
            if(ND_use){
                if (new_node->point.smart_dominate(current->point, ND_cache)) {
                    should_move = true;
                }
            } else{
                if (new_node->point.dominate(current->point)) {
                    should_move = true;
                }
            }

            if(should_move){
                root->remove_child(current);
                current->parent = new_node;
                new_node->add_child(current);
            }

            current = next;
        }
    }

    void lazy_inject(Node* root, Node* new_node){
        if(root->is_root || (Depth(root) < this->depth)){

            float g = INFINITY;
            Node* child = root->head_child;
            Node* chosen;
            while(child != nullptr){
                int d = Desc(child);
                if(ND_use){
                    if(child->point.smart_dominate(new_node->point, ND_cache) && d<g){
                        chosen = child;
                        g = d;
                    }
                } else{
                    if(child->point.dominate(new_node->point) && d<g){
                        chosen = child;
                        g = d;
                    }
                }
            }

            if( chosen != nullptr){
                lazy_inject(chosen, new_node);
                return;
            } else{
                root->add_child(new_node);
                new_node->parent = root;
                child = root->head_child;

                if(root->is_root || Depth(root) < depth){
                    while(child != nullptr){
                        if(ND_use){
                            if(new_node->point.smart_dominate(child->point, ND_cache)){
                                root->remove_child(child);
                                child->parent = new_node;
                                new_node->add_child(child);
                            }
                        } else{
                            if(new_node->point.dominate(child->point)){
                                root->remove_child(child);
                                child->parent = new_node;
                                new_node->add_child(child);
                            }

                        }
                        child = child->next;
                    }
                }
            }

        }
    }

    void eject(Node* example_node) {

        Node* node = find_node_by_id(example_node->point.id, root);

        if (node == nullptr || node->is_root) return;

        Node* parent = node->parent;
        Node* child = node->head_child;

        if (parent != nullptr) {
            parent->remove_child(node);
        }

        while (child != nullptr) {
            Node* next_child = child->next;
            child->parent = nullptr;
            if (lazy) {
                lazy_inject(parent, child);
            } else {
                inject(parent, child);
            }
            child = next_child;
        }

        remove_exists(node);
    }

    void eject_by_id(int id) {
        Node* node = find_node_by_id(id, root);
        if (node == nullptr || node->is_root) {
            cout << "⚠️ Could not find node with id " << id << endl;
            return;
        }

        Node* parent = node->parent;
        Node* child = node->head_child;

        if (parent != nullptr) {
            parent->remove_child(node);
        }

        while (child != nullptr) {
            Node* next_child = child->next;
            child->parent = nullptr;
            if (lazy)
                lazy_inject(parent, child);
            else
                inject(parent, child);
            child = next_child;
        }

        remove_exists(node);
    }


};


#endif