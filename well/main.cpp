#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "modules.hpp"
#include <chrono>
using namespace std;


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

vector<int> init(string address){
    ifstream infoRead(address);
    vector<int> res;
    string info;
    int dim, timeSteps, len, type;
    getline(infoRead, info);
    stringstream ss(info);
    string str;
    while(ss >> str){
        if(str != "small" && str != "medium" && str != "large")
            res.push_back(stoi(str));
    }

    infoRead.close();
    return res;
}

vector<Node*> bubble_sort(vector<Node*> vec) {
    int len = vec.size();
    
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len - 1 - i; j++) {
            if (vec[j]->point.id > vec[j + 1]->point.id) {
                swap(vec[j], vec[j + 1]);
            }
        }
    }
    return vec;
}


int main(){
    auto start_time = chrono::high_resolution_clock::now();
    string version = "large";

    cout << "The program started..." << endl;
    vector<int> info;
    vector<string> times, values;

    if(version == "small"){
        info = init("dataset/small/small.setup");
        times = readData("dataset/small/small.times");
        values = readData("dataset/small/small.input");

    } else if(version == "medium"){
        info = init("dataset/medium/medium.setup");
        times = readData("dataset/medium/medium.times");
        values = readData("dataset/medium/medium.input");

    } else{
        info = init("dataset/large/large.setup");
        times = readData("dataset/large/large.times");
        values = readData("dataset/large/large.input");
    }
    
    int dim = info[1];
    int time_steps = info[2];
    int id = 0;
    vector<string> skyline(time_steps);
    int start, end;
    string start_str, str_end, range, coordinates;

    Node* root = new Node();
    root->is_root = true;
	BJR_tree* my_tree = new BJR_tree(values.size());
    my_tree->root = root;
    my_tree->total_points = values.size();
    my_tree->lazy = false;
    my_tree->ND_use = false;
    my_tree->depth = 10;

    if(my_tree->ND_use){
        int* ND_cache = new int[values.size()];
        for(int i=0 ; i<values.size() ; i++){
            ND_cache[i] = -1;
        }
        my_tree->ND_cache = ND_cache;
    }

    stringstream ss("");
    string temp;

    for(int time = 0 ; time<time_steps ; time++){
        cout << "time: " << time  << endl;
        id = 0;
        for(int t=0 ; t<times.size() ; t++){
            range = times[t];
            ss.clear();
            ss.str(range);
            ss >> temp;
            start = stoi(temp);
            ss >> temp;
            end = stoi(temp);

            if(time >= start && time < end){
                if(my_tree->does_point_exist(id)){
                    id++;
                    continue;
                } else{
                    coordinates = values[id];
                    ss.clear();
                    ss.str(coordinates);
                    Point point;
                    point.id = id;
                    point.dim = dim;
                    for (int i = 0; i < dim; i++) {
                        ss >> temp;
                        point.values[i] = stoi(temp);
                    }
                    Node* newNode = new Node();
                    newNode->point = point;
                    
                    if(my_tree->lazy){
                        my_tree->lazy_inject(root, newNode);
                    } else{
                        my_tree->inject(root, newNode);
                       
                    }
                    my_tree->add_to_exists(newNode);
                }
            } else{
                if (time < start) break;
                if ((time >= end || time < start) && my_tree->does_point_exist(id)) {
                    //todo
                    Point deleting_point;
                    Node* deleting_node = new Node();
                    deleting_point.id = id;
                    deleting_node->point = deleting_point;
                    my_tree->eject(deleting_node);
                    delete deleting_node, deleting_point;
                }
            }
            id++;
        }

        Node** skylinePoints = my_tree->root->get_children();
        int len = my_tree->root->children_size();
        vector<Node*> skyline_vec(skylinePoints, skylinePoints + len);
        skyline_vec = bubble_sort(skyline_vec);
        string skyline_str = "";
        for (int i = 0; i < skyline_vec.size(); i++) {
            if(i==0){
                skyline_str += "" + to_string(skyline_vec[i]->point.id);
            } else{
                skyline_str += " " + to_string(skyline_vec[i]->point.id);
            }

            //nd
            if(my_tree->ND_use)
                my_tree->ND_cache[i] = time;
        }
        skyline[time] = skyline_str;

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
    int errors=0;

    for (int i = 0; i < skyline.size() ; i++) {
        getline(ref, check);
        if(trim(check) !=  trim(skyline[i])){
            cout << "error" << endl;
            cout << "sky " << i << ": " << skyline[i] << endl;
            cout << "ref " << i << ": " << check << endl << endl;
            errors++;
        }
    }

    float error_rate = float(errors)/float(skyline.size())*100;
    printf("error rate: %.2f%%\n", error_rate);
    cout << "program finished" << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "Execution time: " << duration.count() << " ms" << endl;
    return 0;
}