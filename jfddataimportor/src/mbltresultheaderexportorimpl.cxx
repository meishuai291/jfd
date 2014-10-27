/*
 * mbltresultheaderexportorimpl.cxx
 *
 *  Created on: 2014年9月19日
 *      Author: sipesc
 */

#include <mbltresultheaderexportorimpl.h>
#include <org.sipesc.fems.matrix.mvectorfactory.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <qdebug.h>
#include <qmath.h>
#include <qtextstream.h>
#include <qdatetime.h>
#include <qiodevice.h>
#include <qvector.h>
#include <mobjectmanager.h>
#include <org.sipesc.core.engdbs.data.mdatabasemanager.h>
#include <org.sipesc.core.engdbs.data.mdataobjectlist.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.matrixassist.mvectorassistor.h>
#include <org.sipesc.fems.data.mnodedata.h>
#include <org.sipesc.fems.data.mpropertydata.h>
#include <org.sipesc.fems.data.melementdata.h>
#include <org.sipesc.fems.matrix.mvectordata.h>
#include <org.sipesc.fems.matrix.mvector.h>
#include <mextensionmanager.h>
#include <org.sipesc.core.utility.utilitymanager.h>
#include <org.sipesc.core.utility.progressmonitor.h>
#include <org.sipesc.utilities.msharedvariablesmanager.h>
#include <org.sipesc.fems.global.mfemsglobal.h>
#include <QDate>
#include <iostream>
using namespace std;
using namespace org::sipesc::fems::data;
using namespace org::sipesc::core::utility;
using namespace org::sipesc::utilities;
using namespace org::sipesc::fems::global;
using namespace org::sipesc::core::engdbs::data;
using namespace org::sipesc::fems::matrixassist;
using namespace org::sipesc::fems::matrix;
class MBltResultHeaderExportorImpl::Data {
public:
	Data() {
		_isInitialized = false;
		_isRepeated = false;
		_elegroupcnt = 0;
		_objManager = MObjectManager::getManager();
		_extManager = MExtensionManager::getManager();
	}
public:
	MObjectManager _objManager;
	MExtensionManager _extManager;
	ProgressMonitor _monitor;
	MDatabaseManager _dbManager;
	MFemsGlobal _femsGlobal;
	bool _isRepeated;
	bool _isInitialized;

	MDataModel _model, _elementPath;
	MDataManager _LcaseManager, _EleGroupManager;

	MVectorFactory _vFactory;
	int _elegroupcnt;
public:
	QString blank(int s, int i);
	QString eleMseg(int Nele, int gid);
	QString kexue(double s, int i, int w);
	bool ControllingInfoOutput(QTextStream* stream, QString fileName);
	bool NodeInfoOutput(QTextStream* stream);
	bool ElementInfoOutput(QTextStream* stream);
	bool SolidElementInfoOutput(QTextStream* stream,int);
	bool RodElementInfoOutput(QTextStream* stream,int);
	bool ShellElementInfoOutput(QTextStream* stream,int);
	bool StaticResultsOutput(QTextStream* stream);
	bool DynamicResultsOutput(QTextStream* stream);
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

	_data->_dbManager = _data->_objManager.getObject("org.sipesc.core.engdbs.mdatabasemanager");
	Q_ASSERT(!_data->_dbManager.isNull());

	_data->_vFactory = _data->_objManager.getObject("org.sipesc.fems.matrix.vectorfactory");
	Q_ASSERT(!_data->_vFactory.isNull());

	_data->_femsGlobal = _data->_objManager.getObject("org.sipesc.fems.global.femsglobal");
	Q_ASSERT(!_data->_femsGlobal.isNull());
}

MBltResultHeaderExportorImpl::~MBltResultHeaderExportorImpl() {
}

bool MBltResultHeaderExportorImpl::initialize(MDataModel& model,
		bool isRepeated) {
	if (_data->_isInitialized)
		return false; //不能重复初始化
	_data->_model = model;

	_data->_isInitialized = true;
	return true;
}

ProgressMonitor MBltResultHeaderExportorImpl::getProgressMonitor() const {
	return _data->_monitor;
}

bool MBltResultHeaderExportorImpl::dataExport(QTextStream* stream,
		QString fileName) {
	/**
	 * 未处理
	 *   EQUATION NUMBERS
	 *   C O N S T R A I N T   E Q U A T I O N S   D A T A
	 */
	_data->ControllingInfoOutput(stream, fileName);
	_data->NodeInfoOutput(stream);
	_data->ElementInfoOutput(stream);

	MSharedVariablesManager sharedVariables = _data->_extManager.createExtension(
			"org.sipesc.utilities.MSharedVariablesManager");
	Q_ASSERT(!sharedVariables.isNull());
	sharedVariables.initialize(_data->_model);

	QString analysis = sharedVariables.getVariable(
			_data->_femsGlobal.getValue(MFemsGlobal::AnalysisType)).toString();

	if(analysis == _data->_femsGlobal.getValue(MFemsGlobal::Static))
		_data->StaticResultsOutput(stream);
	else
		_data->DynamicResultsOutput(stream);
	return true;
}

