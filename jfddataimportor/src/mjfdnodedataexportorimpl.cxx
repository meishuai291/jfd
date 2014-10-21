/*
 * mjfdnodedataexportorimpl.cxx
 *
 *  Created on: 2013-8-2
 *      Author: cyz
 */
#include <mjfdnodedataexportorimpl.h>
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.fems.data.mnodedata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::data;

class MJfdNodeDataExportorImpl::Data
{
public:
	Data()
	{
		_isInitialized = false;
	}
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;
	MDatabaseManager _baseManager;

	MDataModel _model;
	MDataManager _node;

	bool _isRepeated;
	bool _isInitialized;
};

MJfdNodeDataExportorImpl::MJfdNodeDataExportorImpl()
{
	_data.reset(new MJfdNodeDataExportorImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdNodeDataExportorImpl::"
				"MJfdNodeDataExportorImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdNodeDataExportor", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdNodeDataExportorImpl::~MJfdNodeDataExportorImpl()
{
}

bool MJfdNodeDataExportorImpl::initialize(MDataModel& model,
		bool isRepeated)
{
	if (_data->_isInitialized)
		return false; //不能重复初始化
	_data->_model = model;

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());

	_data->_node = _data->_baseManager.createDataManager();
	bool ok = _data->_node.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Node));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Node DataManager "
				" in MJfdNodeDataExportorImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdNodeDataExportorImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MJfdNodeDataExportorImpl::dataExport(QTextStream* stream,
		const QString& name)
{
	int ncnt = _data->_node.getDataCount();
	for (int i = 0; i < ncnt; i++)
	{
		MNodeData node = _data->_node.getDataAt(i);
		int id = node.getId();
		int coord = node.getCoord();
		double x = node.getX();
		double y = node.getY();
		double z = node.getZ();

		*stream << id << "    " << x << "    "<< y << "    "<< z << "    \n" ;
	}

	return true;
}

