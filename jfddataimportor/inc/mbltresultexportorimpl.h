/*
 * mbltresultexportorimpl.h
 *
 *  Created on: 2014年9月19日
 *      Author: sipesc
 */

#ifndef MBLTRESULTEXPORTORIMPL_H_
#define MBLTRESULTEXPORTORIMPL_H_

#include <memory>
#include <qtextstream.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;

class MBltResultExportorImpl
{
public:
	MBltResultExportorImpl();

	virtual ~MBltResultExportorImpl();
public:

	bool initialize(MDataModel& model, bool isRepeated);

	ProgressMonitor getProgressMonitor() const;

	bool dataExport(QTextStream* stream, const QString& name);

private:
  class Data;
  std::auto_ptr<Data> _data;
};



#endif /* MBLTRESULTEXPORTORIMPL_H_ */
