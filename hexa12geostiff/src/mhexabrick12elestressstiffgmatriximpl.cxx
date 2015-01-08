/*
 * mhexabrick12elestressstiffgmatriximpl.cxx
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#include <mhexabrick12elestressstiffgmatriximpl.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <qdebug.h>

#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.melementlocalcoorddata.h>

#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.utilities.mextensionfactory.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.element.mshpfunction.h>

using namespace org::sipesc::fems::element;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::core::utility;

class MHexaBrick12EleStressStiffGMatrixImpl::Data
  {
  public:
    Data() //Data的构造函数
      {
      _jacobiValue = 0.0;
      }
  public:
    MShpFunction _shpFunction;
    MElementLocalCoordData _coordData;
    MMatrix _lm;
    QVector<MNodeData> _nodes;
    double _jacobiValue;
    MObjectManager _objManager;
    MMatrixFactory _mFactory;
    MMatrixTools _mTools;

  public:
    bool initialize() //对_shpFunction的初始化
      {
      //通过插件找工厂管理器
      MExtensionManager extManager = MExtensionManager::getManager();
      MExtensionFactoryManager factoryManager = extManager.createExtension(
          "org.sipesc.utilities.MExtensionFactoryManager");
      bool isOk = factoryManager.initialize("fems.factory.shapefunction");
      Q_ASSERT(isOk);

      MElementsGlobal global = _objManager.getObject(
    		  "org.sipesc.fems.global.elementsglobal");

      //根据数据类型找解析类对象工厂
      MExtensionFactory preferFactory = factoryManager.getFactory("HexaBrick12ShpFunction");
      Q_ASSERT(!preferFactory.isNull());

      //生成形函数对象
      _shpFunction = preferFactory.createExtension();
      Q_ASSERT(!_shpFunction.isNull());

      return true;
      }
  };

MHexaBrick12EleStressStiffGMatrixImpl::MHexaBrick12EleStressStiffGMatrixImpl()
  {
  _data.reset(new MHexaBrick12EleStressStiffGMatrixImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleStressStiffGMatrixImpl::"
      "MHexaBrick12EleStressStiffGMatrixImpl() ****** failed");

  if (!_data->initialize()) //Data初始化
    mReportError(M_ERROR_FATAL, "MHexaBrick12EleStressStiffGMatrixImpl::"
      "MHexaBrick12EleStressStiffGMatrixImpl() ****** initialize failed");
  _data->_objManager = MObjectManager::getManager();
  }

MHexaBrick12EleStressStiffGMatrixImpl::~MHexaBrick12EleStressStiffGMatrixImpl()
  {
  }

void MHexaBrick12EleStressStiffGMatrixImpl::setState(const MDataObject& localData)
  {
	_data->_mFactory = _data->_objManager.getObject(
			"org.sipesc.fems.matrix.matrixfactory");
	Q_ASSERT(!_data->_mFactory.isNull());
	MExtensionManager extManager = MExtensionManager::getManager();
	_data->_mTools = extManager.createExtension(
			"org.sipesc.fems.matrix.MMatrixTools");
	Q_ASSERT(!_data->_mTools.isNull());

	_data->_coordData = localData;
	Q_ASSERT(!_data->_coordData.isNull());
	_data->_nodes = _data->_coordData.getNodes();
	_data->_lm = _data->_mFactory.createMatrix(12,3);
	for (int i = 0; i < 12; i++)
	{
		_data->_lm(i, 0, _data->_nodes.value(i).getX());
		_data->_lm(i, 1, _data->_nodes.value(i).getY());
		_data->_lm(i, 2, _data->_nodes.value(i).getZ());
	}
  }

MMatrix MHexaBrick12EleStressStiffGMatrixImpl::getStressStiffGMatrix(const MIntegPoint& integ)const
  {
  MMatrix shpFuncD_1 = _data->_mFactory.createMatrix();
  shpFuncD_1 = _data->_shpFunction.getShpFuncDerivative(
			  integ.getIntegCoord(),_data->_nodes); //形函数导数矩阵（在积分点的值）
  MMatrix J = shpFuncD_1*_data->_lm;
  MMatrix invJ = _data->_mTools.inverse(J);
  _data->_jacobiValue = _data->_mTools.determinate(J);
  MMatrix shpFuncD_2 = invJ*shpFuncD_1;

  MMatrix G = _data->_mFactory.createMatrix(9, 36);
  for (int i = 0; i < 12; i++)
	  for (int j = 0; j < 3; j++)
	  {
		  double sigtmp = shpFuncD_2(j,i);
		  G(j,i*3, sigtmp);
		  G(3+j,i*3+1, sigtmp);
		  G(6+j,i*3+2, sigtmp);
	  }
  return G;
  }

double MHexaBrick12EleStressStiffGMatrixImpl::getJacobiValue()const
  {
  return _data->_jacobiValue;
  }
