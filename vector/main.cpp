#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include "modules.hpp"

#include <string>
#include <cctype> 
using namespace std;

vector<string> readData(string address) {
    vector<string> content;
    string line;
    ifstream fin(address);
    while (getline(fin, line)) {
        content.push_back(line);
    }
    fin.close();
    return content;
}

string trim(const std::string& str) {
    size_t first = 0;
    while (first < str.size() && std::isspace(str[first])) {
        ++first;
    }

    size_t last = str.size();
    while (last > first && std::isspace(str[last - 1])) {
        --last;
    }

    return str.substr(first, last - first);
}

vector<int> init(string address) {
    ifstream infoRead(address);
    vector<int> res;
    string info;
    int dim, timeSteps, len, type;
    getline(infoRead, info);
    stringstream ss(info);
    string str;
    while (ss >> str) {
        if (str != "small" && str != "medium" && str != "large")
            res.push_back(stoi(str));
    }

    infoRead.close();
    return res;
}

int main() {
    auto start_time = chrono::high_resolution_clock::now();
    string version = "medium";

    cout << "The program started..." << endl;
    vector<int> info;
    vector<string> times, values;

    if (version == "small") {
        info = init("dataset/small/small.setup");
        times = readData("dataset/small/small.times");
        values = readData("dataset/small/small.input");

    } else if (version == "medium") {
        info = init("dataset/medium/medium.setup");
        times = readData("dataset/medium/medium.times");
        values = readData("dataset/medium/medium.input");

    } else {
        info = init("dataset/large/large.setup");
        times = readData("dataset/large/large.times");
        values = readData("dataset/large/large.input");
    }

    int dim = info[1];
    int time_steps = info[2];
    int id = 0;
    vector<string> skyline(time_steps);
    int start, end;
    string strStart, strEnd, range, coordinates;

    Node* root = new Node();
    root->isRoot = true;
    BJRTree myTree;
    myTree.root = root;
    myTree.depth = 10;
    myTree.lazy = true;
    myTree.ND_use = true;

    if (myTree.ND_use) {
        int* ND_cache = new int[values.size()];
        for (int i = 0; i < values.size(); i++) {
            ND_cache[i] = -1;
        }
        myTree.ND_cach = ND_cache;
    }

    string temp;
    stringstream ss;  // Declare stringstream here

    for (int time = 0; time < time_steps; time++) {
        cout << "time: " << time << endl;
        id = 0;
        for (int t = 0; t < times.size(); t++) {
            range = times[t];
            ss.clear();  // Clear stringstream for reuse
            ss.str(range);
            ss >> strStart >> strEnd;
            start = stoi(strStart);
            end = stoi(strEnd);

            if (time >= start && time <= end - 1) {
                if (myTree.doesPointExist(id)) {
                    id++;
                    continue;
                } else {
                    coordinates = values[id];
                    ss.clear();  // Clear stringstream for reuse
                    ss.str(coordinates);
                    Point point;
                    point.id = id;
                    for (int i = 0; i < dim; i++) {
                        ss >> temp;
                        point.values.push_back(stoi(temp));
                    }
                    Node* newNode = new Node();
                    newNode->point = point;

                    if (myTree.lazy) {
                        myTree.lazyInject(root, newNode);
                    } else {
                        myTree.inject(root, newNode);
                    }
                    myTree.addToExists(newNode);
                }
            } else {
                if (time < start) break;
                if ((time >= end || time < start) && myTree.doesPointExist(id)) {
                    Node* deletingNode = myTree.findNodeById(id);
                    myTree.eject(deletingNode);
                }
            }
            id++;
        }

        vector<Node*> skylinePoints = myTree.root->children;
        sort(skylinePoints.begin(), skylinePoints.end(), [](Node* a, Node* b) {
            return a->point.id < b->point.id;
        });

        string skylineStr = "";
        for (int i = 0; i < skylinePoints.size(); i++) {
            if (i == 0) {
                skylineStr += "" + to_string(skylinePoints[i]->point.id);
            } else {
                skylineStr += " " + to_string(skylinePoints[i]->point.id);
            }

            // Lazy update cache if needed
            if (myTree.ND_use)
                myTree.ND_cach[i] = time;
        }
        skyline[time] = skylineStr;
    }

    ifstream ref;
    if (version == "small") {
        ref.open("dataset/small/small.refout");
    } else if (version == "medium") {
        ref.open("dataset/medium/medium.refout");
    } else {
        ref.open("dataset/large/large.refout");
    }
    string check;
    int errors = 0;

    for (int i = 0; i < skyline.size(); i++) {
        getline(ref, check);
        if (trim(check) != trim(skyline[i])) {
            cout << "error" << endl;
            cout << "sky " << i << ": " << skyline[i] << endl;
            cout << "che " << i << ": " << check << endl << endl;
            errors++;
        }
    }

    float error_rate = float(errors) / float(skyline.size()) * 100;
    printf("error rate: %.2f%%\n", error_rate);
    cout << "program finished" << endl;
}
