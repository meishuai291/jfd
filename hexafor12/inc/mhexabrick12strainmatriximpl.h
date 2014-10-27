/*
 * mhexabrick12strainmatriximpl.h
 *
 *  Created on: 2014年10月10日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12STRAINMATRIXIMPL_H_
#define MHEXABRICK12STRAINMATRIXIMPL_H_

#include <memory>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12strainmatrix.h>
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12StrainMatrixImpl : public MHexaBrick12StrainMatrixInterface
  {
  public:
    MHexaBrick12StrainMatrixImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12StrainMatrixImpl();

    /**
     * to set element node coordinate
     *
     * @param eleData data which stores element node information
     */
    void setState(const MDataObject& localData);
    /**
     * to get strain matrix
     *
     * @param integ data which stores parameter coordinate of integral point
     * @return strain matrix
     */
    MMatrix getStrainMatrix(const MIntegPoint& integ);
    /**
     * to get jacobi determinant value
     *
     * @return jacobi determinant value
     */
    double getJacobiValue() const;
    /**
     * to transfer local to global coordinate
     *
     * @param integ data which stores parameter coordinate of integral point
     * @return global coordinate
     */
    QList<double> coordTrans(const MIntegPoint& integ) const;

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };



#endif /* MHEXABRICK12STRAINMATRIXIMPL_H_ */
