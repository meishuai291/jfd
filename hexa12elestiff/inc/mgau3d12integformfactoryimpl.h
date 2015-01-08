/*
 * mgau3d12integformfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MGAU3D12INTEGFORMFACTORYIMPL_H_
#define MGAU3D12INTEGFORMFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mgau3d12integformimpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mgau3d12integformfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MGau3D12IntegFormFactoryImpl : public MGau3D12IntegFormFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MGau3D12IntegFormFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MElementsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.elementsglobal");
      QList<QVariant> list;
      list << "Gau3D12IntegForm"
          << "HexaBrick12Element";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MGau3D12IntegForm";
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
    	MGau3D12IntegFormImpl* p = new MGau3D12IntegFormImpl;
      return MGau3D12IntegForm(p);
      }
  };



#endif /* MGAU3D12INTEGFORMFACTORYIMPL_H_ */
