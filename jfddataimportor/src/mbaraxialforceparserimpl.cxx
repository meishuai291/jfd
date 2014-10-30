/*
 * mbaraxialforceparserimpl.cxx
 *
 *  Created on: 2014年10月29日
 *      Author: sipesc
 */

#include <mbaraxialforceparserimpl.h>

#include <qdebug.h>
#include <cmath>
#include <iostream>


#include <mobjectmanager.h>
#include <mextensionmanager.h>

#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.utilities.mextensionfactory.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
//#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::utilities;

//#include <org.sipesc.fems.global.mfemsglobal.h>
//using namespace org::sipesc::fems::global;

#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixdata.h>
using namespace org::sipesc::fems::matrix;

#include <org.sipesc.fems.data.melementlocalcoorddata.h>
#include <org.sipesc.fems.data.mpropertyrefdata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.data.mpropertydata.h>
using namespace org::sipesc::fems::data;

#include <org.sipesc.fems.element.mmultigeometryparser.h>
#include <org.sipesc.fems.femsutils.msolidmaterialparser.h>
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::element;

class MBarAxialForceParserImpl::Data
{
public:
	Data()
	{
		_isInitialized = false;
		_objManager = MObjectManager::getManager();
		_extManager = MExtensionManager::getManager();
	}
public:
	MObjectManager _objManager;
	MExtensionManager _extManager;
	MDatabaseManager _baseManager;
	ProgressMonitor _monitor;

	MDataModel _model;
	MDataManager _materials;

	MDataManager _eleManager;
	MDataModel _propertysRef, _coordPath;
	MDataManager _propertysMgr, _geometryMgr, _varGeosMgr, _localDataMgr;

	QMap<int, MSolidMaterialParser> _matMatrixMap;
	MExtensionFactoryManager _matFactoryManager;
	MExtensionFactoryManager _geoFactoryManager;

	MMatrixFactory _mFactory;
	MVectorFactory _vFactory;
	bool _isInitialized;

public:
    MSolidMaterialParser findMatConstiDParser(const int& index){
		//首先在Map中查找
		if (_matMatrixMap.contains(index))
			return _matMatrixMap[index];

		MPropertyData matData;
		matData = _materials.getData(index);

		//若Map中没有,通过工厂管理器查找,初始化后放入Map中
		QString type = matData.getType();
		//      qDebug()<<"material type = "<< type;
		MExtensionFactory factory = _matFactoryManager.getFactory(type);
		Q_ASSERT(!factory.isNull());

		MSolidMaterialParser materialParser = factory.createExtension();
		Q_ASSERT(!materialParser.isNull());

		if (materialParser.initialize(_model)){
			materialParser.setState(matData);
			_matMatrixMap.insert(index, materialParser);
		}

		return materialParser;
	}
};

MBarAxialForceParserImpl::MBarAxialForceParserImpl(){
	_data.reset(new MBarAxialForceParserImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MBarAxialForceParserImpl::"
				"MBarAxialForceParserImpl() ****** failed");

	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MBarAxialForceParser", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());

	_data->_baseManager = _data->_objManager.getObject(
			"org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_matFactoryManager = _data->_extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_matFactoryManager.isNull());
	bool isOk = _data->_matFactoryManager.initialize("fems.factory.solidmaterialparser");
	Q_ASSERT(isOk);

	_data->_geoFactoryManager = _data->_extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_geoFactoryManager.isNull());
	isOk = _data->_geoFactoryManager.initialize("fems.factory.geometryparser");
	Q_ASSERT(isOk);

	_data->_mFactory = _data->_objManager.getObject(
			"org.sipesc.fems.matrix.matrixfactory");
	Q_ASSERT(!_data->_mFactory.isNull());

	_data->_vFactory = _data->_objManager.getObject(
			"org.sipesc.fems.matrix.vectorfactory");
	Q_ASSERT(!_data->_vFactory.isNull());
}
MBarAxialForceParserImpl::~MBarAxialForceParserImpl(){

}
ProgressMonitor MBarAxialForceParserImpl::getProgressMonitor() const{
	return _data->_monitor;
}
bool MBarAxialForceParserImpl::initialize(MDataModel& model, bool isRepeated){
	if (_data->_isInitialized)
		return false; //不能重复初始化
	_data->_model = model;


	MDataModel ElementPathModel = _data->_baseManager.createDataModel();
	bool isOk = ElementPathModel.open(model, "ElementPath");
	if (!isOk){
		QString errorMessage = "can't open ElementPath DataModel "
				"in MBarAxialForceParserImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		mReportError(M_ERROR_FATAL, errorMessage);
		return false;
	}
	_data->_eleManager = _data->_baseManager.createDataManager();
	isOk = _data->_eleManager.open(ElementPathModel, "BarElement", true);
	Q_ASSERT(isOk);

	_data->_propertysRef = _data->_baseManager.createDataModel();
	isOk = _data->_propertysRef.open(_data->_model, "PropertyRefPath", true);
	Q_ASSERT(isOk);
	_data->_propertysMgr = _data->_baseManager.createDataManager();
	isOk = _data->_propertysMgr.open(_data->_propertysRef, "General", true);
	Q_ASSERT(isOk);

	_data->_materials = _data->_baseManager.createDataManager();
	isOk = _data->_materials.open(_data->_model, "Material", true);
	Q_ASSERT(isOk);
	_data->_geometryMgr = _data->_baseManager.createDataManager();
	isOk = _data->_geometryMgr.open(_data->_model, "Geometry", true);
	Q_ASSERT(isOk);
	_data->_varGeosMgr = _data->_baseManager.createDataManager();
	isOk = _data->_varGeosMgr.open(_data->_model, "VariGeometry", true);
	Q_ASSERT(isOk);

	_data->_isInitialized = true;
	return true;
}

