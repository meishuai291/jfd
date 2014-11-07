/*
 * mjfddataimportorimpl.cxx
 *
 *  Created on: 2013-1-8
 *      Author: cyz
 */
#include <mjfddataimportorimpl.h>

#include <qfile.h>
#include <qdebug.h>
#include <qvariant.h>
#include <qvector2d.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.mloadcasedata.h>
#include <org.sipesc.fems.data.mtabledata.h>

#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.global.mdynamicsglobal.h>

#include <org.sipesc.utilities.mtaskmanager.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.utilities.msharedvariablesmanager.h>

#include <org.sipesc.fems.jfdimport.mjfdentryhandler.h>
#include <org.sipesc.fems.jfdimport.mtabled5parser.h>

using namespace std;
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::jfdimport;

enum JFD
{
	NODE, MPC, LINEELEMENT, NLINEELEMENT, FORCE, GRAV
};

class MJfdDataImportorImpl::Data
{
public:
	Data()
	{
		_objManager = MObjectManager::getManager();
		_extManager = MExtensionManager::getManager();
		_isInitialized = false;
	}
public:
	MObjectManager _objManager;
	MExtensionManager _extManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;
	MDynamicsGlobal _dynamicsGlobal;

	MDatabaseManager _baseManager;
	MDataFactoryManager _factoryManager;

	MExtensionFactoryManager _extFactoryManager;

	int _handledLine; //已读行号

	MDataModel _model;
	MDataModel _modelOri;
	QTextStream _stream; //导入文件数据流
	QTextStream _logStream; //log文件数据流

	QVector<QString> _blockList; //按顺序存储需要读取的模块
	QVector<int> _blockNum; //按顺序存储需要读取的模块的数据个数
	QString _ansysType; //分析类型
	int _modalNum; //阵型个数
	int _timeFunsCnt;//时间 函数个数

	MDataManager _dynamicPara; //自振分析数据
	MDataManager _table;//存放时间曲线表格
	MDataModel _loadPath;
	MDataManager _loadcase; //载荷总控

	//读取一行，并在log文件中显示已读行
	QString _readLine()
	{
		QString line = _stream.readLine();
		_logStream << "READ: " << line << endl;
		return line;
	}

	void _importorControl() // 读取前12行总控数据
	{
		/*
		 * 第0行
		 * [0]: 标题行
		 */
//		_readLine();
		/*
		 * 第1行
		 * [1]: 节点单元坐标
		 *  |0~5|:节点总数
		 *  |12~15|:线性单元组总数
		 *  |16~19|:非线性单元组总数
		 *  |60~64|:斜坐标系个数
		 */
		QString line = _readLine();
		_blockNum[NODE] = line.mid(0, 6).toInt(); // |0~5|:节点总数
		_blockNum[LINEELEMENT] = line.mid(12, 4).toInt(); //|12~15|:线性单元组总数
		_blockNum[NLINEELEMENT] = line.mid(16, 4).toInt(); //|16~19|:非线性单元组总数

		/*
		 * 第2行
		 * [2]:空行
		 */
		_readLine();
		/*
		 * 第3行
		 * [3]: 载荷约束
		 *  |0~4|:节点集中力个数
		 *  |35~39|:与质量成正比载荷标志
		 *  |50~54|:多点约束，约束方程个数
		 */
		line = _readLine();
		_blockNum[FORCE] = line.mid(0, 5).toInt(); // |0~4|:节点集中力个数
		_blockNum[GRAV] = line.mid(35, 5).toInt(); //|35~39|:与质量成正比载荷标志
		_blockNum[MPC] = line.mid(50, 5).toInt(); //|50~54|:多点约束，约束方程个数
		/*
		 * 第4行
		 * [4]: 分析类型
		 *  |0~4|:分析类型：0，静力分析； 1，自振分析
		 */
		line = _readLine();
		if (line.mid(0, 5).toInt() == 0)
			_ansysType = _femsGlobal.getValue(MFemsGlobal::Static);
		else
			_ansysType = _femsGlobal.getValue(MFemsGlobal::Vibration);

		/*
		 * 第5行
		 * [5]: 阵型个数
		 *  |10~14|:阵型个数
		 */
		line = _readLine();
		_modalNum = line.mid(10, 5).toInt();


		/*
		 * 第6、7、8、9行
		 * 临时不做处理
		 */
		for (int i = 0; i < 4; i++)
			_readLine();


		/*
		 * 第10行+10行往后
		 * [10]时间函数个数
		 *  |0~4|:时间函数个数
		 */
		line = _readLine();
		_timeFunsCnt = line.mid(0, 5).toInt();
//		qDebug() <<"_timeFunsCnt = " << _timeFunsCnt;

		for (int i = 0; i < _timeFunsCnt; i++)
		{
			line = _readLine();
			int timeId = line.mid(0, 5).toInt();
			int timeCnt = line.mid(5, 5).toInt();

			QString dataType = "org.sipesc.fems.data.tabledata";
			MDataFactory factory = _factoryManager.getFactory(dataType);
			Q_ASSERT(!factory.isNull());

			MTableData table= factory.createObject();
			table.setType("TABLED5");
			table.setId(timeId);
			table.appendPara("LINEAR");
			table.appendPara("LINEAR");

			int lineCnt = (timeCnt+4)/4;
			for (int j = 0; j < lineCnt-1; j++)
			{
				line = _readLine();

				for (int j = 0; j < 4; j++)
				{
					double x = line.mid(20*j, 10).toDouble();
					double y = line.mid(10+20*j, 10).toDouble();
//					qDebug() << "( " << x << " , " << y << ")";

					QVector2D point;
					point.setX(x);
					point.setY(y);
					table.appendPoint(point);
				}
			}

			line = _readLine();
			int syTimeCnt = (timeCnt-1)%4+1;
			for (int j = 0; j < syTimeCnt; j++)
			{
				double x = line.mid(20*j, 10).toDouble();
				double y = line.mid(10+20*j, 10).toDouble();

//				qDebug() << "( " << x << " , " << y << ")";

				QVector2D point;
				point.setX(x);
				point.setY(y);
				table.appendPoint(point);
			}

			_table.appendData(table);
		}

	}

