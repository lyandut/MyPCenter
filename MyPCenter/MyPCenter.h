#pragma once
#define UNREACHABLE INT_MAX

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

typedef struct {
	int serverNode;
	NodeNoDis userNode;
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
	vector<Edge> singleTabuTable;
	vector<vector<int>> doubleTabuTable;
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
	void doubleTabuSearch(int optSol, int alphaCondition, int bateCondition);
	
	/* print check */
	void printGraph();
	void printNoDisArr();
	void printInitSol();
	void printFDTable();
};

