#include <melementdataimpl.h>

MElementDataImpl::MElementDataImpl()
  {
  }

MElementDataImpl::~MElementDataImpl()
  {
  }

int MElementDataImpl::getNodeId(int index) const
  {
  Q_ASSERT_X(index >= 0 && index < _number, "MElementDataImpl::getNodeId()",
      "out of range");
  return _nodeId[index];
  }

void MElementDataImpl::setNodeId(int index, const int& value)
  {
  Q_ASSERT_X(index >= 0 && index < _number, "MElementDataImpl::setNodeId()",
      "out of range");
  _nodeId[index] = value;
  return;
  }

bool MElementDataImpl::read(QDataStream* stream)
  {
  int id;
  *stream >> id;
  this->setId(id);
  for (int i = 0; i < _number; i++)
    *stream >> _nodeId[i];
  *stream >> _propertyId;
  return true;
  }

bool MElementDataImpl::write(QDataStream* stream) const
  {
  *stream << this->getId();
  for (int i = 0; i < _number; i++)
    *stream << _nodeId[i];
  *stream << _propertyId;
  return true;
  }
