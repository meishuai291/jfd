/*
 * mjfdcrodelementhandlerimpl.h
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */

#ifndef MJFDCRODELEMENTHANDLERIMPL_H_
#define MJFDCRODELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdcrodelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdCrodElementHandlerImpl: public MJfdCrodElementHandlerInterface
{
public:
	MJfdCrodElementHandlerImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdCrodElementHandlerImpl();

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

#endif /* MJFDCRODELEMENTHANDLERIMPL_H_ */
