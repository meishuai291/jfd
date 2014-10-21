/*
 * mjfd3dsolidelementhandlerfactoryimpl.h
 *
 *  Created on: 2013-1-19
 *      Author: cyz
 */

#ifndef MJFD3DSOLIDELEMENTHANDLERFACTORYIMPL_H_
#define MJFD3DSOLIDELEMENTHANDLERFACTORYIMPL_H_

#include <mjfd3dsolidelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfd3dsolidelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfd3DSolidElementHandlerFactoryImpl: public MJfd3DSolidElementHandlerFactoryInterface
{
public:
	MJfd3DSolidElementHandlerFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfd3DSolidElementHandlerFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "3";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfd3DSolidElementHandler";
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
		return MJfd3DSolidElementHandler(new MJfd3DSolidElementHandlerImpl);
	}
};

#endif /* MJFD3DSOLIDELEMENTHANDLERFACTORYIMPL_H_ */
