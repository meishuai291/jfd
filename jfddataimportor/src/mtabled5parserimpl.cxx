/*
 * mtabled5parserimpl.cxx
 *
 *  Created on: 2013-9-8
 *      Author: cyz
 */

#include <mtabled5parserimpl.h>

#include <cmath>
#include <qdebug.h>
#include <qvector2d.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.fems.data.mtabledata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;

class MTABLED5ParserImpl::Data
{
public:
	Data()
	{
		_isInitialized = false;
		_isSetData = false;
	}
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;

	MDataModel _model;
	MTableData _tableData;

	QString _para1, _para2;
	double _minX, _maxX;

	bool _isInitialized;
	bool _isSetData;

	/*
	 * 各种插值计算
	 */
	double linelineinterpolation(double x1, double y1, double x2, double y2,
			double x)
	{
		return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
	}

	double loglineinterpolation(double x1, double y1, double x2, double y2,
			double x)
	{
		return log(x2 / x) / log(x2 / x1) * y1 + log(x / x1) / log(x2 / x1)
				* y2;
	}

	double lineloginterpolation(double x1, double y1, double x2, double y2,
			double x)
	{
		return exp(
				(x2 - x) / (x2 - x1) * log(y1) + (x - x1) / (x2 - x1) * log(y2));
	}

	double logloginterpolation(double x1, double y1, double x2, double y2,
			double x)
	{
		return exp(
				log(x2 / x) / log(x2 / x1) * y1 + log(x / x1) / log(x2 / x1)
						* y2);
	}

	double findInterpolation(double x1, double y1, double x2, double y2,
			double x)
	{
		double y;
		if (_para1 == "LINEAR" && _para2 == "LINEAR")
			y = linelineinterpolation(x1, y1, x2, y2, x);
		else if (_para1 == "LOG" && _para2 == "LINEAR")
			y = loglineinterpolation(x1, y1, x2, y2, x);
		else if (_para1 == "LINEAR" && _para2 == "LOG")
			y = lineloginterpolation(x1, y1, x2, y2, x);
		else if (_para1 == "LOG" && _para2 == "LOG")
			y = logloginterpolation(x1, y1, x2, y2, x);

		return y;
	}
};

MTABLED5ParserImpl::MTABLED5ParserImpl()
{
	_data.reset(new MTABLED5ParserImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MTABLED5ParserImpl::"
				"MTABLED5ParserImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MTABLED5Parser", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MTABLED5ParserImpl::~MTABLED5ParserImpl()
{
}

bool MTABLED5ParserImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MTABLED5ParserImpl::getProgressMonitor() const
{
	return _data->_monitor;
}

bool MTABLED5ParserImpl::setData(const MDataObject& tableData)
{
	if (_data->_isSetData)
		_data->_isSetData = false;

	_data->_tableData = tableData;
	Q_ASSERT(!_data->_tableData.isNull());

	_data->_para1 = _data->_tableData.getPara(0).toString();
	_data->_para2 = _data->_tableData.getPara(1).toString();

	if (_data->_para1 == QString())
		_data->_para1 = "LINEAR";

	if (_data->_para2 == QString())
		_data->_para2 = "LINEAR";


	int cnt = _data->_tableData.getCount();
	QVector2D minPoint = _data->_tableData.getOrderPoint(0);
	QVector2D maxPoint = _data->_tableData.getOrderPoint(cnt-1);

	_data->_minX = minPoint.x();
	_data->_maxX = maxPoint.x();


	_data->_isSetData = true;
	return true;
}

double MTABLED5ParserImpl::getY(double x)
{
	if (x < _data->_minX || x > _data->_maxX)//如果超出范围，值为0
		return 0.0;

	int index = _data->_tableData.getOrderIndex(x);//获取x在表中区间的下限插值点index号

	QVector2D p1,p2;
	p1 = _data->_tableData.getOrderPoint(index);
	p2 = _data->_tableData.getOrderPoint(index+1);

	double x1, y1, x2, y2;
	x1 = p1.x();
	y1 = p1.y();
	x2 = p2.x();
	y2 =  p2.y();

	return _data->findInterpolation(x1, y1, x2, y2, x);
}
