/*
 * mbltdataexportorimpl.h
 *
 *  Created on: 2014年9月16日
 *      Author: sipesc
 */

#ifndef MBLTDATAEXPORTORIMPL_H_
#define MBLTDATAEXPORTORIMPL_H_

#include <org.sipesc.fems.bltexport.mbltdataexportor.h>

#include <memory>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>

using namespace org::sipesc::fems::bltexport;
using org::sipesc::core::engdbs::data::MDataModel;
using org::sipesc::core::utility::ProgressMonitor;

class MBltDataExportorImpl: public MBltDataExportorInterface
{
public:
	MBltDataExportorImpl();
	virtual ~MBltDataExportorImpl(){}

public:
	bool initialize(MDataModel& model, bool isRepeated);
	ProgressMonitor getProgressMonitor() const;
	bool start(const QVariant& control=QVariant());

private:
	class Data;
	std::auto_ptr<Data> _data;
};

#endif /* MBLTDATAEXPORTORIMPL_H_ */
