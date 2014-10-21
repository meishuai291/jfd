/*
 * mdataimportassistorimpl.cxx
 *
 *  Created on: 2013-10-21
 *      Author: luxuze
 */
#include <mdataimportassistorimpl.h>

#include <qdebug.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>

#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.utilities.mextensionfactory.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>

#include <org.sipesc.fems.bdfimport.mdataimportor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::utilities;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::bdfimport;

class MDataImportAssistorImpl::Data
{
public:
	Data()
	{
		_isInitialized = false;
	}
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MDataModel _model;

	bool _isInitialized;
};

MDataImportAssistorImpl::MDataImportAssistorImpl()
{
	_data.reset(new MDataImportAssistorImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MDataImportAssistorImpl::"
				"MDataImportAssistorImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject(
			"org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MDataImportAssistor",
			ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MDataImportAssistorImpl::~MDataImportAssistorImpl()
{
}

bool MDataImportAssistorImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MDataImportAssistorImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MDataImportAssistorImpl::importFile(const QString& fileName)
{
	QString file = fileName;
	qDebug() << file;
	QFileInfo fi(file);
	if (!fi.exists())
	{
		QString errorMessage = "Error:: " + file + " is not exist in MDataImportAssistorImpl::importFile() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
		exit(0);
	}

	QString suffix = fi.suffix(); //取文件后缀
	qDebug() << suffix;

	MExtensionManager extManager = MExtensionManager::getManager();
	Q_ASSERT(!extManager.isNull());
	QString name = "org.sipesc.utilities.MExtensionFactoryManager"; //创建数据工厂管理器
	MExtensionFactoryManager factoryManager = extManager.createExtension(name);
	Q_ASSERT(!factoryManager.isNull());
	bool ok = factoryManager.initialize("fems.factory.dataimportor");
	Q_ASSERT(ok);

	MExtensionFactory factory = factoryManager.getFactory(suffix);
	Q_ASSERT(!factory.isNull());
	MDataImportor importor = factory.createExtension();
	Q_ASSERT(!importor.isNull());
	ok = importor.initialize(_data->_model);
	Q_ASSERT(ok);

	importor.import(file);

	return true;
}

