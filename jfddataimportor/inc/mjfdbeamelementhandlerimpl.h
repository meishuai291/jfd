/*
 * mjfdbeamelementhandlerimpl.h
 *
 *  Created on: 2014年12月1日
 *      Author: sipesc
 */

#ifndef MJFDBEAMELEMENTHANDLERIMPL_H_
#define MJFDBEAMELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdbeamelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdBeamElementHandlerImpl: public MJfdBeamElementHandlerInterface
{
public:
	MJfdBeamElementHandlerImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdBeamElementHandlerImpl();

public:
    /**
     * 初始化:
     */
    bool initialize(MDataModel& model, bool isRepeated);

    ProgressMonitor getProgressMonitor() const;

    bool handleEntry(QTextStream* stream, QTextStream* logStream, const QVector<QVariant>& value);

private:
  class Data;
  std::auto_ptr<Data> _data;
};

#endif /* MJFDBEAMELEMENTHANDLERIMPL_H_ */
