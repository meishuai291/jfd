/*
 * mbaraxialforceparserimpl.cxx
 *
 *  Created on: 2014年10月29日
 *      Author: sipesc
 */

#include <mbaraxialforceparserimpl.h>

#include <qdebug.h>
#include <qmath.h>
#include <iostream>


#include <mobjectmanager.h>
#include <mextensionmanager.h>

#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.utilities.mextensionfactory.h>
using namespace org::sipesc::utilities;
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;

#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
using namespace org::sipesc::fems::matrix;

#include <org.sipesc.fems.data.melementlocalcoorddata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.mpropertyrefdata.h>
using namespace org::sipesc::fems::data;

#include <org.sipesc.fems.femsutils.msolidmaterialparser.h>
using namespace org::sipesc::fems::femsutils;
#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.global.melementsglobal.h>
using namespace org::sipesc::fems::global;

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

	MFemsGlobal _femsGlobal;
	MElementsGlobal _elesGlobal;

	MDataModel _model, _elementLocalCoord;

	MDataManager _eleManager;
	MDataManager  _geometryMgr, _coordTransMatrixs;

	MDataManager _propertysMgr, _materials;

	MVectorFactory _vFactory;
	bool _isInitialized;

	QMap<int, MSolidMaterialParser> _matMatrixMap;
	MExtensionFactoryManager _matFactoryManager;

public:
    MSolidMaterialParser findMatConstiDParser(const int& index){
		//首先在Map中查找
		if (_matMatrixMap.contains(index))
			return _matMatrixMap[index];

		MPropertyData matData;
		matData = _materials.getData(index);
		Q_ASSERT(!matData.isNull());

		//若Map中没有,通过工厂管理器查找,初始化后放入Map中
		QString type = matData.getType();
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

	_data->_vFactory = _data->_objManager.getObject(
			"org.sipesc.fems.matrix.vectorfactory");
	Q_ASSERT(!_data->_vFactory.isNull());

	_data->_matFactoryManager = _data->_extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_matFactoryManager.isNull());
	bool isOk = _data->_matFactoryManager.initialize("fems.factory.solidmaterialparser");
	Q_ASSERT(isOk);

	_data->_elesGlobal = _data->_objManager.getObject("org.sipesc.fems.global.elementsglobal");
	Q_ASSERT(!_data->_elesGlobal.isNull());

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());
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

	_data->_elementLocalCoord = _data->_baseManager.createDataModel();
	isOk = _data->_elementLocalCoord.open(_data->_model, "ElementLocalCoordPath", true);
	Q_ASSERT(isOk);

	_data->_geometryMgr = _data->_baseManager.createDataManager();
	isOk = _data->_geometryMgr.open(_data->_model, "Geometry", true);
	Q_ASSERT(isOk);

	MDataModel propertysRef = _data->_baseManager.createDataModel();
	isOk = propertysRef.open(_data->_model, "PropertyRefPath", true);
	Q_ASSERT(isOk);
	_data->_propertysMgr = _data->_baseManager.createDataManager();
	isOk = _data->_propertysMgr.open(propertysRef, "General", true);
	Q_ASSERT(isOk);

	_data->_materials = _data->_baseManager.createDataManager();
	isOk = _data->_materials.open(_data->_model, "Material", true);
	Q_ASSERT(isOk);


	_data->_isInitialized = true;
	return true;
}

// 解析每个单元
double MBarAxialForceParserImpl::getAxialForce(const MDataObject& data){
	bool isOk;
	MDataObjectList integralPoints = data;
	int eleId = integralPoints.getId();

	MElementData eleData = _data->_eleManager.getData(eleId);
    double A = getSecArea(eleData);
    double stress = getAxialStress(integralPoints);

	return stress * A;
}
double MBarAxialForceParserImpl::getAxialStress(const MDataObject& data){
	MDataObjectList integralPoints = data;
   	MVectorData stressIntegData = integralPoints.getDataAt(0);
	MVector stressInteg = _data->_vFactory.createVector();
	stressInteg << stressIntegData;

	return stressInteg(0);
}
double MBarAxialForceParserImpl::getAxialStrain(const MDataObject& data){
	bool isOk;
	MDataObjectList integralPoints = data;
	int eleId = integralPoints.getId();

	MElementData eleData = _data->_eleManager.getData(eleId);
	int pId = eleData.getPropertyId();
	MPropertyRefData pData = _data->_propertysMgr.getData(pId);
	Q_ASSERT(!pData.isNull());
	int mId = pData.getPropertyId(_data->_elesGlobal.getValue(MElementsGlobal::MaterialId));

	MSolidMaterialParser matParser = _data->findMatConstiDParser(mId);
	double E = matParser.getParameters(MSolidMaterialParser::Young).toDouble();

	double stress = getAxialStress(data);

	return stress / E;
}

//-------------------------------------------------
// 单元data
double MBarAxialForceParserImpl::getSecArea(const MDataObject& data){
	MElementData eleData = data;
	int ematId = eleData.getPropertyId();
	MPropertyData geometryData = _data->_geometryMgr.getData(ematId);
    double A = geometryData.getValue(0).toDouble();

    return A;
}

// 单元data
double MBarAxialForceParserImpl::getLength(const MDataObject& data){
	MElementData eleData = data;
    _data->_coordTransMatrixs = _data->_baseManager.createDataManager();
    bool ok = _data->_coordTransMatrixs.open(_data->_elementLocalCoord, "BarElement", true);
	MElementLocalCoordData eleLocalCoordData;
	eleLocalCoordData = _data->_coordTransMatrixs.getData(eleData.getId());
	MNodeData aNode, bNode;
	aNode = eleLocalCoordData.getNodes().value(0);
	bNode = eleLocalCoordData.getNodes().value(1);
	double L = qSqrt(
	      (aNode.getX() - bNode.getX()) * (aNode.getX() - bNode.getX())
	          + (aNode.getY() - bNode.getY()) * (aNode.getY() - bNode.getY())
	          + (aNode.getZ() - bNode.getZ()) * (aNode.getZ() - bNode.getZ()));
	return L;
}

