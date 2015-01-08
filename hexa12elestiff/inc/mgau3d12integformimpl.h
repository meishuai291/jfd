/*
 * mgau3d12integformimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MGAU3D12INTEGFORMIMPL_H_
#define MGAU3D12INTEGFORMIMPL_H_

#include <memory>
#include <org.sipesc.fems.femsutils.mintegpoint.h>
#include <org.sipesc.fems.hexaelement12.mgau3d12integform.h>
using namespace org::sipesc::fems::femsutils;
using namespace org::sipesc::fems::hexaelement12;

class MGau3D12IntegFormImpl : public MGau3D12IntegFormInterface

  {
  public:
	 MGau3D12IntegFormImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MGau3D12IntegFormImpl();
    /**
     * to get integral point count
     *
     * @return integral point count
     */
    int getCount() const;
    /**
     * to get coordinate and weight value for integral point
     *
     * @param index integral point index
     * @return integral point data object
     */
    MIntegPoint getIntegPoint(const int& index) const;

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };



#endif /* MGAU3D12INTEGFORMIMPL_H_ */
