/*
 * mjfdshellelementhandlerfactoryimpl.h
 *
 *  Created on: 2014-10-22
 *      Author: whb
 */

#ifndef MJFDSHELLELEMENTHANDLERFACTORYIMPL_H_
#define MJFDSHELLELEMENTHANDLERFACTORYIMPL_H_

#include <mjfdshellelementhandlerimpl.h>
#include <org.sipesc.fems.jfdimport.mjfdshellelementhandlerfactory.h>
using namespace org::sipesc::fems::jfdimport;

class MJfdShellElementHandlerFactoryImpl: public MJfdShellElementHandlerFactoryInterface {
public:
	MJfdShellElementHandlerFactoryImpl() {
	}
	virtual ~MJfdShellElementHandlerFactoryImpl() {
	}

	QList<QVariant> getTypes() const {
		QList<QVariant> list;
		list << "7";
		return list;
	}

	QString getDescription() const {
		return "standard factory for MJfdShellElementHandler";
	}

	QString getIcon() const {
		return QString();
	}

	/*
	 * 返回优先级
	 */
	unsigned int getPriority(const QVariant& key) const {
		return 10;
	}
	/**
	 * 创建任务对象
	 */
	MExtension createExtension(const QVariant& key) const {
		return MJfdShellElementHandler(new MJfdShellElementHandlerImpl);
	}
};

#endif /* MJFDSHELLELEMENTHANDLERFACTORYIMPL_H_ */
