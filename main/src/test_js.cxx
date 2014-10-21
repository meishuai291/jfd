/*
 * test.cxx
 *
 *  Created on: 2014-1-23
 *      Author: steven
 */

#include <test.h>
#include <iostream>
#include <QTime>
#include <qdir.h>
#include <qdebug.h>
#include <qtextcodec.h>
#include <qscriptengine.h>

#include <mplugin.h>
#include <mfileutility.h>
#include <mpluginmanager.h>
#include <merrormanager.h>

#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatabaseoption.h>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.utilities.msimpleprogressindicator.h>
#include <org.sipesc.utilities.msharedvariablesmanager.h>
#include <org.sipesc.utilities.mextensionfactory.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.utilities.mtaskmanager.h>
#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.bdfimport.mdataimportor.h>
#include <org.sipesc.fems.femstask.manalysisflowcommands.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::bdfimport;
using namespace org::sipesc::fems::femstask;
using namespace std;

Test::Test()
{
	_extManager = MExtensionManager::getManager();
	Q_ASSERT(!_extManager.isNull());
	_objectManager = MObjectManager::getManager();
	Q_ASSERT(!_objectManager.isNull());
}

Test::~Test()
{
}

void Test::initTestCase(const QString& programName, const QString& workSpace)
{
	/***************************************************************
	 *                                                             *
	 *                           插件注册                           *
	 *                                                             *
	 ***************************************************************/
	MPluginManager::initialize(false);
	MPluginManager pluginManager = MPluginManager::getManager();
	Q_ASSERT(!pluginManager.isNull());
//  Debugger debugger;
//  pluginManager.addListener(&debugger);

	bool ok;
	ok = pluginManager.loadAllPlugins();
	if(!ok){
		std::cout << "Failed to load All Plugins." << std::endl;
	}
//	bool ok = pluginManager.loadPlugin("org.sipesc.fems.addplugin");
	Q_ASSERT(ok);
	
//  QStringList plugList = pluginManager.getLoadedPluginList();
//  QStringList apList = pluginManager.getAvailablePluginList();
//  for (int i = 0; i < apList.count(); i++){
//        std::cout << i << " - " << apList[i].toStdString() << std::endl;
//        ok = pluginManager.loadPlugin(apList[i]);
//        std::cout << ok << std::endl;
//  }

	QDir::setCurrent(workSpace); //设置工作目录为给定目录

	//创建数据库管理器
	QString name = "org.sipesc.core.engdbs.mdatabasemanager";
	MDatabaseManager dbManager = _objectManager.getObject(name);
	Q_ASSERT(!dbManager.isNull());

	//创建数据库设置器
	MDatabaseOption option = dbManager.createDatabaseOption();
	Q_ASSERT(!option.isNull());
	option.setAddress(workSpace); //设置路径
	option.setCacheName("org.sipesc.core.engdbs.data.lrucache");
	option.setDeviceName("org.sipesc.core.engdbs.data.file");
	option.setDatabaseName(programName); //设置数据库名称

	//创建数据库
	_db = dbManager.createDatabase("org.sipesc.engdbs");
	ok = _db.open(option, false);
	Q_ASSERT(ok);
	ok = _db.clear();
	Q_ASSERT(ok);

	_model = dbManager.createDataModel();
	ok = _model.open(_db, "FemsEngdbs");
	Q_ASSERT(ok);
	_model.clear();

	MDataManager localCoords = dbManager.createDataManager();
	ok = localCoords.open(_model, "LocalCoordTransMatrix", false);
	Q_ASSERT(ok);
	localCoords.clear();
//  MFemsGlobal femsGl = objectManager.getObject("org.sipesc.fems.global.femsglobal");
//  Q_ASSERT(!femsGl.isNull());
	return;
}

void Test::cleanupTestCase()
{
	_db.close();
	_model = MExtension();
	_db = MExtension();
	MPluginManager::cleanup();
	return;
}

