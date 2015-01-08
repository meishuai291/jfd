/*
 * mhexabrick12elevolumeimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12ELEVOLUMEIMPL_H_
#define MHEXABRICK12ELEVOLUMEIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12elevolume.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12EleVolumeImpl : public MHexaBrick12EleVolumeInterface
  {
  public:
    MHexaBrick12EleVolumeImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12EleVolumeImpl();
    /**
     * initialize Task object with given parent object, provider and receiver
     *
     * @param model object of MDataModel
     * @return if succeed, return true, otherwise return false
     */
    bool initialize(MDataModel& model, bool isRepeated);
    /**
     * 取进度监视对象
     */
    ProgressMonitor getProgressMonitor() const;
    /**
     * to set element node coordinate
     *
     * @param eleData data which stores element node information
     */
    void setState(const MDataObject& eleData);
    /**
     * to get jacobi determinant value
     *
     * @return jacobi determinant value
     */
    double getEleVolume(const MIntegPoint& integ, const int& index) const;

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };



#endif /* MHEXABRICK12ELEVOLUMEIMPL_H_ */
