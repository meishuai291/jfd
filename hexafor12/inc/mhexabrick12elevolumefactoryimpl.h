/*
 * mhexabrick12elevolumefactoryimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELEVOLUMEFACTORYIMPL_H_
#define MHEXABRICK12ELEVOLUMEFACTORYIMPL_H_

#include <mobjectmanager.h>
#include <mhexabrick12elevolumeimpl.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elevolumefactory.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleVolumeFactoryImpl : public MHexaBrick12EleVolumeFactoryInterface
  {
  public:
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleVolumeFactoryImpl()
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
      return "factory for MHexaBrick12EleVolume";
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
      MHexaBrick12EleVolumeImpl* p = new MHexaBrick12EleVolumeImpl;
      return MHexaBrick12EleVolume(p);
      }
  };



#endif /* MHEXABRICK12ELEVOLUMEFACTORYIMPL_H_ */
