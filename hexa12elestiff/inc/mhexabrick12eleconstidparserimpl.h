/*
 * mhexabrick12eleconstidparserimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELECONSTIDPARSERIMPL_H_
#define MHEXABRICK12ELECONSTIDPARSERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12eleconstidparser.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleConstiDParserImpl : public MHexaBrick12EleConstiDParserInterface
  {
  public:
    MHexaBrick12EleConstiDParserImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleConstiDParserImpl();

    bool initialize(MDataModel& model, bool isRepeated);
    /**
     * 取进度监视对象
     */
    ProgressMonitor getProgressMonitor() const;
    /**
     * set element data
     *
     * @param eleData dof data of current node
     */
    void setState(const MDataObject& eleData);
    /**
     * if element constitutive matrix is constant matrix, return true; otherwise return false
     */
    bool isConst() const;
    /**
     * to get element constitutive matrix
     *
     * @param integ global coordinate
     * @return element constitutive matrix
     */
    MMatrix getConstiDMatrix(const MIntegPoint& integ = MIntegPoint());

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };




#endif /* MHEXABRICK12ELECONSTIDPARSERIMPL_H_ */
