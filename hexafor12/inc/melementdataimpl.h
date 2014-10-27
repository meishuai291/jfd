#ifndef MELEMENTDATAIMPL_H
#define MELEMENTDATAIMPL_H

#include <org.sipesc.fems.data.melementdata.h>
using namespace org::sipesc::fems::data;

class MElementDataImpl : public MElementDataInterface
  {
  public:
    MElementDataImpl();
    virtual ~MElementDataImpl();

  public:
    inline int getNodeCount() const;
    inline int getPropertyId() const;
    inline void setPropertyId(const int&);

  public:
    int getNodeId(int) const;
    void setNodeId(int, const int&);

  public:
    inline qint64 getSize() const;
    bool read(QDataStream*);
    bool write(QDataStream*) const;

  protected:
    int _propertyId;
    int _number;
    int* _nodeId;
  };

int MElementDataImpl::getNodeCount() const
  {
  return _number;
  }

int MElementDataImpl::getPropertyId() const
  {
  return _propertyId;
  }

void MElementDataImpl::setPropertyId(const int& value)
  {
  _propertyId = value;
  return;
  }

qint64 MElementDataImpl::getSize() const
  {
  return sizeof(int)*2 + sizeof(int) * _number;
  }

#endif /* MELEMENTDATAIMPL_H */
