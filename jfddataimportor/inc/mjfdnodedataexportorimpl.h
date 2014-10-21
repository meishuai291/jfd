/*
 * mjfdnodedataexportorimpl.h
 *
 *  Created on: 2013-8-2
 *      Author: cyz
 */

#ifndef MJFDNODEDATAEXPORTORIMPL_H_
#define MJFDNODEDATAEXPORTORIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdnodedataexportor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MJfdNodeDataExportorImpl: public MJfdNodeDataExportorInterface
{
public:
	MJfdNodeDataExportorImpl();

	virtual ~MJfdNodeDataExportorImpl();
public:

	bool initialize(MDataModel& model, bool isRepeated);

	ProgressMonitor getProgressMonitor() const;

	bool dataExport(QTextStream* stream, const QString& name);

private:
  class Data;
  std::auto_ptr<Data> _data;
};

#endif /* MJFDNODEDATAEXPORTORIMPL_H_ */