	void _handleLoadCase() //处理节点载荷数据
	{
		QString dataType = "org.sipesc.fems.data.loadcasedata";
		MDataFactory factory = _factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		// !!!!! JFD文件肯定存在自重，因此判断基本无用 !!!!!
		if (_blockNum[GRAV] == 0) //如果没有重力载荷
		{
			int caseId = 1;
			//工况：每个时间步加载
			for (int j = 0; j < _loadPath.getTypeCount(MDatabaseGlobal::ManagerType); j++)
			{
				QString name = _loadPath.getTypeName(MDatabaseGlobal::ManagerType, j);
				int Id = name.toInt();

				if (Id == 0)
					continue;

				MLoadCaseData LoadCaseData = factory.createObject();
				LoadCaseData.setId(caseId);
				LoadCaseData.setLoadCount(1);
				LoadCaseData.setLoadId(0, Id);
				LoadCaseData.setScale(0, 1.0);

				_loadcase.appendData(LoadCaseData);

				caseId++;
			}
		}
		else//如果有重力载荷
		{
			//工况1：重力载荷
			MLoadCaseData LoadCaseData = factory.createObject();
			LoadCaseData.setId(1);
			LoadCaseData.setLoadCount(1);
			LoadCaseData.setLoadId(0, 0);
			LoadCaseData.setScale(0, 1.0);
			_loadcase.appendData(LoadCaseData);

			/**
			 * JFD
			 * 后一个工况要考虑前一个工况的影响
			 * 而SiPESC各个工况独立
			 * 因此，必须保证各个工况按顺序排列
			 *   自重载荷-0，工况一载荷-1，...
			 * 然后，每个loadcase考虑前面工况所有载荷
			 */
#if 1	// 叠加自重，卸载前一工况
			int loadCount = _loadPath.getTypeCount(MDatabaseGlobal::ManagerType);
			loadCount--;	// 删除自重
			for (int j = 0; j < loadCount;j++){
				int Id = j+1;
				int caseId = j+2;

				MLoadCaseData LoadCaseData = factory.createObject();
				LoadCaseData.setId(caseId);
				LoadCaseData.setLoadCount(caseId);
				LoadCaseData.setLoadId(0, 0);
				LoadCaseData.setScale(0, 1.0);

				for(int i=1;i<caseId;i++){
					LoadCaseData.setLoadId(i, i);
					LoadCaseData.setScale(i, 1.0);
				}
				_loadcase.appendData(LoadCaseData);
			}
#else	// 各个工况独立，不叠加自重，每个时间步分别加载
			int caseId = 2;
			int loadCount = _loadPath.getTypeCount(MDatabaseGlobal::ManagerType);
			for (int j = 0; j < loadCount; j++){
				QString name = _loadPath.getTypeName(MDatabaseGlobal::ManagerType, j);
				int Id = name.toInt();
				if (Id == 0)
					continue;

				MLoadCaseData LoadCaseData = factory.createObject();
				LoadCaseData.setId(caseId);
				LoadCaseData.setLoadCount(1);
				LoadCaseData.setLoadId(0, Id);
				LoadCaseData.setScale(0, 1.0);
				_loadcase.appendData(LoadCaseData);

				caseId++;
			}
#endif

		}
	}

	bool _isInitialized;
};

