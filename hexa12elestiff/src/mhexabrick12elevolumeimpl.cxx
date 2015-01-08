/*
 * mhexabrick12elevolumeimpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12elevolumeimpl.h>
#include <qmath.h>
#include <qdebug.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.data.melementlocalcoorddata.h>

using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::data;

class MHexaBrick12EleVolumeImpl::Data
  {
  public:
    Data()
      {
      _isInitialized = false;
      _jacobiValue = 0.0;
      }

  public:
    MDataModel _model, _elementLocalCoord;
    MDataManager _coordTransMatrixs;
    MObjectManager _objManager;
    MDatabaseManager _baseManager;
    ProgressMonitor _monitor;
    MMatrixFactory _mFactory;
    MMatrixTools _mTools;
    double _jacobiValue;
    bool _isInitialized;
  };

MHexaBrick12EleVolumeImpl::MHexaBrick12EleVolumeImpl()
  {
  _data.reset(new MHexaBrick12EleVolumeImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleVolumeImpl::"
        "MHexaBrick12EleVolumeImpl() ****** failed");

  _data->_objManager = MObjectManager::getManager();
  UtilityManager util = _data->_objManager.getObject(
      "org.sipesc.core.utility.utilitymanager");
  Q_ASSERT(!util.isNull());
  _data->_monitor = util.createProgressMonitor("MHexaBrick12EleVolume",
      ProgressMonitor());
  Q_ASSERT(!_data->_monitor.isNull());
  }

MHexaBrick12EleVolumeImpl::~MHexaBrick12EleVolumeImpl()
  {
  }

bool MHexaBrick12EleVolumeImpl::initialize(MDataModel& model, bool isRepeated)
  {
  if (_data->_isInitialized)
    return false; //不能重复初始化

  _data->_model = model;
  MFemsGlobal global = _data->_objManager.getObject(
      "org.sipesc.fems.global.femsglobal");
  _data->_mFactory = _data->_objManager.getObject(
      "org.sipesc.fems.matrix.matrixfactory");
  Q_ASSERT(!_data->_mFactory.isNull());
  _data->_baseManager = _data->_objManager.getObject(
      "org.sipesc.core.engdbs.mdatabasemanager");
  Q_ASSERT(!_data->_baseManager.isNull());

  _data->_elementLocalCoord = _data->_baseManager.createDataModel();
  bool ok = _data->_elementLocalCoord.open(_data->_model,
      global.getValue(MFemsGlobal::ElementLocalCoordPath), true);
  if (!ok)
    {
    QString errorMessage = "can't open element CoordTransMatrixs DataModel "
        " in MHexaBrick12EleVolumeImpl::initialize ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }

  MExtensionManager extManager = MExtensionManager::getManager();
  _data->_mTools = extManager.createExtension(
      "org.sipesc.fems.matrix.MMatrixTools");
  Q_ASSERT(!_data->_mTools.isNull());

  _data->_isInitialized = true;
  return true;
  }

ProgressMonitor MHexaBrick12EleVolumeImpl::getProgressMonitor() const
  {
  return _data->_monitor;
  }

void MHexaBrick12EleVolumeImpl::setState(const MDataObject& eleData)
  {
  Q_ASSERT(_data->_isInitialized);
  MElementData data = eleData;
  Q_ASSERT(!eleData.isNull());

  if (!_data->_coordTransMatrixs.isOpen())
    {
    _data->_coordTransMatrixs = _data->_baseManager.createDataManager();
    QString eleType = data.getType();
    bool ok = _data->_coordTransMatrixs.open(_data->_elementLocalCoord,
        eleType, true);
    if (!ok)
      {
      QString errorMessage =
          "can't open element CoordTransMatrixs DataManager "
              " in MHexaBrick12EleVolumeImpl::setState ";
      qDebug() << errorMessage;
      _data->_monitor.setMessage(errorMessage);
      }
    }

  MElementLocalCoordData eleLocalCoordData =
      _data->_coordTransMatrixs.getData(data.getId());
  double jValue = 0;
  int map[5][4] =
    {
      { 2, 7, 5, 6 },
      { 2, 0, 7, 3 },
      { 2, 5, 0, 1 },
      { 5, 7, 0, 4 },
      { 2, 7, 0, 5 } }; //把六面体剖分为5个四面体计算体积

  for (int iTetra = 0; iTetra < 5; iTetra++)
    { //对5个四面体循环
    MMatrix node = _data->_mFactory.createMatrix(4, 4);
    for (int i = 0; i < 4; i++)
      {
      MNodeData nodeData = eleLocalCoordData.getNodes().value(
          map[iTetra][i]);
      Q_ASSERT(!nodeData.isNull());
      node(i, 0, 1.0);
      node(i, 1, nodeData.getX());
      node(i, 2, nodeData.getY());
      node(i, 3, nodeData.getZ());
      }

    jValue += qFabs(double(_data->_mTools.determinate(node))); //求行列式 (5个四面体累加)
    }

  _data->_jacobiValue = jValue / 6.0; //求体积
  }

double MHexaBrick12EleVolumeImpl::getEleVolume(const MIntegPoint& integ,
    const int& index) const
  {
  Q_ASSERT(_data->_isInitialized);
  return _data->_jacobiValue;
  }


