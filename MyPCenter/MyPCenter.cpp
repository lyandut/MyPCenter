#include "MyPCenter.h"
#include <ctime>
#include <fstream>
#include <algorithm>

MyPCenter::MyPCenter()
{
	srand(time(NULL));
}


MyPCenter::~MyPCenter()
{
}

void MyPCenter::readFileToCreateGraph(string fileName) {
	ifstream file;
	file.open(fileName, ios::in);
	if (!file.is_open()) {
		cout << "文件读取失败！" << endl;
		return;
	}
	
	/* init Graph */
	file >> G.vexNum >> G.edgeNum >> pCenterNum;
	G.edges = vector<vector<int>>(G.vexNum, vector<int>(G.vexNum, UNREACHABLE));
	for (int i = 0; i < G.vexNum; i++) G.edges[i][i] = 0;
	int sNode, eNode, distance;
	while (!file.eof()) {
		file >> sNode >> eNode >> distance;
		sNode -= 1; eNode -= 1;
		G.edges[sNode][eNode] = G.edges[eNode][sNode] = distance;
	}
	file.close();
	
	/* finish Graph */
	for (int k = 0; k < G.vexNum; k++) 
		for (int i = 0; i < G.vexNum; i++) 
			for (int j = 0; j < G.vexNum; j++)
				if (G.edges[i][k] != UNREACHABLE && G.edges[k][j] != UNREACHABLE && G.edges[i][j] > G.edges[i][k] + G.edges[k][j])
					G.edges[i][j] = G.edges[i][k] + G.edges[k][j];

	/* generate NoDisArr */
	NoDisArr = vector<vector<NodeNoDis>>(G.vexNum);
	for (int i = 0; i < G.vexNum; i++) {
		for (int j = 0; j < G.vexNum; j++) {
			NodeNoDis nd = { j, G.edges[i][j] };
			NoDisArr[i].push_back(nd);
		}
	}
	/* sort NoDisArr */
	for (int i = 0; i < G.vexNum; i++) {
		sort(
			NoDisArr[i].begin(), 
			NoDisArr[i].end(), 
			[](NodeNoDis x, NodeNoDis y) { return x.nodeDis < y.nodeDis; }
		);
	}
}

/* search the MaxServeEdge globally */
Edge MyPCenter::maxEdge()
{
	Edge maxServeEdge;
	int maxEdge = 0;
	for (int i = 0; i < serverNodeArr.size(); ++i) {
		int serverNode = serverNodeArr[i];
		vector<NodeNoDis>::reverse_iterator r_iter = NoDisArr[serverNode].rbegin();
		while (r_iter != NoDisArr[serverNode].rend()) {
			if (!serverNodeFlag[(*r_iter).nodeNo] && (*r_iter).nodeDis >= maxEdge) {
				maxEdge = (*r_iter).nodeDis;
				maxServeEdge.serverNode = serverNode;
				maxServeEdge.userNode = (*r_iter);
			}
			++r_iter;
		}
	}
	return maxServeEdge;
}

void MyPCenter::initSolution()
{
	serverNodeFlag = vector<bool>(G.vexNum, false);
	int initPNode = rand() % G.vexNum;
	serverNodeArr.push_back(initPNode);
	serverNodeFlag[initPNode] = true;
	
	while (serverNodeArr.size() < pCenterNum)
	{
		Edge maxServeEdge = maxEdge();
		NodeNoDis userNode = maxServeEdge.userNode;
		//cout << "maxServeEdge: "
		//	<< maxServeEdge.serverNode << "->" << maxServeEdge.userNode.nodeNo
		//	<< ": " << maxServeEdge.userNode.nodeDis << endl;
		
		vector<int> candNodes;
		for (vector<NodeNoDis>::iterator iter = NoDisArr[userNode.nodeNo].begin();
			iter != NoDisArr[userNode.nodeNo].end(); ++iter)
		{
			if (!serverNodeFlag[(*iter).nodeNo])
				candNodes.push_back((*iter).nodeNo);
			else
				break;
		}
		int newPNode = candNodes[rand() % candNodes.size()];
		serverNodeArr.push_back(newPNode);
		serverNodeFlag[newPNode] = true;
	}
}

void MyPCenter::createFDTable()
{
	for (int i = 0; i < G.vexNum; i++) {
		for (int j = 0; j < NoDisArr[i].size(); j++) {
			if(serverNodeFlag[NoDisArr[i][j].nodeNo]) //第一个服务节点
				
		}
		//F[i].first = NoDisArr[i][0].nodeNo;
		//F[i].second = NoDisArr[i][1].nodeNo;
		//D[i].first = NoDisArr[i][0].nodeDis;
		//D[i].second = NoDisArr[i][1].nodeDis;
		for (int j = 0; j < serverNodeArr.size(); j++) {

		}
	}
}

void MyPCenter::printGraph() {
	cout << "[vexNum]: " << G.vexNum << endl;
	cout << "[edgeNum]: " << G.edgeNum << endl;
	cout << "[pCenteNum] "<< pCenterNum << endl;
	for (int i = 0; i < G.vexNum; i++) {
		for (int j = 0; j < G.vexNum; j++)
			cout << G.edges[i][j] << '\t';
		cout << endl;
	}
}

void MyPCenter::printNoDisArr()
{
	if (NoDisArr.size() == G.vexNum)
		for (int i = 0; i < G.vexNum; i++) {
			/* print min_distance */
			cout << "****** Node " << i << " Min Disrtance ******" << endl;
			cout << i << "->" << NoDisArr[i][1].nodeNo << ": " << NoDisArr[i][1].nodeDis << endl;

			/* print all */
			//for (vector<NodeNoDis>::iterator iter = NoDisArr[i].begin(); iter != NoDisArr[i].end(); ++iter)
			//	cout << i << "->"  << (*iter).nodeNo << ": " << (*iter).nodeDis << endl;
		}
	else
		cout << "[ERROR] NoDisArr.size() != G.vexNum" << endl;
}

void MyPCenter::printInitSol()
{
	vector<int>::iterator iter = serverNodeArr.begin();
	while (iter != serverNodeArr.end()) {
		cout << *iter << endl;
		++iter;
	}
	
	for (int i = 0; i < serverNodeFlag.size(); i++) {
		if (serverNodeFlag[i])
			cout << i << endl;
	}
}

