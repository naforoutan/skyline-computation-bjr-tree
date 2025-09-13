#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "modules.hpp"
#include <chrono>
#include <cmath>
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

int partition(vector<Node*>& vec, int low, int high) {
    int pivot = vec[high]->point.id;
    int i = (low - 1);
    
    for (int j = low; j < high; j++) {
        if (vec[j]->point.id < pivot) {
            i++;
            swap(vec[i], vec[j]);
        }
    }
    swap(vec[i + 1], vec[high]);
    return i + 1;
}

void quick_sort(vector<Node*>& vec, int low, int high) {
    if (low < high) {
        int pi = partition(vec, low, high);

        quick_sort(vec, low, pi - 1);
        quick_sort(vec, pi + 1, high);
    }
}


void print(string address, vector<string> data){
    ofstream fout;
    fout.open(address);
    int len = data.size();
    if(fout.is_open()){
        for(const auto& skyline : data){
            fout << skyline << endl;
        }
        fout.close();
        cout << "Data printed in .out" << endl;
    } else{
        cout << "Error: Unable to open file." << endl;
    }
}

int main(){
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

    auto start_time = chrono::high_resolution_clock::now();
    
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

    if (my_tree->ND_use) {
        int* ND_cache = new int[values.size()];
        for (int i = 0; i < values.size(); i++) { 
            ND_cache[i] = -1;
        }
        my_tree->ND_cache = ND_cache;
    }


    stringstream ss("");
    string temp;
    vector<vector<int>> injections(time_steps);
    vector<vector<int>> ejections(time_steps);

    vector<Node*> all_nodes(values.size());
    for (int i = 0; i < values.size(); i++) {
        all_nodes[i] = new Node();
        all_nodes[i]->point.id = i;
        all_nodes[i]->point.dim = dim;
        all_nodes[i]->point.values = new int[dim];
        
        stringstream ss(values[i]);
        string temp;
        for (int j = 0; j < dim; j++) {
            ss >> temp;
            all_nodes[i]->point.values[j] = stoi(temp);
        }
    }

    for (int id = 0; id < times.size(); id++) {
        stringstream ss(times[id]);
        int start, end;
        ss >> start >> end;
        
        for (int time = start; time < min(end, time_steps); time++) {
            injections[time].push_back(id);
        }
        if (end < time_steps) {
            ejections[end].push_back(id);
        }
    }

    for(int time=0 ; time<time_steps ; time++){
        if (time % 1000 == 0) {
            cout << "Processing time: " << time << "/" << time_steps << endl;
        }

        for (int id : injections[time]) {
            if(!my_tree->does_point_exist(id)) {
                if (my_tree->lazy) {
                    my_tree->lazy_inject(root, all_nodes[id]);
                } else {
                    my_tree->inject(root, all_nodes[id]);
                }
                my_tree->add_to_exists(all_nodes[id]);
            }
        }
        
        for (int id : ejections[time]) {
            if(my_tree->does_point_exist(id)) {
                my_tree->eject(all_nodes[id]);
            }
        }

        int skyline_size = root->children_size();
        vector<Node*> skyline_vec;
        skyline_vec.reserve(skyline_size);
        
        Node* current = root->head_child;
        while (current != nullptr) {
            skyline_vec.push_back(current);
            current = current->next;
        }
        
        quick_sort(skyline_vec, 0, skyline_vec.size() - 1);
        
        string skyline_str;
        for (int i = 0; i < skyline_vec.size(); i++) {
            if(i > 0) skyline_str += " ";
            skyline_str += to_string(skyline_vec[i]->point.id);
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
    printf("Error rate: %.f%%\n", error_rate);
    cout << "program finished" << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
    cout << "Execution time: " << duration.count() << " ms" << endl;

    if(version == "small"){
        print("dataset/small/small.out", skyline);
    } else if(version == "medium"){
        print("dataset/medium/medium.out", skyline);
    } else{
        print("dataset/large/large.out", skyline);
    }
    cout << "Done for tree size: " << version << ", lazy: " << my_tree->lazy << ", nd: " << my_tree->ND_use << endl;
    return 0;
}