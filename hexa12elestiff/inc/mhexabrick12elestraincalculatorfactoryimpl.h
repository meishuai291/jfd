/*
 * mhexabrick12elestraincalculatorfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELESTRAINCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELESTRAINCALCULATORFACTORYIMPL_H_

#include <mextensionmanager.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.strain.melementstrainbuilder.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elestraincalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::strain;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleStrainCalculatorFactoryImpl : public MHexaBrick12EleStrainCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleStrainCalculatorFactoryImpl()
      {
      MObjectManager objManager = MObjectManager::getManager();
      _global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      }

    virtual ~MHexaBrick12EleStrainCalculatorFactoryImpl()
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
      return "factory for MHexaBrick12EleStrainCalculator";
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
      MElementStrainBuilder builder = extManager.createExtension(
          "org.sipesc.fems.strain.MElementStrainBuilder");
      Q_ASSERT(!builder.isNull());

      builder.setStrainType("HexaBrick12StrainMatrix");
      builder.setIntegType("Gau3D12IntegForm");

      return builder;
      }

  private:
    MElementsGlobal _global;
  };



#endif /* MHEXABRICK12ELESTRAINCALCULATORFACTORYIMPL_H_ */
