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
	int serverNode, userNode;
	int SC = 0;
	for (int i = 0; i < G.vexNum; i++) {
		if (D[i].first > SC) {
			SC = D[i].first;
			serverNode = F[i].first;
			userNode = i;
		}
	}
	Edge maxServeEdge = { serverNode, {userNode, SC} };
	return maxServeEdge;
}

Edge MyPCenter::maxEdgeWithoutFD()
{
	int serverNode, userNode;
	int SC = 0;
	for (int i = 0; i < G.vexNum; i++) {
		FDTable tmp = findCountByNoDisArr(i, 1);
		if (tmp.second > SC) {
			userNode = i;
			serverNode = tmp.first;
			SC = tmp.second;
		}
	}
	Edge maxServeEdge = { serverNode,{ userNode, SC } };
	return maxServeEdge;
}
 
void MyPCenter::initSolution()
{
	serverNodeFlag = vector<int>(G.vexNum, 0);
	int initPNode = rand() % G.vexNum;
	serverNodeArr.push_back(initPNode);
	serverNodeFlag[initPNode] = 1;
	
	while (serverNodeArr.size() < pCenterNum)
	{
		Edge maxServeEdge = maxEdgeWithoutFD();
		NodeNoDis userNode = maxServeEdge.userNode;
		vector<int> candNodes;
		for (vector<NodeNoDis>::iterator iter = NoDisArr[userNode.nodeNo].begin(); iter != NoDisArr[userNode.nodeNo].end(); ++iter){
			if (!serverNodeFlag[(*iter).nodeNo])
				candNodes.push_back((*iter).nodeNo);
			else
				break;
		}
		int newPNode = candNodes[rand() % candNodes.size()];
		serverNodeArr.push_back(newPNode);
		serverNodeFlag[newPNode] = 1;
	}
}

void MyPCenter::createFDTable()
{
	F = D = vector<FDTable>(G.vexNum);
	for (int i = 0; i < G.vexNum; i++) {
		createFDByserverNodeArr(i);
	}
	//printFDTable();
	//
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
		else if (serverNodeFlag[NoDisArr[i][j].nodeNo] && count > 1) {
			count--;
		}
		else if (!count)
			break;
	}
	return fd;
}

int MyPCenter::addFacility(int newServerNode)
{
	int SC = 0;
	serverNodeArr.push_back(newServerNode);
	serverNodeFlag[newServerNode] = 1;
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
	serverNodeFlag[rmServerNode] = 0;
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
		
		if (D[i].first > SC)
			SC = D[i].first;
	}
	return SC;
}

Edge MyPCenter::findPairForDouble(int tabuIter)
{
	int hisOptSol = UNREACHABLE, hisTabuOptSol = UNREACHABLE;
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
	vector<Edge> L;			/* 非禁忌对 */
	vector<Edge> tabuL;		/* 禁忌对 */
	for (int i = 0; i < candNewNodes.size(); i++) {
		int nodeToAdd = candNewNodes[i];
		addFacility(nodeToAdd);
		/* 对每一个服务节点求M */
		map<int, int> M;
		for (int j = 0; j < serverNodeArr.size(); j++) {
			M[serverNodeArr[j]] = 0;
		}
		for (int j = 0; j < G.vexNum; j++) {
			if (M[F[j].first] < min(G.edges[nodeToAdd][j], D[j].second))
				M[F[j].first] = min(G.edges[nodeToAdd][j], D[j].second);
		}
		/* 比较M与maxDistance，确定用于交换的服务节点和新的目标函数值 */
		vector<int>::iterator iter = serverNodeArr.begin();
		while (iter != serverNodeArr.end() && (*iter)!= nodeToAdd) {
			int nodeToDel = *iter;
			int tempMax = max(maxDistance, M[nodeToDel]);
			if (alphaTabuTable[nodeToAdd] <= tabuIter && betaTabuTable[nodeToDel] <= tabuIter) {  /* 非禁忌状态 */
				if (tempMax == hisOptSol) {
					Edge pairEdge = { nodeToDel,{ nodeToAdd, tempMax } };
					L.push_back(pairEdge);
				}
				else if (tempMax < hisOptSol) {
					hisOptSol = tempMax;
					L.clear();
					Edge pairEdge = { nodeToDel,{ nodeToAdd, tempMax } };
					L.push_back(pairEdge);
				}
			}
			else { /* 禁忌状态 */
				if (tempMax == hisTabuOptSol) {
					Edge pairEdge = { nodeToDel,{ nodeToAdd, tempMax } };
					tabuL.push_back(pairEdge);
				}
				else if (tempMax < hisTabuOptSol) {
					hisTabuOptSol = tempMax;
					tabuL.clear();
					Edge pairEdge = { nodeToDel,{ nodeToAdd, tempMax } };
					tabuL.push_back(pairEdge);
				}
			}
			++iter;
		}
		removeFacility(nodeToAdd);
	}

	/* 从可交换对中返回最好的一对 */
	if (tabuL.size() && hisTabuOptSol < hisOptSol && isAmnesty(hisTabuOptSol) || !L.size())
		return tabuL[rand() % tabuL.size()];
	else
		return L[rand() % L.size()];
}

