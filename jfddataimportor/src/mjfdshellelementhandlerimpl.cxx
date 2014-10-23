/*
 * mjfdshellelementhandlerimpl.cxx
 *
 *  Created on: 2014-10-12
 *      Author: whb
 */
#include <mjfdshellelementhandlerimpl.h>

#include <qdebug.h>
#include <QTextStream>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.global.melementsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.core.engdbs.data.mbytearray.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.mpropertyrefdata.h>
#include <org.sipesc.fems.data.melementdata.h>

#include <org.sipesc.utilities.mextensionfactorymanager.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::utilities;

class MJfdShellElementHandlerImpl::Data {
public:
	Data() {
		_handledLine = 0;
		_isInitialized = false;
		_stream = 0;
		_logStream = 0;
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
	MDataManager _eleGroup;                                              //单元组数据
	MDataManager _quaddkqshManager;                                         //01

	MDataModel _propertyRefPath;
	MDataManager _general;
	MDataManager _geometry;
	MDataManager _material;

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
				{
			QString stateDes =
					"importing jfd file, handled line of SHELLELEMENT ="
							+ QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}

		return line;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdShellElementHandlerImpl::MJfdShellElementHandlerImpl() {
	_data.reset(new MJfdShellElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdShellElementHandlerImpl::"
				"MJfdShellElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject(
			"org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());

	_data->_monitor = util.createProgressMonitor("MJfdShellElementHandler",
			ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdShellElementHandlerImpl::~MJfdShellElementHandlerImpl() {
}

bool MJfdShellElementHandlerImpl::initialize(MDataModel& model,
		bool isRepeated) {
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	_data->_femsGlobal = _data->_objManager.getObject(
			"org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());

	_data->_elesGlobal = _data->_objManager.getObject(
			"org.sipesc.fems.global.elementsglobal");
	Q_ASSERT(!_data->_elesGlobal.isNull());

	_data->_baseManager = _data->_objManager.getObject(
			"org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_factoryManager = _data->_objManager.getObject(
			"org.sipesc.core.engdbs.mdatafactorymanager");
	Q_ASSERT(!_data->_factoryManager.isNull());

	//-- 数据库 ------------------------------------
	// 单元
	_data->_elementPath = _data->_baseManager.createDataModel();
	bool ok = _data->_elementPath.open(_data->_model,
			_data->_femsGlobal.getValue(MFemsGlobal::ElementPath));
	if (!ok) {
		QString errorMessage = "can't open MFEMS::ElementPath DataModel "
				" in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}
	_data->_quaddkqshManager = _data->_baseManager.createDataManager();
	ok = _data->_quaddkqshManager.open(_data->_elementPath, "QuadDKQShElement"); //01
	if (!ok) {
		QString errorMessage =
				"can't open MELEMENTS::QuaddkqshElement DataManager "
						"   in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}
	_data->_eleGroup = _data->_baseManager.createDataManager();
	ok = _data->_eleGroup.open(_data->_model, "EleGroup");
	if (!ok) {
		QString errorMessage = "can't open EleGroup DataManager "
				" in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	// 属性 - 材料 几何
	_data->_propertyRefPath = _data->_baseManager.createDataModel();
	ok = _data->_propertyRefPath.open(_data->_model,
			_data->_femsGlobal.getValue(MFemsGlobal::PropertyRefPath));
	if (!ok) {
		QString errorMessage = "can't open MFEMS::PropertyRefPath DataModel "
				"in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_general = _data->_baseManager.createDataManager();
	ok = _data->_general.open(_data->_propertyRefPath,
			_data->_femsGlobal.getValue(MFemsGlobal::GeneralRef));
	if (!ok) {
		QString errorMessage = "can't open MFEMS::GeneralRef DataManager "
				" in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_material = _data->_baseManager.createDataManager();
	ok = _data->_material.open(_data->_model,
			_data->_femsGlobal.getValue(MFemsGlobal::Material));
	if (!ok) {
		QString errorMessage = "can't open MFEMS::Material DataManager "
				" in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_geometry = _data->_baseManager.createDataManager();
	ok = _data->_geometry.open(_data->_model,
			_data->_femsGlobal.getValue(MFemsGlobal::Geometry));
	if (!ok) {
		QString errorMessage = "can't open MFEMS::Geometry DataManager "
				" in MJfdShellElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdShellElementHandlerImpl::getProgressMonitor() const {
	return _data->_monitor;
}
/*
 7 105   0               1           0                            //线性单元组 1(1号桥面系线性单元组板壳单元数：105)
 18.0020e+00
 2.0600e+08     0.200
 0.069
 1    0    0    1
 1     2    24    23

 [1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数

 [2]
 *  |0~4|   :本组材料号
 *  |5~14|:该材料密度

 [3]
 *  |0~9|     :杨式模量
 *  |10~19|:泊松比

 ......其他材料

 [4]
 *  |0-9|    :单元厚度                                      //这里没有应力输出位置，第四行直接是厚度卡

 [5]
 *  |0~4|     :单元号
 *  |5~9|     :有问题，暂不读取
 *  |10~14|:有问题，暂不读取
 *  |15~19|:单元材料号

 [6]
 *  |0~5|     :node1
 *  |6~11|   :node2
 *  |12~17|:node3
 *  |18~23|:node4

 */

/*
 QVector<QVariant> value;
 value.append(nEle);//本组单元数
 value.append(matType);//本组材料类型
 value.append(nMat);//本组材料种类数
 */
bool MJfdShellElementHandlerImpl::handleEntry(QTextStream* stream,
		QTextStream* logStream, const QVector<QVariant>& value) {
	Q_ASSERT(_data->_isInitialized);

	_data->_stream = stream;
	_data->_logStream = logStream;

	QString dataType = "org.sipesc.fems.data.propertydata";
	MDataFactory factory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!factory.isNull());

	QString dataType1 = "org.sipesc.fems.data.quaddkqshelementdata";        //01
	MDataFactory factory1 = _data->_factoryManager.getFactory(dataType1);
	Q_ASSERT(!factory1.isNull());

	QString dataType2 = "org.sipesc.fems.data.propertyrefdata";
	MDataFactory factory2 = _data->_factoryManager.getFactory(dataType2);
	Q_ASSERT(!factory2.isNull());

	int nEle = value[0].toInt();                                         //本组单元数
	QString matType = value[1].toString();                              //本组材料类型
	int nMat = value[2].toInt();                                       //本组材料种类数
	int eleGroupId = value[3].toInt();                                  //本组单元组号

	QList<int> eles;                                               //本组单元号List

	//先处理材料
	/*
	 [2]
	 *  |0~4|   :本组材料号
	 *  |5~14|:该材料密度

	 [3]
	 *  |0~9|     :杨式模量
	 *  |10~19|:泊松比

	 ......其他材料
	 */
	int matCount = _data->_material.getDataCount();                   //现有材料数据个数

	for (int i = 0; i < nMat; i++) {
		QString line = _data->_readLine();
		//默认，每组材料号严格按照1～nMat 排列
		int matId = matCount + line.mid(0, 5).toInt(); // |0~4|:本组材料号                                        //将组内的每一种材料都在整体情况下编号，存入密度、杨氏模量和泊松比，所有这些形成材料数据
		double DENS = line.mid(5, 10).toDouble();                 //|5~14|:该材料密度

		line = _data->_readLine();
		double E = line.mid(0, 10).toDouble();                 //|0~9|     :杨式模量
		double V = line.mid(10, 10).toDouble();                    //|10~19|:泊松比

		MPropertyData matData = factory.createObject();
		matData.setType(matType);
		matData.setId(matId); //材料id
		matData.setValueCount(3); //材料value个数
		matData.setValue(0, DENS);
		matData.setValue(1, E);
		matData.setValue(2, V);

		_data->_material.appendData(matData);
	}
	//	 [4] 跳过，应力输出节点
	//	_data->_readLine();                                     这里没有应力输出位置，第四行直接是厚度卡

	/*
	 * [4]
	 *  |0-9|    :单元厚度，这里按照单元号进行存储

	 [5]
	 *  |0~4|     :单元号
	 *  |5~9|     :有问题，暂不读取
	 *  |10~14|:有问题，暂不读取
	 *  |15~19|:单元材料号

	 [6]
	 *  |0~5|     :node1
	 *  |6~11|   :node2
	 *  |12~17|:node3
	 *  |18~23|:node4
	 */

	int eleCount = _data->_elementPath.getDataCount();

	for (int i = 0; i < nEle; i++) {
		MPropertyData geometryData = factory.createObject();
		MElementData eleData = factory1.createObject();
		/*
		 [4]
		 *  |0-9|    :单元厚度，这里按照单元号进行存储
		 [5]
		 *  |0~4|     :单元号
		 *  |5~9|     :有问题，暂不读取
		 *  |10~14|:有问题，暂不读取
		 *  |15~19|:单元材料号
		 */
		QString line = _data->_readLine();
		double T = line.mid(0, 10).toDouble();	//|0~9|  :单元厚度

		line = _data->_readLine();
		//默认，每组单元号严格按照1～nEle 排列
		int eleId = eleCount + line.mid(0, 5).toInt();// |0~4|   :单元号                                             //把每一个单元在整体情况下单元路径elementpath下编号，存入单元的节点信息
		int ematId = matCount + line.mid(15, 5).toInt();// |15~19|:单元材料号                                   //这里的材料号，是指该单元的材料号，通过这个材料号，可以从材料数据文件中调取对应的材料数据

		geometryData.setId(eleId); //设置几何性质Id，取单元号作为几何性质ID
		geometryData.setType(_data->_elesGlobal.getValue(MElementsGlobal::GeoConst)); //设置几何性质类型为：常实数
		geometryData.setValueCount(1); //设置value值个数为：1 ，单元厚度
		geometryData.setValue(0, T); //单元厚度
		_data->_geometry.appendData(geometryData);

		eles.append(eleId);              //eles代表单元组的单元号序列，每读入一个单元，就将它的单元号存入eles

		/*
		 [6]
		 *  |0~5|     :node1
		 *  |6~11|   :node2
		 *  |12~17|:node3
		 *  |18~23|:node4
		 */
		line = _data->_readLine();

		int node1 = line.mid(0, 6).toInt(); // |0~5|     :node1                                             //按照列号依次读入每个单元
		int node2 = line.mid(6, 6).toInt();                 // |6~11|     :node2
		int node3 = line.mid(12, 6).toInt();                    // |12~17|:node3
		int node4 = line.mid(18, 6).toInt();                    // |18~23|:node4

		eleData.setId(eleId);
		eleData.setPropertyId(eleId);//单元性质引用Id                                            //！！！！！！！！

		eleData.setNodeId(0, node1);
		eleData.setNodeId(1, node2);
		eleData.setNodeId(2, node3);
		eleData.setNodeId(3, node4);

		_data->_quaddkqshManager.appendData(eleData);

		MPropertyRefData property = factory2.createObject();
		property.setId(eleId);                                        //！！！！！！！！
		property.setPropertyId(
				_data->_elesGlobal.getValue(MElementsGlobal::MaterialId),
				ematId);
		property.setPropertyId(
				_data->_elesGlobal.getValue(MElementsGlobal::GeometryId),
				eleId);                 //！！！！！！！
		_data->_general.appendData(property);
	}

	int ecnt = eles.count();
	MPropertyData elesGroupData = factory.createObject();
	Q_ASSERT(!elesGroupData.isNull());

	elesGroupData.setType("QuadDKQShElement");
	elesGroupData.setId(eleGroupId);
	elesGroupData.setValueCount(ecnt);

	for (int i = 0; i < ecnt; i++)
		elesGroupData.setValue(i, eles[i]);

	_data->_eleGroup.appendData(elesGroupData);

	return true;
}
