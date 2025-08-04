#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

struct Point {
    int id;
    vector<int> values;
    int activate_time;
    int deactivate_time;
};

struct Node {
    Point point;
    Node* parent = nullptr;
    vector<Node*> children;

    Node(const Point& p) : point(p) {}

    void addChild(Node* child) {
        children.push_back(child);
        child->parent = this;
    }

    void removeChild(Node* child) {
        children.erase(remove(children.begin(), children.end(), child), children.end());
    }
};

// بررسی اینکه a نقطه b را dominate می‌کند یا نه
bool dominates(const Point& a, const Point& b) {
    bool strictly_better = false;
    for (int i = 0; i < a.values.size(); ++i) {
        if (a.values[i] > b.values[i]) return false;
        if (a.values[i] < b.values[i]) strictly_better = true;
    }
    return strictly_better;
}

void inject(Node* root, Node* newNode) {
    for (Node* child : root->children) {
        if (dominates(child->point, newNode->point)) {
            inject(child, newNode);
            return;
        }
    }

    root->addChild(newNode);

    vector<Node*> toReattach;
    for (Node* child : root->children) {
        if (child == newNode) continue;
        if (dominates(newNode->point, child->point)) {
            toReattach.push_back(child);
        }
    }

    for (Node* ch : toReattach) {
        root->removeChild(ch);
        newNode->addChild(ch);
    }
}

void eject(Node* root, Node* toRemove) {
    Node* parent = toRemove->parent;
    vector<Node*> children = toRemove->children;

    parent->removeChild(toRemove);
    delete toRemove;

    for (Node* ch : children) {
        inject(parent, ch);
    }
}

int main() {
    ifstream setup("medium.setup");
    ifstream input("medium.input");
    ifstream times("medium.times");
    ofstream fout("medium.out");

    int num_points, num_dimensions;
    setup >> num_points >> num_dimensions;

    vector<Point> all_points(num_points);
    for (int i = 0; i < num_points; ++i) {
        all_points[i].id = i;
        all_points[i].values.resize(num_dimensions);
        for (int j = 0; j < num_dimensions; ++j) {
            input >> all_points[i].values[j];
        }
    }

    for (int i = 0; i < num_points; ++i) {
        times >> all_points[i].activate_time >> all_points[i].deactivate_time;
    }

    int MAX_TIME = 0;
    for (auto& p : all_points)
        MAX_TIME = max(MAX_TIME, p.deactivate_time);

    Node* root = new Node({-1, {}, -1, -1});  // گره ریشه (O)
    vector<Node*> point_nodes(num_points, nullptr);

    for (int t = 0; t <= MAX_TIME; ++t) {
        for (auto& p : all_points) {
            if (p.activate_time == t) {
                Node* node = new Node(p);
                point_nodes[p.id] = node;
                inject(root, node);
            }
        }

        for (auto& p : all_points) {
            if (p.deactivate_time == t) {
                Node* node = point_nodes[p.id];
                if (node) {
                    eject(root, node);
                    point_nodes[p.id] = nullptr;
                }
            }
        }

        vector<int> skyline_ids;
        for (Node* ch : root->children)
            skyline_ids.push_back(ch->point.id);
        sort(skyline_ids.begin(), skyline_ids.end());

        for (int id : skyline_ids)
            fout << id << " ";
        fout << "\n";
    }


    ifstream one("medium.out");
    ifstream two("medium.refout");

    string me, ref;
    int line = 0;
    while(getline(one, me) && getline(two, ref)){
        line++;

        if(me != ref){
            cout << "error " << line << endl << "one: " <<  me << endl << "two: " << ref << endl << endl;
            cout << me << endl;
        }

    }

    return 0;
}