void MyPCenter::doubleTabuSearch(int optSol, int stopCondition)
{
	int doubleIter = 0;	/* 迭代次数 */
	alphaTabuTable = vector<int>(G.vexNum, -1);
	betaTabuTable = vector<int>(G.vexNum, -1);
	int maxServeEdge = maxEdge().userNode.nodeDis;
	while (maxServeEdge != optSol && doubleIter != stopCondition) {
		cout << "Iter: " << doubleIter << " OptSol: " << maxServeEdge << endl;
		hisOptSol.push_back(maxServeEdge);
		Edge findPairEdge = findPairForDouble(doubleIter);
		int nodeToAdd = findPairEdge.userNode.nodeNo;
		int nodeToDel = findPairEdge.serverNode;
		addFacility(nodeToAdd);
		maxServeEdge = removeFacility(nodeToDel);
		alphaTabuTable[nodeToAdd] = ALPHATABUTENURE;
		betaTabuTable[nodeToDel] = BETATABUTENURE;
		doubleIter++;
	}
}

Edge MyPCenter::findPairForSingle(int tabuIter)
{
	int hisOptSol = UNREACHABLE, hisTabuOptSol = UNREACHABLE;
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
	vector<Edge> L;			/* 非禁忌对 */
	vector<Edge> tabuL;		/* 禁忌对 */
	for (int i = 0; i < candNewNodes.size(); i++) {
		int nodeToAdd = candNewNodes[i];
		addFacility(nodeToAdd);
		/* 对每一个服务节点求M */
		map<int, int> M;
		for (int j = 0; j < serverNodeArr.size(); j++) {
			M[serverNodeArr[j]] = 0;
		}
		for (int j = 0; j < G.vexNum; j++) {
			if (M[F[j].first] < min(G.edges[nodeToAdd][j], D[j].second))
				M[F[j].first] = min(G.edges[nodeToAdd][j], D[j].second);
		}
		/* 比较M与maxDistance，确定用于交换的服务节点和新的目标函数值 */
		vector<int>::iterator iter = serverNodeArr.begin();
		while (iter != serverNodeArr.end() && (*iter) != nodeToAdd) {
			int nodeToDel = *iter;
			Edge pairEdge = { nodeToDel, {nodeToAdd, G.edges[nodeToDel][nodeToAdd]} };
			int tempMax = max(maxDistance, M[nodeToDel]);
			if (singleTabuTable[pairEdge] <= tabuIter) {  /* 非禁忌状态 */
				if (tempMax == hisOptSol) {
					L.push_back(pairEdge);
				}
				else if (tempMax < hisOptSol) {
					hisOptSol = tempMax;
					L.clear();
					L.push_back(pairEdge);
				}
			}
			else { /* 禁忌状态 */
				if (tempMax == hisTabuOptSol) {
					tabuL.push_back(pairEdge);
				}
				else if (tempMax < hisTabuOptSol) {
					hisTabuOptSol = tempMax;
					tabuL.clear();
					tabuL.push_back(pairEdge);
				}
			}
			++iter;
		}
		removeFacility(nodeToAdd);
	}

	/* 从可交换对中返回最好的一对 */
	if (tabuL.size() && hisTabuOptSol < hisOptSol && isAmnesty(hisTabuOptSol) || !L.size())
		return tabuL[rand() % tabuL.size()];
	else
		return L[rand() % L.size()];
}

void MyPCenter::singleTabuSearch(int optSol, int stopCondition)	/* stopCondition: 最大迭代次数 */
{
	int singleIter = 0;	/* 迭代次数 */
	int maxServeEdge = maxEdge().userNode.nodeDis;
	while (maxServeEdge != optSol && singleIter != stopCondition) {
		cout << "Iter: " << singleIter << " OptSol: " << maxServeEdge << endl;
		hisOptSol.push_back(maxServeEdge);
		Edge findPairEdge = findPairForSingle(singleIter);
		int nodeToAdd = findPairEdge.userNode.nodeNo;
		int nodeToDel = findPairEdge.serverNode;
		addFacility(nodeToAdd);
		maxServeEdge = removeFacility(nodeToDel);
		singleTabuTable[findPairEdge] = SINGLETABUTENURE;
		singleIter++;
	}
}

bool MyPCenter::isAmnesty(int bestCurrSol)
{
	/* 特赦规则 */
	sort(hisOptSol.begin(), hisOptSol.end());
	int bestHisSol = hisOptSol[0];
	return bestCurrSol <= bestHisSol;
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

void MyPCenter::printOptSol()
{
	cout << "P中心选址集合：";
	vector<int>::iterator iter = serverNodeArr.begin();
	while (iter != serverNodeArr.end()) {
		cout << *iter << '\t';
		++iter;
	}
	cout << endl;
	Edge maxServerEdge = maxEdge();
	cout << "最优解：" << maxServerEdge.serverNode << "->" << maxServerEdge.userNode.nodeNo << ": " <<maxServerEdge.userNode.nodeDis << endl;
}

void MyPCenter::printFDTable()
{
	cout << "****** F&D Table ******" << endl;
	cout << "No\tF[f]\tF[s]\tD[f]\tD[s]" << endl;
	for (int i = 0; i < G.vexNum; i++) {
		cout << i << '\t' << F[i].first << '\t' << F[i].second << '\t' << D[i].first << '\t' << D[i].second << endl;
	}
}

