/*
 * mhexabrick12elestressstiffgmatriximpl.h
 *
 *  Created on: 2014年12月24日
 *      Author: jhello
 */

#ifndef MHEXABRICK12ELESTRESSSTIFFGMATRIXIMPL_H_
#define MHEXABRICK12ELESTRESSSTIFFGMATRIXIMPL_H_

#include <memory>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexa12geostress.mhexabrick12elestressstiffgmatrix.h>
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexa12geostress;

class MHexaBrick12EleStressStiffGMatrixImpl: public MHexaBrick12EleStressStiffGMatrixInterface
  {
  public:
    MHexaBrick12EleStressStiffGMatrixImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleStressStiffGMatrixImpl();
    /**
     * to set element node coordinate
     *
     * @param localData element local coordinate and nodes data
     */
    void setState(const MDataObject& localData);
    /**
     * get G Matrix in Stress Stiff Matrix
     *
     * @param integ data which stores parameter coordinate of integral point
     * @return G matrix
     */
    MMatrix getStressStiffGMatrix(const MIntegPoint& integ) const;
    /**
     * to get jacobi determinant value
     * @return jaccobi determinant value
     */
    double getJacobiValue() const;

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };

#endif /* MHEXABRICK12ELESTRESSSTIFFGMATRIXIMPL_H_ */
