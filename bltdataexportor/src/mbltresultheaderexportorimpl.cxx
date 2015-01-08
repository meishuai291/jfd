/*
 * mbltresultheaderexportorimpl.cxx
 *
 *  Created on: 2014年9月19日
 *      Author: sipesc
 */
#include <format.h>

#include <mbltresultheaderexportorimpl.h>

#include <qtextstream.h>
#include <qdatetime.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
using namespace org::sipesc::core::utility;

class MBltResultHeaderExportorImpl::Data {
public:
	Data() {
		_isInitialized = false;
		_objManager = MObjectManager::getManager();
//		_extManager = MExtensionManager::getManager();
	}
public:
	MObjectManager _objManager;
//	MExtensionManager _extManager;
	ProgressMonitor _monitor;

	bool _isInitialized;
};

MBltResultHeaderExportorImpl::MBltResultHeaderExportorImpl() {
	_data.reset(new MBltResultHeaderExportorImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MBltResultHeaderExportorImpl::"
				"MBltResultHeaderExportorImpl() ****** failed");

	UtilityManager util = _data->_objManager.getObject(
			"org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MBltResultHeaderExportor",
			ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MBltResultHeaderExportorImpl::~MBltResultHeaderExportorImpl() {
}

bool MBltResultHeaderExportorImpl::initialize(MDataModel& model,
		bool isRepeated) {
	if (_data->_isInitialized)
		return false; //不能重复初始化

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MBltResultHeaderExportorImpl::getProgressMonitor() const {
	return _data->_monitor;
}

bool MBltResultHeaderExportorImpl::dataExport(QTextStream* stream,
		QString fileName) {

	QDate d = QDate::currentDate();
	QTime t = QTime::currentTime();
	(*stream) << QString("\n         欢迎使用《开放式结构有限元分析系统SiPESC.FEMS》\n");
	(*stream) << QString("             研制单位 : 大连理工大学 工程力学系\n\n");
	(*stream) << QString("         计算日期 :         ")
			<< QString::number(d.year(), 10) << "           "
			<< QString::number(d.month(), 10) << "          "
			<< QString::number(d.day(), 10) << "\n";
	(*stream) << QString("         计算时间 :         ")
				<< QString::number(t.hour(), 10) << "           "
				<< QString::number(t.minute(), 10) << "          "
				<< QString::number(t.second(), 10) << "\n";
	(*stream) << QString("         输入文件 ：") << "\n";
	(*stream) << QString("         输出文件 ： ") << fileName << "\n";
	(*stream) << "\n\n\n";

	return true;
}