void Test::import(const QString& importName)
{
//      qDebug() << importName;
	QString fileName = importName;
	QFileInfo fi(fileName);
	QString suffix = fi.suffix(); //取文件后缀
//      qDebug() << suffix;

	QString name = "org.sipesc.utilities.MExtensionFactoryManager"; //创建数据工厂管理器
	MExtensionFactoryManager factoryManager = _extManager.createExtension(name);
	if(factoryManager.isNull()){
		std::cout << "MExtensionFactoryManager is Null." << std::endl;
		return;
	}
	bool ok = factoryManager.initialize("fems.factory.dataimportor");
	Q_ASSERT(ok);

	MExtensionFactory factory = factoryManager.getFactory(suffix);
	std::cout << "Data Type: " << suffix.toStdString() << std::endl;
	Q_ASSERT(!factory.isNull());
	MDataImportor importor = factory.createExtension();
	Q_ASSERT(!importor.isNull());
	ok = importor.initialize(_model);
	Q_ASSERT(ok);

	//输出
	MProgressIndicator indicator = _extManager.createExtension(
			"org.sipesc.utilities.MSimpleProgressIndicator"); //indicator
	Q_ASSERT(!indicator.isNull());

	ProgressMonitor monitor = importor.getProgressMonitor(); //monitor
	Q_ASSERT(!monitor.isNull());

	indicator.setMonitor(monitor);

	//导入
	QTime time;
	time.start();
	ok = importor.import(importName);
	Q_ASSERT(ok);
	std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0 << " seconds\n";

	MFemsGlobal femsGl = _objectManager.getObject(
			"org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!femsGl.isNull());

	MSharedVariablesManager sharedVariables = _extManager.createExtension(
			"org.sipesc.utilities.MSharedVariablesManager");
	Q_ASSERT(!sharedVariables.isNull());
	sharedVariables.initialize(_model);

	QString analysis = sharedVariables.getVariable(
			femsGl.getValue(MFemsGlobal::AnalysisType)).toString();

	sharedVariables.setVariable(femsGl.getValue(MFemsGlobal::TransientResp),
			"NewMark");

	qDebug() << "Analysis Type = " << analysis;

	//分析流程
	ok = factoryManager.renew("fems.factory.analysisflow");
	Q_ASSERT(ok);
	MExtensionFactory analysisFactory = factoryManager.getFactory(analysis);
	Q_ASSERT(!analysisFactory.isNull());
	MAnalysisFlowCommands analysisCommands = analysisFactory.createExtension();
	_taskCommands = analysisCommands.getFlowCommands();
	return;
}

bool Test::solve(const QString& importName)
{
	QString fileName = importName;
	QFileInfo fi(fileName);

	MProgressIndicator indicator = _extManager.createExtension(
			"org.sipesc.utilities.MSimpleProgressIndicator"); //indicator
	Q_ASSERT(!indicator.isNull());

	bool ok;
	int count = _taskCommands.count();
	for (int iCmds = 0; iCmds < count; iCmds++)
	{
		QTime time;
		time.start();
		MTaskManager task = _extManager.createExtension(_taskCommands[iCmds]);
		std::cout << "Command:" << _taskCommands[iCmds].toStdString() << "\n";
		Q_ASSERT(!task.isNull());
		ok = task.initialize(_model);
		Q_ASSERT(ok);

		ProgressMonitor monitor = task.getProgressMonitor();
		Q_ASSERT(!monitor.isNull());
		indicator.setMonitor(monitor);

		ok = task.start(fi.baseName());
		Q_ASSERT(ok);
		std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0 << " seconds\n";
	}

	return true;
}
void Test::output(const QString& importName){
	QFileInfo fi(importName);
	QString fileName = fi.absolutePath() + '/' + fi.baseName() + ".out";

	MProgressIndicator indicator = _extManager.createExtension(
			"org.sipesc.utilities.MSimpleProgressIndicator"); //indicator
	Q_ASSERT(!indicator.isNull());

	bool ok;
	QTime time;
	time.start();
	QString cmd = "org.sipesc.fems.bltexport.MBltDataExportor";
	MTaskManager task = _extManager.createExtension(cmd);
	std::cout << "Command:" << cmd.toStdString() << "\n";
	Q_ASSERT(!task.isNull());

	ProgressMonitor monitor = task.getProgressMonitor();
	Q_ASSERT(!monitor.isNull());
	indicator.setMonitor(monitor);

	ok = task.initialize(_model);
	Q_ASSERT(ok);

	std::cout << fileName.toStdString() << std::endl;
	ok = task.start(fileName);
	Q_ASSERT(ok);
	std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0
			<< " seconds\n";
}
void Test::scriptOutput(const QString& path, const QString& importName)
{
	QDir::setCurrent(path); //设置工作目录为给定目录

	QString fileName = "JFDoutput.js";
	QFile file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly)) //打开文件
	{
		QString message = "ERROR: Can't open \"" + fileName
				+ "\" Please check if the file exists!";
		qDebug() << message;
		exit(0);
	}
	QTextStream stream;
	stream.setDevice(&file); //定义文件数据流
	QString code = stream.readAll();

	QtScriptEngineManager scriptenginemanager = _objectManager.getObject(
			"org.sipesc.core.qtscript.qtscriptenginemanager");
	QtScriptEngine myEngine = scriptenginemanager.createEngine();
	myEngine.setEncoding("UTF-8");

	QScriptValue qvalue = myEngine.createValue(_model);

	QScriptEngine* qEngine = myEngine.getPrivateData<QScriptEngine*>();
	QScriptValue gobject = qEngine->globalObject();
	gobject.setProperty("model", qvalue);
	qEngine->setGlobalObject(gobject);
	ScriptHelper* helper = new ScriptHelper(myEngine);

//	helper->evaluate(code);

	fileName = importName;
	code += "\n" + QString("JFDoutput") + "('" + fileName + "',model);";
	helper->evaluate(code);
}

ScriptHelper::ScriptHelper(QtScriptEngine& engine)
{
	myEngine = engine;
	myEngine.setEncoding("UTF-8");
}

QScriptValue ScriptHelper::evaluate(const QString& code)
{
	QObject* object = myEngine.getPrivateData<QObject*>();
	const char *sig, *slt;
	sig = SIGNAL(readyReadStandardOutput());
	slt = SLOT(onScriptStandardOutputReady());
	connect(object, sig, this, slt);
	sig=SIGNAL(error(const QString&));
	slt=SLOT(onScriptError(const QString&));
	connect(object, sig, this, slt);
	QScriptValue tv = myEngine.evaluate(code, "tmp", 1);
	if (!_error.isEmpty())
	{
		QByteArray ba = _error.toLatin1();
		std::cout << "****ERROR****: " << ba.data() << endl;
		_error.clear();
	}
	return tv;
}

void ScriptHelper::onScriptStandardOutputReady()
{
	int length = myEngine.getStandardOutputBytesAvailable();
	QByteArray bastr = myEngine.readStandardOutput(length);
	if (length > 0)
		std::cout << bastr.data();
}
void ScriptHelper::onScriptError(const QString& error)
{
	_error = error;
}
