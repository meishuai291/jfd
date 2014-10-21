/*
 * mvectorassistorimpl.h
 *
 *  Created on: 2013-10-22
 *      Author: luxuze
 */

#ifndef MVECTORASSISTORIMPL_H_
#define MVECTORASSISTORIMPL_H_

#include <qdebug.h>
#include <qvector.h>
#include <mobjectmanager.h>
#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <org.sipesc.fems.matrixassist.mvectorassistor.h>

using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::matrixassist;

class MVectorAssistorImpl: public MVectorAssistorInterface
{
public:
  MVectorAssistorImpl()
  {
  }
  /**
   * 析构函数. 仅为类继承关系而设置.
   */
  virtual ~MVectorAssistorImpl()
  {
  }

public:

  MVector trans(MVectorData data)
  {
    MObjectManager objManager = MObjectManager::getManager();
    MVectorFactory vFactory = objManager.getObject("org.sipesc.fems.matrix.vectorfactory");
    Q_ASSERT(!vFactory.isNull());

    MVector v = vFactory.createVector();
    v << data;

    return v;
  }

};

#endif /* MVECTORASSISTORIMPL_H_ */
