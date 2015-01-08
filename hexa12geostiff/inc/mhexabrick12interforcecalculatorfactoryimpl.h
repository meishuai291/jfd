/*
 * mhexabrick12interforcecalculatorfactoryimpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12INTERFORCECALCULATORFACTORYIMPL_H_
#define MHEXABRICK12INTERFORCECALCULATORFACTORYIMPL_H_

#include <mfemsglobalextension.h>
#include <mextensionmanager.h>
#include <org.sipesc.fems.eleinterforce.mbrickeleinterforcebuilder.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12eleinterforcecalculatorfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexa12geostress;
using namespace org::sipesc::fems::eleinterforce;

class MHexaBrick12EleInterForceCalculatorFactoryImpl: public MHexaBrick12EleInterForceCalculatorFactoryInterface
  {
  public:
    MHexaBrick12EleInterForceCalculatorFactoryImpl()
      {
      _global = getElementGlobal();
      }
    /*
     * 析构函数，仅为类继承关系而设置。
     */
    virtual ~MHexaBrick12EleInterForceCalculatorFactoryImpl()
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
      return "factory for MHexaBrick12EleInterForceCalculator";
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

    	MExtensionManager extManager = MExtensionManager::getManager();
    	MBrickEleInterForceBuilder eleBrickForce = extManager.createExtension(
    			"org.sipesc.fems.eleinterforce.MBrickEleInterForceBuilder");
    	Q_ASSERT(!eleBrickForce.isNull());
    	eleBrickForce.setStrainType("HexaBrick12StrainMatrix");
    	eleBrickForce.setIntegType("Gau3D12IntegForm");

//      MBrickEleInterForceBuilderImpl* p = new MBrickEleInterForceBuilderImpl;
//      p->setStrainType("HexaBrick12StrainMatrix");
//      p->setIntegType("Gau3D12IntegForm");

      return eleBrickForce;
      }

  private:
    MElementsGlobal _global;
  };

#endif /* MHEXABRICK12INTERFORCECALCULATORFACTORYIMPL_H_ */
