/*
 * mjfdforcehandlerimpl.cxx
 *
 *  Created on: 2013-1-23
 *      Author: cyz
 */
#include <mjfdforcehandlerimpl.h>

#include <cmath>
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.core.engdbs.data.mbytearray.h>
#include <org.sipesc.fems.data.mnodeloaddata.h>
#include <org.sipesc.fems.data.mpropertydata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::data;

class MJfdForceHandlerImpl::Data
{
public:
    Data()
    {
        _handledLine = 0;
        _isInitialized = false;
    }
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;
	MDatabaseManager _baseManager;
	MDataFactoryManager _factoryManager;

    MDataModel _model;
    MDataModel _loadPath;
    MDataManager _times;
    MDataManager _loadTimeRef;

    QTextStream *_stream; //导入文件数据流
    QTextStream *_logStream; //log文件数据流

    QMap<int, MDataManager> _loadManagerMap;

    QString _readLine() //读取一行，并显示状态，以及log写入
    {
        QString line = _stream->readLine();
        *_logStream << "READ: " << line << endl;

        _handledLine++;
        if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
        {
            QString stateDes = "importing jfd file, handled line of FORCE ="
                    + QString::number(_handledLine);
            qDebug() << stateDes;
            _monitor.setMessage(stateDes);
        }

        return line;
    }

    MDataManager _getManager(int num)
    {
		MDataManager nodeLoadManager = _baseManager.createDataManager();
		if (_loadManagerMap.contains(num))
			nodeLoadManager = _loadManagerMap[num];
		else
		{
			bool ok = nodeLoadManager.open(_loadPath, QString::number(num));
			Q_ASSERT(ok);
			_loadManagerMap.insert(num,nodeLoadManager);
		}

		return nodeLoadManager;
    }

    int _handledLine;
    bool _isInitialized;
};

MJfdForceHandlerImpl::MJfdForceHandlerImpl()
{
    _data.reset(new MJfdForceHandlerImpl::Data);

    if (!_data.get())
        mReportError(M_ERROR_FATAL, "MJfdForceHandlerImpl::"
                "MJfdForceHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdForceHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdForceHandlerImpl::~MJfdForceHandlerImpl()
{
}

bool MJfdForceHandlerImpl::initialize(MDataModel& model, bool isRepeated)
{
    if (_data->_isInitialized)
        return false;
    _data->_model = model;

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());

	_data->_baseManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_baseManager.isNull());

	_data->_factoryManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatafactorymanager");
	Q_ASSERT(!_data->_factoryManager.isNull());

	_data->_loadPath = _data->_baseManager.createDataModel();
    bool ok = _data->_loadPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::LoadPath));
    if (!ok)
    {
        QString errorMessage =
                "can't open MFEMS::LoadPath DataModel in MJfdForceHandlerImpl::initialize() ";
        qDebug() << errorMessage;
        _data->_monitor.setMessage(errorMessage);
        return false;
    }

	_data->_times = _data->_baseManager.createDataManager();
	ok = _data->_times.open(_data->_model, "Times");
	if (!ok)
	{
		QString errorMessage =
				"can't open Times DataManager in MJfdForceHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_loadTimeRef = _data->_baseManager.createDataManager();
	ok = _data->_loadTimeRef.open(_data->_model, "LoadTimeRef");
	if (!ok)
	{
		QString errorMessage =
				"can't open LoadTimeMap DataManager in MJfdForceHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

    _data->_isInitialized = true;
    return true;
}

ProgressMonitor MJfdForceHandlerImpl::getProgressMonitor() const
{
    return _data->_monitor;
}
/*
 26437    3    1      -8.0       1.9

 [1]
 *  |0~5|    :节点号
 *  |6~10|  :dof
 *  |16~25|:载荷
 */
bool MJfdForceHandlerImpl::handleEntry(QTextStream* stream,
        QTextStream* logStream, const int& num)
{
    Q_ASSERT(_data->_isInitialized);

    if (num <= 0)
        return true;

    //1)显示开始读取NODE
    QString stateDes = "importing jfd file, handled line of FORCE :: START ";
    qDebug() << stateDes;
    _data->_monitor.setMessage(stateDes);

    _data->_stream = stream;
    _data->_logStream = logStream;

    int loadCnt = _data->_loadPath.getTypeCount(MDatabaseGlobal::ManagerType);
	QString dataType = "org.sipesc.fems.data.nodeloaddata";
	MDataFactory factory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!factory.isNull());

    double preTime = 0.0;
    for (int i = 0; i < num; i++)
    {
        QString line = _data->_readLine(); //读取一行

        int nodeId = line.mid(0, 6).toInt(); //|0~5|    :节点号
        int dof = line.mid(6, 5).toInt() - 1; // |6~10|  :dof
        int timeId = line.mid(11, 5).toInt(); // |11~15|  :时间曲线Id
        double f = line.mid(16, 10).toDouble(); // |16~25|:载荷
        double time = line.mid(26, 10).toDouble(); // |26~35|:时间

		if (fabs(time - preTime) > 1e-6) //如果不是一个时间段
		{
			preTime = time;
			loadCnt++;
		}

		MDataManager loadManager = _data->_getManager(loadCnt);

        MNodeLoadData nodeLoad = factory.createObject();
        nodeLoad.setId(loadManager.getDataCount()+1);
        nodeLoad.setNodeId(nodeId);
        nodeLoad.setCoord(0);
        nodeLoad.setLoadValue(dof, f);
        loadManager.appendData(nodeLoad);
    }

    return true;
}
