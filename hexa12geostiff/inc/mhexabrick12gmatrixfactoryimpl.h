/*
 * mhexabrick12gmatrixfactoryimpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12GMATRIXFACTORYIMPL_H_
#define MHEXABRICK12GMATRIXFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mhexabrick12elestressstiffgmatriximpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12gmatrixfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;

class MHexaBrick12GMatrixFactoryImpl: public MHexaBrick12GMatrixFactoryInterface
  {
  public:
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12GMatrixFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MElementsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      QList<QVariant> list;
      list << "HexaBrick12Element";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12GMatrixFactory";
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
      MHexaBrick12EleStressStiffGMatrixImpl* p =
          new MHexaBrick12EleStressStiffGMatrixImpl;
      return MHexaBrick12EleStressStiffGMatrix(p);
      }
  };

#endif /* MHEXABRICK12GMATRIXFACTORYIMPL_H_ */
