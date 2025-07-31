#include <iostream>
#include <vector>
using namespace std;


struct Point{
	vector<int> values;
	int id;

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
};

struct Node{
	Node* parent;
	vector<Node*> children;
	Point point;

};


struct BJRTree{
	Node* root;
	

};
