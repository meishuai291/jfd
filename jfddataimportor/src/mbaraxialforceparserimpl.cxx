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
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::utilities;

#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixdata.h>
using namespace org::sipesc::fems::matrix;

#include <org.sipesc.fems.data.melementlocalcoorddata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.mpropertydata.h>
using namespace org::sipesc::fems::data;
#include <org.sipesc.fems.element.mgeometryparser.h>
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

	MDataModel _model, _elementLocalCoord;

	MDataManager _eleManager;
	MDataManager  _geometryMgr, _coordTransMatrixs;
	MPropertyData geometryData;
	QMap<int, MGeometryParser> _geoParserMap;
	MExtensionFactoryManager _geoFactoryManager;
    QString geoType;
	MMatrixFactory _mFactory;
	MVectorFactory _vFactory;
	bool _isInitialized;

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

	_data->_geoFactoryManager = _data->_extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_geoFactoryManager.isNull());
	bool isOk = _data->_geoFactoryManager.initialize("fems.factory.geometryparser");
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
	_data->_elementLocalCoord = _data->_baseManager.createDataModel();
	  isOk = _data->_elementLocalCoord.open(_data->_model, "ElementLocalCoordPath", true);
	_data->_geometryMgr = _data->_baseManager.createDataManager();
	isOk = _data->_geometryMgr.open(_data->_model, "Geometry", true);
	Q_ASSERT(isOk);

	_data->_isInitialized = true;
	return true;
}

// 解析每个单元
double MBarAxialForceParserImpl::getAxialForce(const MDataObject& data){
	bool isOk;
	MDataObjectList integralPoints = data;
	int eleId = integralPoints.getId();
	// 指定id单元
	MElementData eleData = _data->_eleManager.getData(eleId);
	int ematId = eleData.getPropertyId();
	_data->geometryData = _data->_geometryMgr.getData(ematId);
    double A = _data->geometryData.getValue(0).toInt();
	MVectorData stressIntegData = integralPoints.getDataAt(0);
	MVector stressInteg = _data->_vFactory.createVector();
	stressInteg << stressIntegData;
	double axialforce = stressInteg(0) * A;

	return axialforce;
}

double MBarAxialForceParserImpl::getSecArea(const MDataObject& data){
	MElementData eleData = data;
	int ematId = eleData.getPropertyId();
	_data->geometryData = _data->_geometryMgr.getData(ematId);
    double A = _data->geometryData.getValue(0).toInt();

    return A;
}

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

