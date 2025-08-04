#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Point {
    vector<int> values;
    int id;
    Point() : id(0) {}
    Point(int which) : id(which) {}

    bool dominate(Point secondPoint) {
        bool res = false;
        int len = values.size();
        for (int i = 0; i < len; i++) {
            if (values[i] > secondPoint.values[i]) return false;
            if (values[i] < secondPoint.values[i]) res = true;
        }
        return res;
    }


    bool smartDominate(Point secondPoint, int* ND_cache){
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

struct Node {
	bool isRoot = false;
    Node* parent;
    vector<Node*> children;
    Point point;

    Node() : parent(nullptr) {}

    void addChild(Node* node) {
        children.push_back(node);
    }

    void changeParent(Node* newParent) {
        parent = newParent;
    }

    void remove(Node* child) {
        int id = child->point.id;
        for (int i = 0; i < children.size(); i++) {
            if (children[i]->point.id == id) {
                children.erase(children.begin() + i);
                break;
            }
        }
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

struct BJRTree {
    Node* root;
    // set<Node*, NodeComparator> exists;
    vector<Node*> exists;
	int depth = 0;
    bool lazy = false;
    int* ND_cach = nullptr;
    bool ND_use = false;


    bool doesPointExist(int id) {
        for (auto node : exists) {
            if (node->point.id == id)
                return true;
        }
        return false;
    }

    void eject(Node* node) {
        Node* parent = node->parent;
        vector<Node*> children = node->children;

        vector<Node*>& siblings = parent->children;
        for (int i = 0; i < siblings.size(); i++) {
            if (siblings[i] == node) {
                siblings.erase(siblings.begin() + i);
                break;
            }
        }

        for (int i = 0; i < children.size(); i++) {
            if(lazy){
				lazyInject(parent, children[i]);
			}else{	
				inject(parent, children[i]);
			}

        }

        // exists.erase(node);
        removeFromExists(node);
    }
	
	int Depth(Node* node){
		int res = 0;
		Node* currentNode = node;
		while(!currentNode->isRoot){
			currentNode = currentNode->parent;
			res++;
		}
		return res;
	}

	int Desc(Node* node){
		if(node->children.empty()){
			return 0;
		}

		int res = 0;
		res += node->children.size();
		for(int i=0 ; i<node->children.size() ; i++){
			res += Desc(node->children[i]);
		}

		return res;
	}

    void inject(Node* root, Node* newNode) {
        vector<Node*> children = root->children;
        Node* currentNode = nullptr;
        for (int i = 0; i < children.size(); i++) {
            currentNode = children[i];
            if(ND_use){
                if (currentNode->point.smartDominate(newNode->point, ND_cach)) {
                    inject(currentNode, newNode);
                    return;
                }
            } else{
                if (currentNode->point.dominate(newNode->point)) {
                    inject(currentNode, newNode);
                    return;
                }
            }
            
        }

        root->addChild(newNode);
        newNode->changeParent(root);
        currentNode = nullptr;

        for (int i = 0; i < children.size(); i++) {
            currentNode = children[i];
            if(ND_use){
                if (newNode->point.smartDominate(currentNode->point, ND_cach)) {
                    currentNode->changeParent(newNode);
                    newNode->addChild(currentNode);
                    root->remove(currentNode);
                }
            } else{
                if (newNode->point.dominate(currentNode->point)) {
                    currentNode->changeParent(newNode);
                    newNode->addChild(currentNode);
                    root->remove(currentNode);
                }
            }
            
        }
    }

	void lazyInject(Node* root, Node* newNode){
        
		if(root->isRoot || Depth(root) < depth){
			vector<Node*> children = root->children;
			float g = INFINITY;
			Node* chosenNode = nullptr;
			for(auto child : children){
                int d = Desc(child);
                if(ND_use){
                    if(child->point.smartDominate(newNode->point, ND_cach) && d < g){
                        chosenNode = child;
                        g = d;
                    }
                } else{
                    if(child->point.dominate(newNode->point) && d < g){
                        chosenNode = child;
                        g = d;
                    }
                }
				
			}

            if (chosenNode != nullptr) {
                lazyInject(chosenNode, newNode);
                return;
            } else {
                root->children.push_back(newNode);
                newNode->changeParent(root);

                if (root->isRoot || Depth(root) < depth) {
                    vector<Node*> toReparent;
                    for (auto child : root->children) {
                        if(ND_use){
                            if (newNode->point.smartDominate(child->point, ND_cach)) {
                                toReparent.push_back(child);
                            }
                        } else{
                            if (newNode->point.dominate(child->point)) {
                                toReparent.push_back(child);
                            }
                        }
                        
                    }

                    for (auto child : toReparent) {
                        newNode->addChild(child);
                        child->parent = newNode;
                        root->remove(child);
                    }
                }
            }

		}
    
	}



    Node* findNodeById(int id) {
        for (auto node : exists) {
            if (node->point.id == id)
                return node;
        }
        return nullptr;
    }

    void addToExists(Node* node) {
        for (auto n : exists)
            if (n->point.id == node->point.id)
                return;
        exists.push_back(node);
    }

    void removeFromExists(Node* node) {
        for (auto it = exists.begin(); it != exists.end(); ++it) {
            if ((*it)->point.id == node->point.id) {
                exists.erase(it);
                break;
            }
        }
    }

};