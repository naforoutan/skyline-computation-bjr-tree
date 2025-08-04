#include <iostream>
#include <cmath>
using namespace std;

struct Point{
    int id;
    int dim = 0;
    int* values = new int[dim];
    Point() : id(0) {}
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
        if(head_child == nullptr){
            head_child = child;
            return;
        }
        Node* current = head_child;
        while(current->next != nullptr){
            current = current->next;
        }
        current->next = child;
        child->parent = this;
        delete current;
    }

    int children_size(){
        Node* current = head_child;
        int res = 0;
        while(current != nullptr){
            res++;
            current = current->next;
        }
        delete current;
        return res;
    }

    void remove_child(Node* child){
        int id = child->point.id;
        if(head_child->point.id == child->point.id){
            this->head_child = this->head_child->next;
        }
        Node* current = head_child;

        while(current->next != nullptr){
            if(current->next->point.id == child->point.id){
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

struct NodeComparator {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->point.id < rhs->point.id;
    }
};

struct BJR_tree{
    Node* root;
    int total_points = 0;
    bool* exits = new bool[total_points];
    int depth = 0;
    bool lazy = false;
    int* ND_cache = nullptr;
    bool ND_use = false;

    BJR_tree(Node* set_root, int set_total) : root(set_root), total_points(set_total) {}

    bool does_point_exist(int id){
        return exits[id];
    }

    int Depth(Node* node){
        int res = 0;
        Node* current = node;
        while(current->parent != nullptr){
            res++;
            current = current->parent;
        }
        delete current;
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

        delete current;
        return res;
    }

    void add_to_exists(Node* node){
        int id = node->point.id;
        exits[id] = true;
    }

    void remove_exists(Node* node){
        int id = node->point.id;
        exits[id] = false;
    }


    void inject(Node* root, Node* new_node){
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
            if(ND_use){
                if (new_node->point.smart_dominate(current->point, ND_cache)) {
                    current->parent = new_node;
                    new_node->add_child(current);
                    root->remove_child(current);
                }
            } else{
                if (new_node->point.dominate(current->point)) {
                    current->parent = new_node;
                    new_node->add_child(current);
                    root->remove_child(current);
                }
            }
            current = current->next;
        }

        delete current;

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
            
            delete child;
            delete chosen;

        }
    }

    void eject(Node* node){
        Node* parent = node->parent;
        Node* current = node->head_child;
        Node* child = nullptr;

        if(current->point.id == node->point.id){
            parent->head_child = current->next;
        } else{
            while(current->next->point.id != node->point.id && current->next != nullptr){
                current = current->next;
            }
            if(current->next->point.id != node->point.id){
                cout << "error";
                return;
            }

            //considering coordinates
            node = current->next;
            current->next = current->next->next;
            child = node->head_child;

            while(child != nullptr){
                if(lazy){
                    lazy_inject(parent, child);
                } else{
                    inject(parent, child);
                }
                child = child->next;
            }

        }

        delete parent, current, child;

    }

};