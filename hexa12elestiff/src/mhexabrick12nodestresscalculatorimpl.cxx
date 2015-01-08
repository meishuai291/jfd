/*
 * mhexabrick12nodestresscalculatorimpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mhexabrick12nodestresscalculatorimpl.h>
#include <qmath.h>
#include <qdebug.h>
#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixdata.h>
#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.data.melementlocalcoorddata.h>
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::core::engdbs::data;

class MHexaBrick12NodeStressCalculatorImpl::Data
  {
  public:
    Data()
      {
      }

  public:
    MDataFactory _listFactory;
    MVectorFactory _vFactory;
    MMatrixFactory _mFactory;
    MMatrix _extrapolationMatrix;
    MMatrixTools _mTools;
    MElementLocalCoordData _coordData;

  public:
    bool initialize()
      {
      MObjectManager objManager = MObjectManager::getManager();
      _mFactory = objManager.getObject(
          "org.sipesc.fems.matrix.matrixfactory");
      Q_ASSERT(!_mFactory.isNull());
      _vFactory = objManager.getObject(
          "org.sipesc.fems.matrix.vectorfactory");
      Q_ASSERT(!_vFactory.isNull());
      MDataFactoryManager factoryManager = objManager.getObject(
          "org.sipesc.core.engdbs.mdatafactorymanager");
      Q_ASSERT(!factoryManager.isNull());
      QString name = "org.sipesc.core.engdbs.MDataObjectList";
      _listFactory = factoryManager.getFactory(name);
      Q_ASSERT(!_listFactory.isNull());

      MExtensionManager extManager = MExtensionManager::getManager();
      _mTools = extManager.createExtension(
          "org.sipesc.fems.matrix.MMatrixTools");
      Q_ASSERT(!_mTools.isNull());

      MMatrix interpolation = _mFactory.createMatrix(12, 12); //应力插值矩阵与应力外推时的转换阵 [ N ]12 * 12乘以[sigmaGauss]12 * 1

      int m = 0;
      QList<double> ksi, eit, cait;
      ksi.append(qSqrt(3) / 3);
      ksi.append(-qSqrt(3) / 3);

      eit.append(qSqrt(3) / 3);
      eit.append(-qSqrt(3) / 3);

      cait.append(qSqrt(15) / 5);
      cait.append(0);
      cait.append(-qSqrt(15) / 5);

      for (int k = 0; k < 3; k++)
            for (int j = 0; j < 2; j++)
                   for (int i = 0; i < 2; i++){
            interpolation(m, 0, (1 - ksi.value(i)) * (1 - eit.value(j)) * (1 - cait.value(k)) * (-cait.value(k)) / 8);
            interpolation(m, 1, (1 + ksi.value(i)) * (1 - eit.value(j)) * (1 - cait.value(k)) * (-cait.value(k)) / 8);
            interpolation(m, 2, (1 + ksi.value(i)) * (1 + eit.value(j)) * (1 - cait.value(k)) * (-cait.value(k)) / 8);
            interpolation(m, 3, (1 - ksi.value(i)) * (1 + eit.value(j)) * (1 - cait.value(k)) * (-cait.value(k)) / 8);
            interpolation(m, 4, (1 - ksi.value(i)) * (1 - eit.value(j)) * (1 + cait.value(k)) * (cait.value(k)) / 8);
            interpolation(m, 5, (1 + ksi.value(i)) * (1 - eit.value(j)) * (1 + cait.value(k)) * (cait.value(k)) / 8);
            interpolation(m, 6, (1 + ksi.value(i)) * (1 + eit.value(j)) * (1 + cait.value(k)) * (cait.value(k)) / 8);
            interpolation(m, 7, (1 - ksi.value(i)) * (1 + eit.value(j)) * (1 + cait.value(k)) * (cait.value(k)) / 8);
            interpolation(m, 8, (1 - ksi.value(i)) * (1 - eit.value(j)) * (1 - qPow(cait.value(k), 2)) / 4);
            interpolation(m, 9, (1 + ksi.value(i)) * (1 - eit.value(j)) * (1 - qPow(cait.value(k), 2)) / 4);
            interpolation(m, 10, (1 + ksi.value(i)) * (1 + eit.value(j)) * (1 - qPow(cait.value(k), 2)) / 4);
            interpolation(m, 11, (1 - ksi.value(i)) * (1 + eit.value(j)) * (1 - qPow(cait.value(k), 2)) / 4);

            m++;}


      _extrapolationMatrix = _mTools.inverse(interpolation);
      return true;}

  };

MHexaBrick12NodeStressCalculatorImpl::MHexaBrick12NodeStressCalculatorImpl()
  {
  _data.reset(new MHexaBrick12NodeStressCalculatorImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MHexaBrick12NodeStressCalculatorImpl::"
        "MHexaBrick12NodeStressCalculatorImpl() ****** failed");

  if (!_data->initialize()) //Data初始化
    mReportError(M_ERROR_FATAL, "MHexaBrick12NodeStressCalculatorImpl::"
        "MHexaBrick12NodeStressCalculatorImpl() ****** initialize failed");
  }

MHexaBrick12NodeStressCalculatorImpl::~MHexaBrick12NodeStressCalculatorImpl()
  {
  }

void MHexaBrick12NodeStressCalculatorImpl::setState(
    const MDataObject& localData)
  {
  _data->_coordData = localData;
  Q_ASSERT(!_data->_coordData.isNull());
  }

MDataObject MHexaBrick12NodeStressCalculatorImpl::getNodeStress(
    const MDataObject& integStressData)
  {
  MDataObjectList integralPoints =_data->_listFactory.createObject();
  integralPoints = integStressData; //integralPoints ,nodePoints分别存放传进来的高斯点应力和坐标转换前的外推节点应力
  Q_ASSERT(!integralPoints.isNull());
  int eleId = integStressData.getId();

  MVectorData sigmaData1 = integralPoints.getDataAt(0);
  MVector sigma1 = _data->_vFactory.createVector(6);
  sigma1 << sigmaData1;
  MVectorData sigmaData2 = integralPoints.getDataAt(1);
  MVector sigma2 = _data->_vFactory.createVector(6);
  sigma2 << sigmaData2;
  MVectorData sigmaData3 = integralPoints.getDataAt(2);
  MVector sigma3 = _data->_vFactory.createVector(6);
  sigma3 << sigmaData3;
  MVectorData sigmaData4 = integralPoints.getDataAt(3);
  MVector sigma4 = _data->_vFactory.createVector(6);
  sigma4 << sigmaData4;
  MVectorData sigmaData5 = integralPoints.getDataAt(4);
  MVector sigma5 = _data->_vFactory.createVector(6);
  sigma5 << sigmaData5;
  MVectorData sigmaData6 = integralPoints.getDataAt(5);
  MVector sigma6 = _data->_vFactory.createVector(6);
  sigma6 << sigmaData6;
  MVectorData sigmaData7 = integralPoints.getDataAt(6);
  MVector sigma7 = _data->_vFactory.createVector(6);
  sigma7 << sigmaData7;
  MVectorData sigmaData8 = integralPoints.getDataAt(7);
  MVector sigma8 = _data->_vFactory.createVector(6);
  sigma8 << sigmaData8;
  MVectorData sigmaData9 = integralPoints.getDataAt(8);
  MVector sigma9 = _data->_vFactory.createVector(6);
  sigma9 << sigmaData9;
  MVectorData sigmaData10 = integralPoints.getDataAt(9);
  MVector sigma10 = _data->_vFactory.createVector(6);
  sigma10 << sigmaData10;
  MVectorData sigmaData11 = integralPoints.getDataAt(10);
  MVector sigma11 = _data->_vFactory.createVector(6);
  sigma11 << sigmaData11;
  MVectorData sigmaData12 = integralPoints.getDataAt(11);
  MVector sigma12 = _data->_vFactory.createVector(6);
  sigma12 << sigmaData12;


  MVector NSigma1 = _data->_vFactory.createVector(6);
  MVector NSigma2 = _data->_vFactory.createVector(6);
  MVector NSigma3 = _data->_vFactory.createVector(6);
  MVector NSigma4 = _data->_vFactory.createVector(6);
  MVector NSigma5 = _data->_vFactory.createVector(6);
  MVector NSigma6 = _data->_vFactory.createVector(6);
  MVector NSigma7 = _data->_vFactory.createVector(6);
  MVector NSigma8 = _data->_vFactory.createVector(6);
  MVector NSigma9 = _data->_vFactory.createVector(6);
  MVector NSigma10 = _data->_vFactory.createVector(6);
  MVector NSigma11 = _data->_vFactory.createVector(6);
  MVector NSigma12 = _data->_vFactory.createVector(6);


  MVector integralVector = _data->_vFactory.createVector(12);
  MVector tmp1 = _data->_vFactory.createVector(12);
  for (int i = 0; i < 6; i++) //i为应力分量个数
    {
    integralVector(0, sigma1(i)); //integralVector为12个高斯点应力值的第i个分量
    integralVector(1, sigma2(i));
    integralVector(2, sigma3(i));
    integralVector(3, sigma4(i));
    integralVector(4, sigma5(i));
    integralVector(5, sigma6(i));
    integralVector(6, sigma7(i));
    integralVector(7, sigma8(i));
    integralVector(8, sigma9(i));
    integralVector(9, sigma10(i));
    integralVector(10, sigma11(i));
    integralVector(11, sigma12(i));

    tmp1 = _data->_extrapolationMatrix * integralVector; //tmp1为转化12个节点应力值的第i个分量
    NSigma1(i, tmp1(0)); //把tmp1中的12个节点应力值的第i个分量分别分给12个节点应力向量
    NSigma2(i, tmp1(1));
    NSigma3(i, tmp1(2));
    NSigma4(i, tmp1(3));
    NSigma5(i, tmp1(4));
    NSigma6(i, tmp1(5));
    NSigma7(i, tmp1(6));
    NSigma8(i, tmp1(7));
    NSigma9(i, tmp1(8));
    NSigma10(i, tmp1(9));
    NSigma11(i, tmp1(10));
    NSigma12(i, tmp1(11));

    }

  QList<MVector> nodePoints;
  nodePoints << NSigma1 << NSigma2 << NSigma3 << NSigma4 << NSigma5 << NSigma6
      << NSigma7 << NSigma8 << NSigma9  << NSigma10  << NSigma11  << NSigma12;

  MMatrixData mData = _data->_coordData.getTransMatrixData();
  MMatrix m = _data->_mFactory.createMatrix();
  m << mData;

  MMatrix trans = _data->_mFactory.createMatrix(6, 6);
  trans(0, 0, m(0, 0) * m(0, 0));
  trans(0, 1, m(0, 1) * m(0, 1));
  trans(0, 2, m(0, 2) * m(0, 2));
  trans(1, 0, m(1, 0) * m(1, 0));
  trans(1, 1, m(1, 1) * m(1, 1));
  trans(1, 2, m(1, 2) * m(1, 2));
  trans(2, 0, m(2, 0) * m(2, 0));
  trans(2, 1, m(2, 1) * m(2, 1));
  trans(2, 2, m(2, 2) * m(2, 2));

  trans(0, 3, m(0, 0) * m(0, 1));
  trans(0, 4, m(0, 1) * m(0, 2));
  trans(0, 5, m(0, 2) * m(0, 0));
  trans(1, 3, m(1, 0) * m(1, 1));
  trans(1, 4, m(1, 1) * m(1, 2));
  trans(1, 5, m(1, 2) * m(1, 0));
  trans(2, 3, m(2, 0) * m(2, 1));
  trans(2, 4, m(2, 1) * m(2, 2));
  trans(2, 5, m(2, 2) * m(2, 0));

  trans(3, 0, 2.0 * m(0, 0) * m(1, 0));
  trans(3, 1, 2.0 * m(0, 1) * m(1, 1));
  trans(3, 2, 2.0 * m(0, 2) * m(1, 2));
  trans(4, 0, 2.0 * m(1, 0) * m(2, 0));
  trans(4, 1, 2.0 * m(1, 1) * m(2, 1));
  trans(4, 2, 2.0 * m(1, 2) * m(2, 2));
  trans(5, 0, 2.0 * m(2, 0) * m(0, 0));
  trans(5, 1, 2.0 * m(2, 1) * m(0, 1));
  trans(5, 2, 2.0 * m(2, 2) * m(0, 2));

  trans(3, 3, m(0, 0) * m(1, 1) + m(1, 0) * m(0, 1));
  trans(3, 4, m(0, 1) * m(1, 2) + m(1, 1) * m(0, 2));
  trans(3, 5, m(0, 2) * m(1, 0) + m(1, 2) * m(0, 0));
  trans(4, 3, m(1, 0) * m(2, 1) + m(2, 0) * m(1, 1));
  trans(4, 4, m(1, 1) * m(2, 2) + m(2, 1) * m(1, 2));
  trans(4, 5, m(1, 2) * m(2, 0) + m(2, 2) * m(1, 0));
  trans(5, 3, m(2, 0) * m(0, 1) + m(0, 0) * m(2, 1));
  trans(5, 4, m(2, 1) * m(0, 2) + m(0, 1) * m(2, 2));
  trans(5, 5, m(2, 2) * m(0, 0) + m(0, 2) * m(2, 0));

  MDataObjectList nodeStressVectors = _data->_listFactory.createObject(); //nodeStressVectors存放坐标转换后的节点应力
  nodeStressVectors.setId(eleId);
  for (int i = 0; i < 12; i++)
    {
    MVector tmp = nodePoints[i];
    MVector nodeVector = _data->_mTools.transpose(trans) * tmp;
    MVectorData nodeVectorData;
    nodeVector >> nodeVectorData;
    nodeStressVectors.appendData(nodeVectorData);
    }
  return nodeStressVectors;
  }


