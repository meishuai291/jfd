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

#include <org.sipesc.fems.data.mloadcasedata.h>
#include <org.sipesc.fems.data.mloadcomponent.h>
#include <org.sipesc.fems.data.mnodeloaddata.h>
#include <org.sipesc.fems.data.melementdata.h>
using namespace org::sipesc::fems::data;
#include <org.sipesc.fems.matrix.mnodecontrolmatrix.h>
#include <org.sipesc.fems.matrix.mnodecontrolmatrixdata.h>
#include <org.sipesc.fems.matrix.mnodecontrolmatrixfactory.h>
using namespace org::sipesc::fems::matrix;

Test::Test()
{
	_extManager = MExtensionManager::getManager();
	Q_ASSERT(!_extManager.isNull());
	_objectManager = MObjectManager::getManager();
	Q_ASSERT(!_objectManager.isNull());
	std::cout.setf(std::ios::boolalpha);
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
	QStringList apList;

//	ok = pluginManager.loadAllPlugins();
//	if(!ok){
//		std::cout << "Failed to load All Plugins." << std::endl;
//	}
//  apList = pluginManager.getLoadedPluginList();
//  apList = pluginManager.getAvailablePluginList();
	apList << "org.sipesc.fems.addplugin";
	apList << "org.sipesc.fems.bltdataprocess";
	apList << "org.sipesc.fems.hexabrickelement12";
	for (int i = 0; i < apList.count(); i++){
		ok = pluginManager.loadPlugin(apList[i]);
		std::cout << ok << " - " << i+1 << " - " << apList[i].toStdString() << std::endl;
	}

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
	_dbPath = QDir::currentPath() + "/" + programName;
	if(QFile::exists(_dbPath))
		removePath(_dbPath);

	//创建数据库
	_db = dbManager.createDatabase("org.sipesc.engdbs");
	ok = _db.open(option, false);
	Q_ASSERT(ok);
	ok = _db.clear();
	Q_ASSERT(ok);

	_model = dbManager.createDataModel();
	ok = _model.open(_db, "FemsEngdbs");
	Q_ASSERT(ok);
	ok = _model.clear();
	Q_ASSERT(ok);
	_modelOri = _model;

	MDataManager localCoords = dbManager.createDataManager();
	ok = localCoords.open(_model, "LocalCoordTransMatrix", false);
	Q_ASSERT(ok);
	ok = localCoords.clear();
	Q_ASSERT(ok);

	return;
}

void Test::cleanupTestCase()
{
	bool isOk = _db.close();
	Q_ASSERT(isOk);

	_model = MExtension();
	_modelOri = MExtension();
	_db = MExtension();
	MPluginManager::cleanup();

	if(QFile::exists(_dbPath))
		removePath(_dbPath);
	if(QFile::exists(_logFile))
		QFile::remove(_logFile);

	return;
}

void Test::import(const QString& importName)
{
	QString fileName = importName;
	QFileInfo fi(fileName);
	QString suffix = fi.suffix(); //取文件后缀
	_logFile = fi.absolutePath() + "/" + fi.completeBaseName() + ".log";

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

	//输出
	MProgressIndicator indicator = _extManager.createExtension(
			"org.sipesc.utilities.MSimpleProgressIndicator"); //indicator
	Q_ASSERT(!indicator.isNull());
	ProgressMonitor monitor = importor.getProgressMonitor(); //monitor
	Q_ASSERT(!monitor.isNull());
	indicator.setMonitor(monitor);

	ok = importor.initialize(_model);
	Q_ASSERT(ok);

	//导入
	QTime time;
	time.start();
	ok = importor.import(importName);
	Q_ASSERT(ok);
	std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0 << " seconds\n";
}

