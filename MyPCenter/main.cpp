#include <cstdlib>
#include <ctime>
#include "MyPcenter.h"


int main() {
	MyPCenter mypc = MyPCenter();
	mypc.readFileToCreateGraph("D:\\Hust\\P-Center\\TabuSearch_For_P-center\\Instance\\pmed1.txt");
	mypc.printGraph();
	system("PAUSE");
	return 0;
}