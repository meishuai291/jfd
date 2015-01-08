/*
 * mhexabrick12elejaccobianmatrixfactoryimpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12ELEJACCOBIANMATRIXFACTORYIMPL_H_
#define MHEXABRICK12ELEJACCOBIANMATRIXFACTORYIMPL_H_

#include <mfemsglobalextension.h>
#include <mhexabrick12elejaccobianmatriximpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12elejaccobianmatrixfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;

class MHexaBrick12EleJaccobianMatrixFactoryImpl: public MHexaBrick12EleJaccobianMatrixFactoryInterface
  {
  public:
    virtual ~MHexaBrick12EleJaccobianMatrixFactoryImpl()
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
      return "factory for MHexaBrick12EleJaccobianMatrix";
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
      MHexaBrick12EleJaccobianMatrixImpl* p =
          new MHexaBrick12EleJaccobianMatrixImpl;
      return MHexaBrick12EleJaccobianMatrix(p);
      }
  };

#endif /* MHEXABRICK12ELEJACCOBIANMATRIXFACTORYIMPL_H_ */