void Test::parseAnalysisType(const QString& type){
	MFemsGlobal femsGl = _objectManager.getObject(
			"org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!femsGl.isNull());

	MSharedVariablesManager sharedVariables = _extManager.createExtension(
			"org.sipesc.utilities.MSharedVariablesManager");
	Q_ASSERT(!sharedVariables.isNull());
	sharedVariables.initialize(_model);

	QString analysis = sharedVariables.getVariable(
			femsGl.getValue(MFemsGlobal::AnalysisType)).toString();

	sharedVariables.setVariable(femsGl.getValue(MFemsGlobal::TransientResp), "NewMark");

	std::cout << "Analysis Type = " << analysis.toStdString() << std::endl;

	//分析流程
	MExtensionFactoryManager factoryManager = _extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	if(factoryManager.isNull()){
		std::cout << "MExtensionFactoryManager is Null." << std::endl;
		return;
	}
	bool ok = factoryManager.initialize("fems.factory.analysisflow");
	Q_ASSERT(ok);

	MExtensionFactory analysisFactory = factoryManager.getFactory(analysis);
	Q_ASSERT(!analysisFactory.isNull());
	MAnalysisFlowCommands analysisCommands = analysisFactory.createExtension();
	_taskCommands = analysisCommands.getFlowCommands();
	QVector<QString>::iterator vi = _taskCommands.end();

	// 若模型文件为 jfd 则不输出 unv。
	if(type.compare("jfd",Qt::CaseInsensitive) == 0){
		_taskCommands.erase(vi-1);
	}

}

bool Test::solve(const QString& importName)
{
	QString fileName = importName;
	QFileInfo fi(fileName);
	QString suffix = fi.suffix();
	QString fileBaseName = fi.baseName();

	parseAnalysisType(suffix);
	bool isOk = solveIn(fileBaseName);
	if(!isOk)
		return false;

#ifndef MULTIPLE_ANALYSIS
	qDebug() << "------------------------------------- MULTIPLE_ANALYSIS";

	// 第二种分析
	MDatabaseManager dbManager = _objectManager.getObject(
			QString("org.sipesc.core.engdbs.mdatabasemanager"));
	Q_ASSERT(!dbManager.isNull());
	if(_model.exists("SecondAnalysisPath",MDatabaseGlobal::ModelType)){
		MDataModel modelAnother = dbManager.createDataModel();
		bool ok = modelAnother.open(_model, "SecondAnalysisPath");
		if (!ok){
			std::cout << "can't open SecondAnalysisPath DataModel in MJfdDataImportorImpl::import() " << std::endl;
			return false;
		}

		_model = modelAnother;
		parseAnalysisType(suffix);
		bool isOk = solveIn(fileBaseName);
		if(!isOk)
			return false;
	}

	_model.close();
	_model = _modelOri;
#endif

	return true;
}
bool Test::solveIn(const QString& fileBaseName){
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

		ok = task.start(fileBaseName);
		Q_ASSERT(ok);
		std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0 << " seconds\n";
	}

	return true;
}
void Test::output(const QString& fileName){
	_model = _modelOri;	// 恢复根 model

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

	std::cout << fileName.toLocal8Bit().data() << std::endl;
	ok = task.start(fileName);
	Q_ASSERT(ok);
	std::cout << "Time Elapsed==>" << time.elapsed() / 1000.0
			<< " seconds\n";
}
void Test::checkout(){
	MDatabaseManager dbManager = _objectManager.getObject(
			"org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!dbManager.isNull());

#if	0	// 测试区
		/**
		 * 载荷测试
		 */
		MDataManager loadcase = dbManager.createDataManager();
		ok = loadcase.open( _model, "LoadCase");
		qDebug() << loadcase.getDataCount();
		MLoadCaseData LoadCaseData = loadcase.getDataAt(0);

		MDataManager LoadComponent = dbManager.createDataManager();
		ok = LoadComponent.open( _model, "LoadComponent");
		qDebug() << LoadComponent.getDataCount();
		MLoadComponent lc = LoadComponent.getDataAt(0);
		qDebug() << lc.getId();
		int cc = lc.getComponentCount();
		for (int j = 0; j <cc;j++){
			qDebug() << lc.getDofNumber(j) << lc.getValue(j);
		}

		MDataModel loadPath = dbManager.createDataModel();
		ok = loadPath.open( _model, "LoadPath",true);
		Q_ASSERT(ok);
		MDataManager Load = dbManager.createDataManager();
		ok = Load.open( loadPath, "1",true);
		int lcount = Load.getDataCount();
		for (int j = 0; j <lcount;j++){
			MNodeLoadData d = Load.getDataAt(j);
			qDebug() << d.getNodeId();
			qDebug() << d.getLoadValue(0);
			qDebug() << d.getLoadValue(1);
			qDebug() << d.getLoadValue(2);
			qDebug() << d.getLoadValue(3);
			qDebug() << d.getLoadValue(4);
			qDebug() << d.getLoadValue(5);
		}



//		MDataManager _nodeControl = dbManager.createDataManager();
//		bool isOK = _nodeControl.open(_model, "NodeControlMatrix",true);
//		Q_ASSERT(isOK);
//
//
//		MNodeControlMatrixFactory _nodeCFactory = _objectManager.getObject(
//					"org.sipesc.fems.data.nodecontrolmatrixfactory");
//		Q_ASSERT(!_nodeCFactory.isNull());
//
//
//    	MNodeControlMatrixData cData = _nodeControl.getData(1233);
//    	Q_ASSERT(!cData.isNull());
//        MNodeControlMatrix controlMatrix = _nodeCFactory.createMatrix();
//    	Q_ASSERT(!controlMatrix.isNull());
//    	controlMatrix << cData;
//
//    	int index = controlMatrix.getControlDofNumber(2);
//    	qDebug() << index;
#endif
#if	0
	MDataModel ee = dbManager.createDataModel();
	bool ok = ee.open(_model, "ElementPath");
	MDataManager bb = dbManager.createDataManager();
	ok = bb.open( ee, "QuadDKQShElement");
	qDebug() << bb.getDataCount();
	MElementData eleData = bb.getDataAt(3);
	qDebug() << eleData.getId();
	qDebug() << eleData.getNodeCount();
	qDebug() << eleData.getNodeId(0);
	qDebug() << eleData.getNodeId(1);
	qDebug() << eleData.getNodeId(2);
	qDebug() << eleData.getNodeId(3);

#endif
}