//*************************写入总控信息*************************//
bool MBltResultHeaderExportorImpl::Data::ControllingInfoOutput(QTextStream* stream, QString fileName) {

	QDate d = QDate::currentDate();
	(*stream) << QString("\n         欢迎使用《开放式结构有限元分析系统SiPESC.FEMS》\n");
	(*stream) << QString("             研制单位 : 大连理工大学 工程力学系\n\n");
	(*stream) << QString("         计算日期 :         ")
			<< QString::number(d.year(), 10) << "           "
			<< QString::number(d.month(), 10) << "          "
			<< QString::number(d.day(), 10) << "\n";
	(*stream) << QString("         输入文件 ：") << "\n";
	(*stream) << QString("         输出文件 ： ") << fileName << "\n";
	(*stream) << "\n\n\n";


	bool isOk(false);
	int Nmode = 0;	//模态阶数
	if(_model.exists("Vibration",MDatabaseGlobal::ManagerType)){
		MDataManager VibManager = _dbManager.createDataManager();
		isOk = VibManager.open(_model, "Vibration",true);
		MPropertyData vibData = VibManager.getData(1);
		Nmode = vibData.getValue(0).toInt();
	}

	_LcaseManager = _dbManager.createDataManager();
	isOk = _LcaseManager.open(_model, "LoadCase",true);
	int Nste = _LcaseManager.getDataCount();  //时间步数

	(*stream) << "     NUMBER OF MODE SHAPES TO BE PRINTED. . . . . .(NMODE) ="
			<< blank(Nmode, 5) << "\n";
	(*stream) << "     NUMBER OF TIME STEPS IN FIRST SOLUTION BLOCK (NSTE)   ="
			<< blank(Nste, 5) << "\n";

	int Neig = 0;
	(*stream) << "     FREQUENCIES SOLUTION CODE . . . . . . . . . . .(IEIG) ="
			<< blank(Neig, 5) << "\n";
	(*stream) << "\n\n\n\n\n\n";
	return true;
}

