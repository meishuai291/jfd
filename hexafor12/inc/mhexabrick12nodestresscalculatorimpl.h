/*
 * mhexabrick12nodestresscalculatorimpl.h
 *
 *  Created on: 2014年10月11日
 *      Author: Zhoudw
 */

#ifndef MHEXABRICK12NODESTRESSCALCULATORIMPL_H_
#define MHEXABRICK12NODESTRESSCALCULATORIMPL_H_

#include <memory>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
#include <org.sipesc.fems.hexaelement12.mhexabrick12nodestresscalculator.h>
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::hexaelement12;

class MHexaBrick12NodeStressCalculatorImpl : public MHexaBrick12NodeStressCalculatorInterface
  {
  public:
    MHexaBrick12NodeStressCalculatorImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MHexaBrick12NodeStressCalculatorImpl();
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
    MDataObject getNodeStress(const MDataObject& integStressData);

  private:
    class Data;
    std::auto_ptr<Data> _data;
  };



#endif /* MHEXABRICK12NODESTRESSCALCULATORIMPL_H_ */
