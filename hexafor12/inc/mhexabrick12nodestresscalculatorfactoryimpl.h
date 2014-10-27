/*
 * mhexabrick12nodestresscalculatorfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12NODESTRESSCALCULATORFACTORYIMPL_H_
#define MHEXABRICK12NODESTRESSCALCULATORFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mhexabrick12nodestresscalculatorimpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12nodestresscalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12NodeStressCalculatorFactoryImpl : public MHexaBrick12NodeStressCalculatorFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12NodeStressCalculatorFactoryImpl()
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
      return "factory for MHexaBrick12NodeStressCalculator";
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
      MHexaBrick12NodeStressCalculatorImpl* p =
          new MHexaBrick12NodeStressCalculatorImpl;
      return MHexaBrick12NodeStressCalculator(p);
      }
  };



#endif /* MHEXABRICK12NODESTRESSCALCULATORFACTORYIMPL_H_ */
