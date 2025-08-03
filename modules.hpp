#ifndef BJR_TREE_HPP
#define BJR_TREE_HPP

#include <vector>
#include <set>
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

    bool operator==(const Point& other) const {
        return id == other.id;
    }
};

struct Node {
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
    set<Node*, NodeComparator> exists;

    bool doesPointExist(int id) {
        for (auto node : exists) {
            if (node->point.id == id)
                return true;
        }
        return false;
    }

    void inject(Node* root, Node* newNode) {
        vector<Node*> children = root->children;
        Node* currentNode = nullptr;
        for (int i = 0; i < children.size(); i++) {
            currentNode = children[i];
            if (currentNode->point.dominate(newNode->point)) {
                inject(currentNode, newNode);
                return;
            }
        }

        root->addChild(newNode);
        newNode->changeParent(root);
        currentNode = nullptr;

        for (int i = 0; i < children.size(); i++) {
            currentNode = children[i];
            if (newNode->point.dominate(currentNode->point)) {
                currentNode->changeParent(newNode);
                newNode->addChild(currentNode);
                root->remove(currentNode);
            }
        }
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
            inject(parent, children[i]);
        }

        exists.erase(node);
    }

    Node* findNodeById(int id) {
        for (auto node : exists) {
            if (node->point.id == id)
                return node;
        }
        return nullptr;
    }
};

#endif // BJR_TREE_HPP
