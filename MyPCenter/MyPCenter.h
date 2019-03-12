#pragma once
#define UNREACHABLE INT_MAX

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

// 距离nodeNo节点的距离为distance
typedef struct {
	int nodeNo;
	int nodeDis;
}NodeNoDis;

typedef struct {
	int vexNum, edgeNum;
	vector<int> vexs;
	vector<vector<int>> edges;
}Graph;

class MyPCenter
{
public:
	MyPCenter();
	~MyPCenter();

	Graph G;
	vector<NodeNoDis> ND;
	int pCenterNum;

	void readFileToCreateGraph(string fileName);
	void printGraph();
};

