/*
 * mhexabrick12eleconstidparserfactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELECONSTIDPARSERFACTORYIMPL_H_
#define MHEXABRICK12ELECONSTIDPARSERFACTORYIMPL_H_
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mhexabrick12eleconstidparserimpl.h>
#include <org.sipesc.fems.global.mmaterialsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12eleconstidparserfactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleConstiDParserFactoryImpl : public MHexaBrick12EleConstiDParserFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleConstiDParserFactoryImpl()
      {
      }

  public:
    QList<QVariant> getTypes() const
      {
      MObjectManager objManager = MObjectManager::getManager();
      MMaterialsGlobal global = objManager.getObject(
          "org.sipesc.fems.global.materialsglobal");
      QList<QVariant> list;
      list << global.getValue(MMaterialsGlobal::SolidIsoType);
      return list;
      }

    QString getDescription() const
      {
      return "factory for MHexaBrick12EleConstiDParser";
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
      return 20;
      }
    /**
     * create task object
     *
     * @return return a valid MExtensionWrapper object
     */
    MExtension createExtension(const QVariant& key) const
      {
      MHexaBrick12EleConstiDParserImpl* p = new MHexaBrick12EleConstiDParserImpl;
      return MHexaBrick12EleConstiDParser(p);
      }
  };



#endif /* MHEXABRICK12ELECONSTIDPARSERFACTORYIMPL_H_ */
