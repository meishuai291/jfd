/*
 * mdataimportassistorimpl.h
 *
 *  Created on: 2013-10-21
 *      Author: luxuze
 */

#ifndef MDATAIMPORTASSISTORIMPL_H_
#define MDATAIMPORTASSISTORIMPL_H_

#include <memory>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.fems.importorassist.mdataimportassistor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::importorassist;

class MDataImportAssistorImpl: public MDataImportAssistorInterface
    {
public:
    MDataImportAssistorImpl();
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MDataImportAssistorImpl();
public:
    /**
     * 初始化,
     */
    bool initialize(MDataModel& model, bool isRepeated);
    /**
     * 取进度监视对象
     */
    ProgressMonitor getProgressMonitor() const;

    bool importFile(const QString& fileName);

private:
    class Data;
    std::auto_ptr<Data> _data;
    };
#endif /* MDATAIMPORTASSISTORIMPL_H_ */
