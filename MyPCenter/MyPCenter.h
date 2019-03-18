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
	vector<int> serverNodeFlag;
	vector<FDTable> F, D;
	vector<int> alphaTabuTable;
	vector<int> betaTabuTable;
	vector<int> hisOptSol;

	void readFileToCreateGraph(string fileName);
	Edge maxEdge();
	Edge maxEdgeWithoutFD();
	void initSolution();
	void createFDTable();
	void createFDByserverNodeArr(int currNode);
	void createFDByNoDisArr(int currNode);
	FDTable findCountByNoDisArr(int currNode, int count);
	int addFacility(int);
	int removeFacility(int);
	Edge findPair(int tabuIter);
	void doubleTabuSearch(int optSol, int stopCondition);
	bool isAmnesty(Edge feaEdge);
	
	/* [TODO] */	
	map<Edge, int> singleTabuTable;
	void singleTabuSearch(int optSol, int stopCondition);

	/* print check */
	void printGraph();
	void printNoDisArr();
	void printOptSol();
	void printFDTable();
};

