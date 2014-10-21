/*
 * mtransdataimportimpl.h
 *
 *  Created on: 2013年11月27日
 *      Author: sipesc
 */

#ifndef MTRANSDATAIMPORTIMPL_H_
#define MTRANSDATAIMPORTIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.importorassist.mtransdataimport.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::importorassist;

class MTransDataImportImpl: public MTransDataImportInterface
{
public:
	MTransDataImportImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MTransDataImportImpl();
public:
	/**
	 * 初始化,
	 */
	bool initialize(MDataModel& model, bool isRepeated);
	/**
	 * 取进度监视对象
	 */
	ProgressMonitor getProgressMonitor() const;

	bool start(const QVariant& control=QVariant());

private:
	class Data;
	std::auto_ptr<Data> _data;
};

#endif /* MTRANSDATAIMPORTIMPL_H_ */
