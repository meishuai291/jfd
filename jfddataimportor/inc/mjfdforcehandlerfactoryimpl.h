/*
 * mjfdforcehandlerfactoryimpl.h
 *
 *  Created on: 2013-1-23
 *      Author: cyz
 */

#ifndef MJFDFORCEHANDLERFACTORYIMPL_H_
#define MJFDFORCEHANDLERFACTORYIMPL_H_

#include <mjfdforcehandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdforcehandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdForceHandlerFactoryImpl: public MJfdForceHandlerFactoryInterface
{
public:
    MJfdForceHandlerFactoryImpl()
    {
    }
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MJfdForceHandlerFactoryImpl()
    {
    }

    QList<QVariant> getTypes() const
    {
        QList<QVariant> list;
        list << "FORCE";
        return list;
    }

    QString getDescription() const
    {
        return "standard factory for MJfdForceHandler";
    }

    QString getIcon() const
    {
        return QString();
    }

    /*
     * 返回优先级
     */
    unsigned int getPriority(const QVariant& key) const
    {
        return 10;
    }
    /**
     * 创建任务对象
     */
    MExtension createExtension(const QVariant& key) const
    {
        return MJfdForceHandler(new MJfdForceHandlerImpl);
    }
};

#endif /* MJFDFORCEHANDLERFACTORYIMPL_H_ */
