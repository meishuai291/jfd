/*
 * test.h
 *
 *  Created on: 2014-1-23
 *      Author: steven
 */

#ifndef TEST_H_
#define TEST_H_

#include <qvector.h>
#include <iostream>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.engdbs.data.mdatabase.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>

//#include <org.sipesc.core.qtscript.qtscriptenginemanager.h>
//#include <org.sipesc.core.qtscript.qtscriptengine.h>

//using namespace org::sipesc::core::qtscript;
using namespace org::sipesc::core::engdbs::data;

class Test
{
public:
	Test();

	~Test();

public:

	void initTestCase(const QString&, const QString&);

	void cleanupTestCase();

	void import(const QString&);

	bool solve(const QString&);

	void output(const QString&);

//	void scriptOutput(const QString&, const QString&);

	void checkout();

private:
	void parseAnalysisType(const QString& );
	bool solveIn(const QString&);

private:
	MExtensionManager _extManager;
	MObjectManager _objectManager;
	MDatabase _db;
	MDataModel _model;
	MDataModel _modelOri;
	QVector<QString> _taskCommands;
	QString _dbPath;
	QString _logFile;
};

//class ScriptHelper: public QObject
//{
//	Q_OBJECT
//public:
//	ScriptHelper(QtScriptEngine& engine);
//	QScriptValue evaluate(const QString& code);
//private:
//	QtScriptEngine myEngine;
//	QString _error;
//public slots:
//	void onScriptStandardOutputReady();
//	void onScriptError(const QString& error);
//};

#endif /* TEST_H_ */
