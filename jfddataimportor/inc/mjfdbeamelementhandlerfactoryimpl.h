/*
 * mjfdbeamelementhandlerfactoryimpl.h
 *
 *  Created on: 2014年12月1日
 *      Author: sipesc
 */

#ifndef MJFDBEAMELEMENTHANDLERFACTORYIMPL_H_
#define MJFDBEAMELEMENTHANDLERFACTORYIMPL_H_

#include <mjfdbeamelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdbeamelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdBeamElementHandlerFactoryImpl: public MJfdBeamElementHandlerFactoryInterface
{
public:
	MJfdBeamElementHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdBeamElementHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "4";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdBeamElementHandler";
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
		return MJfdBeamElementHandler(new MJfdBeamElementHandlerImpl);
	}
};



#endif /* MJFDBEAMELEMENTHANDLERFACTORYIMPL_H_ */
