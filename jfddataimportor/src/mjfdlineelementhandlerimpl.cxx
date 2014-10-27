/*
 * mjfdlineelementhandlerimpl.cxx
 *
 *  Created on: 2013-1-18
 *      Author: cyz
 */
#include <mjfdlineelementhandlerimpl.h>

#include <math.h>
#include <qdebug.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.jfdimport.mjfdsubentryhandler.h>
#include <org.sipesc.utilities.mextensionfactorymanager.h>

using namespace std;
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::jfdimport;
using namespace org::sipesc::utilities;

class MJfdLineElementHandlerImpl::Data
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
	MDataModel _model;

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流

	MExtensionFactoryManager _extFactoryManager;
	QMap<QString, MJfdSubEntryHandler> _handlerMap;

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled  LINEELEMENT's group = "+ QString::number(_handledLine);
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
			MExtensionFactory factory = _extFactoryManager.getFactory(key);
			if (!factory.isNull()) //判断工厂是否为空
			{
				handler = factory.createExtension();
				handler.initialize(_model);
				_handlerMap.insert(key, handler);
			}
			else
			{
				//				QString warningMessage = "Now, don't handle " + key + " Entry in MJfdLineElementHandlerImpl()";
				//				qDebug() << warningMessage;
				//				_monitor.setMessage(warningMessage);
			}
		}
		return handler;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdLineElementHandlerImpl::MJfdLineElementHandlerImpl()
{
	_data.reset(new MJfdLineElementHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdLineElementHandlerImpl::"
				"MJfdLineElementHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdLineElementHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdLineElementHandlerImpl::~MJfdLineElementHandlerImpl()
{
}

bool MJfdLineElementHandlerImpl::initialize(MDataModel& model, bool isRepeated)
{
	if (_data->_isInitialized)
		return false;
	_data->_model = model;

	MExtensionManager extManager = MExtensionManager::getManager();
	_data->_extFactoryManager = extManager.createExtension(
			"org.sipesc.utilities.MExtensionFactoryManager");
	Q_ASSERT(!_data->_extFactoryManager.isNull());
	_data->_extFactoryManager.initialize("fems.factory.jfdentryhandler");

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdLineElementHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
 3   9   0   0       0  12           4   4       1       1   1
 12.6504e+00
 3.4500e+07     0.200
 1    2    3    4    5    6    7    8
 1   12   12    1    1
 204   203   202   201   244   243   242   241
 0   224   223   222   221

 [1]
 *  |0~3|     :本组单元类型标志
 *  |4~7|     :本组单元总数
 *  |56~59|:本组材料类型标志
 *  |60~63|:本组材料类型总数

 [2]
 *  |0~4|   :本组材料号
 *  |5~14|:该材料密度

 [3]
 *  |0~9|     :杨式模量
 *  |10~19|:泊松比

 ......其他材料

 [4] 跳过，应力输出节点

 [5]
 *  |0~4|     :单元号
 *  |5~9|     :单元节点数
 *  |20~24|:材料号

 [6]
*  |0~5|     :node1
*  |6~9|     :node2
*  |12~17|:node3
*  |18~23|:node4
*  |24~29|:node5
*  |30~35|:node6
*  |36~41|:node7
*  |42~47|:node8

 [7]
*  |48~53|:node9
*  |54~59|:node10
*  |60~65|:node11
*  |66~71|:node12
 */
bool MJfdLineElementHandlerImpl::handleEntry(QTextStream* stream,
		QTextStream* logStream, const int& num)
{
	Q_ASSERT(_data->_isInitialized);

    if (num <= 0)
        return true;

	//1)显示开始读取NODE
	QString stateDes = "importing jfd file, handled  LINEELEMENT's group :: START ";
	qDebug() << stateDes;
	_data->_monitor.setMessage(stateDes);

	_data->_stream = stream;
	_data->_logStream = logStream;

	_data->_readLine();//去除空行

	for (int i = 0; i < num; i++)
	{
		/*
		 [1]第一行
		 *  |0~3|     :本组单元类型标志
		 *  |4~7|     :本组单元总数
		 *  |56~59|:本组材料类型标志
		 *  |60~63|:本组材料类型总数
		 *
		 *TODO 不同单元的“材料类型标志”不同，因此在此处理，不太恰当。
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
		if(nMat==0)
			nMat=1;

		int eleGroupId = 0;//单元组号
		QString str = line.mid(70);
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
