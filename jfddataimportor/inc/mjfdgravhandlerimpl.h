/*
 * mjfdgravhandlerimpl.h
 *
 *  Created on: 2013-1-23
 *      Author: cyz
 */

#ifndef MJFDGRAVHANDLERIMPL_H_
#define MJFDGRAVHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdgravhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdGravHandlerImpl: public MJfdGravHandlerInterface
{
public:
    MJfdGravHandlerImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MJfdGravHandlerImpl();

public:
    /**
     * 初始化:
     */
    bool initialize(MDataModel& model, bool isRepeated);

    ProgressMonitor getProgressMonitor() const;

    bool handleEntry(QTextStream* stream, QTextStream* logStream, const int& num);

private:
  class Data;
  std::auto_ptr<Data> _data;
};

#endif /* MJFDGRAVHANDLERIMPL_H_ */
