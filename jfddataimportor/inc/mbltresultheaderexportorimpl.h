/*
 * mbltresultheaderexportorimpl.h
 *
 *  Created on: 2014年9月19日
 *      Author: sipesc
 */

#ifndef MBLTRESULTHEADEREXPORTORIMPL_H_
#define MBLTRESULTHEADEREXPORTORIMPL_H_

#include <memory>
#include <qtextstream.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;

class MBltResultHeaderExportorImpl
{
public:
	MBltResultHeaderExportorImpl();

	virtual ~MBltResultHeaderExportorImpl();
public:

	bool initialize(MDataModel& model, bool isRepeated=false);

	ProgressMonitor getProgressMonitor() const;
	bool dataExport(QTextStream* stream, QString fileName);

private:
  class Data;
  std::auto_ptr<Data> _data;
};

#endif /* MBLTRESULTHEADEREXPORTORIMPL_H_ */
