/*
 * main.cxx
 *
 *  Created on: 2014-1-23
 *      Author: steven
 */

#include <test.h>
#include <iostream>
#include <iomanip>
#include <QTime>
#include <qapplication.h>
using namespace std;

bool inputIsTrue = false;
QMap<QString, QString> inputComdsMap;
void printUsage();
void parseCommands(QStringList commands);

int main(int argc, char *argv[])
{
	std::cout
			<< "\n***************Finite Element Analysis start!***************"
			<< std::endl;

	QApplication app(argc, argv);
	parseCommands(QApplication::arguments()); //解析传入参数

	QString path = QApplication::applicationDirPath(); //获取当前应用程序路径

	if (!inputIsTrue)
	{
		printUsage();
		exit(0);
	}

	QString programName = inputComdsMap["-j"]; //工程名
	QString workSpace = inputComdsMap["-w"]; //工作路径名
	QString fileName = inputComdsMap["-i"]; //输入模型文件名

	QTime timeAll;
	int tmpTime;

	{
		Test mainTest = Test();
		mainTest.initTestCase(programName, workSpace);

		timeAll.start();
		QTime time;
		time.start();
		mainTest.import(fileName);
		std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0 << " seconds"
				<< std::endl;

		mainTest.solve(fileName);
		tmpTime = timeAll.elapsed();

		mainTest.scriptOutput(path, fileName);

		mainTest.cleanupTestCase();
	}

	std::cout
			<< "\n***************Finite Element Analysis Complete!***************"
			<< std::endl;

	std::cout << "Total Time Elapsed==>" << tmpTime / 1000.0 << " seconds"
			<< std::endl;
	return 0;
}

void printUsage()
{
	std::cout << setiosflags(ios::left);

	std::cout << "USAGE：" << endl;
	std::cout << "  main [OPTION] <parameters>" << endl;
	std::cout << "OPTIONS：" << endl;
	std::cout << "  -j   Job Name" << endl;
	std::cout << "  -w   Working Directory " << endl;
	std::cout << "  -i   Input FileName" << endl;
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
		if (!commands.contains("-j"))
			return;
		if (!commands.contains("-w"))
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
