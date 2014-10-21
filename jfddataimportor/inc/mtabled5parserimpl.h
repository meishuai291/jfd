/*
 * mtabled5parserimpl.h
 *
 *  Created on: 2013-9-8
 *      Author: cyz
 */

#ifndef MTABLED5PARSERIMPL_H_
#define MTABLED5PARSERIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.jfdimport.mtabled5parser.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;

class MTABLED5ParserImpl: public MTABLED5ParserInterface
{
public:
	MTABLED5ParserImpl();
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MTABLED5ParserImpl();

public:
    /**
     * 初始化:
     */
    bool initialize(MDataModel& model, bool isRepeated);

    ProgressMonitor getProgressMonitor() const;

    bool setData(const MDataObject& table);

	double getY(double x);

private:
  class Data;
  std::auto_ptr<Data> _data;
};



#endif /* MTABLED5PARSERIMPL_H_ */
