#pragma once
#define UNREACHABLE INT_MAX
#define SINGLETABUTENURE 10 + rand() % 10
#define ALPHATABUTENURE doubleIter + pCenterNum / 10 + rand() % 10
#define BETATABUTENURE doubleIter + (G.vexNum - pCenterNum) / 10 + rand() % 100

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>

using namespace std;

/* 距离nodeNo节点的距离为nodeDis */
typedef struct {
	int nodeNo;
	int nodeDis;
}NodeNoDis;

typedef struct EdgeTag{
	int serverNode;
	NodeNoDis userNode;
	bool operator < (const EdgeTag &test) const {
		return userNode.nodeDis < test.userNode.nodeDis;
	}
}Edge;

typedef struct {
	int vexNum, edgeNum;
	vector<int> vexs;
	vector<vector<int>> edges;
}Graph;

typedef pair<int, int> FDTable;

class MyPCenter
{
public:
	MyPCenter();
	~MyPCenter();

	Graph G;
	vector<vector<NodeNoDis>> NoDisArr;
	int pCenterNum;
	vector<int> serverNodeArr;
	vector<bool> serverNodeFlag;
	vector<FDTable> F, D;
	map<Edge, int> singleTabuTable;
	vector<int> alphaTabuTable;
	vector<int> betaTabuTable;
	vector<int> hisOptSol;
 
	void readFileToCreateGraph(string fileName);
	Edge maxEdge();
	void initSolution();
	void createFDTable();
	void createFDByserverNodeArr(int currNode);
	void createFDByNoDisArr(int currNode);
	FDTable findCountByNoDisArr(int currNode, int count);
	void addFacility(int);
	void removeFacility(int);
	Edge findPair(Edge maxServeEdge);
	void singleTabuSearch(int optSol, int stopCondition);
	void doubleTabuSearch(int optSol, int alphaCondition, int betaCondition);
	bool isAmnesty(Edge feaEdge);


	/* print check */
	void printGraph();
	void printNoDisArr();
	void printInitSol();
	void printFDTable();
};

