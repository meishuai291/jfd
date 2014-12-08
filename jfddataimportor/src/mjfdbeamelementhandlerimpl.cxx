/*
 * mjfdbeamelementhandlerimpl.cxx
 *
 *  Created on: 2014年12月1日
 *      Author: sipesc
 */

#include <mjfdbeamelementhandlerimpl.h>

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
#include <org.sipesc.fems.data.mnodedata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;

#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixdata.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
using namespace org::sipesc::fems::matrix;

class MJfdBeamElementHandlerImpl::Data
{
public:
	Data()
	{
		_stream = 0;
		_logStream = 0;
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
	MMatrixFactory _mFactory;
	MVectorFactory _vFactory;

	MDataModel _model;

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流

	MDataModel _elementPath;
	MDataManager _beamManager;

	MDataManager _material;

	MDataModel _propertyRefPath;
	MDataManager _general;
	MDataManager _geometry,_varGeos,_geoReferManager;
	MDataManager _eleGroup;//单元组数据
	MDataManager _node;

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

MJfdBeamElementHandlerImpl::MJfdBeamElementHandlerImpl()
{
	_data.reset(new MJfdBeamElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdBeamElementHandlerImpl::"
				"MJfdBeamElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdBeamElementHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());

	_data->_vFactory = _data->_objManager.getObject(
				"org.sipesc.fems.matrix.vectorfactory");
	Q_ASSERT(!_data->_vFactory.isNull());
}

MJfdBeamElementHandlerImpl::~MJfdBeamElementHandlerImpl()
{
}

bool MJfdBeamElementHandlerImpl::initialize(MDataModel& model, bool isRepeated)
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
				" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_beamManager = _data->_baseManager.createDataManager();
	ok = _data->_beamManager.open(_data->_elementPath, _data->_elesGlobal.getValue(MElementsGlobal::BeamElement));
	if (!ok)
	{
		QString errorMessage = "can't open MELEMENTS::BeamElement DataManager "
				"   in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_material = _data->_baseManager.createDataManager();
	ok = _data->_material.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Material));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Material DataManager "
				" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_propertyRefPath = _data->_baseManager.createDataModel();
	ok = _data->_propertyRefPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::PropertyRefPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::PropertyRefPath DataModel "
				"in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_general = _data->_baseManager.createDataManager();
	ok = _data->_general.open(_data->_propertyRefPath, _data->_femsGlobal.getValue(MFemsGlobal::GeneralRef));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::GeneralRef DataManager "
				" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_geometry = _data->_baseManager.createDataManager();
	ok = _data->_geometry.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Geometry));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Geometry DataManager "
			" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_varGeos = _data->_baseManager.createDataManager();
	ok = _data->_varGeos.open(_data->_model,
		  _data->_femsGlobal.getValue(MFemsGlobal::VariGeometry));
	if (!ok)
	{
		QString errorMessage = "can't open Variable Geometry DataManager "
			" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_geoReferManager = _data->_baseManager.createDataManager();
	ok = _data->_geoReferManager.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::GeoOffsetRefer));
	if (!ok) {
		QString errorMessage = "can't open GeoOffsetRefer DataManager"
				" in  MJfdBeamElementHandlerImpl::initialize()";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_eleGroup = _data->_baseManager.createDataManager();
	ok = _data->_eleGroup.open(_data->_model, "EleGroup");
	if (!ok)
	{
		QString errorMessage = "can't open EleGroup DataManager "
				" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_node = _data->_baseManager.createDataManager();
	ok = _data->_node.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Node),true);
	if (!ok)
	{
		QString errorMessage = "can't open Node DataManager "
				" in MJfdBeamElementHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdBeamElementHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
----------------
   4   3   0    0   1                           0           1   1                  线性单元组 8(7组线性梁单元组非桥面系（1 单元1端系梁)单元数:3)
    13.4500e+07     0.2002.6504e+00
3.2958e+011.8667e+011.4292e+011.4000e+01     0.000     0.000
    1  470  605  604    1    1
    2  605  606  604    1    1
    3  606  519  604    1    1
