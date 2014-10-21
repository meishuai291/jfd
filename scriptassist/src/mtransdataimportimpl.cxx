/*
 * mtransdataimportimpl.cxx
 *
 *  Created on: 2013年11月27日
 *      Author: sipesc
 */
#include <mtransdataimportimpl.h>

#include <qmap.h>
#include <qdebug.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qtextstream.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>

#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.fems.data.mpropertydata.h>


#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>

#include <org.sipesc.utilities.mextensionfactory.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::utilities;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::matrix;

class MTransDataImportImpl::Data
{
public:
	Data()
	{
		_isInitialized = false;
	}
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;
	MDatabaseManager _baseManager;
	MDataFactoryManager _factoryManager;

	MDataManager _tstepManager;

	MDataModel _model;
	MDataModel _dispPath;

	MVectorFactory _vFactory;

	bool _isInitialized;
};

MTransDataImportImpl::MTransDataImportImpl()
{
	_data.reset(new MTransDataImportImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MTransDataImportImpl::"
				"MTransDataImportImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject(
			"org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MTransDataImport",
			ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MTransDataImportImpl::~MTransDataImportImpl()
{
}

bool MTransDataImportImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	_data->_baseManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_factoryManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatafactorymanager");
	Q_ASSERT(!_data->_factoryManager.isNull());

	_data->_vFactory = _data->_objManager.getObject("org.sipesc.fems.matrix.vectorfactory");
	Q_ASSERT(!_data->_vFactory.isNull());

	_data->_dispPath = _data->_baseManager.createDataModel();
	bool ok = _data->_dispPath.open(_data->_model, "DisplacementPath");
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::DisplacementPath DataModel "
				"in MTransDataImportImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_tstepManager = _data->_baseManager.createDataManager();
	ok = _data->_tstepManager.open(_data->_model, "TransientResp");
	if (!ok)
	{
		QString errorMessage = "can't open TransientResp DataManager "
				"in MTransDataImportImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MTransDataImportImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MTransDataImportImpl::start(const QVariant& control)
{
	//1. 打开文件
	QString importName = control.toString();

	QFile file;
	file.setFileName(importName);
	if (!file.open(QIODevice::ReadOnly)) //打开文件
	{
		QString errorMessage = "Can't open file in MTransDataImportImpl::start()!";
		_data->_monitor.setMessage(errorMessage);
		qDebug() << errorMessage;
		return false;
	}

	QTextStream stream;
	stream.setDevice(&file); //定义文件数据流

	//2. 首先读取第一个节点的时间历程数据，获取_nt,_dt
	int _nt = 0;
	double _dt = 0;

	QString line = stream.readLine();
	while (!stream.atEnd() && !line.isEmpty())
	{
		line = stream.readLine();
		_nt++;
	}
	_nt = _nt - 1;
	qDebug() << "nt = " << _nt;

	stream.seek(0);
	line = stream.readLine();

	line = stream.readLine();
	QStringList list = line.simplified().split(' ');
	double t0 = list[0].toDouble();

	line = stream.readLine();
	list = line.simplified().split(' ');
	double t1 = list[0].toDouble();

	_dt = t1 - t0;
	qDebug() << "dt = " << _dt;


	//3. 每个时间步下都要创建一个MDataManager，用来存储本时刻各节点时称响应结果
	//注意：： QMap容器无法存储太多的MDataManager，所以无法先打开存储，再写数据
//	for (int i = 0; i < _nt; i++)
//	{
//		MDataManager IDisManager = _data->_baseManager.createDataManager();
//		bool ok = IDisManager.open(_data->_dispPath, QString::number(i));
//		if (!ok)
//		{
//			QString errorMessage = "can't open " + QString::number(i) + " DataManager in MTransDataImportImpl::start() ";
//			qDebug() << errorMessage;
//			_data->_monitor.setMessage(errorMessage);
//			return false;
//		}
//
//		_data->_disMap.insert(i,IDisManager);
//	}

	//4. 创建时间步数据
	QString dataType = "org.sipesc.core.engdbs.MDataObjectList";
	MDataFactory factory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!factory.isNull());

	MDataObjectList tstepDataList = factory.createObject();
	tstepDataList.setId(1);

	QString dataType2 = "org.sipesc.fems.data.propertydata";
	MDataFactory factory2 = _data->_factoryManager.getFactory(dataType2);
	Q_ASSERT(!factory2.isNull());

	MPropertyData tstepData = factory2.createObject();
	tstepData.setId(1);
	tstepData.setType("TSTEP");

	tstepData.setValueCount(6);
	tstepData.setValue(0, 0.0); //初始值
	tstepData.setValue(1, _nt-1); //时间步数
	tstepData.setValue(2, _dt); //时间步长
	tstepData.setValue(3, 0); //跳步

	tstepDataList.appendData(tstepData);
	_data->_tstepManager.appendData(tstepDataList);


	//5. 将各节点点各时间步数据写入数据库
	stream.seek(0);

	int _handledLine = 0;
	while (!stream.atEnd())
	{
		line = stream.readLine();
		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing Transient file, handled line=" + QString::number(_handledLine);
			qDebug() << stateDes;
		}


		list = line.simplified().split(' ');
		int nodeId = list[0].toInt();
		qDebug() << "NodeId = " << nodeId;

		for (int i = 0; i < _nt; i++)
		{
			line = stream.readLine();
			_handledLine++;
			if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
			{
				QString stateDes = "importing Transient file, handled line=" + QString::number(_handledLine);
				qDebug() << stateDes;
			}

			list = line.simplified().split(' ');
			double x = list[1].toDouble();
			double y = list[2].toDouble();
			double z = list[3].toDouble();

//			qDebug() << x << ", "<< y << ", "<< z << ", !";
			MVector disp = _data->_vFactory.createVector(3);
			disp(0,x);
			disp(1,y);
			disp(2,z);

			MVectorData dispData;
			disp >> dispData;

			dispData.setId(nodeId);

			MDataManager IDisManager = _data->_baseManager.createDataManager();
			bool ok = IDisManager.open(_data->_dispPath, QString::number(i));
			if (!ok)
			{
				QString errorMessage = "can't open " + QString::number(i)
						+ " DataManager in MTransDataImportImpl::start() ";
				qDebug() << errorMessage;
				_data->_monitor.setMessage(errorMessage);
				return false;
			}
			IDisManager.appendData(dispData);
		}

		line = stream.readLine();
		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing Transient file, handled line=" + QString::number(_handledLine);
			qDebug() << stateDes;
		}
	}

	file.close();
	return true;
}

