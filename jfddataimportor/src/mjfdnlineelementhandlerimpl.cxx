/*
 * mjfdnlineelementhandlerimpl.cxx
 *
 *  Created on: 2013-1-21
 *      Author: cyz
 */
#include <mjfdnlineelementhandlerimpl.h>

#include <math.h>
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.fems.global.mfemsglobal.h>

#include <org.sipesc.utilities.mextensionfactorymanager.h>
#include <org.sipesc.fems.jfdimport.mjfdsubentryhandler.h>

using namespace org::sipesc::core::utility;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::jfdimport;

class MJfdNLineElementHandlerImpl::Data
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
	MDataModel _model;

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流

	MExtensionFactoryManager _factoryManager;
	QMap<QString, MJfdSubEntryHandler> _handlerMap;

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled  NLINEELEMENT's group = "+ QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}
		return line;
	}

	MJfdSubEntryHandler _findHandler(QString key)
	{
		MJfdSubEntryHandler handler;
		if (_handlerMap.contains(key))
			handler = _handlerMap[key];
		else
		{
			MExtensionFactory factory = _factoryManager.getFactory(key);
			if (!factory.isNull()) //判断工厂是否为空
			{
				handler = factory.createExtension();
				handler.initialize(_model);
				_handlerMap.insert(key, handler);
			}
			else
			{
				//				QString warningMessage = "Now, don't handle " + key + " Entry in MJfdNLineElementHandlerImpl()";
				//				qDebug() << warningMessage;
				//				_monitor.setMessage(warningMessage);
			}
		}
		return handler;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdNLineElementHandlerImpl::MJfdNLineElementHandlerImpl()
{
	_data.reset(new MJfdNLineElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdNLineElementHandlerImpl::"
				"MJfdNLineElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdNLineElementHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdNLineElementHandlerImpl::~MJfdNLineElementHandlerImpl()
{
}

bool MJfdNLineElementHandlerImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	MExtensionManager extManager = MExtensionManager::getManager();
	_data->_factoryManager = extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_factoryManager.isNull());
	_data->_factoryManager.initialize("fems.factory.jfdentryhandler");

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdNLineElementHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
   1   1   2       0                           0   0       1   1
    11.6820e-038.0020e+00
2.0000e+08
    1    0    1    1    00.00321596
 14809 14810

 [1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数
 *  |56~59|:本组材料类型标志
 *  |60~63|:本组材料类型总数

 [2]
 *  |0~4|      :材料号
 *  |5~14|   :横截面积
 *  |15~24|:该材料密度

 [3]
 *  |0~9|     :杨式模量

 ......其他材料

 [4]
 *  |0~4|     :单元号
 *  |10~14|:材料号

 [6]
 *  |0~5|     :node1
 *  |6~11|  :node2
 */
bool MJfdNLineElementHandlerImpl::handleEntry(QTextStream* stream,
		QTextStream* logStream, const int& num)
{
	Q_ASSERT(_data->_isInitialized);

    if (num <= 0)
        return true;

	//1)显示开始读取NODE
	QString stateDes = "importing jfd file, handled  NLINEELEMENT's group :: START ";
	qDebug() << stateDes;
	_data->_monitor.setMessage(stateDes);

	_data->_stream = stream;
	_data->_logStream = logStream;

	for (int i = 0; i < num; i++)
	{
		/*
		 [1]第一行
		 *  |0~3|     :本组单元类型标志
		 *  |4~7|     :本组单元总数
		 *  |56~59|:本组材料类型标志
		 *  |60~63|:本组材料类型总数
		 */
		QString line = _data->_readLine();

		QString key = line.mid(0, 4).simplified(); //|0~3|     :本组单元类型标志
		int nEle = line.mid(4, 4).toInt(); //|4~7|     :本组单元总数
		int mat = line.mid(56, 4).toInt(); //|56~59|:本组材料类型标志

		QString matType = "SolidIsotropy";
		if (mat == 0 || mat == 1)
			matType == "SolidIsotropy";
		else if (mat == 2)
			matType == "SolidOrthotropy";

		int nMat = line.mid(60, 4).toInt(); // |60~63|:本组材料类型总数

		int eleGroupId = 0;//单元组号
		QString str = line.mid(80);
		QRegExp rx("\\d+\\(");
		int pos = rx.indexIn(str);
		if (pos > -1)
		{
			QString s = rx.cap().remove("(");
			eleGroupId = s.toInt();
		}


		QVector<QVariant> value;
		value.append(nEle);//本组单元数
		value.append(matType);//本组材料类型
		value.append(nMat);//本组材料种类数
		value.append(eleGroupId);//本组单元组号

		MJfdSubEntryHandler handler = _data->_findHandler(key);
		handler.handleEntry(_data->_stream,_data->_logStream,value);
	}

	return true;
}
