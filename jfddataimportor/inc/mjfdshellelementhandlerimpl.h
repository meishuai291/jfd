/*
 * mjfdshellelementhandlerimpl.h
 *
 *  Created on: 2014-10-22
 *      Author: whb
 */

#ifndef MJFDSHELLELEMENTHANDLERIMPL_H_
#define MJFDSHELLELEMENTHANDLERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mjfdshellelementhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;
class QTextStream;

class MJfdShellElementHandlerImpl: public MJfdShellElementHandlerInterface {
public:
	MJfdShellElementHandlerImpl();
	virtual ~MJfdShellElementHandlerImpl();

public:
	bool initialize(MDataModel& model, bool isRepeated);
	ProgressMonitor getProgressMonitor() const;
	bool handleEntry(QTextStream* stream, QTextStream* logStream,
			const QVector<QVariant>& value);

private:
	class Data;
	std::auto_ptr<Data> _data;
};

#endif /* MJFDSHELLELEMENTHANDLERIMPL_H_ */
