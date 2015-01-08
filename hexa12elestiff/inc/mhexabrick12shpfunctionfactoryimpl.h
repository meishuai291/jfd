/*
 * mhexabrick12shpfunctionfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12SHPFUNCTIONFACTORYIMPL_H_
#define MHEXABRICK12SHPFUNCTIONFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mhexabrick12shpfunctionimpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12shpfunctionfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12ShpFunctionFactoryImpl : public MHexaBrick12ShpFunctionFactoryInterface
  {
  public:
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12ShpFunctionFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MElementsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      QList<QVariant> list;
      list << "HexaBrick12ShpFunction";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12ShpFunction";
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
     * @return  return a valid MExtensionWrapper object
     */
    MExtension createExtension(const QVariant& key) const
      {
      MHexaBrick12ShpFunctionImpl* p = new MHexaBrick12ShpFunctionImpl;
      return MHexaBrick12ShpFunction(p);
      }
  };



#endif /* MHEXABRICK12SHPFUNCTIONFACTORYIMPL_H_ */
