/*
 * mjfddataimportorimpl.h
 *
 *  Created on: 2013-1-8
 *      Author: cyz
 */

#ifndef MJFDDATAIMPORTORIMPL_H_
#define MJFDDATAIMPORTORIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfddataimportor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdDataImportorImpl: public MJfdDataImportorInterface
{
public:
	MJfdDataImportorImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdDataImportorImpl();
public:
	/**
	 * 初始化,
	 */
	bool initialize(MDataModel& model, bool isRepeated);
	/**
	 * 取进度监视对象
	 */
	ProgressMonitor getProgressMonitor() const;

	bool import(const QString& fileName);

private:
	bool importIn();
	bool initDatabase(MDataModel& model);

private:
  class Data;
  std::auto_ptr<Data> _data;

};

#endif /* MJFDDATAIMPORTORIMPL_H_ */
