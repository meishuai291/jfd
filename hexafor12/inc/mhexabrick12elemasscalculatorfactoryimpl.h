/*
 * mhexabrick12elemasscalculatorfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELEMASSCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12ELEMASSCALCULATORFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.mass.melementmassbuilder.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elemasscalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::mass;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleMassCalculatorFactoryImpl : public MHexaBrick12EleMassCalculatorFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleMassCalculatorFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      QList<QVariant> list;
      list << "HexaBrick12Element";
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12EleMassCalculator";
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
      MExtensionManager  extManager = MExtensionManager :: getManager();
      MElementMassBuilder eleMassBuilder = extManager.createExtension(
          "org.sipesc.fems.mass.MElementMassBuilder");
      Q_ASSERT(!eleMassBuilder.isNull());
      eleMassBuilder.setMassType("hexabrick12mass");

      return eleMassBuilder;
      }
  };



#endif /* MHEXABRICK12ELEMASSCALCULATORFACTORYIMPL_H_ */
