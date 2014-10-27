/*
 * main.cxx
 *
 *  Created on: 2014-1-23
 *      Author: steven
 */

#include <test.h>
#include <iostream>
#include <iomanip>
#include <QTextCodec>
#include <QFileInfo>
#include <QCoreApplication>
using namespace std;

bool inputIsTrue = false;
QMap<QString, QString> inputComdsMap;
void printUsage();
void parseCommands(QStringList commands);

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));

	parseCommands(QCoreApplication::arguments()); //解析传入参数
	if (!inputIsTrue){
		printUsage();
		exit(0);
	}

	QString blt = inputComdsMap["-g"]; //BLT 几何文件
	QString jfd = inputComdsMap["-i"]; //JFD 模型文件
	QFileInfo fi(blt);
	QString out = fi.absolutePath() + '/' + fi.completeBaseName() + ".out";
	QString workSpace = fi.absolutePath();
	QString programName = "SiPESC.FEMS";

//=======================================================================================
	{
		std::cout << "\n***************Finite Element Analysis start!***************" << std::endl;

		Test mainTest = Test();
		mainTest.initTestCase(programName, workSpace);

		mainTest.import(jfd);

		mainTest.solve(jfd);
		
		mainTest.output(out);

//		mainTest.cleanupTestCase();

//		mainTest.checkout();

		std::cout << "\n***************Finite Element Analysis Complete!***************" << std::endl;
	}
//=======================================================================================


	return 0;
}

void printUsage()
{
	std::cout << setiosflags(ios::left);

	std::cout << "USAGE:" << endl;
	std::cout << "  main [OPTION] <parameters>" << endl;
	std::cout << "OPTIONS:" << endl;
	std::cout << "  -g   BLT Input FileName" << endl;
	std::cout << "  -i   JFD Input FileName" << endl;
	return;
}

void parseCommands(QStringList commands)
{
	if (commands.contains("-help"))
	{
		printUsage();
		exit(0);
	}
	else
	{
		if (!commands.contains("-g"))
			return;
		if (!commands.contains("-i"))
			return;
	}

	int count = commands.count(); //命令个数
	for (int i = 1; i < count; i = i + 2)
	{
		inputComdsMap.insert(commands[i], commands[i + 1]);
	}

	inputIsTrue = true;
	return;
}
