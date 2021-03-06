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

bool inputIsTrue = false;
QMap<QString, QString> inputComdsMap;
void printUsage();
void parseCommands(QStringList commands);

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QTextCodec* codec = QTextCodec::codecForName("system");
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForLocale(codec);

	parseCommands(QCoreApplication::arguments()); //解析传入参数
	if (!inputIsTrue){
		printUsage();
		exit(0);
	}

	QString blt = inputComdsMap["-g"]; //BLT 几何文件
	QString jfd = inputComdsMap["-i"]; //JFD 模型文件
	QString solverType = inputComdsMap["-s"];
	QFileInfo fi(blt);
	QString out = fi.absolutePath() + '/' + fi.completeBaseName() + ".out";
	QString workSpace = fi.absolutePath();
	QString programName = "SiPESC.FEMS";
	QFileInfo fi2(jfd);
	QString type = fi2.suffix();
//=======================================================================================
	{
		std::cout << "\n***************Finite Element Analysis start!***************" << std::endl;

		Test mainTest = Test();
		mainTest.initTestCase(programName, workSpace);

		mainTest.import(jfd);

		mainTest.solve(jfd,solverType);
		
		// 若模型文件为 jfd 则输出 out。
		if(type.compare("jfd",Qt::CaseInsensitive) == 0){
			mainTest.output(out);
		}

		mainTest.cleanupTestCase();

//		mainTest.checkout();

		std::cout << "\n***************Finite Element Analysis Complete!***************" << std::endl;
	}
//=======================================================================================


	return 0;
}

void printUsage()
{
	std::cout << setiosflags(std::ios::left);

	std::cout << "USAGE:" << std::endl;
	std::cout << "  main [OPTION] <parameters>" << std::endl;
	std::cout << "OPTIONS:" << std::endl;
	std::cout << "  -g   BLT Input FileName" << std::endl;
	std::cout << "  -i   JFD Input FileName" << std::endl;
	return;
}

void parseCommands(QStringList commands)
{
	if (commands.contains("-help"))
	{
		printUsage();
		exit(0);
	}else{
		if (!commands.contains("-g"))
			return;
		if (!commands.contains("-i"))
			return;
	}

	if (commands.contains("-s"))
	{
		inputComdsMap.insert("-s", "Pardiso");
		commands.removeOne("-s");
	}else{
		inputComdsMap.insert("-s", QString());
	}


	int count = commands.count(); //命令个数
	for (int i = 1; i < count; i = i + 2)
	{
		inputComdsMap.insert(commands[i], commands[i + 1]);
	}

	inputIsTrue = true;
	return;
}
