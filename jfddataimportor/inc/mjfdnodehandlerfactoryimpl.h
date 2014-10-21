/*
 * mjfdnodehandlerfactoryimpl.h
 *
 *  Created on: 2013-1-17
 *      Author: cyz
 */

#ifndef MJFDNODEHANDLERFACTORYIMPL_H_
#define MJFDNODEHANDLERFACTORYIMPL_H_

#include <mjfdnodehandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdnodehandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdNodeHandlerFactoryImpl: public MJfdNodeHandlerFactoryInterface
{
public:
	MJfdNodeHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdNodeHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "NODE";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdNodeHandler";
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
		return MJfdNodeHandler(new MJfdNodeHandlerImpl);
	}
};

#endif /* MJFDNODEHANDLERFACTORYIMPL_H_ */
