/*
 * mhexabrick12eleconstidparserimpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12eleconstidparserimpl.h>
#include <qdebug.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.data.mpropertyrefdata.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.femsutils.msolidmaterialparser.h>
#include <org.sipesc.fems.element.mgeometryparser.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::core::engdbs::data;
class MHexaBrick12EleConstiDParserImpl::Data
  {
  public:
    Data()
      {
      _isInitialized = false;
      _isConst = true;
      }

  public:
    MDataModel  _model;
    MDataManager  _propertys, _materials;
    MObjectManager _objManager;
    ProgressMonitor  _monitor;
    MSolidMaterialParser  _matParser;
    //定义 MElementParserMap, 以提高计算效率
    QMap<int, MMatrix> _matMatrixMap;
    QString _mId;
    int _matId;
    bool _isInitialized, _isConst;

  public:
    /*
     * 查找单元解析对象
     */
    MMatrix findConstiDMatrix(const int& index)
      {
      //首先在Map中查找
      if (_matMatrixMap.contains(index))
        return _matMatrixMap[index];

      MPropertyData matData;
      matData = _materials.getData(index);
      _matParser.setState(matData);

      MMatrix constMatrix;
      constMatrix = _matParser.getElasticD();
      _matMatrixMap.insert(index, constMatrix);


      return constMatrix;
      }
  };

MHexaBrick12EleConstiDParserImpl::MHexaBrick12EleConstiDParserImpl()
  {
  _data.reset(new MHexaBrick12EleConstiDParserImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleConstiDParserImpl::"
        "MHexaBrick12EleConstiDParserImpl() ****** failed");

  _data->_objManager = MObjectManager::getManager();
  UtilityManager util = _data->_objManager.getObject(
      "org.sipesc.core.utility.utilitymanager");
  Q_ASSERT(!util.isNull());
  _data->_monitor = util.createProgressMonitor("MHexaBrick12EleConstiDParser",
      ProgressMonitor());
  Q_ASSERT(!_data->_monitor.isNull());
  }

MHexaBrick12EleConstiDParserImpl::~MHexaBrick12EleConstiDParserImpl()
  {
  }

bool MHexaBrick12EleConstiDParserImpl::initialize(MDataModel& model,
    bool isRepeated)
  {
  if (_data->_isInitialized)
    return false; //不能重复初始化

  _data->_model = model;
  MFemsGlobal global = _data->_objManager.getObject(
      "org.sipesc.fems.global.femsglobal");
  MElementsGlobal eleGlobal = _data->_objManager.getObject(
      "org.sipesc.fems.global.elementsglobal");
  _data->_mId = eleGlobal.getValue(MElementsGlobal::MaterialId);
  MDatabaseManager baseManager = _data->_objManager.getObject(
      "org.sipesc.core.engdbs.mdatabasemanager");
  Q_ASSERT(!baseManager.isNull());

  MDataModel propertysRef = baseManager.createDataModel();
  bool ok = propertysRef.open(_data->_model,
      global.getValue(MFemsGlobal::PropertyRefPath), true);
  if (!ok)
    {
    QString errorMessage = "can't open property DataModel "
        " in MHexaBrick12EleConstiDParserImpl::initialize() ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }

  _data->_propertys = baseManager.createDataManager();
  ok = _data->_propertys.open(propertysRef,
      global.getValue(MFemsGlobal::GeneralRef), true);
  if (!ok)
    {
    QString errorMessage = "can't open property DataManager "
        " in MHexaBrick12EleConstiDParserImpl::initialize() ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }
  //只读方式打开material数据
  _data->_materials = baseManager.createDataManager();
  ok = _data->_materials.open(_data->_model,
      global.getValue(MFemsGlobal::Material), true);
  if (!ok)
    {
    QString errorMessage = "can't open material DataManager "
        " in MHexaBrick12EleConstiDParserImpl::initialize() ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }

  //初始化自由度数据解析类工厂管理对象
  MExtensionManager extManager = MExtensionManager::getManager();
  _data->_matParser = extManager.createExtension(
      "org.sipesc.fems.elastoconstis.MSolid3DIsoMaterialParser");
  Q_ASSERT(!_data->_matParser.isNull());
  bool isOk = _data->_matParser.initialize(_data->_model);
  if (!isOk)
    {
    QString errorMessage = "can't initialize MSolidIsoMaterialParser"
        " in MHexaBrick12EleConstiDParserImpl::initialize() ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }

  _data->_isInitialized = true;
  return true;
  }

ProgressMonitor MHexaBrick12EleConstiDParserImpl::getProgressMonitor() const
  {
  return _data->_monitor;
  }

void MHexaBrick12EleConstiDParserImpl::setState(const MDataObject& eleData)
  {
  Q_ASSERT(_data->_isInitialized);
  MElementData elementData = eleData;
  Q_ASSERT(!elementData.isNull());

  MPropertyRefData propertyData;
  propertyData = _data->_propertys.getData(elementData.getPropertyId());
  _data->_matId = propertyData.getPropertyId(_data->_mId);
  }

bool MHexaBrick12EleConstiDParserImpl::isConst() const
  {
  Q_ASSERT(_data->_isInitialized);
  return _data->_isConst;
  }

MMatrix MHexaBrick12EleConstiDParserImpl::getConstiDMatrix(
    const MIntegPoint& integ)
  {
  Q_ASSERT(_data->_isInitialized);
  return _data->findConstiDMatrix(_data->_matId);
  }


