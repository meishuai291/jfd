/*
 * mjfddataimportorfactoryimpl.h
 *
 *  Created on: 2013-1-8
 *      Author: cyz
 */

#ifndef MJFDDATAIMPORTORFACTORYIMPL_H_
#define MJFDDATAIMPORTORFACTORYIMPL_H_

#include <mjfddataimportorimpl.h>
#include <org.sipesc.fems.jfdimport.mjfddataimportorfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdDataImportorFactoryImpl: public MJfdDataImportorFactoryInterface
{
public:
	MJfdDataImportorFactoryImpl()
	{
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MJfdDataImportorFactoryImpl()
	{
	}

	QList<QVariant> getTypes() const
	{
		QList<QVariant> list;
		list << "jfd";
		return list;
	}

	QString getDescription() const
	{
		return "standard factory for MJfdDataImportor";
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
		return MJfdDataImportor(new MJfdDataImportorImpl);
	}
};



#endif /* MJFDDATAIMPORTORFACTORYIMPL_H_ */
