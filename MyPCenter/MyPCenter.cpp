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
	/* 遍历serverNodeArr，找出距离最短的前两者 */
	int firNode, secNode;
	/* 三目运算符：加括号！！！*/
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
	/* 遍历已排序的NoDisArr[i]，找出前两近的服务节点 */
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
	/* 遍历已排序的NoDisArr[i]，找出第count个服务节点 */
	/* j: 距离i第j近节点 */
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
	/* 探测可升级为服务节点的用户节点集合 candNewNodes */
	vector<Edge> L;
	vector<Edge> tabuL;
	for (int i = 0; i < candNewNodes.size(); i++) {
		int newPNode = candNewNodes[i];
		addFacility(newPNode);
		/* 对每一个服务节点求M */
		map<int, int> M;
		for (int j = 0; j < serverNodeArr.size(); j++) {
			M[serverNodeArr[j]] = 0;
		}
		for (int j = 0; j < G.vexNum; j++) {
			if (M.at(F[j].first) < min(G.edges[newPNode][j], D[j].second))
				M[F[j].first] = min(G.edges[newPNode][j], D[j].second);
		}
		/* 比较M与maxDistance，确定用于交换的服务节点和新的目标函数值 */
		int nodeToAdd = newPNode;
		vector<int>::iterator iter = serverNodeArr.begin();
		while (iter != serverNodeArr.end() && (*iter)!= nodeToAdd) {
			int nodeToDel = *iter;
			if (alphaTabuTable[nodeToDel] <= tabuIter || betaTabuTable[nodeToAdd] <= tabuIter) {  /* 非禁忌状态 */
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
			else { /* 禁忌状态 */
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

	/* 从可交换对中随机返回一对 */
	if (tabuL.size())
		return tabuL[rand() % tabuL.size()];
	else
		return L[rand() % L.size()];
}

void MyPCenter::doubleTabuSearch(int optSol, int stopCondition)
{
	int doubleIter = 0;	/* 迭代次数 */
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

void MyPCenter::singleTabuSearch(int optSol, int stopCondition)	/* stopCondition: 最大迭代次数 */
{
	int singleIter = 0;	/* 迭代次数 */
	Edge maxServeEdge = maxEdge();
	while (maxServeEdge.userNode.nodeDis != optSol && singleIter < stopCondition)
	{
		hisOptSol.push_back(maxServeEdge.userNode.nodeDis);
		Edge findPairEdge = findPair(singleIter);
		if (singleTabuTable.count(findPairEdge)) { /* 已禁忌 */
			if (singleTabuTable.at(findPairEdge) < singleIter) { /* 禁忌周期结束 */
				addFacility(findPairEdge.userNode.nodeNo);
				removeFacility(findPairEdge.serverNode);
			}
			else {  /* 禁忌周期内 */
				if (isAmnesty(findPairEdge))
					singleTabuTable.erase(findPairEdge);  /* 特赦解禁 */
				else {
					addFacility(findPairEdge.serverNode);
					removeFacility(findPairEdge.userNode.nodeNo);
				}
			}
		}
		else { /* 未禁忌 */
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
	/* 特赦规则 */
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

