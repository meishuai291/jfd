/*
 * mjfd3dsolidelementhandlerimpl.h
 *
 *  Created on: 2013-1-19
 *      Author: cyz
 */

#ifndef MJFD3DSOLIDELEMENTHANDLERIMPL_H_
#define MJFD3DSOLIDELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfd3dsolidelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfd3DSolidElementHandlerImpl: public MJfd3DSolidElementHandlerInterface
{
public:
	MJfd3DSolidElementHandlerImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfd3DSolidElementHandlerImpl();

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

#endif /* MJFD3DSOLIDELEMENTHANDLERIMPL_H_ */
