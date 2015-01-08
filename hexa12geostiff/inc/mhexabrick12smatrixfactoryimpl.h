/*
 * mhexabrick12smatrixfactoryimpl.h
 *
 *  Created on: 2015年1月6日
 *      Author: sipesc
 */

#ifndef MHEXABRICK12SMATRIXFACTORYIMPL_H_
#define MHEXABRICK12SMATRIXFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <org.sipesc.fems.geostress.mbrickelestressstiffsmatrix.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12smatrixfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;
using namespace org::sipesc::fems::geostress;

class MHexaBrick12SMatrixFactoryImpl: public MHexaBrick12SMatrixFactoryInterface
  {
  public:
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12SMatrixFactoryImpl()
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
    	MExtensionManager extensionManager = MExtensionManager::getManager();
    	Q_ASSERT(!extensionManager.isNull());
    	MBrickEleStressStiffSMatrix p = extensionManager.createExtension(
    			"org.sipesc.fems.geostress.MBrickEleStressStiffSMatrix");
    	Q_ASSERT(!p.isNull());

    	return p;
      }
  };


#endif /* MHEXABRICK12SMATRIXFACTORYIMPL_H_ */
