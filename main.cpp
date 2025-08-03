#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
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


int main() {
    cout << "program started!" << endl;
    ifstream infoRead("small.setup");
    string info;
    int dim, timeSteps;
    getline(infoRead, info);
    stringstream ss(info);
    string numPoints, dimention, timeStepsNum, size, type;
    ss >> numPoints >> dimention >> timeStepsNum >> size >> type;
    dim = stoi(dimention);
    timeSteps = stoi(timeStepsNum);
    infoRead.close();

    vector<string> times = readData("small.times");
    vector<string> values = readData("small.input");

    int id = 0;
    vector<string> skyline(timeSteps);
    int start, end;
    string strStart, strEnd, range, coordinates;
	Node* root = new Node();
    root->isRoot = true;
	BJRTree myTree;
    myTree.root = root;
    myTree.depth = 10;
    myTree.lazy = true;
    string temp;

    for (int time = 0; time < timeSteps; time++) {
        id = 0;
        for (int t = 0; t < times.size(); t++) {
            range = times[t];
            ss.clear();
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
                    ss.clear();
                    ss.str(coordinates);
                    Point point;
                    point.id = id;
                    for (int i = 0; i < dim; i++) {
                        ss >> temp;
                        point.values.push_back(stoi(temp));
                    }
                    Node* newNode = new Node();
                    newNode->point = point;
                    
                    if(myTree.lazy){
                        myTree.lazyInject(root, newNode);
                    } else{
                        myTree.inject(root, newNode);
                       
                    }
                    myTree.exists.insert(newNode);
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
            if(i==0){
                skylineStr += "" + to_string(skylinePoints[i]->point.id);
            } else{
                skylineStr += " " + to_string(skylinePoints[i]->point.id);
            }
        }
        skyline[time] = skylineStr;
    }

    ifstream ref("small.refout");
    string check;
    int errors=0;

    for (int i = 0; i < skyline.size(); i++) {
        getline(ref, check);
        if(trim(check) !=  trim(skyline[i])){
            cout << "error" << endl;
            cout << "sky " << i << ": " << skyline[i] << endl;
            cout << "che " << i << ": " << check << endl << endl;
            errors++;
        }
    }

    float error_rate = errors/skyline.size()*100;
    printf("erro rate: %.2f%%\n", error_rate);
    cout << "program finished" << endl;
}
