/*
 * mhexabrick12strainmatrixfactoryimpl.h
 *
 *  Created on: 2014年10月10日
 *      Author: jhello
 */

#ifndef MHEXABRICK12STRAINMATRIXFACTORYIMPL_H_
#define MHEXABRICK12STRAINMATRIXFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mhexabrick12strainmatriximpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12strainmatrixfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12StrainMatrixFactoryImpl : public MHexaBrick12StrainMatrixFactoryInterface
  {
  public:
    /*
     * 析构函数，仅为类继承关系而设置
     */
    ~MHexaBrick12StrainMatrixFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MElementsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      QList<QVariant> list;
      list << "HexaBrick12StrainMatrix";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12StrainMatrix";
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
      MHexaBrick12StrainMatrixImpl* p = new MHexaBrick12StrainMatrixImpl;
      return MHexaBrick12StrainMatrix(p);
      }
  };



#endif /* MHEXABRICK12STRAINMATRIXFACTORYIMPL_H_ */
