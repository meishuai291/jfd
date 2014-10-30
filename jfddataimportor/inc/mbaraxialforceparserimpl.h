/*
 * mbaraxialforceparserimpl.h
 *
 *  Created on: 2014年10月29日
 *      Author: sipesc
 */

#ifndef MBARAXIALFORCEPARSERIMPL_H_
#define MBARAXIALFORCEPARSERIMPL_H_

#include <org.sipesc.utilities.mtask.h>
#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdataobject.h>
using namespace org::sipesc::core::engdbs::data;

class MBarAxialForceParserImpl : public org::sipesc::utilities::MTaskInterface{
public:
	MBarAxialForceParserImpl();
	~MBarAxialForceParserImpl();

public:
	bool initialize(MDataModel& model, bool isRepeated=false);
	org::sipesc::core::utility::ProgressMonitor getProgressMonitor() const;

public:
    double getAxialForce(const MDataObject&);
    double getSecArea(const MDataObject&);
    double getLength(const MDataObject&);
private:
	class Data;
	std::auto_ptr<Data> _data;
};

#endif /* MBARAXIALFORCEPARSERIMPL_H_ */
