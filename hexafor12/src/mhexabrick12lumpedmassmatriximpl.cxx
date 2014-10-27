/*
 * mhexabrick12lumpedmassmatriximpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12lumpedmassmatriximpl.h>
#include <qdebug.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
#include <org.sipesc.fems.mass.melementmass.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elevolume.h>

using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::mass;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12LumpedMassMatrixImpl::Data
  {
  public:
    Data()
      {
      _isInitialized = false;
      }

  public:
    MDataModel _model;
    MObjectManager _objManager;
    ProgressMonitor _monitor;
    MMatrix _lumpedMass;
    MHexaBrick12EleVolume _volParser;
    bool _isInitialized;
  };

MHexaBrick12LumpedMassMatrixImpl::MHexaBrick12LumpedMassMatrixImpl()
  {
  _data.reset(new MHexaBrick12LumpedMassMatrixImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12LumpedMassMatrixImpl::"
        "MHexaBrick12LumpedMassMatrixImpl() ****** failed");

  _data->_objManager = MObjectManager::getManager();
  UtilityManager util = _data->_objManager.getObject(
      "org.sipesc.core.utility.utilitymanager");
  Q_ASSERT(!util.isNull());
  _data->_monitor = util.createProgressMonitor("MHexaBrick12LumpedMassMatrix",
      ProgressMonitor());
  Q_ASSERT(!_data->_monitor.isNull());
  }

MHexaBrick12LumpedMassMatrixImpl::~MHexaBrick12LumpedMassMatrixImpl()
  {
  }

bool MHexaBrick12LumpedMassMatrixImpl::initialize(MDataModel& model,
    bool isRepeated)
  {
  if (_data->_isInitialized)
    return false; //不能重复初始化

  _data->_model = model;
  MMatrixFactory mFactory = _data->_objManager.getObject(
      "org.sipesc.fems.matrix.matrixfactory");
  Q_ASSERT(!mFactory.isNull());

  //创建六面体体积解析扩展
  MExtensionManager extManager = MExtensionManager::getManager();
  _data->_volParser = extManager.createExtension(
      "org.sipesc.fems.hexaelement12.MHexaBrick12EleVolume");
  Q_ASSERT(!_data->_volParser.isNull());
  bool isOk = _data->_volParser.initialize(_data->_model);
  if (!isOk)
    {
    QString errorMessage = "can't initialize MHexaBrick12EleVolume"
        " in MHexaBrick12LumpedMassMatrixImpl::initialize() ";
    qDebug() << errorMessage;
    _data->_monitor.setMessage(errorMessage);
    return false;
    }

  MMatrix mdn = mFactory.createMatrix(36, 1 );
  for (int i = 0; i < 36; i++)
    mdn(i, 0, 1.0 / 12.0);

  _data->_lumpedMass = mdn;

  _data->_isInitialized = true;
  return true;
  }

ProgressMonitor MHexaBrick12LumpedMassMatrixImpl::getProgressMonitor() const
  {
  return _data->_monitor;
  }

void MHexaBrick12LumpedMassMatrixImpl::setState(const MDataObject& eleData)
  {
  Q_ASSERT(_data->_isInitialized);
  MElementData data = eleData;
  Q_ASSERT(!data.isNull());

  _data->_volParser.setState(data);
  }

MMatrix MHexaBrick12LumpedMassMatrixImpl::getMassMatrix(
    const MElementMass& eleMass) const
  {
  double density = eleMass.getEleMass();
  double volume = _data->_volParser.getEleVolume();
  return _data->_lumpedMass * density * volume;
  }

