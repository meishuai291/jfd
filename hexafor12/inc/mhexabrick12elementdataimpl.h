/*
 * mhexabrick12elementdataimpl.h
 *
 *  Created on: 2014-10-9
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELEMENTDATAIMPL_H_
#define MHEXABRICK12ELEMENTDATAIMPL_H_

#include <melementdataimpl.h>
#include <org.sipesc.core.engdbs.data.mdatafactory.h>
using namespace org::sipesc::core::engdbs::data;

const QUuid MHexaBrick12ElementDataUuid =
    "a2cbcde0-5d3b-4dd5-b4e8-83bdf67c9860";

class MHexaBrick12ElementDataImpl : public MElementDataImpl
  {
  public:
    MHexaBrick12ElementDataImpl();
    virtual ~MHexaBrick12ElementDataImpl();

  public:
    QString getType() const;
    QList<int> getActiveDofs(int);

  public:
    QUuid getTypeId() const
      {
      return MHexaBrick12ElementDataUuid;
      }

  private:
    int _node12Id[12];
  };

class MHexaBrick12ElementDataFactoryImpl : public MDataFactoryInterface
  {
  public:
    MHexaBrick12ElementDataFactoryImpl();
    virtual ~MHexaBrick12ElementDataFactoryImpl()
      {
      }

  public:
    QUuid getTypeId() const
      {
      return MHexaBrick12ElementDataUuid;
      }
    MDataObject createObject() const;
    int supportedType(const QUuid&) const;
  };

#endif /* MHEXABRICKELEMENTDATAIMPL_H_ */
