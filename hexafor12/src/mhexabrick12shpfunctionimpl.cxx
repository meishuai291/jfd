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

  mdn(0, 4, (1 - ksi) * (1 - eit) * (1 + cait) * cait / 8);
  mdn(0, 5, (1 + ksi) * (1 - eit) * (1 + cait) * cait / 8);
  mdn(0, 6, (1 + ksi) * (1 + eit) * (1 + cait) * cait / 8);
  mdn(0, 7, (1 - ksi) * (1 + eit) * (1 + cait) * cait / 8);

  mdn(0, 8, (1 - ksi) * (1 - eit) * (1 - qPow(cait, 2)) / 4);
  mdn(0, 9, (1 + ksi) * (1 - eit) * (1 - qPow(cait, 2)) / 4);
  mdn(0, 10, (1 + ksi) * (1 + eit) * (1 - qPow(cait, 2)) / 4);
  mdn(0, 11, (1 - ksi) * (1 + eit) * (1 - qPow(cait, 2)) / 4);

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

  msfd(0, 0, -(1 - eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 0, -(1 - ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 0, (1 - eit) * (1 - ksi) * (2 * cait - 1) / 8);

  msfd(0, 1, (1 - eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 1, -(1 + ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 1, (1 - eit) * (1 + ksi) * (2 * cait - 1) / 8);

  msfd(0, 2, (1 + eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 2, (1 + ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 2, (1 + eit) * (1 + ksi) * (2 * cait - 1) / 8);

  msfd(0, 3, -(1 + eit) * (1 - cait) * (-cait) / 8);
  msfd(1, 3, (1 - ksi) * (1 - cait) * (-cait) / 8);
  msfd(2, 3, (1 + eit) * (1 - ksi) * (2 * cait - 1) / 8);

  msfd(0, 4, -(1 - eit) * (1 + cait) * cait / 8);
  msfd(1, 4, -(1 - ksi) * (1 + cait) * cait / 8);
  msfd(2, 4, (1 - eit) * (1 - ksi) * (2 * cait + 1) / 8);

  msfd(0, 5, (1 - eit) * (1 + cait) * cait / 8);
  msfd(1, 5, -(1 + ksi) * (1 + cait) * cait / 8);
  msfd(2, 5, (1 - eit) * (1 + ksi) * (2 * cait + 1) / 8);

  msfd(0, 6, (1 + eit) * (1 + cait) * cait / 8);
  msfd(1, 6, (1 + ksi) * (1 + cait) * cait / 8);
  msfd(2, 6, (1 + eit) * (1 + ksi) * (2 * cait + 1) / 8);

  msfd(0, 7, -(1 + eit) * (1 + cait) * cait / 8);
  msfd(1, 7, (1 - ksi) * (1 + cait) * cait / 8);
  msfd(2, 7, (1 + eit) * (1 - ksi) * (2 * cait + 1) / 8);

  msfd(0, 8, -(1 - eit) * (1 - qPow(cait, 2)) / 4);
  msfd(1, 8, -(1 - ksi) * (1 - qPow(cait, 2)) / 4);
  msfd(2, 8, (1 - eit) * (1 - ksi) * (-cait) / 2);

  msfd(0, 9, (1 - eit) * (1 - qPow(cait, 2)) / 4);
  msfd(1, 9, -(1 + ksi) * (1 - qPow(cait, 2)) / 4);
  msfd(2, 9, (1 - eit) * (1 + ksi) * (-cait) / 2);

  msfd(0, 10, (1 + eit) * (1 - qPow(cait, 2)) / 4);
  msfd(1, 10, (1 + ksi) * (1 - qPow(cait, 2)) / 4);
  msfd(2, 10, (1 + eit) * (1 + ksi) * (-cait) / 2);

  msfd(0, 11, -(1 + eit) * (1 - qPow(cait, 2)) / 4);
  msfd(1, 11, (1 - ksi) * (1 - qPow(cait, 2)) / 4);
  msfd(2, 11, (1 + eit) * (1 - ksi) * (-cait) / 2);

  return msfd;
  }


