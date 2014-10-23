/*
 * mjfdcrodelementhandlerimpl.cxx
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */

#include <mjfdcrodelementhandlerimpl.h>

#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.global.melementsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.mpropertyrefdata.h>
#include <org.sipesc.fems.data.melementdata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;

class MJfdCrodElementHandlerImpl::Data
{
public:
	Data()
	{
		_handledLine = 0;
		_isInitialized = false;
	}
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;
	MElementsGlobal _elesGlobal;
	MDatabaseManager _baseManager;
	MDataFactoryManager _factoryManager;

	MDataModel _model;

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流

	MDataModel _elementPath;
	MDataManager _rodManager;

	MDataManager _material;

	MDataModel _propertyRefPath;
	MDataManager _general;
	MDataManager _geometry;
	MDataManager _eleGroup;//单元组数据

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled line of NLINEELEMENT ="
					+ QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}

		return line;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdCrodElementHandlerImpl::MJfdCrodElementHandlerImpl()
{
	_data.reset(new MJfdCrodElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdCrodElementHandlerImpl::"
				"MJfdCrodElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdCrodElementHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdCrodElementHandlerImpl::~MJfdCrodElementHandlerImpl()
{
}

bool MJfdCrodElementHandlerImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());

	_data->_elesGlobal = _data->_objManager.getObject("org.sipesc.fems.global.elementsglobal");
	Q_ASSERT(!_data->_elesGlobal.isNull());

	_data->_baseManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_factoryManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatafactorymanager");
	Q_ASSERT(!_data->_factoryManager.isNull());

	_data->_elementPath = _data->_baseManager.createDataModel();
	bool ok = _data->_elementPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::ElementPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::ElementPath DataModel "
				" in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_rodManager = _data->_baseManager.createDataManager();
	ok = _data->_rodManager.open(_data->_elementPath, _data->_elesGlobal.getValue(MElementsGlobal::BarElement));
	if (!ok)
	{
		QString errorMessage = "can't open MELEMENTS::BarElement DataManager "
				"   in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_material = _data->_baseManager.createDataManager();
	ok = _data->_material.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Material));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Material DataManager "
				" in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_propertyRefPath = _data->_baseManager.createDataModel();
	ok = _data->_propertyRefPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::PropertyRefPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::PropertyRefPath DataModel "
				"in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_general = _data->_baseManager.createDataManager();
	ok = _data->_general.open(_data->_propertyRefPath, _data->_femsGlobal.getValue(MFemsGlobal::GeneralRef));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::GeneralRef DataManager "
				" in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_geometry = _data->_baseManager.createDataManager();
	ok = _data->_geometry.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Geometry));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Geometry DataManager "
			" in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_eleGroup = _data->_baseManager.createDataManager();
	ok = _data->_eleGroup.open(_data->_model, "EleGroup");
	if (!ok)
	{
		QString errorMessage = "can't open EleGroup DataManager "
				" in MJfdCrodElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdCrodElementHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
   1   1   2       0                           0   0       1   1
    11.6820e-038.0020e+00
2.0000e+08
    1    0    1    1    00.00321596
 14809 14810

 [1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数
 *  |56~59|:本组材料类型标志
 *  |60~63|:本组材料类型总数

 [2]
 *  |0~4|      :材料号
 *  |5~14|   :横截面积
 *  |15~24|:该材料密度

 [3]
 *  |0~9|     :杨式模量

 ......其他材料

 [4]
 *  |0~4|     :单元号
 *  |15~19|:材料号

 [5]
 *  |0~5|     :node1
 *  |6~11|  :node2
 */
/*
 		QVector<QVariant> value;
		value.append(nEle);//本组单元数
		value.append(matType);//本组材料类型
		value.append(nMat);//本组材料种类数
 */
bool MJfdCrodElementHandlerImpl::handleEntry(QTextStream* stream, QTextStream* logStream, const QVector<QVariant>& value)
{
	Q_ASSERT(_data->_isInitialized);

	_data->_stream = stream;
	_data->_logStream = logStream;

	int nEle = value[0].toInt();//本组单元数
	QString matType = value[1].toString();//本组材料类型
	int nMat = value[2].toInt();//本组材料种类数
	int eleGroupId =  value[3].toInt();//本组单元组号

	QList<int> eles;//本组单元号List

	//先处理材料
	/*
       [2]
       *  |0~4|      :材料号
       *  |5~14|   :横截面积
       *  |15~24|:该材料密度

       [3]
       *  |0~9|     :杨式模量

	 ......其他材料
	 */
	int matCount = _data->_material.getDataCount();//现有材料数据个数

	for (int i = 0; i < nMat; i++)
	{
		QString line = _data->_readLine();
		//默认，每组材料号严格按照1～nMat 排列
		int matId = matCount + line.mid(0,5).toInt();// |0~4|   :材料号
		double A = line.mid(5,10).toDouble();//|5~14|  :横截面积
		double DENS = line.mid(15,10).toDouble();//|5~14|:该材料密度

		line = _data->_readLine();
		double E = line.mid(0,10).toDouble();//|0~9|     :杨式模量

		QString dataType = "org.sipesc.fems.data.propertydata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MPropertyData matData = factory.createObject();
		matData.setType(matType);
		matData.setId(matId); //材料id
		matData.setValueCount(3); //材料value个数
		matData.setValue(0, DENS);
		matData.setValue(1, E);

		_data->_material.appendData(matData);

		MPropertyData geometryData = factory.createObject();
		geometryData.setId(matId); //设置几何性质Id
		geometryData.setType(_data->_elesGlobal.getValue(MElementsGlobal::GeoConst)); //设置几何性质类型为：常实数
		geometryData.setValueCount(1); //设置value值个数为：1 ，截面积
		geometryData.setValue(0, A); //截面积
		_data->_geometry.appendData(geometryData);

	}

//	int eleCount = _data->_rodManager.getDataCount();

    int eleCount = _data->_elementPath.getDataCount();

	for (int i = 0; i < nEle; i++)
	{
		/*
	     [4]
	     *  |0~4|     :单元号
	     *  |15~19|:材料号
		 */
		QString line = _data->_readLine();

		QString dataType = "org.sipesc.fems.data.barelementdata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MElementData eleData = factory.createObject();//默认，每组单元号严格按照1～nEle 排列
		int eleId = eleCount + line.mid(0,5).toInt();// |0~4|   :单元号
		int  ematId = matCount + line.mid(15,5).toInt();//|15~19|:材料号
		eles.append(eleId);
		/*
		[5]
		 *  |0~5|     :node1
		 *  |6~11|  :node2
		 */
		line = _data->_readLine();
		int node1 = line.mid(0,6).toInt();// |0~5|     :node1
		int node2 = line.mid(6,6).toInt();// |6~9|     :node2

		eleData.setId(eleId);

		//单元性质引用Id
		eleData.setPropertyId(ematId);
		eleData.setNodeId(0, node1);
		eleData.setNodeId(1, node2);
		_data->_rodManager.appendData(eleData);

		QString dataType2 = "org.sipesc.fems.data.propertyrefdata";
		MDataFactory factory2 = _data->_factoryManager.getFactory(dataType2);
		Q_ASSERT(!factory2.isNull());

		MPropertyRefData property = factory2.createObject();
		property.setId(ematId);
		property.setPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::MaterialId), ematId);
		property.setPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::GeometryId), ematId);
		_data->_general.appendData(property);
	}

	{
		QString dataType = "org.sipesc.fems.data.propertydata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());
		int ecnt = eles.count();
		MPropertyData elesGroupData = factory.createObject();
		Q_ASSERT(!elesGroupData.isNull());
		elesGroupData.setType(_data->_elesGlobal.getValue(MElementsGlobal::BarElement));
		elesGroupData.setId(eleGroupId);
		elesGroupData.setValueCount(ecnt);
		for (int i = 0; i < ecnt; i++)
			elesGroupData.setValue(i,eles[i]);
		_data->_eleGroup.appendData(elesGroupData);
	}

	return true;
}
