/*
 * mhexabrick12shpfunctionimpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12shpfunctionimpl.h>
#include <mobjectmanager.h>
#include <qmath.h>
#include <qdebug.h>
MHexaBrick12ShpFunctionImpl::MHexaBrick12ShpFunctionImpl()
  {
  MObjectManager objManager = MObjectManager::getManager();
  _mFactory = objManager.getObject("org.sipesc.fems.matrix.matrixfactory");
  Q_ASSERT(!_mFactory.isNull());
  }

MHexaBrick12ShpFunctionImpl::~MHexaBrick12ShpFunctionImpl()
  {
  }

MMatrix MHexaBrick12ShpFunctionImpl::getShpFunction(
    const QList<double>& paraCoord,
    const QVector<MNodeData>& nodeLocalCoord) const //以下这两个程序可以更方便
  {
  MMatrix mdn = _mFactory.createMatrix(1, 12);
  double ksi, eit, cait;
  ksi = paraCoord.value(0);
  eit = paraCoord.value(1);
  cait = paraCoord.value(2);

  mdn(0, 0, (1 - ksi) * (1 - eit) * (1 - cait) * (-cait) / 8);
  mdn(0, 1, (1 + ksi) * (1 - eit) * (1 - cait) * (-cait) / 8);
  mdn(0, 2, (1 + ksi) * (1 + eit) * (1 - cait) * (-cait) / 8);
  mdn(0, 3, (1 - ksi) * (1 + eit) * (1 - cait) * (-cait) / 8);

//  mdn(0, 0, (1 - ksi) * (1 + eit) * (1 - cait) * (-ksi) / 8);
//  mdn(0, 1, (1 - ksi) * (1 - eit) * (1 - cait) * (-ksi) / 8);
//  mdn(0, 2, (1 - ksi) * (1 - eit) * (1 + cait) * (-ksi) / 8);
//  mdn(0, 3, (1 - ksi) * (1 + eit) * (1 + cait) * (-ksi) / 8);

  mdn(0, 4, (1 - ksi) * (1 - eit) * (1 + cait) * cait / 8);
  mdn(0, 5, (1 + ksi) * (1 - eit) * (1 + cait) * cait / 8);
  mdn(0, 6, (1 + ksi) * (1 + eit) * (1 + cait) * cait / 8);
  mdn(0, 7, (1 - ksi) * (1 + eit) * (1 + cait) * cait / 8);

//  mdn(0, 4, (1 + ksi) * (1 + eit) * (1 - cait) * ksi / 8);
//  mdn(0, 5, (1 + ksi) * (1 - eit) * (1 - cait) * ksi / 8);
//  mdn(0, 6, (1 + ksi) * (1 - eit) * (1 + cait) * ksi / 8);
//  mdn(0, 7, (1 + ksi) * (1 + eit) * (1 + cait) * ksi / 8);

  mdn(0, 8, (1 - qPow(cait, 2)) * (1 - ksi) * (1 - eit) / 4);
  mdn(0, 9, (1 - qPow(cait, 2)) * (1 - ksi) * (1 + eit) / 4);
  mdn(0, 10, (1 - qPow(cait, 2)) * (1 + ksi) * (1 + eit) / 4);
  mdn(0, 11, (1 - qPow(cait, 2)) * (1 + ksi) * (1 - eit) / 4);

//  mdn(0, 8, (1 - qPow(ksi, 2)) * (1 + eit) * (1 - cait) / 4);
//  mdn(0, 9, (1 - qPow(ksi, 2)) *  (1 - eit) * (1 - cait) / 4);
//  mdn(0, 10, (1 - qPow(ksi, 2)) * (1 - eit) * (1 + cait) / 4);
//  mdn(0, 11, (1 - qPow(ksi, 2)) * (1 + eit) * (1 + cait) / 4);
//	int Row =mdn.getRowCount();
//  	int Col = mdn.getColCount();
//
//  	qDebug() << Row;
//  	qDebug() << Col;
//
//  	for (int i = 0; i < Row; i++)
//  		for (int j = 0; j < Col; j++) {
//  			qDebug() << mdn(i, j)
//  					<< "-------------------------------------------------getShpFunctiongetShpFunctiongetShpFunctiongetShpFunctiongetShpFunction";
//  		}

  return mdn;
  }

MMatrix MHexaBrick12ShpFunctionImpl::getShpFuncDerivative(
    const QList<double>& paraCoord,
    const QVector<MNodeData>& nodeLocalCoord) const
  {
  MMatrix msfd= _mFactory.createMatrix(3, 12);
  double ksi, eit, cait;
  ksi = paraCoord.value(0);
  eit = paraCoord.value(1);
  cait = paraCoord.value(2);


  msfd(0, 0, - (1 - eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 0, - (1 - ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 0, (-1 + 2 * cait) * (1 - ksi) * (1 - eit) / 8);

//  msfd(0, 0, (2 * ksi - 1) * (1 + eit) * (1 - cait) / 8);
//  msfd(1, 0, (1 - ksi) * (1 - cait) * (-ksi) / 8);
//  msfd(2, 0, -(1 - ksi) * (1 + eit) * (-ksi) / 8);

  msfd(0, 1, (1 - eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 1, -(1 + ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 1, (-1 + 2 * cait) * (1 + ksi) * (1 - eit) / 8);

//  msfd(0, 1, (2 * ksi - 1) * (1 - eit) * (1 - cait) / 8);
//  msfd(1, 1, -(1 - ksi) * (1 - cait) * (-ksi) / 8);
//  msfd(2, 1, -(1 - ksi) * (1 - eit) * (-ksi) / 8);

  msfd(0, 2, (1 + eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 2, (1 + ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 2, (-1 + 2 * cait) * (1 + ksi) * (1 + eit) / 8);

//  msfd(0, 2, (2 * ksi - 1) * (1 - eit) * (1 + cait) / 8);
//  msfd(1, 2, -(1 - ksi) * (1 + cait) * (-ksi) / 8);
//  msfd(2, 2, (1 - ksi) * (1 - eit) * (-ksi) / 8);

  msfd(0, 3, -(1 + eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 3, (1 - ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 3, (-1 + 2 * cait) * (1 - ksi) * (1 + eit) / 8);

//  msfd(0, 3, (2 * ksi - 1) * (1 + eit) * (1 + cait) / 8);
//  msfd(1, 3, (1 - ksi) * (1 + cait) * (-ksi) / 8);
//  msfd(2, 3, (1 - ksi) * (1 + eit) * (-ksi) / 8);

  msfd(0, 4, -(1 - eit) * (1 + cait) * cait / 8);
  msfd(1, 4, -(1 - ksi) * (1 + cait) * cait / 8);
  msfd(2, 4, (1 + 2 * cait) * (1 - ksi) * (1 - eit) / 8);

//  msfd(0, 4, (1 + 2 * ksi) * (1 + eit) * (1 - cait) / 8);
//  msfd(1, 4, (1 + ksi) * (1 - cait) * ksi / 8);
//  msfd(2, 4, -(1 + ksi) * (1 + eit)  * ksi / 8);

  msfd(0, 5, (1 - eit) * (1 + cait) * cait / 8);
  msfd(1, 5, -(1 + ksi) * (1 + cait) * cait / 8);
  msfd(2, 5, (1 + 2 * cait) * (1 + ksi) * (1 - eit) / 8);

//  msfd(0, 5, (1 + 2 * ksi) * (1 - eit) * (1 - cait) / 8);
//  msfd(1, 5, -(1 + ksi) * (1 - cait) * ksi / 8);
//  msfd(2, 5, -(1 + ksi) * (1 - eit) * ksi / 8);

  msfd(0, 6, (1 + eit) * (1 + cait) * cait / 8);
  msfd(1, 6, (1 + ksi) * (1 + cait) * cait / 8);
  msfd(2, 6, (1 + 2 * cait) * (1 + ksi) * (1 + eit) / 8);

//  msfd(0, 6, (1 + 2 * ksi) * (1 - eit) * (1 + cait) / 8);
//  msfd(1, 6, -(1 + ksi) * (1 + cait) * ksi / 8);
//  msfd(2, 6, (1 + ksi) * (1 - eit) * ksi / 8);

  msfd(0, 7, -(1 + eit) * (1 + cait) * cait / 8);
  msfd(1, 7, (1 - ksi) * (1 + cait) * cait / 8);
  msfd(2, 7, (1 + 2 * cait) * (1 - ksi) * (1 + eit) / 8);

//  msfd(0, 7, (1 + 2 * ksi) * (1 + eit) * (1 + cait) / 8);
//  msfd(1, 7, (1 + ksi) * (1 + cait) * ksi / 8);
//  msfd(2, 7, (1 + ksi) * (1 + eit) * ksi / 8);

  msfd(0, 8, -(1 - qPow(cait, 2)) * (1 - eit) / 4);
  msfd(1, 8, -(1 - qPow(cait, 2)) * (1 - ksi) / 4);
  msfd(2, 8, -cait * (1 - ksi) * (1 - eit) / 2);

//  msfd(0, 8, -ksi * (1 + eit) * (1 - cait) / 2);
//  msfd(1, 8, (1 - qPow(ksi, 2)) * (1 - cait) / 4);
//  msfd(2, 8, -(1 - qPow(ksi, 2)) * (1 + eit) / 4);

  msfd(0, 9, -(1 - qPow(cait, 2)) * (1 + eit) / 4);
  msfd(1, 9, (1 - qPow(cait, 2)) * (1 - ksi) / 4);
  msfd(2, 9, -cait * (1 - ksi) * (1 + eit) / 2);

//  msfd(0, 9, -ksi * (1 - eit) * (1 - cait) / 2);
//  msfd(1, 9, -(1 - qPow(ksi, 2)) * (1 - cait) / 4);
//  msfd(2, 9, -(1 - qPow(ksi, 2)) * (1 - eit) / 4);

  msfd(0, 10, (1 - qPow(cait, 2)) * (1 + eit) / 4);
  msfd(1, 10, (1 - qPow(cait, 2)) * (1 + ksi) / 4);
  msfd(2, 10, -cait * (1 + ksi) * (1 + eit) / 2);

//  msfd(0, 10, -ksi * (1 - eit) * (1 + cait) / 4);
//  msfd(1, 10, -(1 - qPow(ksi, 2)) * (1 + cait) / 4);
//  msfd(2, 10, (1 - qPow(ksi, 2)) * (1 - eit) / 4);

  msfd(0, 11, (1 - qPow(cait, 2)) * (1 - eit) / 4);
  msfd(1, 11, -(1 - qPow(cait, 2)) * (1 + ksi) / 4);
  msfd(2, 11, -cait * (1 + ksi) * (1 - eit) / 4);

//  msfd(0, 11, -ksi * (1 + eit) * (1 + cait) / 4);
//  msfd(1, 11, (1 - qPow(ksi, 2)) * (1 + cait) / 4);
//  msfd(2, 11, (1 - qPow(ksi, 2)) * (1 + eit) / 4);

//	int Row =msfd.getRowCount();
//	int Col = msfd.getColCount();
//	qDebug() << Row;
//	qDebug() << Col;
//
//
//		for (int j = 0; j < Col; j++)
//			for (int i = 0; i < Row; i++){
//			qDebug() << msfd(i, j)
//					<< "-------------------------------------------------DerivativeDerivativeDerivativeDerivativeDerivativeDerivativeDerivative";
//		}
  return msfd;
  }


