/*
 * mhexabrick12strainmatriximpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12strainmatriximpl.h>
#include <math.h>
#include <qdebug.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.melementlocalcoorddata.h>
#include <org.sipesc.fems.element.mshpfunction.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::element;

class MHexaBrick12StrainMatrixImpl::Data
  {
  public:
    Data() //Data的构造函数
      {
      _jacobiValue = 0.0; //对_jacobiValue初始化，对_shpFunction的初始化在initialize()中
      }

  public:
    MMatrixFactory _mFactory;
    MMatrixTools _mTools;
    MShpFunction _shpFunction;
    MElementLocalCoordData _coordData;
    MMatrix _nodeCoord,  _nodeCoord1;
    double _jacobiValue;

  public:
    bool initialize() //对_shpFunction的初始化
      {
      MObjectManager objManager = MObjectManager::getManager();
      _mFactory = objManager.getObject(
          "org.sipesc.fems.matrix.matrixfactory");
      Q_ASSERT(!_mFactory.isNull());
      //通过插件找工厂管理器
      MExtensionManager extManager = MExtensionManager::getManager();
      _shpFunction = extManager.createExtension(
          "org.sipesc.fems.hexaelement12.MHexaBrick12ShpFunction");
      Q_ASSERT(!_shpFunction.isNull());

      return true;
      }
  };

MHexaBrick12StrainMatrixImpl::MHexaBrick12StrainMatrixImpl()
  {
  _data.reset(new MHexaBrick12StrainMatrixImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12StrainMatrixImpl::"
        "MHexaBrick12StrainMatrixImpl() ****** failed");

  if (!_data->initialize()) //Data初始化
    mReportError(M_ERROR_FATAL, "MHexaBrick12StrainMatrixImpl::"
        "MHexaBrick12StrainMatrixImpl() ****** initialize failed");
  }

MHexaBrick12StrainMatrixImpl::~MHexaBrick12StrainMatrixImpl()
  {
  }

void MHexaBrick12StrainMatrixImpl::setState(const MDataObject& localData)
  {
  _data->_coordData = localData;
  Q_ASSERT(!_data->_coordData.isNull());

  MNodeData nodeData;
  MMatrix node = _data->_mFactory.createMatrix(12, 3);
  for (int i = 0; i < 12; i++)
    {
    nodeData = _data->_coordData.getNodes().value(i);
    Q_ASSERT(!nodeData.isNull());
    node(i, 0, nodeData.getX());
    node(i, 1, nodeData.getY());
    node(i, 2, nodeData.getZ());
    }

  _data->_nodeCoord = node;
  }

MMatrix MHexaBrick12StrainMatrixImpl::getStrainMatrix(
    const MIntegPoint& integ)
  {

  MMatrix shpFuncDerivative = _data->_mFactory.createMatrix(3, 12);
  shpFuncDerivative = _data->_shpFunction.getShpFuncDerivative(
      integ.getIntegCoord()); //形函数导数矩阵（在积分点的值）
//  qDebug() <<integ.getIntegCoord().value(0)<<integ.getIntegCoord().value(1)<<integ.getIntegCoord().value(2)<<"integinteginteginteginteginteginteginteginteg";
  MMatrix jacobiMatrix = _data->_mFactory.createMatrix(3, 3);
  jacobiMatrix = shpFuncDerivative * _data->_nodeCoord;
//	int Row3 =_data->_nodeCoord.getRowCount();
//	int Col3 = _data->_nodeCoord.getColCount();
//
//	qDebug() << Row3;
//	qDebug() << Col3;
//
//	for (int i = 0; i < Row3; i++)
//		for (int j = 0; j < Col3; j++) {
//			qDebug() <<_data->_nodeCoord(i, j)
//					<< "----------------------nodeCoordnodeCoordnodeCoordnodeCoord";
//		}
//
//	int Row =jacobiMatrix.getRowCount();
//	int Col =jacobiMatrix.getColCount();
//
//	qDebug() << Row;
//	qDebug() << Col;
//
//	for (int i = 0; i < Row; i++)
//		for (int j = 0; j < Col; j++) {
//			qDebug() << jacobiMatrix(i, j)
//					<< "----------------------jacobiMatrixjacobiMatrixjacobiMatrixjacobiMatrixjacobiMatrix";
//		}

	MExtensionManager extManager = MExtensionManager::getManager();
	_data->_mTools = extManager.createExtension(
            "org.sipesc.fems.matrix.MMatrixTools");
   Q_ASSERT(!_data->_mTools.isNull());
  _data->_jacobiValue = _data->_mTools.determinate(jacobiMatrix); // 计算雅克比矩阵行列式值

//  qDebug()<<_data->_jacobiValue<<"jacobiValuejacobiValuejacobiValuejacobiValuejacobiValue";
  MMatrix invJacobiMatrix = _data->_mFactory.createMatrix(3, 3);
  invJacobiMatrix = _data->_mTools.inverse(jacobiMatrix); //雅克比矩阵的逆


//	int Row =invJacobiMatrix.getRowCount();
//  	int Col = invJacobiMatrix.getColCount();
//
//  	qDebug() << Row;
//  	qDebug() << Col;
//
//  	for (int i = 0; i < Row; i++)
//  		for (int j = 0; j < Col; j++) {
//  			qDebug() <<invJacobiMatrix(i, j)
//  					<< "----------------------invJacobiMatrixinvJacobiMatrixinvJacobiMatrixinvJacobiMatrix";
//  		}

  MMatrix B = _data->_mFactory.createMatrix(6, 36);
  MMatrix DeriLocal = _data->_mFactory.createMatrix(3, 1);
  MMatrix DeriGlobal = _data->_mFactory.createMatrix(3, 1);
  for (int i = 0; i < 12; i++)
    {
    DeriLocal(0, 0, shpFuncDerivative(0, i));
    DeriLocal(1, 0, shpFuncDerivative(1, i));
    DeriLocal(2, 0, shpFuncDerivative(2, i));

    DeriGlobal = invJacobiMatrix * DeriLocal;

    B(0, i * 3, DeriGlobal(0, 0));
    B(0, i * 3 + 1, 0.0);
    B(0, i * 3 + 2, 0.0);
    B(1, i * 3, 0.0);
    B(1, i * 3 + 1, DeriGlobal(1, 0));
    B(1, i * 3 + 2, 0.0);
    B(2, i * 3, 0.0);
    B(2, i * 3 + 1, 0.0);
    B(2, i * 3 + 2, DeriGlobal(2, 0));

    B(3, i * 3, DeriGlobal(1, 0));
    B(3, i * 3 + 1, DeriGlobal(0, 0));
    B(3, i * 3 + 2, 0.0);
    B(4, i * 3, 0.0);
    B(4, i * 3 + 1, DeriGlobal(2, 0));
    B(4, i * 3 + 2, DeriGlobal(1, 0));
    B(5, i * 3, DeriGlobal(2, 0));
    B(5, i * 3 + 1, 0.0);
    B(5, i * 3 + 2, DeriGlobal(0, 0));
    }
//	int Row2 =B.getRowCount();
//    	int Col2 = B.getColCount();
//
//    	qDebug() << Row2;
//    	qDebug() << Col2;
//
//    	for (int i = 0; i < Row2; i++)
//    		for (int j = 0; j < Col2; j++) {
//    			qDebug() << B(i, j)
//    					<< "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
//    		}
  return B;
  }

double MHexaBrick12StrainMatrixImpl::getJacobiValue() const
  {
  return _data->_jacobiValue;
  }

QList<double> MHexaBrick12StrainMatrixImpl::coordTrans(
    const MIntegPoint& integ) const
  {
  MMatrix shpFunc = _data->_mFactory.createMatrix(1, 12);
  shpFunc = _data->_shpFunction.getShpFunction(integ.getIntegCoord()); //得到形函数，（对应于积分点）
  MMatrix co = _data->_mFactory.createMatrix(1, 3);
  co = shpFunc * _data->_nodeCoord; //得到真实的点坐标

  MMatrixData mData = _data->_coordData.getTransMatrixData();
  MMatrix m = _data->_mFactory.createMatrix();
  m << mData; //坐标转换阵

  MMatrix coord = _data->_mFactory.createMatrix(3, 1);
  coord(0, 0, co(0, 0));
  coord(1, 0, co(0, 1));
  coord(2, 0, co(0, 2));
  MMatrix tmp = _data->_mTools.transpose(m) * coord;

  QList<double> tmpCoord;
  tmpCoord.append(tmp(0, 0));
  tmpCoord.append(tmp(1, 0));
  tmpCoord.append(tmp(2, 0));

  return tmpCoord;
  }


