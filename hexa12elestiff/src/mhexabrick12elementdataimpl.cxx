/*
 * mhexabrick12elementdataimpl.cxx
 *
 *  Created on: 2013-6-8
 *      Author: Zhoudw
 */

#include <mhexabrick12elementdataimpl.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.global.melementsglobal.h>
#include <org.sipesc.fems.data.melementactivedofs.h>
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;

MHexaBrick12ElementDataImpl::MHexaBrick12ElementDataImpl()
  {
  _propertyId = 0;
  _number = 12;
  _nodeId = _node12Id;
  }

MHexaBrick12ElementDataImpl::~MHexaBrick12ElementDataImpl()
  {
  }

QString  MHexaBrick12ElementDataImpl::getType()  const
  {
  QString mDataType = "HexaBrick12Element";
  return mDataType;
  }

QList<int> MHexaBrick12ElementDataImpl::getActiveDofs(int index)
  {
  MObjectManager manager = MObjectManager::getManager();
  MElementActiveDofs dof = manager.getObject(
      "org.sipesc.fems.data.elementactivedofs");

  return dof.getActiveDofs(MElementActiveDofs::MBrick);
  }

// =========================================================================
MHexaBrick12ElementDataFactoryImpl::MHexaBrick12ElementDataFactoryImpl()
  {
  }

MDataObject MHexaBrick12ElementDataFactoryImpl::createObject() const
  {
  return MDataObject(new MHexaBrick12ElementDataImpl);
  }

int MHexaBrick12ElementDataFactoryImpl::supportedType(
    const QUuid& typeId) const
  {
  bool is = (typeId == MHexaBrick12ElementDataUuid);
  return is ? 1 : 0;
  }
