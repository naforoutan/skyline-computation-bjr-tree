#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <string>
using namespace std;


struct Point{
	vector<int> values;
	int id;
    Point() : id(0) {} 
	Point(int which) : id(which){}

	bool dominate(Point secondPoint){
		bool res = false;
		int len = values.size();
		for(int i=0 ; i<len ; i++){
			if(values[i] > secondPoint.values[i]){
				return false;
			}

			if(values[i] < secondPoint.values[i]){
				res = true;
			}
		}
		return res;
	}

    bool operator==(const Point& other) const {
		return id == other.id;
	}

};

struct Node{
	Node* parent;
	vector<Node*> children;
	Point point;

    Node() : parent(nullptr) {}

	void addChild(Node* node){
		children.push_back(node);
	}

	void changeParent(Node* newParent){
		parent = newParent;
	}

	void remove(Node* child){
		int id = child->point.id;
		for(int i=0 ; i<children.size() ; i++){
			if(children[i]->point.id == id){
				children.erase(children.begin() + i);
				break;
			}
		}
	}


/*	bool operator==(const Node& other) const {
		bool res = true;
		vector<int> thisValues = point.values;
		vector<int> otherValues = other->point.values; 
		for(int i=0; i<thisValues.size() ; i++){
			if(thisValues[i] != otherValues[i]){
				return false;
			}
		}
		return res;
	}*/

    bool operator==(const Node& other) const {
        return point.id == other.point.id; 
    }

};

struct NodeComparator {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->point.id < rhs->point.id;
    }
};


struct BJRTree{
	Node* root;
	set<Node*, NodeComparator> exists;
	
	bool doesPointExist(int id){
		Node* searchNode = new Node();
        Point searchPoint;
        searchPoint.id = id;
		searchNode->point = searchPoint;

		auto it = exists.find(searchNode);
		if (it != exists.end()) {
			return true;
		} else {
			return false;
		}
	}

	void inject(Node* root, Node* newNode){
		vector<Node*> children = root->children;
		Node* currentNode = nullptr;
		for(int i=0 ; i<children.size() ; i++){
			currentNode = children[i];
			if(currentNode->point.dominate(newNode->point)){
				inject(currentNode, newNode);
				return;
			}
		}
		
		root->addChild(newNode);
		newNode->changeParent(root);
		currentNode = nullptr;
		
		for(int i=0 ; i<children.size() ; i++){
			currentNode = children[i];

			if(newNode->point.dominate(currentNode->point)){
				currentNode->changeParent(newNode);
				newNode->addChild(currentNode);
				root->remove(currentNode);
			}
		}
	
	}

	void eject(Node* node){
		Node* parent = node->parent;
		vector<Node*> children = node->children;
		//remove the node as the parent of the children and as the child of the parents
		for(int i=0 ; i<children.size() ; i++){
			children[i]->changeParent(nullptr);
		}
		vector<Node*> siblings = parent->children;
		for(int i=0 ; i<siblings.size() ; i++){
			if(siblings[i] == node){
				siblings.erase(siblings.begin() + i);
				break;
			}
		}

		for(int i=0 ; i<children.size() ; i++){
			inject(parent, children[i]);
		}

	}
	
	Node* findNodeById(int id) {
        for (auto node : exists) {
            if (node->point.id == id) {
                return node;
            }
        }
        return nullptr;
    }
};

int main(){
	cout << "program started!" << endl;
	ifstream infoRead("small.setup");
	string info;
	int dim;
	int timeSteps;

	getline(infoRead, info);
	stringstream ss(info);
	string numPoints, dimention, timeStepsNum, size, type;
	ss >> numPoints >>  dimention >> timeStepsNum >> size >> type;
   	dim = stoi(dimention);
	timeSteps = stoi(timeStepsNum);

	ss.clear();
	infoRead.close();
	ifstream fin("small.input");
	ifstream inTime("small.times");


	int id = 0;
	vector<string> skyline(timeSteps);
	int start, end;
	string strStart, strEnd, range, coordinates;
	Node* root = new Node();
	BJRTree myTree;
	myTree.root = root;
	vector<string> currentCoordinates(dim);
	string temp;


	//set the timesteps correct
	for(int time=0 ; time<timeSteps ; time++){
		id = 0;
		while(getline(inTime, range)){
			fin.clear();
    		fin.seekg(0, std::ios::beg);
			ss.clear();
			ss.str(range);
			ss >> strStart >> strEnd;
			start = stoi(strStart);
			end = stoi(strEnd);

			
			if(time >= start && time <= end ){
				if(myTree.doesPointExist(id)){
					id++;
					continue;
				} else{

					for(int i=0 ; i<id ; i++){
						getline(fin, coordinates);
					}
					getline(fin, coordinates);
                    ss.clear();
                    ss.str(coordinates);
                    Point point;
					point.id = id;
					for(int i=0 ; i<dim ; i++){
                        ss >> temp;
          				point.values.push_back(stoi(temp));
                    }
					Node* newNode = new Node();
					newNode->point = point;
					myTree.inject(root, newNode);
					myTree.exists.insert(newNode);
    
                }
            } else{
	            if(time < start){
                    break;              			
				}
				if((time > end || time < start) && myTree.doesPointExist(id)){
					Node* deletingNode = myTree.findNodeById(id);
					myTree.eject(deletingNode);
				}
			}
			id++;
        }

		vector<Node*> skylinePoints = myTree.root->children;
		string skylineStr = "";
		for(int i=0 ; i<skylinePoints.size() ; i++){
			skylineStr += " "+to_string(skylinePoints[i]->point.id);
		}
		skyline[time] = skylineStr;


		inTime.clear();
		inTime.seekg(0, std::ios::beg);
		//todo clear reading buffers
	}

	cout << skyline[3] << endl;
	cout << skyline[4] << endl;
	cout << "end";


}
