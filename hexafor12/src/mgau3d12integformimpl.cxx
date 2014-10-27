/*
 * mgau3d12integformimpl.cxx
 *
 *  Created on: 2014年10月12日
 *      Author: jhello
 */

#include <mgau3d12integformimpl.h>
#include <qmath.h>
#include <mextensionmanager.h>
#include <qdebug.h>
class MGau3D12IntegFormImpl::Data
  {
  public:
    Data()
      {
      }

    QList<MIntegPoint> _integ;

  public:
    bool initialize()
      {
      //通过插件找工厂管理器
      MExtensionManager extManager = MExtensionManager::getManager();
      //生成自由度数据类解析器

      QList<double> weights, integCoord;
      weights.append(0.555555555555556);
      integCoord.append(0.774596669241483);
      weights.append(0.555555555555556);
      integCoord.append(-0.774596669241483);

      QList<double> weights1, integCoord1;
      weights1.append(0.555555555555556);
      integCoord1.append(0.774596669241483);
      weights1.append(0.888888888888889);
      integCoord1.append(0.000000000000000);
      weights1.append(0.555555555555556);
      integCoord1.append(-0.774596669241483);


      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
          for (int k = 0; k < 3; k++)
            {
            MIntegPoint integPoint = extManager.createExtension(
                "org.sipesc.fems.femsutils.MIntegPoint");
            Q_ASSERT(!integPoint.isNull());
            integPoint.setWeight(
                weights1.value(k) * weights.value(j) * weights.value(i));
            QList<double> paraCoord;
            paraCoord.append(integCoord1.value(k));
            paraCoord.append(integCoord.value(j));
            paraCoord.append(integCoord.value(i));
            integPoint.setIntegCoord(paraCoord);
            int index = k * 4 + j * 2 + i;
            integPoint.setIndex(index);
            _integ.append(integPoint);
            //qDebug()<<paraCoord.value(0)<<paraCoord.value(1)<<paraCoord.value(2)<<weights1.value(k) * weights.value(j) * weights.value(i)<<"paraCoordparaCoordparaCoordparaCoordparaCoord";

            }

      return true;
      }
  };

MGau3D12IntegFormImpl::MGau3D12IntegFormImpl()
  {
  _data.reset(new MGau3D12IntegFormImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MGau3D12IntegFormImpl::"
        "MGau3D12IntegFormImpl() ****** failed");

  if (!_data->initialize())
    mReportError(M_ERROR_FATAL, "MGau3D12IntegFormImpl::"
        "MGau3D12IntegFormImpl() ****** initialize failed");
  }

MGau3D12IntegFormImpl::~MGau3D12IntegFormImpl()
  {
  }

int MGau3D12IntegFormImpl::getCount() const
  {
  return 12;
  }

MIntegPoint MGau3D12IntegFormImpl::getIntegPoint(const int& index) const
  {
  return _data->_integ.value(index);
  }


