/*
 * mjfdlineelementhandlerimpl.h
 *
 *  Created on: 2013-1-18
 *      Author: cyz
 */

#ifndef MJFDLINEELEMENTHANDLERIMPL_H_
#define MJFDLINEELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdlineelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdLineElementHandlerImpl: public MJfdLineElementHandlerInterface
{
public:
	MJfdLineElementHandlerImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdLineElementHandlerImpl();

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

#endif /* MJFDLINEELEMENTHANDLERIMPL_H_ */
