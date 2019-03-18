#include "MyPcenter.h"
#include <ctime>

int main() {
	srand(time(NULL));
	vector<int> knownOptSol = {
		127,98,93,74,48,84,64,55,37,20,59,51,36,26,18,47,39,28,18,13,40,38,22,15,11,38,32,18,13,9,30,29,15,11,30,27,15,29,23,13
	};

	MyPCenter myPC = MyPCenter();
	myPC.readFileToCreateGraph("D:\\Hust\\P-Center\\TabuSearch_For_P-center\\Instance\\pmed1.txt");
	myPC.initSolution();
	myPC.createFDTable();
	myPC.doubleTabuSearch(127, 100);
	
	//myPC.printGraph();
	//myPC.printNoDisArr();
	//myPC.printFDTable();
	myPC.printOptSol();
	
	system("PAUSE");
	return 0;
}