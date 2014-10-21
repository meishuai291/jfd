/*
 * mjfdlineelementhandlerfactoryimpl.h
 *
 *  Created on: 2013-1-18
 *      Author: cyz
 */

#ifndef MJFDLINEELEMENTHANDLERFACTORYIMPL_H_
#define MJFDLINEELEMENTHANDLERFACTORYIMPL_H_

#include <mjfdlineelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdlineelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdLineElementHandlerFactoryImpl: public MJfdLineElementHandlerFactoryInterface
{
public:
	MJfdLineElementHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdLineElementHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "LINEELEMENT";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdLineElementHandler";
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
		return MJfdLineElementHandler(new MJfdLineElementHandlerImpl);
	}
};

#endif /* MJFDLINEELEMENTHANDLERFACTORYIMPL_H_ */