//*************************节点数据输出*************************//
bool MBltResultHeaderExportorImpl::Data::NodeInfoOutput(QTextStream* stream) {
	printf("Writing Node Message ...\n");

	QString str;

	str.append("  GENERATED NODAL DATA \n\n");
	str.append("                     BOUNDARY  CONDITION  CODES                      NODAL  POINT  COORDINATES                DIRECTION  COSINES  OF \n");
	str.append("                                                                                                                 DIRECTOR  VECTOR\n");
	str.append("  NODE     X   Y   Z  XX  YY  ZZ OV1 OV2 OV3 OV4 OV5 OV6 PHI        X            Y            Z     NABC IGL (VN,X)  (VN,Y)  (VN,Z)\n");
	str.append("                  ( IN GLOBAL COORDINATE SYSTEM )               ( IN GLOBAL XYZ COORDINATE SYSTEM )\n\n");

	QString nstring1 = "    0   0   0   1   1   1   1   1   1   1   1   1   1  ";
	QString nstring2 = "  0   0";

	MDataManager nodeManager = _dbManager.createDataManager();
	bool isOk = nodeManager.open(_model, "Node",true);
	if(!isOk){
		QString errorMessage = "can't open Node DataManager "
				"in MBltResultHeaderExportorImpl::Data::NodeInfoOutput() ";
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}
	int ncnt = nodeManager.getDataCount();

	int i = 0;
	while (i < ncnt) {
		if (i % 100 == 0) {
			cout << "The Node Count " << ncnt << " now in " << i << "\n";
		}
		MNodeData node = nodeManager.getDataAt(i);

		int nId = node.getId();
		double nx = node.getX();
		double ny = node.getY();
		double nz = node.getZ();
		QString nodeId = blank(nId, 8);                         //nId 占8位
		QString NX = kexue(nx, 6, 12);                     //坐标有效数字6，占位12
		QString NY = kexue(ny, 6, 12);
		QString NZ = kexue(nz, 6, 12);

		str.append(nodeId + " " + nstring1 + " " + NX + " " + NY + " " + NZ + " " + nstring2 + "\n");

		i++;
	}

	str.append("\n\n\n\n\n");

	(*stream) << str;
	return true;
}
//*************************单元信息输出*************************//
bool MBltResultHeaderExportorImpl::Data::ElementInfoOutput(QTextStream* stream) {
	std::cout << "Writing Element Message ..." <<std::endl;

	_EleGroupManager = _dbManager.createDataManager();
	bool isOk = _EleGroupManager.open(_model, "EleGroup");
	if(!isOk){
		QString errorMessage = "can't open EleGroup DataManager "
				"in MBltResultHeaderExportorImpl::Data::ElementInfoOutput() ";
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}
	_elegroupcnt = _EleGroupManager.getDataCount();

	_elementPath = _dbManager.createDataModel();
	isOk = _elementPath.open(_model, "ElementPath");
	if(!isOk){
		QString errorMessage = "can't open ElementPath DataManager "
				"in MBltResultHeaderExportorImpl::Data::ElementInfoOutput() ";
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}

	//-----------------------------------------
	QString str;
	str += " E L E M E N T   G R O U P   D A T A \n\n\n\n";
	(*stream) << str;

	int i = 0;
	while (i < _elegroupcnt) {
		std::cout << "The EleGroup Count " << _elegroupcnt << " now in " << i+1 <<std::endl;

		MPropertyData eleGroup = _EleGroupManager.getDataAt(i);
		int gId = eleGroup.getId();
		QString type = eleGroup.getType();

		if(type == "HexaBrickElement"){
			SolidElementInfoOutput(stream, gId);
		}else if(type == "HexaBrick12Element"){
			SolidElementInfoOutput(stream, gId);
		}else if(type == "BarElement"){
			RodElementInfoOutput(stream, gId);
		}else if(type == "QuadDKQShElement"){
			ShellElementInfoOutput(stream, gId);
		}
		i = i + 1;
	}

	QString end = "TOTAL SYSTEM DATA\n";
	end += "\n\n\n\n\n\n                                         * E N D   O F   D A T A   P R I N T *\n\n\n\n\n";
	(*stream) << end;

	return true;
}
bool MBltResultHeaderExportorImpl::Data::SolidElementInfoOutput(QTextStream* stream,int id){

	QString eleHeader = " E L E M E N T   I N F O R M A T I O N \n\n\n";
	QString solidHeader = "    M IELD IELX  IPS  ISV MTYP MAXES   KG    ETIME      ETIME2    INTLOC NODE 1 NODE 2 NODE 3 NODE 4 NODE 5 NODE 6 NODE 7 NODE 8 \n";
	solidHeader += "                                                                         NODE 9 NODE10 NODE11 NODE12 NODE13 NODE14 NODE15 NODE16 \n";
	solidHeader += "                                                                         NODE17 NODE18 NODE19 NODE20 NODE21 NODE22 NODE23 NODE24 \n";
	solidHeader += "                                                                         NODE25 NODE26 NODE27 \n\n";
	solidHeader += "                                                              STRESS TABLE                         GLOBAL  COORDINATES \n";
	solidHeader += "                                                                  POINT                          X           Y           Z \n";
	QString solidEH ="   12   12    1    0    1    0    1  0.0000E+00  0.0000E+00      0";

	MPropertyData eleGroup = _EleGroupManager.getData(id);
	QString type = eleGroup.getType();
	int Elecount = eleGroup.getValueCount();

	MDataManager EleManager = _dbManager.createDataManager();
	bool isOk = EleManager.open(_elementPath, type);
	if(!isOk){
		QString errorMessage = QString("can't open %1 DataManager "
				"in MBltResultHeaderExportorImpl::Data::SolidElementInfoOutput() ").arg(eleGroup.getType());
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}

	QString str;
	QString groupId = blank(id, 5);     ///=号后9位，gid在前5位
	str.append("E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
//	str.append(eleMseg(Elecount, id));
	str.append(eleHeader);
	str.append(solidHeader);

	//-- 输出 ------------------------------------------------------------------------------------
	if(type == "HexaBrickElement"){
		Elecount /= 2;
		for (int j = 0; j < Elecount; j++) {
			QString sEid = blank(j + 1, 5);

			MElementData eleData1 = EleManager.getData(
					eleGroup.getValue(2 * j).toInt());
			MElementData eleData2 = EleManager.getData(
					eleGroup.getValue(2 * j + 1).toInt());

			QString SnodeId = " ";
			int gnc = eleData1.getNodeCount();
			for (int ttt = 0; ttt < 4; ttt++) {
				int nodeId = eleData1.getNodeId(ttt);
				SnodeId = SnodeId + blank(nodeId, 6) + QString(" ");
			}
			for (int t = 4; t < 8; t++) {
				int nodeId = eleData2.getNodeId(t);
				SnodeId = SnodeId + blank(nodeId, 6) + " ";
			}
			SnodeId =
					SnodeId + "\n"
							+ "                                                                             0      0      0      0      0      0      0      0\n";
			SnodeId =
					SnodeId
							+ "                                                                        ";

			for (int t = 0; t < 4; t++) {
				int nodeId = eleData2.getNodeId(t);
				SnodeId = SnodeId + blank(nodeId, 6) + " ";
			}

			SnodeId = SnodeId + "     0      0      0      0\n";
			SnodeId =
					SnodeId
							+ "                                                                             0      0      0\n\n";

			str.append(sEid + solidEH + SnodeId);   //存入一个单元块信息
		}
		(*stream) << str;

	}else{

		for (int j = 0; j < Elecount; j++) {
			QString sEid = blank(j + 1, 5);

			MElementData eleData1 = EleManager.getData(
					eleGroup.getValue(j).toInt());
			QString SnodeId = " ";
			for (int ttt = 0; ttt < 8; ttt++) {
				int nodeId = eleData1.getNodeId(ttt);
				SnodeId += blank(nodeId, 6) + QString(" ");
			}
			SnodeId += "\n";
			SnodeId += "                                                                             0      0      0      0      0      0      0      0 \n";
			SnodeId += "                                                                       ";

			QString SnodeId2 = " ";
			for (int t = 8; t < 12; t++) {
				int nodeId = eleData1.getNodeId(t);
				SnodeId2 += blank(nodeId, 6) + " ";
			}
			SnodeId2 += "     0      0      0      0 \n";
			SnodeId2 += "                                                                             0      0      0 \n";
			SnodeId2 += "\n";
			str.append(sEid + solidEH + SnodeId + SnodeId2);   //存入一个单元块信息
		}

		(*stream) << str;
	}

	return true;
}
bool MBltResultHeaderExportorImpl::Data::RodElementInfoOutput(QTextStream* stream,int id){

	QString eleHeader = " E L E M E N T   I N F O R M A T I O N \n\n\n";
	QString rodHeader = "    N IELD  IPS  ISV  MTYP   KG    INITIAL       GAP        LENGTH        ETIME       ETIME2     INTLOC   NODE1  NODE2  NODE3  NODE4 \n";
	rodHeader += "                                   STRAIN       WIDTH                                 INTEGRATION       GLOBAL  COORDINATES \n";
	rodHeader += "                                                                                         POINT      X            Y            Z \n";
	rodHeader += "\n\n\n";
	QString rodEH ="    2    1    0     1    1   0.0000E+00   0.0000E+00 ";
	QString rodEH2 ="   0.0000E+00   0.0000E+00       0 ";

	MPropertyData eleGroup = _EleGroupManager.getData(id);
	QString type = eleGroup.getType();
	int Elecount = eleGroup.getValueCount();

	MDataManager EleManager = _dbManager.createDataManager();
	bool isOk = EleManager.open(_elementPath, type);
	if(!isOk){
		QString errorMessage = QString("can't open %1 DataManager "
				"in MBltResultHeaderExportorImpl::Data::RodElementInfoOutput() ").arg(eleGroup.getType());
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}

	QString str;
	QString groupId = blank(id, 5);     ///=号后9位，gid在前5位
	str.append("E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
//	str.append(eleMseg(Elecount, id));
	str.append(eleHeader);
	str.append(rodHeader);

	for (int j = 0; j < Elecount; j++) {
		QString sEid = blank(j + 1, 5);
		QString length = kexue(0,5,12);

		MElementData eleData1 = EleManager.getData(eleGroup.getValue(j).toInt());

		QString SnodeId = " ";
		int gnc = eleData1.getNodeCount();
		for (int ttt = 0; ttt < gnc; ttt++) {
			int nodeId = eleData1.getNodeId(ttt);
			SnodeId += blank(nodeId, 8);
		}
		str.append(sEid + rodEH + length + rodEH2 + SnodeId);   //存入一个单元块信息
		str += "\n\n";
	}

	(*stream) << str;

	return true;
}
bool MBltResultHeaderExportorImpl::Data::ShellElementInfoOutput(QTextStream* stream,int id){

	QString eleHeader = " E L E M E N T   I N F O R M A T I O N \n\n\n";
	QString shellHeader = "    N IPS ISV MTYP   KG     BET       THIC       ETIME     ETIME2   NODE1 NODE2 NODE3 INTLOC INTEG.       GLOBAL  COORDINATES \n";
	shellHeader += "                                                                                             POINT      X          Y          Z \n";
	shellHeader += "\n\n\n";
	QString shellEH ="   1   0    1    1  0.000E+00 ";
	QString shellEH2 ="  0.000E+00  0.000E+00";

	MPropertyData eleGroup = _EleGroupManager.getData(id);
	QString type = eleGroup.getType();
	int Elecount = eleGroup.getValueCount();

	MDataManager EleManager = _dbManager.createDataManager();
	bool isOk = EleManager.open(_elementPath, type);
	if(!isOk){
		QString errorMessage = QString("can't open %1 DataManager "
				"in MBltResultHeaderExportorImpl::Data::RodElementInfoOutput() ").arg(eleGroup.getType());
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}

	QString str;
	QString groupId = blank(id, 5);     ///=号后9位，gid在前5位
	str.append("E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
//	str.append(eleMseg(Elecount, id));
	str.append(eleHeader);
	str.append(shellHeader);

	for (int j = 0; j < Elecount; j++) {
		QString sEid = blank(j + 1, 5);
		QString thick = kexue(0,4,10);

		MElementData eleData1 = EleManager.getData(eleGroup.getValue(j).toInt());

		QString SnodeId;
		int gnc = eleData1.getNodeCount();
		for (int ttt = 0; ttt < gnc; ttt++) {
			int nodeId = eleData1.getNodeId(ttt);
			SnodeId += blank(nodeId, 6);
		}
		str.append(sEid + shellEH + thick + shellEH2 + SnodeId);   //存入一个单元块信息
		str += "\n\n";
	}

	(*stream) << str;
	return true;
}

//****************************计算结果输出*************************//
bool MBltResultHeaderExportorImpl::Data::StaticResultsOutput(QTextStream* stream) {

	bool isOk;

	MDataModel DisPathModel = _dbManager.createDataModel();
	isOk = DisPathModel.open(_model, "DisplacementPath");
	Q_ASSERT(isOk);
	MDataManager DisManager = _dbManager.createDataManager();

	MDataModel EleStressPathModel = _dbManager.createDataModel();
	isOk = EleStressPathModel.open(_model, "EleNodeStressPath");
	Q_ASSERT(isOk);
	MDataModel EleStressModel = _dbManager.createDataModel();

	int TimeStepCount = _LcaseManager.getDataCount();

	for (int TimeStep = 1; TimeStep <= TimeStepCount; TimeStep++) {

		//****************************节点位移数据*************************//
		QString str;
		QString TimeStepMseg = "  P R I N T   O U T   F O R   T I M E   S T E P"
				+ blank(TimeStep, 5);
		TimeStepMseg = TimeStepMseg
				+ "     (TIME STEP=  0.10000E+01     SOLUTION TIME="
				+ kexue(TimeStep, 6, 12) + ")\n\n";
		str.append(TimeStepMseg);

		QString DisplacementHead1 = "  D I S P L A C E M E N T S\n\n";
		QString DisplacementHead2 = "  NODE   X-TRANSLATION  Y-TRANSLATION  Z-TRANSLATION  X/V1-ROTATION  Y/V2-ROTATION  Z/VN-ROTATION  FLUID PHI/P0*  REF SYST  REF SYST\n\n";
		str.append(DisplacementHead1);
		str.append(DisplacementHead2);

		DisManager.open(DisPathModel, QString::number(TimeStep));
		int Dcnt = DisManager.getDataCount();
		for (int nId = 1; nId <= Dcnt; nId++) {
			if (nId % 45 == 0) {
				str.append(DisplacementHead1);
				str.append(DisplacementHead2);
			}
			QString SNodeId = blank(nId, 6) + " ";   //
			MVectorData DisData = DisManager.getData(nId);
			Q_ASSERT(!DisData.isNull());
			MVector nodeDisV = _vFactory.createVector(); ////////////////
			Q_ASSERT(!nodeDisV.isNull());
			nodeDisV << DisData;

			QString Sdisplacement = "";   //
			for (int dx = 0; dx < nodeDisV.getCount(); dx++)   //
					{
				Sdisplacement = Sdisplacement
						+ kexue(double(nodeDisV(dx)), 7, 14) + " ";

			}
			Sdisplacement = Sdisplacement
					+ " 0.000000E+00    GLOBAL    GLOBAL \n";

			str.append(SNodeId);
			str.append(Sdisplacement);

		}

		str.append("\n");
		(*stream) << str;
// TODO
#if 0
		//****************************单元应力*********************************//
		QString str1;
		QString stressMseg1 =
				" S T R E S S   C A L C U L A T I O N S   F O R   E L E M E N T   G R O U P";
		QString stressMseg2 =
				"STRESSES ARE CALCULATED IN THE GLOBAL COORDINATE SYSTEM \n \
          \n\
          \n\
           ELEMENT   STRESS TABLE                                 STRESSES / TOTAL STRAINS \n\
            NUMBER      POINT                                   C  O  M  P  O  N  E  N  T  S \n\
                         ITB              XX            YY            ZZ            XY            XZ            YZ \n\
          \n\
                                              (STRAINS ARE ONLY PRINTED WHEN ELEMENT FLAG (IPS) SO INDICATES)\n\n\n";
		//****************************************************************************************************


		isOk = EleStressModel.open(EleStressPathModel, QString::number(TimeStep));
		Q_ASSERT(isOk);
		MDataManager EleStressManager = _dbManager.createDataManager();

		for (int i = 1; i <= _elegroupcnt; i++) {

			MPropertyData eleGroup = _EleGroupManager.getData(i);
			int gId = eleGroup.getId();
			QString type = eleGroup.getType();
			/**TODO 应力输出与单元类型有关 **/
			QString groupId = blank(gId, 5);     ///=号后9位，gid在前5位
			bool ok = EleStressManager.open(EleStressModel,type);

			str1.append(stressMseg1 + groupId + "   (3/D CONTINUUM)\n\n");
			str1.append(stressMseg2);

			int Elecount = (eleGroup.getValueCount() / 2);
			for (int j = 0; j < Elecount; j++) {
				QString sEid = blank(j + 1, 8) + "\n";
				MDataObjectList eleStressData1 = EleStressManager.getData(eleGroup.getValue(2 * j).toInt());
				MDataObjectList eleStressData2 = EleStressManager.getData(eleGroup.getValue(2 * j + 1).toInt());

				QString ss;
				for (int n = 0; n < 4; n++) {
					MVector stress1V = _vFactory.createVector();
					MVectorData stress1 = eleStressData1.getDataAt(n);
					stress1V<< stress1;

					ss = ss + blank(n + 1, 17) + QString("          ");
					for (int vi = 0; vi < stress1V.getCount(); vi++) {
						ss = ss + kexue(stress1V(vi), 6, 13) + " ";
					}
					ss += "\n";
				}

				for (int n = 4; n < 8; n++) {
					MVector stress2V = _vFactory.createVector(6);
					MVectorData stress2 = eleStressData2.getDataAt(n);
					stress2V << stress2;

					ss = ss + blank(n + 1, 17) + "          ";
					for (int vi = 0; vi < stress2V.getCount(); vi++) {
						ss = ss + kexue(stress2V(vi), 6, 13) + " ";
					}
					ss += "\n";
				}

				ss += "\n";
				str1.append(sEid);
				str1.append(ss);
			}
		}

		(*stream) << str1;
#endif
	}

	return true;
}

bool MBltResultHeaderExportorImpl::Data::DynamicResultsOutput(QTextStream* stream) {

	bool isOk;

	MDataManager evManager = _dbManager.createDataManager();
	isOk = evManager.open(_model, _femsGlobal.getValue(MFemsGlobal::EigenValues), true);
	if (!isOk){
		QString errorMessage = "can't open EigenValues DataManager "
				"in MBltResultHeaderExportorImpl::Data::DynamicResultsOutput() ";
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}
	MVectorData eigenVectorData = evManager.getDataAt(0);
	if(eigenVectorData.isNull()){
		QString errorMessage = QString("In MBltResultHeaderExportorImpl::Data::DynamicResultsOutput()"
						" ****** EigenValues Data is Null.");
		mReportError(M_ERROR_FATAL, errorMessage);
		_monitor.setMessage(errorMessage);
		return false;
	}
	MVector eigenValues = _vFactory.createVector();
	eigenValues << eigenVectorData;

	MDataModel DisPathModel = _dbManager.createDataModel();
	isOk = DisPathModel.open(_model, "DisplacementPath");
	Q_ASSERT(isOk);
	MDataManager DisManager = _dbManager.createDataManager();

	int count = DisPathModel.getTypeCount(MDatabaseGlobal::ManagerType);
	int count2 = eigenValues.getCount();

	QString strr = "   F R E Q U E N C I E S \n\n";
	strr += "      FREQUENCY NUMBER            FREQUENCY (RAD/SEC)      FREQUENCY (CYCLES/SEC)        PERIOD (SECONDS) \n\n";
	for(int i=0;i<count2;i++){
		double val = eigenValues(i);
		double freqR = pow(val,0.5);
		double freqC = freqR/2/3.1415926;
		double sec = 1/freqC;
		strr += blank(i+1,13) + "         ";
		strr += kexue(freqR, 5, 26);
		strr += kexue(freqC, 5, 26);
		strr += kexue(sec, 5, 26);
		strr += "\n";
	}
	strr += "\n";
	strr += "      NOTE - READ TIME UNIT 'SEC' AS OTHER TIME UNIT IF OTHER TIME UNIT IS USED\n";
	strr += "\n\n\n\n\n";
	(*stream) << strr;


	for (int order=1; order<count; order++){

		QString str = QString("   M O D E  S H A P E  N O .   %1                                                       ").arg(order);
		str += "( FREQUENCY =  " + kexue(pow(eigenValues(order-1),0.5), 5, 10) + " RAD/SEC )\n";
		str += "\n\n";
		str += "  NODE   X-TRANSLATION  Y-TRANSLATION  Z-TRANSLATION  X/V1-ROTATION  Y/V2-ROTATION  Z/VN-ROTATION  FLUID PHI/P0*  REF SYST  REF SYST\n";
		str += "                                                                                                                  TRAN-DOF  ROT- DOF\n";
		str += "\n";

		bool isOk = DisManager.open(DisPathModel, QString::number(order));
		int Dcnt = DisManager.getDataCount();
		for (int nId = 1; nId <= Dcnt; nId++) {

			MVectorData DisData = DisManager.getData(nId);
			Q_ASSERT(!DisData.isNull());
			MVector nodeDisV = _vFactory.createVector();
			Q_ASSERT(!nodeDisV.isNull());
			nodeDisV << DisData;

			str += blank(nId, 6) + " ";
			for (int dx = 0; dx < nodeDisV.getCount(); dx++){
				str += kexue(double(nodeDisV(dx)), 7, 15);

			}
			str += "  0.000000E+00    GLOBAL    GLOBAL \n";
		}

		str.append("\n\n\n");
		(*stream) << str;
	}

	return true;
}
//*************************格式函数*************************//

QString MBltResultHeaderExportorImpl::Data::blank(int s, int i)         /////补空格
		{

	QString string = QString::number(s, 10);

	int n = i - string.length();
	for (int t = 0; t < n; t++)
		string = QString(" ") + string;
	return string;
}

QString MBltResultHeaderExportorImpl::Data::kexue(double s, int i, int w) ////科学计数法，i为保留有效数字,w总为占位数
{
	QString string = QString("%1").arg(s, w, 'E', i - 1);
	return string;
}
QString MBltResultHeaderExportorImpl::Data::eleMseg(int NEle, int GId) {

	QString eleMseg = " E L E M E N T   D E F I N I T I O N\n\n\n";
	eleMseg += " ELEMENT TYPE  . . . . . . . . . . . . .( NPAR(1) ). . =    3\n";
	eleMseg += "     EQ.1, TRUSS ELEMENTS\n";
	eleMseg += "     EQ.2, 2-DIM ELEMENTS\n";
	eleMseg += "     EQ.3, 3-DIM ELEMENTS\n";
	eleMseg += "     EQ.4, BEAM  ELEMENTS\n";
	eleMseg += "     EQ.5, ISO/BEAM ELEMENTS\n";
	eleMseg += "     EQ.6, PLATE ELEMENTS\n";
	eleMseg += "     EQ.7, SHELL ELEMENTS\n";
	eleMseg += "     EQ.8, PIPE ELEMENTS\n";
	eleMseg += "     EQ.9, GENERAL ELEMENTS\n";
	eleMseg += "     EQ.10, EMPTY\n";
	eleMseg += "     EQ.11, 2-DIM FLUID ELEMENTS\n";
	eleMseg += "     EQ.12, 3-DIM FLUID ELEMENTS\n\n";

	eleMseg += " NUMBER OF ELEMENTS. . . . . . . . . . .( NPAR(2) ). . ="
			+ blank(NEle, 5) + "\n\n";

	eleMseg += " TYPE OF NONLINEAR ANALYSIS  . . . . . .( NPAR(3) ). . =    0\n";
	eleMseg += "     EQ.0, LINEAR\n";
	eleMseg += "     EQ.1, MATERIAL NONLINEARITY ONLY\n";
	eleMseg += "     EQ.2, LARGE DISPLACEMENTS WITH\n";
	eleMseg += "           SMALL STRAINS\n";
	eleMseg += "     GE.3, LARGE DISPLACEMENTS WITH\n";
	eleMseg += "           LARGE STRAINS\n\n\n";

	eleMseg = eleMseg
			+ " ELEMENT BIRTH AND DEATH OPTIONS . . . .( NPAR(4) ). . =    0\n";
	eleMseg += "     EQ.0, OPTION NOT ACTIVE\n";
	eleMseg += "     EQ.1, BIRTH OPTION ACTIVE\n";
	eleMseg += "     EQ.2, DEATH OPTION ACTIVE\n";
	eleMseg += "     EQ.3, DEATH AFTER BIRTH OPTION ACTIVE\n\n\n\n";

	eleMseg = eleMseg + " SKEW COORDINATE SYSTEM\n";
	eleMseg = eleMseg
			+ "     REFERENCE INDICATOR . . . . . . . .( NPAR(6) ). . =    0\n";
	eleMseg += "     EQ.0, ALL ELEMENT NODES\n";
	eleMseg += "           USE THE GLOBAL SYSTEM ONLY\n";
	eleMseg += "     EQ.1, ELEMENT NODES REFER\n";
	eleMseg += "           TO SKEW COORDINATE SYSTEM\n\n";

	eleMseg = eleMseg + " MAX NUMBER OF NODES DESCRIBING\n";
	eleMseg =
			eleMseg
					+ "     ANY ONE ELEMENT . . . . . . . . . .( NPAR(7) ). . =   12\n\n\n";

	eleMseg = eleMseg
			+ " DEGENERATION INDICATOR  . . . . . . . .( NPAR(8) ). . =    0\n";
	eleMseg += "     EQ.0, NO DEGENERATION OR NO CORRECTION\n";
	eleMseg += "           FOR SPATIAL ISOTROPY\n";
	eleMseg += "     EQ.1, SPATIAL ISOTROPY CORRECTIONS\n";
	eleMseg += "           APPLIED TO SPECIALLY\n";
	eleMseg += "           DEGENERATED 20-NODE ELEMENTS\n\n";

	eleMseg = eleMseg + " FLAG FOR CALCULATION OF ENERGY\n";
	eleMseg = eleMseg
			+ "     RELEASE RATES . . . . . . . . . . .( NPAR(9) ). . =    0\n";
	eleMseg += "     EQ.0, NO CALCULATION\n";
	eleMseg += "     EQ.1, CALCULATE ENERGY RELEASE RATES\n";
	eleMseg += "           AT NODAL POINTS\n\n\n";

	eleMseg = eleMseg + " INTEGRATION ORDER (R-S DIRECTIONS) FOR\n";
	eleMseg =
			eleMseg
					+ "     ELEMENT STIFFNESS GENERATION. . . .( NPAR(10)). . =    4\n\n";

	eleMseg = eleMseg + " INTEGRATION ORDER (T DIRECTION) FOR\n";
	eleMseg =
			eleMseg
					+ "     ELEMENT STIFFNESS GENERATION. . . .( NPAR(11)). . =    4\n\n";

	eleMseg = eleMseg
			+ " NUMBER OF STRESS OUTPUT TABLES  . . . .( NPAR(13)). . =    1\n";
	eleMseg += "     EQ.-1, PRINT NODAL FORCES\n";
	eleMseg += "     EQ. 0, PRINT STRESSES AT INTEGRATION POINTS\n";
	eleMseg = eleMseg
			+ "     GT. 0, PRINT STRESSES AT STRESS OUTPUT POINTS\n\n\n\n\n";

	eleMseg = eleMseg + " E L E M E N T   V A R I A B L E S\n\n\n";

	eleMseg = eleMseg
			+ " VARIABLES FLAG. . . . . . . . . . . . .( NPAR(14)). . =    0\n";
	eleMseg = eleMseg + "     EQ. 0, DISPLACEMENT-BASED ELEMENT\n";
	eleMseg = eleMseg
			+ "     NE. 0, NPAR(14) PRESSURE VARIABLES PER ELEMENT\n\n";
	eleMseg = eleMseg + " M A T E R I A L   D E F I N I T I O N\n\n\n";

	eleMseg = eleMseg
			+ " MATERIAL MODEL. . . . . . . . . . . . .( NPAR(15)). . =    1\n";
	eleMseg += "     EQ. 1, LINEAR ELASTIC ISOTROPIC\n";
	eleMseg += "     EQ. 2, LINEAR ELASTIC ORTHOTROPIC\n";
	eleMseg += "     EQ. 3, THERMOELASTIC MODEL\n";
	eleMseg += "     EQ. 4, NONLINEAR CURVE DESCRIPTION MODEL\n";
	eleMseg += "     EQ. 5, CONCRETE CRACKING MODEL\n";
	eleMseg += "     EQ. 6, (EMPTY)\n";
	eleMseg += "     EQ. 7, DRUCKER PRAGER (CAP) MODEL\n";
	eleMseg += "\
     EQ. 8, ELASTIC-PLASTIC WITH ISOTROPIC HARDENING  \n\
     EQ. 9, ELASTIC-PLASTIC WITH KINEMATIC HARDENING  \n\
     EQ.10, ELASTIC-PLASTIC WITH CREEP (ISOTROPIC)   \n\
     EQ.11, ELASTIC-PLASTIC WITH CREEP (KINEMATIC)  \n\
     EQ.12, (EMPTY)                                \n\
     EQ.13, INCOMPRESSIBLE ELASTIC (MOONEY-RIVLIN) \n\
     EQ.14, USER-SUPPLIED MODEL    \n\
 \n\
  NUMBER OF DIFFERENT SETS OF MATERIAL \n\
      CONSTANTS . . . . . . . . . . . . .( NPAR(16)). . =    1 \n\
 \n\
  NUMBER OF MATERIAL CONSTANTS PER SET. .( NPAR(17)). . =    2 \n\
 \n\
  NUMBER OF AXIS ORIENTATION SETS . . . .( NPAR(18)). . =    0 \n\
  \n\
  \n\
  \n\
   S T O R A G E   I N F O R M A T I O N \n\
 \n\
 \n\
  LENGTH OF ARRAY NEEDED FOR STORING ELEMENT GROUP \n\
  DATA (GROUP" + blank(GId,3) + "). . . . . . . . . . . . .( MIDEST ). . =    2402 \n\
 \n\
 \n\
  MATERIAL CONSTANTS SET NUMBER ....     1 \n\
 \n\
      DEN ..........( DENSITY ).. =  0.254840E+01 \n\
 \n\
      E ............( PROP(1) ).. =  0.285000E+08 \n\
      VNU ..........( PROP(2) ).. =  0.200000E+00 \n\
 \n\
 \n\
 \n\
 \n\
 \n\
  S T R E S S   O U T P U T   T A B L E S \n\
 \n\
      TABLE      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16 \n\
 \n\
 \n\
          1      1      2      3      4      5      6      7      8      0      0      0      0      0      0      0      0 \n\n\n\n";

	return eleMseg;
}
