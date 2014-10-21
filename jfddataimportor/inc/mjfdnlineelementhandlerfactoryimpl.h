/*
 * mjfdnlineelementhandlerfactoryimpl.h
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */

#ifndef MJFDNLINEELEMENTHANDLERFACTORYIMPL_H_
#define MJFDNLINEELEMENTHANDLERFACTORYIMPL_H_

#include <mjfdnlineelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdnlineelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdNLineElementHandlerFactoryImpl: public MJfdNLineElementHandlerFactoryInterface
{
public:
	MJfdNLineElementHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdNLineElementHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "NLINEELEMENT";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdNLineElementHandler";
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
		return MJfdNLineElementHandler(new MJfdNLineElementHandlerImpl);
	}
};

#endif /* MJFDNLINEELEMENTHANDLERFACTORYIMPL_H_ */
