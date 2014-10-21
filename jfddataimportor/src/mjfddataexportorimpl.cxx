/*
 * mjfddataexportorimpl.cxx
 *
 *  Created on: 2013-8-2
 *      Author: cyz
 */
#include <mjfddataexportorimpl.h>
#include <qfile.h>
#include <qdebug.h>
#include <qtextstream.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.fems.jfdimport.mjfdnodedataexportor.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::fems::jfdimport;

class MJfdDataExportorImpl::Data
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

MJfdDataExportorImpl::MJfdDataExportorImpl()
{
	_data.reset(new MJfdDataExportorImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdDataExportorImpl::"
				"MJfdDataExportorImpl ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdDataExportor", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdDataExportorImpl::~MJfdDataExportorImpl()
{

}

bool MJfdDataExportorImpl::initialize(MDataModel& model, bool isRepeated)
{
	_data->_isInitialized = true;
	_data->_model = model;

	return true;
}

ProgressMonitor MJfdDataExportorImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MJfdDataExportorImpl::start(const QVariant& control)
{
	Q_ASSERT(_data->_isInitialized);

	QString fileName = "JFDData.out";
	QFile outFile;
	outFile.setFileName(fileName);
	if (!outFile.open(QIODevice::WriteOnly))
	{
		QString message = "Can't open JFDData.out file in MJfdDataExportorImpl::start()!";
		qDebug() << message;
		_data->_monitor.setMessage(message);
		return false;
	}
	QTextStream outStream;
	outStream.setDevice(&outFile); //定义文件的数据流

	//节点输出
	MExtensionManager extManager = MExtensionManager::getManager();
	MJfdNodeDataExportor nodeExportor = extManager.createExtension("org.sipesc.fems.jfdimport.MJfdNodeDataExportor");
	Q_ASSERT(!nodeExportor.isNull());
	bool ok = nodeExportor.initialize(_data->_model );
	Q_ASSERT(ok);
	nodeExportor.dataExport(&outStream, QString());


//	//节点输出
//	MJfdElementDataExportor eleExportor = extManager.createExtension("org.sipesc.fems.jfd.MJfdElementDataExportor");
//	bool ok = eleExportor.initialize(_data->_model );
//	Q_ASSERT(ok);
//	nodeExportor.eleExportor(&outStream, QString());

	return true;
}
