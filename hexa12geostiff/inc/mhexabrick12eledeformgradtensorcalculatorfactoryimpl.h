/*
 * mhexabrick12eledeformgradtensorcalculatorfactoryimpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12ELEDEFORMGRADTENSORCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELEDEFORMGRADTENSORCALCULATORFACTORYIMPL_H_

#include <mfemsglobalextension.h>
#include <mextensionmanager.h>
#include <org.sipesc.fems.matrictensor.melementdeformgradtensorbuilder.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12eledeformgradtensorcalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;
using namespace org::sipesc::fems::matrictensor;

class MHexaBrick12EleDeformGradTensorCalculatorFactoryImpl: public MHexaBrick12EleDeformGradTensorCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleDeformGradTensorCalculatorFactoryImpl()
      {
      _global = getElementGlobal();
      }

    virtual ~MHexaBrick12EleDeformGradTensorCalculatorFactoryImpl()
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
      QList<QVariant> list;
      list << "HexaBrick12Element";
      return list;
      }
    /**
     * get description of the Mextension created by the current factory.
     * The method is  used for extension.
     * @return description of the mextension
     */
    QString getDescription() const
      {
      return "factory for MHexaBrick12EleDeformGradTensorCalculator";
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
    	MExtensionManager extManager = MExtensionManager::getManager();
    	MElementDeformGradTensorBuilder deformGradBuilder = extManager.createExtension(
    			"org.sipesc.fems.matrictensor.MElementDeformGradTensorBuilder");
    	Q_ASSERT(!deformGradBuilder.isNull());
    	deformGradBuilder.setDeformGradType("HexaBrick12Element");
    	deformGradBuilder.setIntegType("Gau3D12IntegForm");
//      MElementDeformGradTensorBuilderImpl* p = new MElementDeformGradTensorBuilderImpl;
//      p->setDeformGradType("HexaBrick12Element");
//      p->setIntegType("Gau3D12IntegForm");

      return deformGradBuilder;
      }

  private:
    MElementsGlobal _global;
  };

#endif /* MHEXABRICK12ELEDEFORMGRADTENSORCALCULATORFACTORYIMPL_H_ */