---------------

 [1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数
 *  |8~11|    :非线性分析类型标志
 *  |56~59|   :本组材料类型标志
 *  |60~63|   :本组材料类型总数

 [2]
 *  |0~4|      :材料号
 *  |5~14|     :杨式模量
 *  |15~24|    :泊松比
 *  |25~34|    :该材料密度

 [3] 截面
 *  |0~9|     :r轴
 *  |10~19|   :s轴
 *  |20~29|   :t轴
 *  |30~39|   :截面积
 *  |40~49|   :s方向有效剪切面积
 *  |50~59|   :t方向有效剪切面积

 [4]
 *  |0~4|     :单元号
 *  |5~9|     :节点号
 *  |10~14|   :节点号
 *  |15~19|   :局部坐标辅助节点
 *  |20~24|   :材料

/*
 		QVector<QVariant> value;
		value.append(nEle);//本组单元数
		value.append(matType);//本组材料类型
		value.append(nMat);//本组材料种类数
 */
bool MJfdBeamElementHandlerImpl::handleEntry(QTextStream* stream, QTextStream* logStream, const QVector<QVariant>& value)
{
	Q_ASSERT(_data->_isInitialized);

	_data->_stream = stream;
	_data->_logStream = logStream;

	int nEle = value[0].toInt();//本组单元数
	QString matType = value[1].toString();//本组材料类型
	int nMat = value[2].toInt();//本组材料种类数
	int eleGroupId =  value[3].toInt();//本组单元组号
	int nonline(0);

	if(matType != "SolidIsotropy")
		nonline = 1;

	QList<int> eles;//本组单元号List

	//先处理材料
	/*
	 [2]
	 *  |0~4|      :材料号
	 *  |5~14|     :杨式模量
	 *  |15~24|    :泊松比
	 *  |25~34|    :该材料密度

	 [3] 截面
	 *  |0~9|     :r轴
	 *  |10~19|   :s轴
	 *  |20~29|   :t轴
	 *  |30~39|   :截面积
	 *  |40~49|   :s方向有效剪切面积
	 *  |50~59|   :t方向有效剪切面积
	 */
	int matCount = _data->_material.getDataCount();//现有材料数据个数

	for (int i = 0; i < nMat; i++)
	{
		QString line = _data->_readLine();
		//默认，每组材料号严格按照1～nMat 排列
		int matId = matCount + line.mid(0,5).toInt();// |0~4|   :材料号
		int secId = matId;
		double E = line.mid(5,10).toDouble();// |5~14|     :杨式模量
		double pois = line.mid(15,10).toDouble();// |15~24|    :泊松比
		double DENS = line.mid(25,10).toDouble();//|25~34|	:该材料密度

		line = _data->_readLine();
		double rMoment = line.mid(0,10).toDouble();
		double sMoment = line.mid(10,10).toDouble();
		double tMoment = line.mid(20,10).toDouble();
		double A = line.mid(30,10).toDouble();

		QString dataType = "org.sipesc.fems.data.propertydata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MPropertyData matData = factory.createObject();
		matData.setType(matType);
		matData.setId(matId); //材料id
		matData.setValueCount(3); //材料value个数
		matData.setValue(0, DENS);
		matData.setValue(1, E);
		matData.setValue(2, pois);
		_data->_material.appendData(matData);

		MPropertyData geometryData = factory.createObject();
		geometryData.setId(matId); //设置几何性质Id
		geometryData.setType(_data->_elesGlobal.getValue(MElementsGlobal::GeoConst)); //设置几何性质类型为：常实数
		geometryData.setValueCount(1); // 设置value值个数为：1 ，截面Id
		geometryData.setValue(0, secId); // 截面Id
		_data->_geometry.appendData(geometryData);

		MPropertyData variGeoData = factory.createObject();
		variGeoData.setId(secId);
		QString cross = "PBAR";
		variGeoData.setType(cross);
		variGeoData.setValueCount(7);//7
		variGeoData.setValue(0, QVariant(A));		// 截面积
		variGeoData.setValue(1, QVariant(tMoment));	//I1 Izz t
		variGeoData.setValue(2, QVariant(sMoment));	//I2 Iyy s
		variGeoData.setValue(3, QVariant(rMoment));	//J  Ixx r
		variGeoData.setValue(4, QVariant(0.0));		//K1 y向 s
		variGeoData.setValue(5, QVariant(0.0));		//K2 z向 t
		variGeoData.setValue(6, QVariant(0.0));		//I12 Izy
		_data->_varGeos.appendData(variGeoData);

	}

//	int eleCount = _data->_beamManager.getDataCount();

    int eleCount = _data->_elementPath.getDataCount();
	for (int i = 0; i < nEle; i++)
	{
		/*
	     *  |0~4|     :单元号
		 *  |5~9|     :节点号
		 *  |10~14|   :节点号
		 *  |15~19|   :局部坐标辅助节点
		 *  |20~24|   :材料
		 */
		QString line = _data->_readLine();

		QString dataType = "org.sipesc.fems.data.beamelementdata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MElementData eleData = factory.createObject();//默认，每组单元号严格按照1～nEle 排列
		int eleId = eleCount + line.mid(0,5).toInt();// |0~4|   :单元号
		int  ematId = matCount + line.mid(20,5).toInt();//|20~24|:材料号
		eles.append(eleId);

		int node1 = line.mid(5,5).toInt();
		int node2 = line.mid(10,5).toInt();
		int node3 = line.mid(15,5).toInt();

		eleData.setId(eleId);

		//单元性质引用Id
		eleData.setPropertyId(ematId);
		eleData.setNodeId(0, node1);
		eleData.setNodeId(1, node2);
		_data->_beamManager.appendData(eleData);

		QString dataType2 = "org.sipesc.fems.data.propertyrefdata";
		MDataFactory factory2 = _data->_factoryManager.getFactory(dataType2);
		Q_ASSERT(!factory2.isNull());

		MPropertyRefData property = factory2.createObject();
		property.setId(ematId);
		property.setPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::MaterialId), ematId);
		property.setPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::GeometryId), ematId);
		_data->_general.appendData(property);

		MNodeData node1Data = _data->_node.getData(node1);
		MNodeData node3Data = _data->_node.getData(node3);
		double x2 = node3Data.getX() - node1Data.getX();
		double y2 = node3Data.getY() - node1Data.getY();
		double z2 = node3Data.getZ() - node1Data.getZ();

		MVector b = _data->_vFactory.createVector(9);
		b.fill(0);

		b(6,x2);
		b(7,y2);
		b(8,z2);

		MVectorData vData;
		b >> vData;
		vData.setId(eleId);
		_data->_geoReferManager.appendData(vData);
	}

	{
		QString dataType = "org.sipesc.fems.data.propertydata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());
		int ecnt = eles.count();
		MPropertyData elesGroupData = factory.createObject();
		Q_ASSERT(!elesGroupData.isNull());

		eleGroupId = _data->_eleGroup.getDataCount()+1;
		elesGroupData.setType(_data->_elesGlobal.getValue(MElementsGlobal::BeamElement));
		elesGroupData.setId(eleGroupId);
		elesGroupData.setValueCount(ecnt+1);
		elesGroupData.setValue(0,QVariant(nonline));
		for (int i = 0; i < ecnt; i++)
			elesGroupData.setValue(i+1,QVariant(eles[i]));
		_data->_eleGroup.appendData(elesGroupData);
	}

	return true;
}

