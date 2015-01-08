/*
 * mhexabrick12elestressstiffcalculatorfactoryimpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12ELESTRESSSTIFFCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELESTRESSSTIFFCALCULATORFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.fems.geostress.melementstressstiffbuilder.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12elestressstiffcalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;
using namespace org::sipesc::fems::geostress;

class MHexaBrick12EleStressStiffCalculatorFactoryImpl: public MHexaBrick12EleStressStiffCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleStressStiffCalculatorFactoryImpl()
      {
      MObjectManager objManager = MObjectManager::getManager();
      _global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      }
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12EleStressStiffCalculatorFactoryImpl()
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
      return "factory for MHexaBrick12EleStressStiffCalculator";
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
    	MElementStressStiffBuilder eleStressStiffBuilder = extManager.createExtension(
    			"org.sipesc.fems.geostress.MElementStressStiffBuilder");
    	Q_ASSERT(!eleStressStiffBuilder.isNull());
    	eleStressStiffBuilder.setGSType("HexaBrick12Element");
    	eleStressStiffBuilder.setIntegType("Gau3D12IntegForm");

      return eleStressStiffBuilder;
      }

  private:
    MElementsGlobal _global;
  };

#endif /* MHEXABRICK12ELESTRESSSTIFFCALCULATORFACTORYIMPL_H_ */
