/*
 * mbrickelegreenstraincalculatorfactoryimpl.h
 *
 *  Created on: 2012-10-24
 *      Author: liuyin
 */

#ifndef MBRICKELEGREENSTRAINCALCULATORFACTORYIMPL_H_
#define MBRICKELEGREENSTRAINCALCULATORFACTORYIMPL_H_

#include <mfemsglobalextension.h>
#include <org.sipesc.fems.matrictensor.mbrickelegreenstraincalculator.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mbrickelegreenstraincalculatorfactory.h>
using namespace org::sipesc::fems::global;
using org::sipesc::fems::matrictensor::MBrickEleGreenStrainCalculator;
using namespace org::sipesc::fems::hexa12geostress;

class MBrickEleGreenStrainCalculatorFactory2Impl: public MBrickEleGreenStrainCalculatorFactoryInterface
  {
  public:
    virtual ~MBrickEleGreenStrainCalculatorFactory2Impl()
      {
      }
    /**
     * get types which current factory support, one factory may support multiple
     * types.
     *
     * @return types which current factory support
     */
    QList<QVariant> getTypes() const
      {
      MElementsGlobal global = getElementGlobal();
      QList<QVariant> list;
      list << global.getValue(MElementsGlobal::HexaBrickElement)
          << global.getValue(MElementsGlobal::TetraBrickElement)
          << "HexaBrick12Element";
      return list;
      }
    /**
     * get description of the Mextension created by the current factory.
     * The method is  used for extension.
     * @return description of the mextension
     */
    QString getDescription() const
      {
      return "factory for MBrickEleGreenStrainCalculator";
      }
    /**
     * get priority with given type, priority should be integer greater than 0.
     * larger value has highter priority.
     *
     * @param type types which current factory supports
     * @return priority of current factory
     */
    uint getPriority(const QVariant& type) const
      {
      return 10;
      }
    /**
     * get icon path of MExtension;
     * the QString value is a path pointing to QIcon object,
     * The method is used for extension.
     * @return icon path of MExtension
     */
    QString getIcon() const
      {
      return QString();
      }
    /**
     * create an object of MExtension with given type;
     * if type is empty, return an object with top priority by default.
     *
     * @param type type which current factory supports
     * @return object of MExtension
     */
    MExtension createExtension(const QVariant& type) const
      {
    	MExtensionManager extensionManager = MExtensionManager::getManager();
    	Q_ASSERT(!extensionManager.isNull());
    	MBrickEleGreenStrainCalculator p = extensionManager.createExtension(
    			"org.sipesc.fems.matrictensor.MBrickEleGreenStrainCalculator");
    	Q_ASSERT(!p.isNull());
      return p;
      }
  };

#endif /* MBRICKELEGREENSTRAINCALCULATORFACTORYIMPL_H_ */
