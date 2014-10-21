/*
 * mjfd3dsolidelementhandlerimpl.cxx
 *
 *  Created on: 2013-1-19
 *      Author: cyz
 */
#include <mjfd3dsolidelementhandlerimpl.h>

#include <qdebug.h>
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

class MJfd3DSolidElementHandlerImpl::Data
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
	MDataManager _hexaManager;

	MDataManager _material;

	MDataModel _propertyRefPath;
	MDataManager _general;

	MDataManager _eleGroup;//单元组数据

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled line of LINEELEMENT ="
					+ QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}

		return line;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfd3DSolidElementHandlerImpl::MJfd3DSolidElementHandlerImpl()
{
	_data.reset(new MJfd3DSolidElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfd3DSolidElementHandlerImpl::"
				"MJfd3DSolidElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfd3DSolidElementHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfd3DSolidElementHandlerImpl::~MJfd3DSolidElementHandlerImpl()
{
}

bool MJfd3DSolidElementHandlerImpl::initialize(MDataModel& model, bool isRepeated)
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

	_data->_material = _data->_baseManager.createDataManager();
	bool ok = _data->_material.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Material));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Material DataManager "
				" in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_elementPath = _data->_baseManager.createDataModel();
	ok = _data->_elementPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::ElementPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::ElementPath DataModel "
				" in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_hexaManager = _data->_baseManager.createDataManager();
	ok = _data->_hexaManager.open(_data->_elementPath, _data->_elesGlobal.getValue(MElementsGlobal::HexaBrickElement));
	if (!ok)
	{
		QString errorMessage = "can't open MELEMENTS::HexaBrickElement DataManager "
				"   in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_propertyRefPath = _data->_baseManager.createDataModel();
	ok = _data->_propertyRefPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::PropertyRefPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::PropertyRefPath DataModel "
				"in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_general = _data->_baseManager.createDataManager();
	ok = _data->_general.open(_data->_propertyRefPath, _data->_femsGlobal.getValue(MFemsGlobal::GeneralRef));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::GeneralRef DataManager "
				" in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_eleGroup = _data->_baseManager.createDataManager();
	ok = _data->_eleGroup.open(_data->_model, "EleGroup");
	if (!ok)
	{
		QString errorMessage = "can't open EleGroup DataManager "
				" in MJfd3DSolidElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfd3DSolidElementHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
   3   9   0   0       0  12           4   4       1       1   1
    12.6504e+00
3.4500e+07     0.200
    1    2    3    4    5    6    7    8
    1   12   12    1    1
   204   203   202   201   244   243   242   241
                                               0   224   223   222   221

[1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数
 *  |56~59|:本组材料类型标志
 *  |60~63|:本组材料类型总数

[2]
 *  |0~4|   :本组材料号
 *  |5~14|:该材料密度

[3]
 *  |0~9|     :杨式模量
 *  |10~19|:泊松比

 ......其他材料

 [4] 跳过，应力输出节点

 [5]
 *  |0~4|     :单元号
 *  |5~9|     :单元节点数
 *  |20~24|:材料号

 [6]
*  |0~5|     :node1
*  |6~9|     :node2
*  |12~17|:node3
*  |18~23|:node4
*  |24~29|:node5
*  |30~35|:node6
*  |36~41|:node7
*  |42~47|:node8

 [7]
*  |48~53|:node9
*  |54~59|:node10
*  |60~65|:node11
*  |66~71|:node12
 */

/*
 		QVector<QVariant> value;
		value.append(nEle);//本组单元数
		value.append(matType);//本组材料类型
		value.append(nMat);//本组材料种类数
 */
bool MJfd3DSolidElementHandlerImpl::handleEntry(QTextStream* stream, QTextStream* logStream, const QVector<QVariant>& value)
{
	Q_ASSERT(_data->_isInitialized);

	_data->_stream = stream;
	_data->_logStream = logStream;

	QString dataType = "org.sipesc.fems.data.propertydata";
	MDataFactory factory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!factory.isNull());

	QString dataType1 = "org.sipesc.fems.data.hexabrickelementdata";
	MDataFactory factory1 = _data->_factoryManager.getFactory(dataType1);
	Q_ASSERT(!factory1.isNull());

	QString dataType2 = "org.sipesc.fems.data.propertyrefdata";
	MDataFactory factory2 = _data->_factoryManager.getFactory(dataType2);
	Q_ASSERT(!factory2.isNull());

	int nEle = value[0].toInt();//本组单元数
	QString matType = value[1].toString();//本组材料类型
	int nMat = value[2].toInt();//本组材料种类数
	int eleGroupId =  value[3].toInt();//本组单元组号

	QList<int> eles;//本组单元号List

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
	int matCount = _data->_material.getDataCount();//现有材料数据个数

	for (int i = 0; i < nMat; i++)
	{
		QString line = _data->_readLine();
		//默认，每组材料号严格按照1～nMat 排列
		int matId = matCount + line.mid(0,5).toInt();// |0~4|   :本组材料号
		double DENS = line.mid(5,10).toDouble();//|5~14|:该材料密度

		line = _data->_readLine();
		double E = line.mid(0,10).toDouble();//|0~9|     :杨式模量
		double V = line.mid(10,10).toDouble();//|10~19|:泊松比

		MPropertyData matData= factory.createObject();
		matData.setType(matType);
		matData.setId(matId); //材料id
		matData.setValueCount(3); //材料value个数
		matData.setValue(0, DENS);
		matData.setValue(1, E);
		matData.setValue(2, V);

		_data->_material.appendData(matData);
	}
	 //[4] 跳过，应力输出节点
	_data->_readLine();

	/*
	 [6]
	 *  |0~5|     :node1
	 *  |6~9|     :node2
	 *  |12~17|:node3
	 *  |18~23|:node4
	 *  |24~29|:node5
	 *  |30~35|:node6
	 *  |36~41|:node7
	 *  |42~47|:node8

	 [7]
	 *  |48~53|:node9
	 *  |54~59|:node10
	 *  |60~65|:node11
	 *  |66~71|:node12
	 */

//	int eleCount = _data->_hexaManager.getDataCount();
	int eleCount = _data->_elementPath.getDataCount();

	for (int i = 0; i < nEle; i++)
	{
		MElementData eleData1 = factory1.createObject();
		MElementData eleData2 = factory1.createObject();
		/*
		 [5]
		 *  |0~4|     :单元号
		 *  |5~9|     :单元节点数
		 *  |20~24|:材料号
		 */
		QString line = _data->_readLine();
//		qDebug() << line.mid(0,5).toInt();
		//默认，每组单元号严格按照1～nEle 排列
		int eleId1 = eleCount + 2*line.mid(0,5).toInt()-1;// |0~4|   :单元号
		int eleId2 = eleCount + 2*line.mid(0,5).toInt();// |0~4|   :单元号
		int  ematId = matCount + line.mid(20,5).toInt();// |20~24|:材料号

		eles.append(eleId1);
		eles.append(eleId2);

//		qDebug() << eleId1 << "*****" << eleId2 << "*****" << ematId;
		/*
		 [6]
		 *  |0~5|     :node1
		 *  |6~9|     :node2
		 *  |12~17|:node3
		 *  |18~23|:node4
		 *  |24~29|:node5
		 *  |30~35|:node6
		 *  |36~41|:node7
		 *  |42~47|:node8
		 */
		line = _data->_readLine();

		int node1 = line.mid(0,6).toInt();// |0~5|     :node1
		int node2 = line.mid(6,6).toInt();// |6~9|     :node2
		int node3 = line.mid(12,6).toInt();// |12~17|:node3
		int node4 = line.mid(18,6).toInt();// |18~23|:node4
		int node5 = line.mid(24,6).toInt();// |24~29|:node5
		int node6 = line.mid(30,6).toInt();// |30~35|:node6
		int node7 = line.mid(36,6).toInt();// |36~41|:node7
		int node8 = line.mid(42,6).toInt();// |42~47|:node8

		/*
		 [7]
		 *  |48~53|:node9
		 *  |54~59|:node10
		 *  |60~65|:node11
		 *  |66~71|:node12
		 */
		line = _data->_readLine();
		int node9 = line.mid(48,6).toInt();//   |48~53|:node9
		int node10 = line.mid(54,6).toInt();// ||54~59|:node10
		int node11 = line.mid(60,6).toInt();// ||60~65|:node11
		int node12 = line.mid(66,6).toInt();// ||66~71|:node12

		eleData1.setId(eleId1);
		eleData2.setId(eleId2);
		//单元性质引用Id
		eleData1.setPropertyId(ematId);
		eleData2.setPropertyId(ematId);

		eleData1.setNodeId(0, node1);
		eleData1.setNodeId(1, node2);
		eleData1.setNodeId(2, node3);
		eleData1.setNodeId(3, node4);
		eleData1.setNodeId(4, node9);
		eleData1.setNodeId(5, node10);
		eleData1.setNodeId(6, node11);
		eleData1.setNodeId(7, node12);

		eleData2.setNodeId(0, node9);
		eleData2.setNodeId(1, node10);
		eleData2.setNodeId(2, node11);
		eleData2.setNodeId(3, node12);
		eleData2.setNodeId(4, node5);
		eleData2.setNodeId(5, node6);
		eleData2.setNodeId(6, node7);
		eleData2.setNodeId(7, node8);

		_data->_hexaManager.appendData(eleData1);
		_data->_hexaManager.appendData(eleData2);

		MPropertyRefData property = factory2.createObject();
		property.setId(ematId);
		property.setPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::MaterialId), ematId);
		_data->_general.appendData(property);
	}

	int ecnt = eles.count();
	MPropertyData elesGroupData = factory.createObject();
	Q_ASSERT(!elesGroupData.isNull());

	elesGroupData.setType("HexaBrickElement");
	elesGroupData.setId(eleGroupId);
	elesGroupData.setValueCount(ecnt);

	for (int i = 0; i < ecnt; i++)
		elesGroupData.setValue(i,eles[i]);

	_data->_eleGroup.appendData(elesGroupData);

	return true;
}
