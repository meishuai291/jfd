/*
 * mjfddataexportorimpl.h
 *
 *  Created on: 2013-8-2
 *      Author: cyz
 */

#ifndef MJFDDATAEXPORTORIMPL_H_
#define MJFDDATAEXPORTORIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfddataexportor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdDataExportorImpl: public MJfdDataExportorInterface
{
public:
	MJfdDataExportorImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdDataExportorImpl();
public:
	/**
	 * 初始化,
	 */
	bool initialize(MDataModel& model, bool isRepeated);
	/**
	 * 取进度监视对象
	 */
	ProgressMonitor getProgressMonitor() const;

	bool start(const QVariant& control);

private:
  class Data;
  std::auto_ptr<Data> _data;

};

#endif /* MJFDDATAEXPORTORIMPL_H_ */
