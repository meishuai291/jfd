/*
 * mbrickelecauchystresscalculatorfactoryimpl.h
 *
 *  Created on: 2012-10-24
 *      Author: liuyin
 */

#ifndef MBRICKELECAUCHYSTRESSCALCULATORFACTORYIMPL_H_
#define MBRICKELECAUCHYSTRESSCALCULATORFACTORYIMPL_H_

#include <mfemsglobalextension.h>
#include <org.sipesc.fems.matrictensor.mbrickelecauchystresscalculator.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mbrickelecauchystresscalculatorfactory.h>
using namespace org::sipesc::fems::global;
using org::sipesc::fems::matrictensor::MBrickEleCauchyStressCalculator;
using namespace org::sipesc::fems::hexa12geostress;

class MBrickEleCauchyStressCalculatorFactory2Impl: public MBrickEleCauchyStressCalculatorFactoryInterface
  {
  public:
    virtual ~MBrickEleCauchyStressCalculatorFactory2Impl()
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
      return "factory for MBrickEleCauchyStressCalculator";
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
    	MBrickEleCauchyStressCalculator p = extensionManager.createExtension(
    			"org.sipesc.fems.matrictensor.MBrickEleCauchyStressCalculator");
    	Q_ASSERT(!p.isNull());
      return p;
      }
  };

#endif /* MBRICKELECAUCHYSTRESSCALCULATORFACTORYIMPL_H_ */
