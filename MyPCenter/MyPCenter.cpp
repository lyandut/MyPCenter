#include "MyPCenter.h"
#include <fstream>
#include <algorithm>

MyPCenter::MyPCenter()
{
}


MyPCenter::~MyPCenter()
{
}

void MyPCenter::readFileToCreateGraph(string fileName) {
	ifstream file;
	file.open(fileName, ios::in);
	if (!file.is_open()) {
		cout << "�ļ���ȡʧ�ܣ�" << endl;
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
	F = D = vector<FDTable>(G.vexNum);
	for (int i = 0; i < G.vexNum; i++) {
		createFDByserverNodeArr(i);
	}
	//printFDTable();
	
	//for (int i = 0; i < G.vexNum; i++) {
	//	createFDByNoDisArr(i);
	//}
	//printFDTable();
}

void MyPCenter::createFDByserverNodeArr(int i)
{
	/* ����serverNodeArr���ҳ�������̵�ǰ���� */
	int firNode, secNode;
	/* ��Ŀ������������ţ�����*/
	G.edges[i][serverNodeArr[0]] < G.edges[i][serverNodeArr[1]] ?
		(firNode = serverNodeArr[0], secNode = serverNodeArr[1]) :
		(firNode = serverNodeArr[1], secNode = serverNodeArr[0]);
	for (int j = 2; j < serverNodeArr.size(); j++) {
		if (G.edges[i][serverNodeArr[j]] < G.edges[i][firNode]) {
			secNode = firNode;
			firNode = serverNodeArr[j];
		}
		else if (G.edges[i][serverNodeArr[j]] < G.edges[i][secNode]) {
			secNode = serverNodeArr[j];
		}
		else
			continue;
	}
	F[i].first = firNode;
	D[i].first = G.edges[i][firNode];
	F[i].second = secNode;
	D[i].second = G.edges[i][secNode];
}

void MyPCenter::createFDByNoDisArr(int i)
{
	/* �����������NoDisArr[i]���ҳ�ǰ�����ķ���ڵ� */
	FDTable tmp1, tmp2;
	for (int i = 0; i < G.vexNum; i++) {
		tmp1 = findCountByNoDisArr(i, 1);
		tmp2 = findCountByNoDisArr(i, 2);
		F[i].first = tmp1.first;
		D[i].first = tmp1.second;
		F[i].second = tmp2.first;
		D[i].second = tmp2.second;
	}
}

FDTable MyPCenter::findCountByNoDisArr(int i, int count)
{
	/* �����������NoDisArr[i]���ҳ���count������ڵ� */
	/* j: ����i��j���ڵ� */
	FDTable fd(i, 0);
	for (int j = 0; j < NoDisArr[i].size(); j++) {
		if (serverNodeFlag[NoDisArr[i][j].nodeNo] && count == 1) {
			fd = make_pair(NoDisArr[i][j].nodeNo, NoDisArr[i][j].nodeDis);
			count--;
		}
		else if (serverNodeFlag[NoDisArr[i][j].nodeNo]) {
			count--;
		}
		else if (!count)
			break;
		else
			continue;
	}
	return fd;
}

int MyPCenter::addFacility(int newServerNode)
{
	int SC = 0;
	serverNodeArr.push_back(newServerNode);
	serverNodeFlag[newServerNode] = true;
	for (int i = 0; i < G.vexNum; i++) {
		if (G.edges[i][newServerNode] < D[i].first) {
			D[i].second = D[i].first;
			F[i].second = F[i].first;
			D[i].first = G.edges[i][newServerNode];
			F[i].first = newServerNode;
		}
		else if (G.edges[i][newServerNode] < D[i].second) {
			D[i].second = G.edges[i][newServerNode];
			F[i].second = newServerNode;
		}
		else 
			continue;
		
		if (D[i].first > SC)
			SC = D[i].first;
	}
	return SC;
}

int MyPCenter::removeFacility(int rmServerNode)
{
	int SC = 0;
	vector<int>::iterator iter = serverNodeArr.begin();
	while (iter != serverNodeArr.end()) {
		if (*iter == rmServerNode) {
			iter = serverNodeArr.erase(iter);
			break;
		}
		++iter;
	}
	serverNodeFlag[rmServerNode] = false;
	for (int i = 0; i < G.vexNum; i++) {
		if (F[i].first == rmServerNode) {
			D[i].first = D[i].second;
			F[i].first = F[i].second;
			// find_next
			FDTable tmp = findCountByNoDisArr(i, 2);
			D[i].second = tmp.second;
			F[i].second = tmp.first;
		}
		else if (F[i].second == rmServerNode) {
			// find_next
			FDTable tmp = findCountByNoDisArr(i, 2);
			D[i].second = tmp.second;
			F[i].second = tmp.first;
		}
		else
			continue;
		
		if (D[i].first > SC)
			SC = D[i].first;
	}
	return SC;
}

Edge MyPCenter::findPair(int tabuIter)
{
	Edge maxServeEdge = maxEdge();
	int maxServerNode = maxServeEdge.serverNode;
	int maxUserNode = maxServeEdge.userNode.nodeNo;
	int maxDistance = maxServeEdge.userNode.nodeDis;
	vector<int> candNewNodes;
	vector<NodeNoDis>::iterator iter = NoDisArr[maxUserNode].begin();
	while (iter != NoDisArr[maxUserNode].end() && !serverNodeFlag[(*iter).nodeNo]) {
		candNewNodes.push_back((*iter).nodeNo);
		++iter;
	}
	/* ̽�������Ϊ����ڵ���û��ڵ㼯�� candNewNodes */
	vector<Edge> L;
	vector<Edge> tabuL;
	for (int i = 0; i < candNewNodes.size(); i++) {
		int newPNode = candNewNodes[i];
		addFacility(newPNode);
		/* ��ÿһ������ڵ���M */
		map<int, int> M;
		for (int j = 0; j < serverNodeArr.size(); j++) {
			M[serverNodeArr[j]] = 0;
		}
		for (int j = 0; j < G.vexNum; j++) {
			if (M.at(F[j].first) < min(G.edges[newPNode][j], D[j].second))
				M[F[j].first] = min(G.edges[newPNode][j], D[j].second);
		}
		/* �Ƚ�M��maxDistance��ȷ�����ڽ����ķ���ڵ���µ�Ŀ�꺯��ֵ */
		int nodeToAdd = newPNode;
		vector<int>::iterator iter = serverNodeArr.begin();
		while (iter != serverNodeArr.end() && (*iter)!= nodeToAdd) {
			int nodeToDel = *iter;
			if (alphaTabuTable[nodeToDel] <= tabuIter || betaTabuTable[nodeToAdd] <= tabuIter) {  /* �ǽ���״̬ */
				if (M.at(nodeToDel) == maxDistance) {
					Edge pairEdge = { nodeToDel,{ nodeToAdd, G.edges[nodeToDel][nodeToAdd] } };
					L.push_back(pairEdge);
				}
				else if (M.at(nodeToDel) < maxDistance) {
					maxDistance = M.at(nodeToDel);
					L.clear();
					Edge pairEdge = { nodeToDel,{ nodeToAdd, G.edges[nodeToDel][nodeToAdd] } };
					L.push_back(pairEdge);
				}
			}
			else { /* ����״̬ */
				if (M.at(nodeToDel) == maxDistance) {
					Edge pairEdge = { nodeToDel,{ nodeToAdd, G.edges[nodeToDel][nodeToAdd] } };
					tabuL.push_back(pairEdge);
				}
				else if (M.at(nodeToDel) < maxDistance) {
					maxDistance = M.at(nodeToDel);
					tabuL.clear();
					Edge pairEdge = { nodeToDel,{ nodeToAdd, G.edges[nodeToDel][nodeToAdd] } };
					tabuL.push_back(pairEdge);
				}
			}
			++iter;
		}
		removeFacility(newPNode);
	}

	/* �ӿɽ��������������һ�� */
	if (tabuL.size())
		return tabuL[rand() % tabuL.size()];
	else
		return L[rand() % L.size()];
}

void MyPCenter::doubleTabuSearch(int optSol, int stopCondition)
{
	int doubleIter = 0;	/* �������� */
	int maxServeEdge = maxEdge().userNode.nodeDis;
	while (maxServeEdge != optSol && doubleIter != stopCondition) {
		cout << doubleIter << ": " << maxServeEdge << endl;
		hisOptSol.push_back(maxServeEdge);
		Edge findPairEdge = findPair(doubleIter);
		addFacility(findPairEdge.userNode.nodeNo);
		maxServeEdge = removeFacility(findPairEdge.serverNode);
		alphaTabuTable[findPairEdge.userNode.nodeNo] = ALPHATABUTENURE;
		betaTabuTable[findPairEdge.serverNode] = BETATABUTENURE;
		doubleIter++;
	}
}

void MyPCenter::singleTabuSearch(int optSol, int stopCondition)	/* stopCondition: ���������� */
{
	int singleIter = 0;	/* �������� */
	Edge maxServeEdge = maxEdge();
	while (maxServeEdge.userNode.nodeDis != optSol && singleIter < stopCondition)
	{
		hisOptSol.push_back(maxServeEdge.userNode.nodeDis);
		Edge findPairEdge = findPair(singleIter);
		if (singleTabuTable.count(findPairEdge)) { /* �ѽ��� */
			if (singleTabuTable.at(findPairEdge) < singleIter) { /* �������ڽ��� */
				addFacility(findPairEdge.userNode.nodeNo);
				removeFacility(findPairEdge.serverNode);
			}
			else {  /* ���������� */
				if (isAmnesty(findPairEdge))
					singleTabuTable.erase(findPairEdge);  /* ������ */
				else {
					addFacility(findPairEdge.serverNode);
					removeFacility(findPairEdge.userNode.nodeNo);
				}
			}
		}
		else { /* δ���� */
			singleTabuTable[findPairEdge] = SINGLETABUTENURE;
			addFacility(findPairEdge.userNode.nodeNo);
			removeFacility(findPairEdge.serverNode);
		}
		maxServeEdge = maxEdge();
		singleIter++;
	}
}

bool MyPCenter::isAmnesty(Edge feaEdge)
{
	/* ������� */
	sort(hisOptSol.begin(), hisOptSol.end());
	int bestHisSol = hisOptSol[0];
	addFacility(feaEdge.userNode.nodeNo);
	removeFacility(feaEdge.serverNode);
	int feaSol = maxEdge().userNode.nodeDis;
	return feaSol <= bestHisSol;
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
	//vector<int>::iterator iter = serverNodeArr.begin();
	//while (iter != serverNodeArr.end()) {
	//	cout << *iter << '\t';
	//	++iter;
	//}
	//cout << endl;
	
	for (int i = 0; i < serverNodeFlag.size(); i++)
		if (serverNodeFlag[i])
			cout << i << '\t';
	cout << endl;
}

void MyPCenter::printFDTable()
{
	cout << "****** F&D Table ******" << endl;
	cout << "No\tF[f]\tF[s]\tD[f]\tD[s]" << endl;
	for (int i = 0; i < G.vexNum; i++) {
		cout << i << '\t' << F[i].first << '\t' << F[i].second << '\t' << D[i].first << '\t' << D[i].second << endl;
	}
}

