/*
 * mjfdgravhandlerimpl.cxx
 *
 *  Created on: 2013-1-23
 *      Author: cyz
 */
#include <mjfdgravhandlerimpl.h>

#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.data.meleaccloaddata.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::data;

class MJfdGravHandlerImpl::Data
{
public:
    Data()
    {
        _handledLine = 0;
        _isInitialized = false;
        _stream = 0;
        _logStream = 0;
    }
public:
	MObjectManager _objManager;
	ProgressMonitor _monitor;

	MFemsGlobal _femsGlobal;
	MDatabaseManager _baseManager;
	MDataFactoryManager _factoryManager;

    MDataModel _model;
    MDataModel _loadPath;
    MDataManager _gravLoad;

    QTextStream *_stream; //导入文件数据流
    QTextStream *_logStream; //log文件数据流

    QString _readLine() //读取一行，并显示状态，以及log写入
    {
        QString line = _stream->readLine();
        *_logStream << "READ: " << line << endl;

        _handledLine++;
        if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
        {
            QString stateDes = "importing jfd file, handled line of GRAV ="
                    + QString::number(_handledLine);
            qDebug() << stateDes;
            _monitor.setMessage(stateDes);
        }

        return line;
    }

    int _handledLine;
    bool _isInitialized;
};

MJfdGravHandlerImpl::MJfdGravHandlerImpl()
{
    _data.reset(new MJfdGravHandlerImpl::Data);

    if (!_data.get())
        mReportError(M_ERROR_FATAL, "MJfdGravHandlerImpl::"
                "MJfdGravHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdGravHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdGravHandlerImpl::~MJfdGravHandlerImpl()
{
}

bool MJfdGravHandlerImpl::initialize(MDataModel& model, bool isRepeated)
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
                "can't open MFEMS::LoadPath DataModel in MJfdGravHandlerImpl::initialize() ";
        qDebug() << errorMessage;
        _data->_monitor.setMessage(errorMessage);
        return false;
    }

	_data->_gravLoad = _data->_baseManager.createDataManager();
    ok = _data->_gravLoad.open(_data->_loadPath, "0");
    if (!ok)
    {
        QString errorMessage =
                "can't open MCDBS::nodeLoad DataManagerl in MJfdGravHandlerImpl::initialize() ";
        qDebug() << errorMessage;
        _data->_monitor.setMessage(errorMessage);
        return false;
    }

    _data->_isInitialized = true;
    return true;
}

ProgressMonitor MJfdGravHandlerImpl::getProgressMonitor() const
{
    return _data->_monitor;
}
/*
 1                        -9.810       1.0

 [1]
 *  |5~14|  :x方向系数
 *  |15~24|:y方向系数
 *  |25~34|:z方向系数
 *  |35~44|:总系数
 */
bool MJfdGravHandlerImpl::handleEntry(QTextStream* stream,
        QTextStream* logStream, const int& num)
{
    Q_ASSERT(_data->_isInitialized);

    if (num <= 0)
        return true;

    //1)显示开始读取NODE
    QString stateDes = "importing jfd file, handled line of GRAV :: START ";
    qDebug() << stateDes;
    _data->_monitor.setMessage(stateDes);

    _data->_stream = stream;
    _data ->_logStream = logStream;

    QString line = _data->_readLine(); //读取一行
    double x = line.mid(5, 10).toDouble(); //|5~14|  :x方向系数
    double y = line.mid(15, 10).toDouble(); // |15~24|:y方向系数
    double z = line.mid(25, 10).toDouble(); //|25~34|:z方向系数
    double a = line.mid(35, 10).toDouble(); //|35~44|:总系数

	QString dataType = "org.sipesc.fems.data.eleaccloaddata";
	MDataFactory factory = _data->_factoryManager.getFactory(dataType);
	Q_ASSERT(!factory.isNull());

    MEleAccLoadData loadData = factory.createObject();
    loadData.setId(1);
    loadData.setCoord(0);
    loadData.setLoadValue(0, x*a);
    loadData.setLoadValue(1, y*a);
    loadData.setLoadValue(2, z*a);

    _data->_gravLoad.appendData(loadData);

    return true;
}
