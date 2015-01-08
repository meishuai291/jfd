/*
 * mhexabrick12lumpedmassmatrixfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12LUMPEDMASSMATRIXFACTORYIMPL_H_
#define MHEXABRICK12LUMPEDMASSMATRIXFACTORYIMPL_H_
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mhexabrick12lumpedmassmatriximpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12lumpedmassmatrixfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12LumpedMassMatrixFactoryImpl : public MHexaBrick12LumpedMassMatrixFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12LumpedMassMatrixFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MElementsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      QList<QVariant> list;
      list << global.getValue(MElementsGlobal::LumpedMass);
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12LumpedMassMatrix";
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
      MHexaBrick12LumpedMassMatrixImpl* p = new MHexaBrick12LumpedMassMatrixImpl;
      return MHexaBrick12LumpedMassMatrix(p);

      }
  };



#endif /* MHEXABRICK12LUMPEDMASSMATRIXFACTORYIMPL_H_ */
