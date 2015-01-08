/*
 * mhexabrick12elestiffcalculatorfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELESTIFFCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELESTIFFCALCULATORFACTORYIMPL_H_

#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.element.melementstiffbuilder.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elestiffcalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::element;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleStiffCalculatorFactoryImpl : public MHexaBrick12EleStiffCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleStiffCalculatorFactoryImpl()
      {
      MObjectManager objManager = MObjectManager::getManager();
      _global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      }
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12EleStiffCalculatorFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      QList<QVariant> list;
      list << "HexaBrick12Element";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12EleStiffCalculator";
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
    /*
     * create task object
     *
     * @ return a valid MExtensionWrapper object
     */
    MExtension createExtension(const QVariant& key) const
      {
      MExtensionManager extManager = MExtensionManager::getManager();
      MElementStiffBuilder eleStiffBuilder = extManager.createExtension(
          "org.sipesc.fems.element.MElementStiffBuilder");
      Q_ASSERT(!eleStiffBuilder.isNull());
      eleStiffBuilder.setStrainType("HexaBrick12StrainMatrix");
      eleStiffBuilder.setConstType("brickeleconstidparser");
      eleStiffBuilder.setIntegType("Gau3D12IntegForm");

      return eleStiffBuilder;
      }

  private:
    MElementsGlobal _global;
  };



#endif /* MHEXABRICK12ELESTIFFCALCULATORFACTORYIMPL_H_ */
