/*
 * mhexabrick12shpfunctionimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12SHPFUNCTIONIMPL_H_
#define MHEXABRICK12SHPFUNCTIONIMPL_H_

#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12shpfunction.h>
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12ShpFunctionImpl : public MHexaBrick12ShpFunctionInterface
  {
  public:
    MHexaBrick12ShpFunctionImpl();
    /**
     * 析构函数，仅为类继承关系而设置。
     */
    ~MHexaBrick12ShpFunctionImpl();
    /*
     * to get shape function
     *
     * @param paraCoord data which stores parameter coordinate of integral point
     * @ return shape function matrix
     */
    MMatrix getShpFunction(const QList<double>& paraCoord,
        const QVector<MNodeData>& nodeLocalCoord = QVector<MNodeData>()) const;
    /*
     * to get shape function derivative matrix
     *
     * @param paraCoord data which stores parameter coordinate of integral point
     * @return shape function derivative matrix
     */
    MMatrix getShpFuncDerivative(const QList<double>& paraCoord,
        const QVector<MNodeData>& nodeLocalCoord = QVector<MNodeData>()) const;

  private:
    MMatrixFactory _mFactory;
  };



#endif /* MHEXABRICK12SHPFUNCTIONIMPL_H_ */
