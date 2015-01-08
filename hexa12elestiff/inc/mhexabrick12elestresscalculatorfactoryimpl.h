/*
 * mhexabrick12elestresscalculatorfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELESTRESSCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELESTRESSCALCULATORFACTORYIMPL_H_

#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.stress.melementstressbuilder.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elestresscalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::stress;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleStressCalculatorFactoryImpl : public MHexaBrick12EleStressCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleStressCalculatorFactoryImpl()
      {
      MObjectManager objManager = MObjectManager::getManager();
      _global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      }
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleStressCalculatorFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      QList < QVariant > list;
      list << "HexaBrick12Element";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12EleStressCalculator";
      }

    QString getIcon() const
      {
      return QString();
      }

    /*
     * 返回优先级
     */
    uint getPriority(const QVariant& key) const
      {
      return 10;
      }

    /**
     * create task object
     *
     * @return return a valid MExtensionWrapper object
     */
    MExtension createExtension(const QVariant& key) const
      {
      MExtensionManager extManager = MExtensionManager::getManager();
      MElementStressBuilder eleStressBuilder = extManager.createExtension(
          "org.sipesc.fems.stress.MElementStressBuilder");
      Q_ASSERT(!eleStressBuilder.isNull());

      eleStressBuilder.setConstiType("brickmatdparser");
      eleStressBuilder.setIntegType("Gau3D12IntegForm");

      return eleStressBuilder;
      }

  private:
    MElementsGlobal _global;
  };



#endif /* MHEXABRICK12ELESTRESSCALCULATORFACTORYIMPL_H_ */
