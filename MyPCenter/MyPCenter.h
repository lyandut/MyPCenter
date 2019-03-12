#pragma once
#define UNREACHABLE INT_MAX

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

using namespace std;

/* ����nodeNo�ڵ�ľ���ΪnodeDis */
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

	void readFileToCreateGraph(string fileName);
	Edge maxEdge();
	void initSolution();
	void createFDTable();
	
	/* print check */
	void printGraph();
	void printNoDisArr();
	void printInitSol();
};

