/*
 * mjfdcrodelementhandlerfactoryimpl.h
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */

#ifndef MJFDCRODELEMENTHANDLERFACTORYIMPL_H_
#define MJFDCRODELEMENTHANDLERFACTORYIMPL_H_

#include <mjfdcrodelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdcrodelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdCrodElementHandlerFactoryImpl: public MJfdCrodElementHandlerFactoryInterface
{
public:
	MJfdCrodElementHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdCrodElementHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "1";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdCrodElementHandler";
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
		return MJfdCrodElementHandler(new MJfdCrodElementHandlerImpl);
	}
};

#endif /* MJFDCRODELEMENTHANDLERFACTORYIMPL_H_ */