MJfdDataImportorImpl::MJfdDataImportorImpl()
{
	_data.reset(new MJfdDataImportorImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdDataImportorImpl::"
				"MJfdDataImportorImpl() ****** failed");

	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());

	_data->_monitor = util.createProgressMonitor("MJfdDataImportor", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());

	_data->_dynamicsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.dynamicsglobal");
	Q_ASSERT(!_data->_dynamicsGlobal.isNull());

	_data->_baseManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_factoryManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatafactorymanager");
	Q_ASSERT(!_data->_factoryManager.isNull());

	_data->_extFactoryManager = _data->_extManager.createExtension("org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_extFactoryManager.isNull());
	_data->_extFactoryManager.initialize( "fems.factory.jfdentryhandler");
}
MJfdDataImportorImpl::~MJfdDataImportorImpl(){
	_data->_modelOri = MExtension();
}

bool MJfdDataImportorImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false; //不能重复初始化

	//读入模块顺序：节点，多点约束，线性单元组，非线性单元组，节点力，重力
	_data->_blockList << "NODE" << "MPC" << "LINEELEMENT" << "NLINEELEMENT"
			<< "FORCE" << "GRAV";
	_data->_blockNum.resize(_data->_blockList.count());

	_data->_model = model;
	_data->_modelOri = _data->_model;

	bool isOk = initDatabase(_data->_model);
	if (!isOk){
		QString str = _data->_model.getName();
		QString errorMessage ="In " + str +" DataModel, "
						"can't open some DataManager or DataModel in MJfdDataImportorImpl::initialize().";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}
