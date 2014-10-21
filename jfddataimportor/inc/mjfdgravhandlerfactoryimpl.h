/*
 * mjfdgravhandlerfactoryimpl.h
 *
 *  Created on: 2013-1-23
 *      Author: cyz
 */

#ifndef MJFDGRAVHANDLERFACTORYIMPL_H_
#define MJFDGRAVHANDLERFACTORYIMPL_H_

#include <mjfdgravhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdgravhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdGravHandlerFactoryImpl: public MJfdGravHandlerFactoryInterface
{
public:
    MJfdGravHandlerFactoryImpl()
    {
    }
    /**
     * 析构函数. 仅为类继承关系而设置.
     */
    virtual ~MJfdGravHandlerFactoryImpl()
    {
    }

    QList<QVariant> getTypes() const
    {
        QList<QVariant> list;
        list << "GRAV";
        return list;
    }

    QString getDescription() const
    {
        return "standard factory for MJfdGravHandler";
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
        return MJfdGravHandler(new MJfdGravHandlerImpl);
    }
};

#endif /* MJFDGRAVHANDLERFACTORYIMPL_H_ */
