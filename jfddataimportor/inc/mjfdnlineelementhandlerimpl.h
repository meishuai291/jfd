/*
 * mjfdnlineelementhandlerimpl.h
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */

#ifndef MJFDNLINEELEMENTHANDLERIMPL_H_
#define MJFDNLINEELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdnlineelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdNLineElementHandlerImpl: public MJfdNLineElementHandlerInterface
{
public:
	MJfdNLineElementHandlerImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdNLineElementHandlerImpl();

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

#endif /* MJFDNLINEELEMENTHANDLERIMPL_H_ */
