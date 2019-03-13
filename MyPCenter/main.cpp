#include "MyPcenter.h"


int main() {
	
	MyPCenter mypc = MyPCenter();
	mypc.readFileToCreateGraph("D:\\Hust\\P-Center\\TabuSearch_For_P-center\\Instance\\pmed1.txt");
	mypc.initSolution();
	mypc.createFDTable();

	//mypc.printGraph();
	//mypc.printNoDisArr();
	//mypc.printInitSol();
	//mypc.printFDTable();

	system("PAUSE");
	return 0;
}