// 解析每个单元
double MBarAxialForceParserImpl::getAxialForce(const MDataObject& data){
	bool isOk;
	MDataObjectList nodeStressVectors = data;
	int eleId = nodeStressVectors.getId();

	if(!_data->_localDataMgr.isOpen()){
		_data->_coordPath = _data->_baseManager.createDataModel();
		isOk = _data->_coordPath.open(_data->_model, "ElementLocalCoordPath", true);
		Q_ASSERT(isOk);
		_data->_localDataMgr = _data->_baseManager.createDataManager();
		isOk = _data->_localDataMgr.open(_data->_coordPath, "BarElement", true);
		Q_ASSERT(isOk);
	}

// 指定id单元
	MElementData eleData = _data->_eleManager.getData(eleId);
	MPropertyRefData propertyData;
	propertyData = _data->_propertysMgr.getData(eleData.getPropertyId());
	int matId = propertyData.getPropertyId("MaterialId");
	MSolidMaterialParser matParser = _data->findMatConstiDParser(matId);
	double E = matParser.getParameters(MSolidMaterialParser::Young).toDouble();

	MPropertyData geometryData;
	geometryData = _data->_geometryMgr.getData(
			propertyData.getPropertyId("GeometryId"));
	MPropertyData variGeoData;
	variGeoData = _data->_varGeosMgr.getData(geometryData.getValue(0).toInt());
	QString type = variGeoData.getType();

	MExtensionFactory GeoParserFactory = _data->_geoFactoryManager.getFactory(type);
	Q_ASSERT(!GeoParserFactory.isNull());
	MMultiGeometryParser GeoParser = GeoParserFactory.createExtension();
	Q_ASSERT(!GeoParser.isNull());
	GeoParser.initialize(_data->_model);
	GeoParser.setState(variGeoData);
	double A = GeoParser.getGeometry(MIntegPoint(), 0);
	MMatrixTools mTools = _data->_extManager.createExtension(
			"org.sipesc.fems.matrix.MMatrixTools");
	Q_ASSERT(!mTools.isNull());
	MElementLocalCoordData coordData = _data->_localDataMgr.getData(eleId);
	MMatrixData mData = coordData.getTransMatrixData();

	MMatrix m = _data->_mFactory.createMatrix();
	m << mData;
	MMatrix m1 = _data->_mFactory.createMatrix(6, 6);
	m1(0, 0, m(0, 0));
	m1(0, 1, m(0, 1));
	m1(0, 2, m(0, 2));
	m1(1, 0, m(1, 0));
	m1(1, 1, m(1, 1));
	m1(1, 2, m(1, 2));
	m1(2, 0, m(2, 0));
	m1(2, 1, m(2, 1));
	m1(2, 2, m(2, 2));

	m1(3, 3, m(0, 0));
	m1(3, 4, m(0, 1));
	m1(3, 5, m(0, 2));
	m1(4, 3, m(1, 0));
	m1(4, 4, m(1, 1));
	m1(4, 5, m(1, 2));
	m1(5, 3, m(2, 0));
	m1(5, 4, m(2, 1));
	m1(5, 5, m(2, 2));

	MMatrix m2 = _data->_mFactory.createMatrix();
	m2 = mTools.inverse(m1);

	int nodecount = eleData.getNodeCount();
	MVector nodestress = _data->_vFactory.createVector();
	MVector tmp = _data->_vFactory.createVector();
	double axialforce(0);
	for (int k = 0; k < nodecount; k++) //对节点循环
	{
		MVectorData nodestressdata = nodeStressVectors.getDataAt(k);
		nodestress << nodestressdata;
		tmp = m2 * nodestress;
		axialforce += tmp(0) * E * A;
	}
	axialforce /= nodecount;

	return axialforce;
}
double MBarAxialForceParserImpl::getSecArea(const MDataObject& data){
	MElementData eleData = data;

	MPropertyRefData propertyData = _data->_propertysMgr.getData(eleData.getPropertyId());
	MPropertyData geometryData = _data->_geometryMgr.getData(propertyData.getPropertyId("GeometryId"));
	MPropertyData variGeoData = _data->_varGeosMgr.getData(geometryData.getValue(0).toInt());

	QString type = variGeoData.getType();
	MExtensionFactory GeoParserFactory = _data->_geoFactoryManager.getFactory(type);
	Q_ASSERT(!GeoParserFactory.isNull());
	MMultiGeometryParser GeoParser = GeoParserFactory.createExtension();
	Q_ASSERT(!GeoParser.isNull());

	GeoParser.initialize(_data->_model);
	GeoParser.setState(variGeoData);
	double A = GeoParser.getGeometry(MIntegPoint(), 0);

	return A;
}
double MBarAxialForceParserImpl::getLength(const MDataObject& data){
	MElementData eleData = data;
	double len(0);





	return len;
}
#if 0 // 周大为提供整体计算
double MBarAxialForceParserImpl::getAxialForce(const MDataObject& data){


	bool isOk;
//	MDataModel nodestress, propertysRef, coord;
//	MDataManager propertys, Geometry, varGeos, LocalData;



	int elecount = _data->_eleManager.getDataCount();
	int stepcount = _data->_nodestressPath.getDataCount();
	for (int i = 1; i <= stepcount; i++) //对工况循环
	{
		MDataModel TimeStep;
		TimeStep = _data->_baseManager.createDataModel();
		isOk = TimeStep.open(_data->_nodestressPath, QString::number(i), true);
		MDataManager elenodestress;
		elenodestress = _data->_baseManager.createDataManager();
		isOk = elenodestress.open(TimeStep, "BarElement", true);
		for (int j = 0; j < elecount; j++) //对单元循环
		{
			MElementData eleData = _data->_eleManager.getDataAt(j);
			MPropertyRefData propertyData;
			propertyData = _data->_propertysMgr.getData(eleData.getPropertyId());
			int matId = propertyData.getPropertyId("MaterialId");
			MSolidMaterialParser matParser = _data->findMatConstiDParser(matId);
			double E = matParser.getParameters(MSolidMaterialParser::Young).toDouble();

			MPropertyData geometryData;
			geometryData = _data->_geometryMgr.getData(
					propertyData.getPropertyId("GeometryId"));
			MPropertyData variGeoData;
			variGeoData = _data->_varGeosMgr.getData(geometryData.getValue(0).toInt());
			QString type = variGeoData.getType();

			MExtensionFactory GeoParserFactory = _data->_geoFactoryManager.getFactory(type);
			Q_ASSERT(!GeoParserFactory.isNull());
			MMultiGeometryParser GeoParser = GeoParserFactory.createExtension();
			Q_ASSERT(!GeoParser.isNull());
			GeoParser.initialize(_data->_model);
			GeoParser.setState(variGeoData);
			double A = GeoParser.getGeometry(MIntegPoint(), 0);
			MMatrixTools mTools = _data->_extManager.createExtension(
					"org.sipesc.fems.matrix.MMatrixTools");
			Q_ASSERT(!mTools.isNull());
			MElementLocalCoordData coordData = _data->_localDataMgr.getData(eleData.getId());
			MMatrixData mData = coordData.getTransMatrixData();
			MMatrixFactory mFactory = _data->_objManager.getObject(
					"org.sipesc.fems.matrix.matrixfactory");
			Q_ASSERT(!mFactory.isNull());
			MMatrix m = mFactory.createMatrix();
			m << mData;
			MMatrix m1 = mFactory.createMatrix(6, 6);
			m1(0, 0, m(0, 0));
			m1(0, 1, m(0, 1));
			m1(0, 2, m(0, 2));
			m1(1, 0, m(1, 0));
			m1(1, 1, m(1, 1));
			m1(1, 2, m(1, 2));
			m1(2, 0, m(2, 0));
			m1(2, 1, m(2, 1));
			m1(2, 2, m(2, 2));

			m1(3, 3, m(0, 0));
			m1(3, 4, m(0, 1));
			m1(3, 5, m(0, 2));
			m1(4, 3, m(1, 0));
			m1(4, 4, m(1, 1));
			m1(4, 5, m(1, 2));
			m1(5, 3, m(2, 0));
			m1(5, 4, m(2, 1));
			m1(5, 5, m(2, 2));

			MMatrix m2 = mFactory.createMatrix();
			m2 = mTools.inverse(m1);
			MDataObjectList nodeStressVectors = elenodestress.getData(
					eleData.getId());
			int nodecount = eleData.getNodeCount();
			for (int k = 0; k < nodecount; k++) //对节点循环
			{
				MVectorData nodestressdata = nodeStressVectors.getDataAt(k);
				MVectorFactory vFactory = _data->_objManager.getObject(
						"org.sipesc.fems.matrix.vectorfactory");
				MVector nodestress = vFactory.createVector();
				MVector tmp = vFactory.createVector();
				nodestress << nodestressdata;
				tmp = m2 * nodestress;
				double axialforce = tmp(0) * E * A;
			}
		}
	}
}
#endif
