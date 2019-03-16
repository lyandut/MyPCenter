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

/* ����nodeNo�ڵ�ľ���ΪnodeDis */
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
	map<int, int> alphaTabuTable;
	map<int, int> betaTabuTable;

	void readFileToCreateGraph(string fileName);
	Edge maxEdge();
	void initSolution();
	void createFDTable();
	void createFDByserverNodeArr(int currNode);
	void createFDByNoDisArr(int currNode);
	FDTable findCountByNoDisArr(int currNode, int count);
	int addFacility(int);
	int removeFacility(int);
	Edge findPair(int tabuIter);
	void doubleTabuSearch(int optSol, int stopCondition);
	
	/* [TODO] */
	map<Edge, int> singleTabuTable;
	vector<int> hisOptSol;
	void singleTabuSearch(int optSol, int stopCondition);
	bool isAmnesty(Edge feaEdge);

	/* print check */
	void printGraph();
	void printNoDisArr();
	void printInitSol();
	void printFDTable();
};

