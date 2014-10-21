/*
 * mjfdmpchandlerimpl.cxx
 *
 *  Created on: 2013-1-17
 *      Author: cyz
 */
#include <mjfdmpchandlerimpl.h>

#include <math.h>
#include <qdebug.h>

#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>

#include <org.sipesc.fems.global.mfemsglobal.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.data.mcoupledequationdata.h>
#include <org.sipesc.utilities.msharedvariablesmanager.h>

using namespace std;
using namespace org::sipesc::core::utility;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::utilities;

class MJfdMPCHandlerImpl::Data
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
	MDataManager _mpcLoad;

	QTextStream *_stream; //导入文件数据流
	QTextStream *_logStream; //log文件数据流


	QString _readLine() //读取一行，并显示状态，以及log写入
	{
		QString line = _stream->readLine();
		*_logStream << "READ: " << line << endl;

		_handledLine++;
		if ((_handledLine % 100) == 0 && _handledLine > 0) //读满百行即更新状态
		{
			QString stateDes = "importing jfd file, handled line of MPC ="
					+ QString::number(_handledLine);
			qDebug() << stateDes;
			_monitor.setMessage(stateDes);
		}

		return line;
	}

	int _handledLine;
	bool _isInitialized;
};

MJfdMPCHandlerImpl::MJfdMPCHandlerImpl()
{
	_data.reset(new MJfdMPCHandlerImpl::Data);

	if (!_data.get())
		mReportError(M_ERROR_FATAL, "MJfdMPCHandlerImpl::"
				"MJfdMPCHandlerImpl() ****** failed");

	_data->_objManager = MObjectManager::getManager();
	UtilityManager util = _data->_objManager.getObject("org.sipesc.core.utility.utilitymanager");
	Q_ASSERT(!util.isNull());
	_data->_monitor = util.createProgressMonitor("MJfdMPCHandler", ProgressMonitor());
	Q_ASSERT(!_data->_monitor.isNull());
}

MJfdMPCHandlerImpl::~MJfdMPCHandlerImpl()
{
}

bool MJfdMPCHandlerImpl::initialize(MDataModel& model, bool isRepeated)
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

	_data->_mpcLoad = _data->_baseManager.createDataManager();
	bool ok = _data->_mpcLoad.open(_data->_model, "MPC");
	if (!ok)
	{
		QString errorMessage = "can't open MPC MDataManager "
			" in MMPCHandlerImpl::initialize() ";
		qDebug() << errorMessage;
		_data->_monitor.setMessage(errorMessage);
		return false;
	}

	MExtensionManager extManager = MExtensionManager::getManager();
	MSharedVariablesManager sharedVariables = extManager.createExtension("org.sipesc.utilities.MSharedVariablesManager");
	Q_ASSERT(!sharedVariables.isNull());
	sharedVariables.initialize(_data->_model);
	sharedVariables.setVariable(_data->_femsGlobal.getValue(MFemsGlobal::HasMPC),true);

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MJfdMPCHandlerImpl::getProgressMonitor() const
{
	return _data->_monitor;
}
/*
[1]    396 14499     3    4
[2] 14213  3     0.135 14216  3     0.135 14215  3     0.302 14209  3     0.427

[1]
  *  |0~5|    :约束方程Id
 *  |6~11|  :从节点Id
 *  |12~17|:从节点自由度
 *  |18~22|:主节点个数

[2]
 *  |0~5|  :主节点1的Id
 *  |6~8|  :主节点1的自由度
 *  |9~18|:系数
 *  ...：其他主节点
 */
bool MJfdMPCHandlerImpl::handleEntry(QTextStream* stream,
		QTextStream* logStream, const int& num)
{
	Q_ASSERT(_data->_isInitialized);

    if (num <= 0)
        return true;

	//1)显示开始读取NODE
	QString stateDes = "importing jfd file, handled line of MPC :: START ";
	qDebug() << stateDes;
	_data->_monitor.setMessage(stateDes);

	_data->_stream = stream;
	_data->_logStream = logStream;

	//读取模块第一行：MPC总个数，与num相等
	QString line = _data->_readLine();

	for (int i = 0; i < num; i++)
	{
		QString line = _data->_readLine();//读取一行
		int mpcId = 	line.mid(0, 6).toInt(); //mpc号
		QVector<int> nodes;//存放约束方程的节点Id
		QVector<int> dofs;//存放约束方程节点自由度
		QVector<double> ratios;//存放约束方程节点系数

		nodes.append(line.mid(6,6).toInt());//从节点id
		dofs.append(line.mid(12,6).toInt()-1);//从节点dof
		ratios.append(-1);//从节点系数

		int count = line.mid(18,5).toInt();//主节点个数
		int nLines = (count+3)/4;//每行最多设置4主节点，nlines是count个主节点需要的行数
		int nValues = (count%4 == 0)? 4:count%4;//最后一行的主节点数

		for (int j = 0; j < nLines-1; j++)//除了最后一行，其余全是4个主节点
		{
			line = _data->_readLine();//读取一行
			for (int k = 0; k < 4; k++ )
			{
				nodes.append(line.mid(0+19*k,6).toInt());//主节点1的id
				dofs.append(line.mid(6+19*k,3).toInt()-1);//从节点dof
				ratios.append(line.mid(9+19*k,10).toDouble());//从节点系数
			}
		}
		line = _data->_readLine();//读取最后一行
		for (int k = 0; k < nValues; k++ )
		{
			nodes.append(line.mid(0+19*k,6).toInt());//主节点1的id
			dofs.append(line.mid(6+19*k,3).toInt()-1);//从节点dof
			ratios.append(line.mid(9+19*k,10).toDouble());//从节点系数
		}

		QString dataType = "org.sipesc.fems.data.coupledequationdata";
		MDataFactory factory = _data->_factoryManager.getFactory(dataType);
		Q_ASSERT(!factory.isNull());

		MCoupledEquationData load = factory.createObject();
		load.setId(mpcId);
		load.setConstValue(0.0); //耦合方程常数项为0
		load.setItemCount(nodes.count());

		for (int i = 0; i < nodes.count(); i++)
		{
			load.setCoupledNodeId(i, nodes[i]);
			load.setCoupledDof(i, dofs[i]);
			load.setCoupledCoefficient(i, ratios[i]);
		}

		_data->_mpcLoad.appendData(load);
	}

	return true;
}
