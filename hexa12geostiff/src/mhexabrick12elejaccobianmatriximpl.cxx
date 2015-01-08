/*
 * mhexabrick12elejaccobianmatriximpl.cxx
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#include <mhexabrick12elejaccobianmatriximpl.h>
#include <mfemsglobalextension.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.melementlocalcoorddata.h>
#include <org.sipesc.fems.element.mshpfunction.h>
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::element;

class MHexaBrick12EleJaccobianMatrixImpl::Data
  {
  public:
    Data() //Data的构造函数
      {
      _jacobiValue = 0.0;
      }

  public:
    MMatrixFactory _mFactory;
    MMatrixTools _mTools;
    MShpFunction _shpFunction;
    MElementLocalCoordData _coordData;
    MMatrix _J, _JaccbianMatrix, _lm;
    QVector<MNodeData> _nodes;
    double _jacobiValue;

  public:
    bool initialize() //对_shpFunction的初始化
      {
      //通过插件找工厂管理器
      _mFactory = getMatrixFactory();
      Q_ASSERT(!_mFactory.isNull());

      _shpFunction = globalExtensionManager.createExtension(
          "org.sipesc.fems.hexaelement12.MHexaBrick12ShpFunction");
      Q_ASSERT(!_shpFunction.isNull());

      _mTools = globalExtensionManager.createExtension(
          "org.sipesc.fems.matrix.MMatrixTools");
      Q_ASSERT(!_mTools.isNull());

      return true;
      }
  };

MHexaBrick12EleJaccobianMatrixImpl::MHexaBrick12EleJaccobianMatrixImpl()
  {
  _data.reset(new MHexaBrick12EleJaccobianMatrixImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleJaccobianMatrixImpl::"
        "MHexaBrick12EleJaccobianMatrixImpl() ****** failed");

  if (!_data->initialize()) //Data初始化
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleJaccobianMatrixImpl::"
        "MHexaBrick12EleJaccobianMatrixImpl() ****** initialize failed");
  }

MHexaBrick12EleJaccobianMatrixImpl::~MHexaBrick12EleJaccobianMatrixImpl()
  {
  }

void MHexaBrick12EleJaccobianMatrixImpl::setState(
    const MDataObject& localData)
  {
  _data->_coordData = localData;
  Q_ASSERT(!_data->_coordData.isNull());

  _data->_nodes = _data->_coordData.getNodes();
  MMatrixData TransMatrix = _data->_coordData.getTransMatrixData();
  MMatrix Tm = _data->_mFactory.createMatrix();
  Tm << TransMatrix;

  _data->_lm = _data->_mFactory.createMatrix(12, 3);
  for (int i = 0; i < 12; i++)
    {
    _data->_lm(i, 0, _data->_nodes.value(i).getX());
    _data->_lm(i, 1, _data->_nodes.value(i).getY());
    _data->_lm(i, 2, _data->_nodes.value(i).getZ());
    }
  }

MMatrix MHexaBrick12EleJaccobianMatrixImpl::getJaccobianMatrix(
    const MIntegPoint& integ)
  {
  MMatrix shpFuncD_1 = _data->_shpFunction.getShpFuncDerivative(
      integ.getIntegCoord(), _data->_nodes); //形函数导数矩阵（在积分点的值）
  MMatrix J = shpFuncD_1 * _data->_lm;

  return _data->_mTools.transpose(J);
  }

