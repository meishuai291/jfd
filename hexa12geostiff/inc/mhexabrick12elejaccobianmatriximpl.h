/*
 * mhexabrick12elejaccobianmatriximpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12ELEJACCOBIANMATRIXIMPL_H_
#define MHEXABRICK12ELEJACCOBIANMATRIXIMPL_H_

#include <memory>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12elejaccobianmatrix.h>
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexa12geostress;

class MHexaBrick12EleJaccobianMatrixImpl: public MHexaBrick12EleJaccobianMatrixInterface
  {
  public:
    MHexaBrick12EleJaccobianMatrixImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleJaccobianMatrixImpl();
    /**
     * to set element node coordinate
     *
     * @param localData local coordinate matrix
     */
    void setState(const MDataObject& localData);
    /**
     * get the jaccobianMatrix
     *
     * @param integ integration point
     * @return Jaccobian matrix
     */
    MMatrix getJaccobianMatrix(const MIntegPoint& integ);

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };

#endif /* MHEXABRICK12ELEJACCOBIANMATRIXIMPL_H_ */
