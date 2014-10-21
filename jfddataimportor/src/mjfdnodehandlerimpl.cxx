/*
 * mjfdnodehandlerimpl.cxx
 *
 *  Created on: 2013-1-16
 *      Author: cyz
 */
#include <mjfdnodehandlerimpl.h>

#include <math.h>
#include <qdebug.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.core.engdbs.data.mbytearray.h>
#include <org.sipesc.core.engdbs.data.mdatamanager.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.mjfxdofdata.h>
#include <org.sipesc.fems.data.mtabledata.h>

#include <org.sipesc.fems.global.mfemsglobal.h>

using namespace std;
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;

class MJfdNodeHandlerImpl::Data
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

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流

	MDataManager _nodes;
	MDataManager _JIFEX;
	MDataManager _nodeDofMap;
	MDataModel _nodeDofDataPath;

	QMap<int, int> _dofMap; //存储节点id及其对应的JIFEX数据id

	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled line of NODE =" + QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}

		return line;
	}

	bool _handleDof(int nodeId, QVector<int> dofs)
	{
		//1) 将节点6个自由度的约束标志转换为JIFEX的id
		Q_ASSERT(dofs.count() == 6);
		//必须是6个自由度
		int jifexId = 0;
		for (int i = 0; i < 6; i++)
			jifexId += dofs[i] * pow(2, 5 - i);
		jifexId += 1;

//		qDebug() << jifexId;
		//2) 判断该Id是否已存在，如果不存在创建
		//只要不是约束的都设为自由

		QString dataType = "org.sipesc.fems.data.jfxdofdata";
		MDataFactory factory = _factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MJfxDofData JIFEX = _JIFEX.getData(jifexId);
		if (JIFEX.isNull())
		{
			JIFEX = factory.createObject();
			JIFEX.setId(jifexId);
			JIFEX.setCoord(0);
			for (int i = 0; i < 6; i++)
			{
				if (dofs[i] == 1) //约束
					JIFEX.setDof(i, 3);
				else
					JIFEX.setDof(i, 1);
			}
			_JIFEX.appendData(JIFEX);
		}

		//3) 处理dofMap
		_dofMap.insert(nodeId, jifexId);

		return true;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdNodeHandlerImpl::MJfdNodeHandlerImpl()
{
	_data.reset(new MJfdNodeHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdNodeHandlerImpl::"
				"MJfdNodeHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdNodeHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());

}

MJfdNodeHandlerImpl::~MJfdNodeHandlerImpl()
{
}

bool MJfdNodeHandlerImpl::initialize(MDataModel& model, bool isRepeated)
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

	_data->_nodes = _data->_baseManager.createDataManager();
	bool ok = _data->_nodes.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::Node));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::Node DataManager "
				"                 in MJfdNodeHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_nodeDofDataPath = _data->_baseManager.createDataModel();
	ok = _data->_nodeDofDataPath.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::NodeDofPath));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::NodeDofPath DataModel "
				"in MJfdNodeHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_JIFEX = _data->_baseManager.createDataManager();
	ok = _data->_JIFEX.open(_data->_nodeDofDataPath, _data->_femsGlobal.getValue(MFemsGlobal::JfxDof));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::JfxDof DataManager "
				" in MJfdNodeHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_nodeDofMap = _data->_baseManager.createDataManager();
	ok = _data->_nodeDofMap.open(_data->_model, _data->_femsGlobal.getValue(MFemsGlobal::NodeDofMap));
	if (!ok)
	{
		QString errorMessage = "can't open MFEMS::NodeDofMap DataManager "
				" in MJfdNodeHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdNodeHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
 1    0    0    0    1    1    1   -10.000     7.550   100.000
 *  |0~5|    :节点号
 *  |6~10|  :x自由度标志， 0：自由； 1：约束； -2：从自由度，当作自由处理
 *  |11~15|:y自由度标志
 *  |16~20|:z自由度标志
 *  |21~25|:rotx自由度标志
 *  |26~30|:roty自由度标志
 *  |31~35|:rotz自由度标志
 *  |36~45|：x坐标
 *  |46~55|：y坐标
 *  |56~65|：z坐标
 *  |71~75|：斜坐标系标号
 */
bool MJfdNodeHandlerImpl::handleEntry(QTextStream* stream, QTextStream* logStream, const int& num)
{
	Q_ASSERT(_data->_isInitialized);

	if (num <= 0)
		return true;

	//1)显示开始读取NODE
	QString stateDes = "importing jfd file, handled line of NODE :: START ";
	qDebug() << stateDes;
	_data->_monitor.setMessage(stateDes);

	_data->_stream = stream;
	_data->_logStream = logStream;

	for (int i = 0; i < num; i++)
	{
		QString line = _data->_readLine();
		int nodeId = line.mid(0, 6).toInt(); //节点号

		//节点自由度
		QVector<int> dofs;
		for (int k = 0; k < 6; k++)
		{
			int dof = line.mid(6 + 5 * k, 5).toInt();
			if (dof != 0 && dof != 1) //0：自由； 1：约束； -2：从自由度，当作自由处理
				dof = 0;
			dofs.append(dof);
		}

//		qDebug() << dofs;

		//处理节点自由度
		_data->_handleDof(nodeId, dofs);

		QString dataType = "org.sipesc.fems.data.nodedata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MNodeData Node= factory.createObject();
		Node.setId(nodeId); //以节点号作为数据的id
		Node.setCoord(0); //存入节点的坐标系号,默认为0
		Node.setX(line.mid(36, 10).toDouble()); //存入节点的X坐标
		Node.setY(line.mid(46, 10).toDouble()); //存入节点的Y坐标
		Node.setZ(line.mid(56, 10).toDouble()); //存入节点的Z坐标
		_data->_nodes.appendData(Node);
	}

	MByteArray ba = _data->_baseManager.createByteArray();
	QDataStream *inStream = ba.getWriteStream();
	*inStream << _data->_dofMap;

	_data->_nodeDofMap.appendData(ba);

	return true;
}