bool MJfdDataImportorImpl::initDatabase(MDataModel& model){

	_data->_dynamicPara = _data->_baseManager.createDataManager();
	bool ok = _data->_dynamicPara.open(model, _data->_femsGlobal.getValue(MFemsGlobal::Vibration));
	if (!ok)
	{
		QString errorMessage =
				"can't open MVibration DataManager in MJfdDataImportorImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_loadPath = _data->_baseManager.createDataModel();
	ok = _data->_loadPath.open(model, _data->_femsGlobal.getValue(MFemsGlobal::LoadPath));
	if (!ok)
	{
		QString errorMessage =
				"can't open MFEMS::LoadPath DataModel in MJfdDataImportorImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_loadcase = _data->_baseManager.createDataManager();
	ok = _data->_loadcase.open(model, _data->_femsGlobal.getValue(MFemsGlobal::LoadCase));
	if (!ok)
	{
		QString errorMessage =
				"can't open MFEMS::LoadCase DataManagerl in MJfdDataImportorImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_table = _data->_baseManager.createDataManager();
	ok = _data->_table.open(model, _data->_femsGlobal.getValue(MFemsGlobal::LoadTable));
	if (!ok)
	{
		QString errorMessage =
				"can't open MFEMS::LoadTable DataManager in MJfdDataImportorImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}



	return true;
}

ProgressMonitor MJfdDataImportorImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MJfdDataImportorImpl::import(const QString& fileName)
{
	//打开文件
	/*
	 * 1) 打开文件，定义文件数据流
	 */
	QFile file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly)) //enum JFD {NODE, MPC, LINEELEMENT, NLINEELEMENT,FORCE, GRAV};打开文件
	{
		QString message = "Can't open model file in MJfdDataImportorImpl!";
		qDebug() << message;
		_data->_monitor.setMessage(message);
		return true;
	}
	_data->_stream.setDevice(&file); //定义文件数据流
	_data->_stream.setCodec("GBK");

	/*
	 * 2) 创建log文件，定义log文件数据流
	 */
	QFileInfo fi(fileName);
	QString logFileName = fi.absolutePath() + "/" + fi.completeBaseName()
			+ ".log"; //创建文件读取的log文件
	QFile logFile;
	logFile.setFileName(logFileName);
	if (!logFile.open(QIODevice::WriteOnly))
	{
		QString message = "Can't open *.log file in MJfdDataImportorImpl!";
		qDebug() << message;
		_data->_monitor.setMessage(message);
		return true;
	}
	_data->_logStream.setDevice(&logFile); //定义log文件的数据流

	QString firstLine = _data->_readLine();
	importIn();	// 1

#ifndef MULTIPLE_ANALYSIS
	qDebug() << "------------------------------------- MULTIPLE_ANALYSIS";

	/** 对于含两种以上分析类型的jfd文件，各个分析数据独立，因此可分别读取处理
	 *  默认，根model存储第一种分析类型，其他分析数据存储于子model中
	 *  如：第二个分析 SecondAnalysisPath
	 *  对于本项目，最多含有两种分析类型 **/

	while(true){
		QString line = _data->_readLine();

		if(line.left(4) == "stop" || _data->_stream.atEnd())
			break;

		if(line == firstLine){
			MDataModel modelAnother = _data->_baseManager.createDataModel();
			bool ok = modelAnother.open(_data->_model, "SecondAnalysisPath");
			if (!ok)
			{
				QString errorMessage =
						"can't open SecondAnalysisPath DataModel in MJfdDataImportorImpl::import() ";
				qDebug() << errorMessage;
				_data->_monitor.setMessage(errorMessage);
				return false;
			}
			MDataManager localCoords = _data->_baseManager.createDataManager();
			ok = localCoords.open(modelAnother, "LocalCoordTransMatrix", false);
			Q_ASSERT(ok);

			_data->_model = modelAnother;
			bool isOk = initDatabase(_data->_model);
			if (!isOk){
				QString str = _data->_model.getName();
				QString errorMessage ="In " + str +" DataModel, "
								"can't open some DataManager or DataModel in MJfdDataImportorImpl::import().";
				qDebug() << errorMessage;
				_data->_monitor.setMessage(errorMessage);
				return false;
			}

			importIn();	// 2

			_data->_model.close();
			_data->_model = _data->_modelOri;
		}

	}

#endif

	file.close();
	logFile.close();
	return true;
}

bool MJfdDataImportorImpl::importIn(){
	/*
	 * 3) 导入总控数据
	 */
	_data->_importorControl();

//	qDebug() << _data->_blockNum;
//	qDebug() << _data->_ansysType; //分析类型
//	qDebug() << _data->_modalNum; //阵型个数

	qDebug() << "NODE:" << _data->_blockNum[NODE];
	qDebug() << "MPC:" << _data->_blockNum[MPC];
	qDebug() << "LINEELEMENT:" << _data->_blockNum[LINEELEMENT];
	qDebug() << "NLINEELEMENT:" << _data->_blockNum[NLINEELEMENT];
	qDebug() << "FORCE:" << _data->_blockNum[FORCE];
	qDebug() << "GRAV:" << _data->_blockNum[GRAV];


	MSharedVariablesManager sharedVariables = _data->_extManager.createExtension(
			"org.sipesc.utilities.MSharedVariablesManager");
	Q_ASSERT(!sharedVariables.isNull());
	sharedVariables.initialize(_data->_model);
	sharedVariables.setVariable(_data->_femsGlobal.getValue(MFemsGlobal::AnalysisType), _data->_ansysType);

	QString dataType = "org.sipesc.fems.data.propertydata";
	MDataFactory dataFactory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!dataFactory.isNull());

	MPropertyData Lanczos = dataFactory.createObject();
	Lanczos.setId(1);
	Lanczos.setType(_data->_dynamicsGlobal.getValue(MDynamicsGlobal::Lanczos));
	Lanczos.setValueCount(3);
	Lanczos.setValue(0, _data->_modalNum);
	Lanczos.setValue(1, 0.0);
	Lanczos.setValue(2, 0.0);

	_data->_dynamicPara.appendData(Lanczos);
	/*
	 * 4) 按顺序通过工厂查找相应模块Handler
	 *
	 */
	{
		QString key = "NODE";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[NODE]);
	}

	{
		QString key = "MPC";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[MPC]);
	}

	{
		QString key = "LINEELEMENT";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		// 根据总控卡，确定单元组个数
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[LINEELEMENT]);
	}

	{
		QString key = "NLINEELEMENT";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		// 根据总控卡，确定单元组个数
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[NLINEELEMENT]);
	}

	{
		QString key = "FORCE";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[FORCE]);
	}

	{
		QString key = "GRAV";
		MExtensionFactory factory = _data->_extFactoryManager.getFactory(key);
		Q_ASSERT(!factory.isNull());
		MJfdEntryHandler handler = factory.createExtension();
		Q_ASSERT(!handler.isNull());
		handler.initialize(_data->_model);
		handler.handleEntry(&_data->_stream, &_data->_logStream,
				_data->_blockNum[GRAV]);
	}

//-- 处理数据 ---------------------------------------------------------------------
	_data->_handleLoadCase();

	MDataManager mpc = _data->_baseManager.createDataManager();
	bool ok = mpc.open(_data->_model, "MPC");
	if (!ok)
	{
		QString errorMessage = "can't open MPC MDataManager in MJfdDataImportorImpl::import() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	if (mpc.getDataCount() > 0)
	{
		MTaskManager mpcTask = _data->_extManager.createExtension("org.sipesc.fems.coupledeqs.MCoupledEqsPreprocessorManager");
		Q_ASSERT(!mpcTask.isNull());
		ok = mpcTask.initialize(_data->_model);
		if (!ok)
		{
			QString errorMessage = "MCoupledEquationPreprocessorManager initialize() failed in MJfdDataImportorImpl::import()";
			qDebug() << errorMessage;
			_data->_monitor.setMessage(errorMessage);
			return false;
		}

		mpcTask.start();
	}


	return true;
}

