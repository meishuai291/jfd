/*
 * mbltdataexportorimpl.cxx
 *
 *  Created on: 2014年9月16日
 *      Author: sipesc
 */

#include <mbltdataexportorimpl.h>

#include <qdatetime.h>
#include <qdebug.h>
#include <qfile.h>
#include <qtextstream.h>

#include <merrorreport.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatamodel.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>

#include <mbltresultexportorimpl.h>

using namespace org::sipesc::core::engdbs::data;
using org::sipesc::core::utility::UtilityManager;

class MBltDataExportorImpl::Data
  {
  public:
    Data(){
    	_isInitialized = false;
    	_isRunning = false;
    	_isRepeated = false;
    	_objManager = MObjectManager::getManager();
    	Q_ASSERT(!_objManager.isNull());
    	_extManager = MExtensionManager::getManager();
    	Q_ASSERT(!_extManager.isNull());
      }
  public:
    ProgressMonitor _monitor;
    bool _isRepeated;

	MObjectManager _objManager;
	MDatabaseManager _baseManager;
	MExtensionManager _extManager;

	MDataModel _model;

    bool _isInitialized;
    bool _isRunning;
  };

MBltDataExportorImpl::MBltDataExportorImpl(){
  _data.reset(new MBltDataExportorImpl::Data);

  if (!_data.get())
    mReportError(M_ERROR_FATAL, "MBltDataExportorImpl::"
      "MBltDataExportorImpl() ****** failed");

	UtilityManager utilManager = _data->_objManager.getObject(
			"org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!utilManager.isNull());
	_data->_monitor = utilManager.createProgressMonitor("MBltDataExportor",
			ProgressMonitor());
    Q_ASSERT(!_data->_monitor.isNull());

	_data->_baseManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

}

bool MBltDataExportorImpl::initialize(MDataModel& model, bool isRepeated){
	if (_data->_isInitialized)
		return false; //不能重复初始化

	_data->_model = model;

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MBltDataExportorImpl::getProgressMonitor() const{
	return _data->_monitor;
 }

bool MBltDataExportorImpl::start(const QVariant& control){
	Q_ASSERT(_data->_isInitialized);

	QString fileName;
	if(control.isNull())
		fileName = "JFDData.out";
	else
		fileName = control.toString();

	QFile outFile;
	outFile.setFileName(fileName);
	if (!outFile.open(QIODevice::Text|QIODevice::WriteOnly))
	{
		QString message = QString("Can't open %1 file "
				"in MBltDataExportorImpl::start()!").arg(fileName);
		qDebug() << message;
		mReportError(M_ERROR_FATAL, message);
		_data->_monitor.setMessage(message);
		return false;
	}
	QTextStream outStream;
	outStream.setCodec("UTF-8");
	outStream.setDevice(&outFile); //定义文件的数据流

	//- 数据头 -------------------------------------------------------------------
	QDate d = QDate::currentDate();
	QTime t = QTime::currentTime();
	outStream << QString("\n         欢迎使用《开放式结构有限元分析系统SiPESC.FEMS》\n");
	outStream << QString("             研制单位 : 大连理工大学 工程力学系\n\n");
	outStream << QString("         计算日期 :         ")
			<< QString::number(d.year(), 10) << "           "
			<< QString::number(d.month(), 10) << "          "
			<< QString::number(d.day(), 10) << "\n";
	outStream << QString("         计算时间 :         ")
				<< QString::number(t.hour(), 10) << "           "
				<< QString::number(t.minute(), 10) << "          "
				<< QString::number(t.second(), 10) << "\n";
	outStream << QString("         输入文件 ：") << "\n";
	outStream << QString("         输出文件 ： ") << fileName << "\n";
	outStream << "\n\n\n";
	//- 数据头 -------------------------------------------------------------------


	MBltResultExportorImpl resEpt;
	resEpt.initialize(_data->_model);
	resEpt.dataExport(&outStream);

	outFile.close();

	return true;
}
