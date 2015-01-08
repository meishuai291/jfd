/*
 * mhexabrick12lumpedmassmatriximpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12LUMPEDMASSMATRIXIMPL_H_
#define MHEXABRICK12LUMPEDMASSMATRIXIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
#include <org.sipesc.fems.mass.melementmass.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12lumpedmassmatrix.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::mass;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12LumpedMassMatrixImpl : public MHexaBrick12LumpedMassMatrixInterface
  {
  public:
    MHexaBrick12LumpedMassMatrixImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12LumpedMassMatrixImpl();

    bool initialize(MDataModel& model, bool isRepeated);
    /**
     * 取进度监视对象
     */
    ProgressMonitor getProgressMonitor() const;
    /**
     * to set element mass parameter
     *
     * @param eleData data which stores element mass information
     */
    void setState(const MDataObject& eleData);
    /**
     * to get shape function
     *
     * @param paraCoord data which stores parameter coordinate of integral point
     * @return shape function matrix
     */
    MMatrix getMassMatrix(const MElementMass& eleMass) const;

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };



#endif /* MHEXABRICK12LUMPEDMASSMATRIXIMPL_H_ */
