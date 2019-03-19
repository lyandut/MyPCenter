#define INSTANCEFOLDER "D:\\Hust\\MyPCenter\\Instance\\"

#include "MyPcenter.h"
#include <ctime>

int main() {
	srand(time(NULL));
	vector<int> knownOptSol = {
		127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13
	};

	for (int i = 0; i < knownOptSol.size(); i++) {
		clock_t startTime = clock();
		MyPCenter myPC = MyPCenter();
		string instanceFile = INSTANCEFOLDER;
		instanceFile = instanceFile +"pmed" + to_string(i + 1) + ".txt";
		myPC.readFileToCreateGraph(instanceFile);
		myPC.initSolution();
		myPC.createFDTable();
		myPC.doubleTabuSearch(knownOptSol[i], 1000);
		//myPC.singleTabuSearch(127, 100);

		//myPC.printGraph();
		//myPC.printNoDisArr();
		//myPC.printFDTable();
		myPC.printOptSol();
		clock_t endTime = clock();
		cout << "ºÄÊ±(s)£º" << double(endTime - startTime) / CLOCKS_PER_SEC << endl;
	}

	system("PAUSE");
	return 0;
}