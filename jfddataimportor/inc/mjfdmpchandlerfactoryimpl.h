/*
 * mjfdmpchandlerfactoryimpl.h
 *
 *  Created on: 2013-1-17
 *      Author: cyz
 */

#ifndef MJFDMPCHANDLERFACTORYIMPL_H_
#define MJFDMPCHANDLERFACTORYIMPL_H_

#include <mjfdmpchandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdmpchandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdMPCHandlerFactoryImpl: public MJfdMPCHandlerFactoryInterface
{
public:
	MJfdMPCHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdMPCHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "MPC";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdMPCHandler";
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
		return MJfdMPCHandler(new MJfdMPCHandlerImpl);
	}
};

#endif /* MJFDMPCHANDLERFACTORYIMPL_H_ */
