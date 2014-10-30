/*
 * mbltresultheaderexportorimpl.cxx
 *
 *  Created on: 2014年9月19日
 *      Author: sipesc
 */
#include <format.h>

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
using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::bltexport;

#include <mbaraxialforceparserimpl.h>

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
	QString SolidEleMsg(int Nele, int gid);
	QString RodEleMsg(int Nele, int gid);
	QString ShellEleMsg(int Nele, int gid);

	bool ControllingInfoOutput(QTextStream* stream, QString fileName);
	bool NodeInfoOutput(QTextStream* stream);
	bool ElementInfoOutput(QTextStream* stream);
	bool SolidElementInfoOutput(QTextStream* stream,int);
	bool RodElementInfoOutput(QTextStream* stream,int);
	bool ShellElementInfoOutput(QTextStream* stream,int);
	bool SolidEleStress(QTextStream* stream,MPropertyData&,MDataManager&);
	bool RodEleStress(QTextStream* stream,MPropertyData&,MDataManager&);
	bool ShellEleStress(QTextStream* stream,MPropertyData&,MDataManager&);
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
			<< BltForamt::blank(Nmode, 5) << "\n";
	(*stream) << "     NUMBER OF TIME STEPS IN FIRST SOLUTION BLOCK (NSTE)   ="
			<< BltForamt::blank(Nste, 5) << "\n";

	int Neig = 0;
	(*stream) << "     FREQUENCIES SOLUTION CODE . . . . . . . . . . .(IEIG) ="
			<< BltForamt::blank(Neig, 5) << "\n";
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
		QString nodeId = BltForamt::blank(nId, 8);                         //nId 占8位
		QString NX = BltForamt::sciNot(nx, 6, 12);                     //坐标有效数字6，占位12
		QString NY = BltForamt::sciNot(ny, 6, 12);
		QString NZ = BltForamt::sciNot(nz, 6, 12);

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
	QString groupId = BltForamt::blank(id, 5);     ///=号后9位，gid在前5位
	str.append(" E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
	str.append(SolidEleMsg(Elecount, id));
	str.append(eleHeader);
	str.append(solidHeader);

	//-- 输出 ------------------------------------------------------------------------------------
	if(type == "HexaBrickElement"){
		Elecount /= 2;
		for (int j = 0; j < Elecount; j++) {
			QString sEid = BltForamt::blank(j + 1, 5);

			MElementData eleData1 = EleManager.getData(
					eleGroup.getValue(2 * j).toInt());
			MElementData eleData2 = EleManager.getData(
					eleGroup.getValue(2 * j + 1).toInt());

			QString SnodeId = " ";
			int gnc = eleData1.getNodeCount();
			for (int ttt = 0; ttt < 4; ttt++) {
				int nodeId = eleData1.getNodeId(ttt);
				SnodeId = SnodeId + BltForamt::blank(nodeId, 6) + QString(" ");
			}
			for (int t = 4; t < 8; t++) {
				int nodeId = eleData2.getNodeId(t);
				SnodeId = SnodeId + BltForamt::blank(nodeId, 6) + " ";
			}
			SnodeId =
					SnodeId + "\n"
							+ "                                                                             0      0      0      0      0      0      0      0\n";
			SnodeId =
					SnodeId
							+ "                                                                        ";

			for (int t = 0; t < 4; t++) {
				int nodeId = eleData2.getNodeId(t);
				SnodeId = SnodeId + BltForamt::blank(nodeId, 6) + " ";
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
			QString sEid = BltForamt::blank(j + 1, 5);

			MElementData eleData1 = EleManager.getData(
					eleGroup.getValue(j).toInt());
			QString SnodeId = " ";
			for (int ttt = 0; ttt < 8; ttt++) {
				int nodeId = eleData1.getNodeId(ttt);
				SnodeId += BltForamt::blank(nodeId, 6) + QString(" ");
			}
			SnodeId += "\n";
			SnodeId += "                                                                             0      0      0      0      0      0      0      0 \n";
			SnodeId += "                                                                       ";

			QString SnodeId2 = " ";
			for (int t = 8; t < 12; t++) {
				int nodeId = eleData1.getNodeId(t);
				SnodeId2 += BltForamt::blank(nodeId, 6) + " ";
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
	QString groupId = BltForamt::blank(id, 5);     ///=号后9位，gid在前5位
	str.append(" E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
	str.append(RodEleMsg(Elecount, id));
	str.append(eleHeader);
	str.append(rodHeader);

//	MBarAxialForceParserImpl barParser;
//	barParser.initialize(_data->_model);

	for (int j = 0; j < Elecount; j++) {
		MElementData eleData1 = EleManager.getData(eleGroup.getValue(j).toInt());

		QString sEid = BltForamt::blank(j + 1, 5);

//		double len = barParser.getSecArea(eleData1);
		QString length = BltForamt::sciNot(0,5,12);

		QString SnodeId = " ";
		int gnc = eleData1.getNodeCount();
		for (int ttt = 0; ttt < gnc; ttt++) {
			int nodeId = eleData1.getNodeId(ttt);
			SnodeId += BltForamt::blank(nodeId, 8);
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
	QString groupId = BltForamt::blank(id, 5);     ///=号后9位，gid在前5位
	str.append(" E L E M E N T   G R O U P ........................... =" + groupId + "    ( LINEAR )\n\n\n\n");
	str.append(ShellEleMsg(Elecount, id));
	str.append(eleHeader);
	str.append(shellHeader);

	for (int j = 0; j < Elecount; j++) {
		QString sEid = BltForamt::blank(j + 1, 5);
		QString thick = BltForamt::sciNot(0,4,10);

		MElementData eleData1 = EleManager.getData(eleGroup.getValue(j).toInt());

		QString SnodeId;
		int gnc = eleData1.getNodeCount();
		for (int ttt = 0; ttt < gnc; ttt++) {
			int nodeId = eleData1.getNodeId(ttt);
			SnodeId += BltForamt::blank(nodeId, 6);
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

	int TimeStepCount = _LcaseManager.getDataCount();

	for (int TimeStep = 1; TimeStep <= TimeStepCount; TimeStep++) {

		//****************************节点位移数据*************************//
		QString str;
		QString TimeStepMseg = "  P R I N T   O U T   F O R   T I M E   S T E P"
				+ BltForamt::blank(TimeStep, 5);
		TimeStepMseg = TimeStepMseg
				+ "     (TIME STEP=  0.10000E+01     SOLUTION TIME="
				+ BltForamt::sciNot(TimeStep, 6, 12) + ")\n\n";
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
			QString SNodeId = BltForamt::blank(nId, 6) + " ";   //
			MVectorData DisData = DisManager.getData(nId);
			Q_ASSERT(!DisData.isNull());
			MVector nodeDisV = _vFactory.createVector(); ////////////////
			Q_ASSERT(!nodeDisV.isNull());
			nodeDisV << DisData;

			QString Sdisplacement = "";   //
			for (int dx = 0; dx < nodeDisV.getCount(); dx++)   //
					{
				Sdisplacement = Sdisplacement
						+ BltForamt::sciNot(double(nodeDisV(dx)), 7, 14) + " ";

			}
			Sdisplacement = Sdisplacement
					+ " 0.000000E+00    GLOBAL    GLOBAL \n";

			str.append(SNodeId);
			str.append(Sdisplacement);

		}

		str.append("\n");
		(*stream) << str;

		//****************************单元应力*********************************//
		MDataModel EleStressPathModel = _dbManager.createDataModel();
		isOk = EleStressPathModel.open(_model, "EleNodeStressPath");
		Q_ASSERT(isOk);
		MDataModel EleStressModel = _dbManager.createDataModel();

		isOk = EleStressModel.open(EleStressPathModel, QString::number(TimeStep));
		Q_ASSERT(isOk);
		MDataManager EleStressManager = _dbManager.createDataManager();

		for (int i = 0; i < _elegroupcnt; i++) {

			MPropertyData eleGroup = _EleGroupManager.getDataAt(i);
			int gId = eleGroup.getId();
			QString type = eleGroup.getType();
			bool ok = EleStressManager.open(EleStressModel,type);

			/**TODO 应力输出与单元类型有关，先测试实体单元 **/
			if(type == "HexaBrick12Element" || type == "HexaBrickElement"){
				SolidEleStress(stream,eleGroup,EleStressManager);
			}else if(type == "BarElement"){
				RodEleStress(stream, eleGroup,EleStressManager);
			}else if(type == "QuadDKQShElement"){
				ShellEleStress(stream, eleGroup,EleStressManager);
			}
		}
	}

	return true;
}
bool MBltResultHeaderExportorImpl::Data::SolidEleStress(QTextStream* stream, MPropertyData& eleGroup ,MDataManager& EleStressManager){
	QString str1;
	QString stressMseg1 = " S T R E S S   C A L C U L A T I O N S   F O R   E L E M E N T   G R O U P";
	QString stressMseg2 = " STRESSES ARE CALCULATED IN THE GLOBAL COORDINATE SYSTEM \n";
	stressMseg2 += "\n\n";
	stressMseg2 += " ELEMENT   STRESS TABLE                                 STRESSES / TOTAL STRAINS \n";
	stressMseg2 += "  NUMBER      POINT                                   C  O  M  P  O  N  E  N  T  S \n";
	stressMseg2 += "               ITB              XX            YY            ZZ            XY            XZ            YZ \n";
	stressMseg2 += "\n";
	stressMseg2 += "                                    (STRAINS ARE ONLY PRINTED WHEN ELEMENT FLAG (IPS) SO INDICATES) \n";
	stressMseg2 += "\n\n";
	//****************************************************************************************************

	int gId = eleGroup.getId();
	QString type = eleGroup.getType();
	QString groupId = BltForamt::blank(gId, 5);     ///=号后9位，gid在前5位
	str1.append(stressMseg1 + groupId + "   (3/D CONTINUUM)\n\n");
	str1.append(stressMseg2);

	int Elecount = eleGroup.getValueCount();
	if(type == "HexaBrickElement"){
		Elecount /= 2;
		for (int j = 0; j < Elecount; j++) {
			QString sEid = BltForamt::blank(j + 1, 8) + "\n";
			MDataObjectList eleStressData1 = EleStressManager.getData(eleGroup.getValue(2 * j).toInt());
			MDataObjectList eleStressData2 = EleStressManager.getData(eleGroup.getValue(2 * j + 1).toInt());

			QString ss;
			for (int n = 0; n < 4; n++) {
				MVector stress1V = _vFactory.createVector();
				MVectorData stress1 = eleStressData1.getDataAt(n);
				stress1V<< stress1;

				ss = ss + BltForamt::blank(n + 1, 17) + QString("          ");
				for (int vi = 0; vi < stress1V.getCount(); vi++) {
					ss = ss + BltForamt::sciNot(stress1V(vi), 6, 13) + " ";
				}
				ss += "\n";
			}

			for (int n = 4; n < 8; n++) {
				MVector stress2V = _vFactory.createVector(6);
				MVectorData stress2 = eleStressData2.getDataAt(n);
				stress2V << stress2;

				ss = ss + BltForamt::blank(n + 1, 17) + "          ";
				for (int vi = 0; vi < stress2V.getCount(); vi++) {
					ss = ss + BltForamt::sciNot(stress2V(vi), 6, 13) + " ";
				}
				ss += "\n";
			}

			ss += "\n";
			str1.append(sEid);
			str1.append(ss);
		}
	}else{
		// 12节点单元
		for (int j = 0; j < Elecount; j++) {
			QString sEid = BltForamt::blank(j + 1, 8) + "\n";
			MDataObjectList eleStressData1 = EleStressManager.getData(eleGroup.getValue(j).toInt());

			QString ss;
			MVector stress1V = _vFactory.createVector();

			for (int n = 0; n < 8; n++) {
				MVectorData stress1 = eleStressData1.getDataAt(n);
				stress1V << stress1;

				ss = ss + BltForamt::blank(n + 1, 17) + QString("          ");
				for (int vi = 0; vi < stress1V.getCount(); vi++) {
					ss = ss + BltForamt::sciNot(stress1V(vi), 6, 13) + " ";
				}
				ss += "\n";
			}

			ss += "\n";
			str1.append(sEid);
			str1.append(ss);
		}
	}

	(*stream) << str1;
	return true;
}
// TODO 杆单元轴力
bool MBltResultHeaderExportorImpl::Data::RodEleStress(QTextStream* stream, MPropertyData& eleGroup ,MDataManager& EleStressManager){
	QString str1;
	int gId = eleGroup.getId();
	QString type = eleGroup.getType();
	QString groupId = BltForamt::blank(gId, 5);     ///=号后9位，gid在前5位
	str1 += " S T R E S S   C A L C U L A T I O N S   F O R   E L E M E N T   G R O U P" + groupId + "   ( TRUSS )\n\n\n";
	str1 += " FORCES AND STRESSES ARE CALCULATED IN THE LOCAL COORDINATE SYSTEM \n";
	str1 += "\n";
	str1 += "  ELEMENT    OUTPUT \n";
	str1 += "   NUMBER  LOCATION            FORCE           STRESS           STRAIN \n";
	str1 += "\n";

	int Elecount = eleGroup.getValueCount();

	MBarAxialForceParserImpl barParser;
	barParser.initialize(_model);

	for (int j = 0; j < Elecount; j++) {
		QString sEid = BltForamt::blank(j + 1, 9);

		MDataObjectList eleStressData = EleStressManager.getData(eleGroup.getValue(j).toInt());

		double af = barParser.getAxialForce(eleStressData);

		QString val = BltForamt::sciNot(af, 7, 17);	// 轴力?

		str1 += sEid;
		str1 += "         1";
		str1 += val;
		str1 += val;
		str1 += val;
		str1 += "\n\n";
	}


	(*stream) << str1;
	return true;
}
// TODO 壳单元应力
bool MBltResultHeaderExportorImpl::Data::ShellEleStress(QTextStream* stream, MPropertyData& eleGroup ,MDataManager& EleStressManager){
	QString str1;
	int gId = eleGroup.getId();
	QString type = eleGroup.getType();
	QString groupId = BltForamt::blank(gId, 5);     ///=号后9位，gid在前5位

	(*stream) << str1;
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

	QString strr = "  F R E Q U E N C Y    A N A L Y S I S \n";
	strr += "\n\n\n\n\n";
	strr += "  E I G E N V A L U E S \n";
	int ct = count2%6;
	int ctn = count2/6;
	for(int i=0;i<ctn;i++){
		strr += "0";
		for(int j=0;j<6;j++){
			strr += BltForamt::sciNot(eigenValues(i*6+j), 15, 22);
		}
		strr += "\n";
	}
	if(ct!=0){
		strr += "0";
		int tt = count2-ct;
		for(int i=0;i<ct;i++){
			strr += BltForamt::sciNot(eigenValues(tt+i), 15, 22);
		}
		strr += "\n";
	}
	strr += "\n\n\n";

	strr += "   F R E Q U E N C I E S \n\n";
	strr += "      FREQUENCY NUMBER            FREQUENCY (RAD/SEC)      FREQUENCY (CYCLES/SEC)        PERIOD (SECONDS) \n\n";
	for(int i=0;i<count2;i++){
		double val = eigenValues(i);
		double freqR = pow(val,0.5);
		double freqC = freqR/2/3.1415926;
		double sec = 1/freqC;
		strr += BltForamt::blank(i+1,13) + "         ";
		strr += BltForamt::sciNot(freqR, 5, 26);
		strr += BltForamt::sciNot(freqC, 5, 26);
		strr += BltForamt::sciNot(sec, 5, 26);
		strr += "\n";
	}
	strr += "\n";
	strr += "      NOTE - READ TIME UNIT 'SEC' AS OTHER TIME UNIT IF OTHER TIME UNIT IS USED\n";
	strr += "\n\n\n\n\n";
	(*stream) << strr;


	for (int order=1; order<count; order++){

		QString str = QString("   M O D E  S H A P E  N O .   %1                                                       ").arg(order);
		str += "( FREQUENCY =  " + BltForamt::sciNot(pow(eigenValues(order-1),0.5), 5, 10) + " RAD/SEC )\n";
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

			str += BltForamt::blank(nId, 6) + " ";
			for (int dx = 0; dx < nodeDisV.getCount(); dx++){
				str += BltForamt::sciNot(double(nodeDisV(dx)), 7, 15);

			}
			str += "  0.000000E+00    GLOBAL    GLOBAL \n";
		}

		str.append("\n\n\n");
		(*stream) << str;
	}

	return true;
}

// ----------------------------------------------------------------------------------
// TODO NPAR 参数
QString MBltResultHeaderExportorImpl::Data::SolidEleMsg(int NEle, int GId) {

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
	eleMseg += " NUMBER OF ELEMENTS. . . . . . . . . . .( NPAR(2) ). . =" + BltForamt::blank(NEle, 5) + "\n\n";
	eleMseg += " TYPE OF NONLINEAR ANALYSIS  . . . . . .( NPAR(3) ). . =    0\n";
	eleMseg += "     EQ.0, LINEAR\n";
	eleMseg += "     EQ.1, MATERIAL NONLINEARITY ONLY\n";
	eleMseg += "     EQ.2, LARGE DISPLACEMENTS WITH\n";
	eleMseg += "           SMALL STRAINS\n";
	eleMseg += "     GE.3, LARGE DISPLACEMENTS WITH\n";
	eleMseg += "           LARGE STRAINS\n\n\n";
	eleMseg += " ELEMENT BIRTH AND DEATH OPTIONS . . . .( NPAR(4) ). . =    0\n";
	eleMseg += "     EQ.0, OPTION NOT ACTIVE\n";
	eleMseg += "     EQ.1, BIRTH OPTION ACTIVE\n";
	eleMseg += "     EQ.2, DEATH OPTION ACTIVE\n";
	eleMseg += "     EQ.3, DEATH AFTER BIRTH OPTION ACTIVE\n\n\n\n";
	eleMseg += " SKEW COORDINATE SYSTEM\n";
	eleMseg += "     REFERENCE INDICATOR . . . . . . . .( NPAR(6) ). . =    0\n";
	eleMseg += "     EQ.0, ALL ELEMENT NODES\n";
	eleMseg += "           USE THE GLOBAL SYSTEM ONLY\n";
	eleMseg += "     EQ.1, ELEMENT NODES REFER\n";
	eleMseg += "           TO SKEW COORDINATE SYSTEM\n\n";
	eleMseg += " MAX NUMBER OF NODES DESCRIBING\n";
	eleMseg += "     ANY ONE ELEMENT . . . . . . . . . .( NPAR(7) ). . =   12\n\n\n";
	eleMseg += " DEGENERATION INDICATOR  . . . . . . . .( NPAR(8) ). . =    0\n";
	eleMseg += "     EQ.0, NO DEGENERATION OR NO CORRECTION\n";
	eleMseg += "           FOR SPATIAL ISOTROPY\n";
	eleMseg += "     EQ.1, SPATIAL ISOTROPY CORRECTIONS\n";
	eleMseg += "           APPLIED TO SPECIALLY\n";
	eleMseg += "           DEGENERATED 20-NODE ELEMENTS\n\n";
	eleMseg += " FLAG FOR CALCULATION OF ENERGY\n";
	eleMseg += "     RELEASE RATES . . . . . . . . . . .( NPAR(9) ). . =    0\n";
	eleMseg += "     EQ.0, NO CALCULATION\n";
	eleMseg += "     EQ.1, CALCULATE ENERGY RELEASE RATES\n";
	eleMseg += "           AT NODAL POINTS\n\n\n";
	eleMseg += " INTEGRATION ORDER (R-S DIRECTIONS) FOR\n";
	eleMseg += "     ELEMENT STIFFNESS GENERATION. . . .( NPAR(10)). . =    4\n\n";
	eleMseg += " INTEGRATION ORDER (T DIRECTION) FOR\n";
	eleMseg += "     ELEMENT STIFFNESS GENERATION. . . .( NPAR(11)). . =    4\n\n";
	eleMseg += " NUMBER OF STRESS OUTPUT TABLES  . . . .( NPAR(13)). . =    1\n";
	eleMseg += "     EQ.-1, PRINT NODAL FORCES\n";
	eleMseg += "     EQ. 0, PRINT STRESSES AT INTEGRATION POINTS\n";
	eleMseg += "     GT. 0, PRINT STRESSES AT STRESS OUTPUT POINTS\n\n\n\n\n";
	eleMseg += " E L E M E N T   V A R I A B L E S\n\n\n";
	eleMseg += " VARIABLES FLAG. . . . . . . . . . . . .( NPAR(14)). . =    0\n";
	eleMseg += "     EQ. 0, DISPLACEMENT-BASED ELEMENT\n";
	eleMseg += "     NE. 0, NPAR(14) PRESSURE VARIABLES PER ELEMENT\n\n";
	eleMseg += " M A T E R I A L   D E F I N I T I O N\n\n\n";
	eleMseg += " MATERIAL MODEL. . . . . . . . . . . . .( NPAR(15)). . =    1\n";
	eleMseg += "     EQ. 1, LINEAR ELASTIC ISOTROPIC\n";
	eleMseg += "     EQ. 2, LINEAR ELASTIC ORTHOTROPIC\n";
	eleMseg += "     EQ. 3, THERMOELASTIC MODEL\n";
	eleMseg += "     EQ. 4, NONLINEAR CURVE DESCRIPTION MODEL\n";
	eleMseg += "     EQ. 5, CONCRETE CRACKING MODEL\n";
	eleMseg += "     EQ. 6, (EMPTY)\n";
	eleMseg += "     EQ. 7, DRUCKER PRAGER (CAP) MODEL\n";
	eleMseg += "     EQ. 8, ELASTIC-PLASTIC WITH ISOTROPIC HARDENING \n";
	eleMseg += "     EQ. 9, ELASTIC-PLASTIC WITH KINEMATIC HARDENING \n";
	eleMseg += "     EQ.10, ELASTIC-PLASTIC WITH CREEP (ISOTROPIC) \n";
	eleMseg += "     EQ.11, ELASTIC-PLASTIC WITH CREEP (KINEMATIC) \n";
	eleMseg += "     EQ.12, (EMPTY) \n";
	eleMseg += "     EQ.13, INCOMPRESSIBLE ELASTIC (MOONEY-RIVLIN) \n";
	eleMseg += "     EQ.14, USER-SUPPLIED MODEL \n";
	eleMseg += " \n";
	eleMseg += " NUMBER OF DIFFERENT SETS OF MATERIAL \n";
	eleMseg += "     CONSTANTS . . . . . . . . . . . . .( NPAR(16)). . =    1 \n";
	eleMseg += " \n";
	eleMseg += " NUMBER OF MATERIAL CONSTANTS PER SET. .( NPAR(17)). . =    2 \n";
	eleMseg += " \n";
	eleMseg += " NUMBER OF AXIS ORIENTATION SETS . . . .( NPAR(18)). . =    0 \n";
	eleMseg += " \n\n\n\n\n";
	eleMseg += " S T O R A G E   I N F O R M A T I O N \n";
	eleMseg += " \n\n";
	eleMseg += " LENGTH OF ARRAY NEEDED FOR STORING ELEMENT GROUP \n";
	eleMseg += " DATA (GROUP" + BltForamt::blank(GId,3) + "). . . . . . . . . . . . .( MIDEST ). . =     986 \n";
	eleMseg += " \n\n";
	eleMseg += " MATERIAL CONSTANTS SET NUMBER ....     1 \n";
	eleMseg += " \n";
	eleMseg += "     DEN ..........( DENSITY ).. =  0.000000E+00 \n";	// TODO
	eleMseg += " \n";
	eleMseg += "     E ............( PROP(1) ).. =  0.000000E+00 \n";	// TODO
	eleMseg += "     VNU ..........( PROP(2) ).. =  0.000000E+00 \n";	// TODO
	eleMseg += " \n\n\n\n\n";
	eleMseg += " S T R E S S   O U T P U T   T A B L E S \n";
	eleMseg += " \n";
	eleMseg += "     TABLE      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16 \n";
	eleMseg += " \n\n";
	eleMseg += "         1      1      2      3      4      5      6      7      8      0      0      0      0      0      0      0      0 \n";
	eleMseg += " \n\n\n";

	return eleMseg;
}
// TODO NPAR 参数
QString MBltResultHeaderExportorImpl::Data::RodEleMsg(int NEle, int GId)
{
	QString msg = " E L E M E N T   D E F I N I T I O N \n";
	msg += "\n\n";
	msg += " ELEMENT TYPE  . . . . . . . . . . . . .( NPAR(1) ). . =    1 \n";	// 单元类型
	msg += "     EQ.1, TRUSS ELEMENTS \n";
	msg += "     EQ.2, 2-DIM ELEMENTS \n";
	msg += "     EQ.3, 3-DIM ELEMENTS \n";
	msg += "     EQ.4, BEAM  ELEMENTS \n";
	msg += "     EQ.5, ISO/BEAM ELEMENTS \n";
	msg += "     EQ.6, PLATE ELEMENTS \n";
	msg += "     EQ.7, SHELL ELEMENTS \n";
	msg += "     EQ.8, PIPE ELEMENTS \n";
	msg += "     EQ.9, GENERAL ELEMENTS \n";
	msg += "     EQ.10, EMPTY \n";
	msg += "     EQ.11, 2-DIM FLUID ELEMENTS \n";
	msg += "     EQ.12, 3-DIM FLUID ELEMENTS \n";
	msg += "\n";
	msg += " NUMBER OF ELEMENTS. . . . . . . . . . .( NPAR(2) ). . =   " + BltForamt::blank(NEle, 5) + "\n";	// 单元个数
	msg += "\n";
	msg += " TYPE OF NONLINEAR ANALYSIS. . . . . . .( NPAR(3) ). . =    0 \n";	// 分析类型
	msg += "     EQ.0, LINEAR \n";
	msg += "     EQ.1, MATERIALLY NONLINEAR ONLY \n";
	msg += "     EQ.2, LARGE DISPLACEMENTS WITH \n";
	msg += "           SMALL STRAINS \n";
	msg += "\n";
	msg += " ELEMENT BIRTH AND DEATH OPTIONS . . . .( NPAR(4) ). . =    0 \n";
	msg += "     EQ.0, OPTION NOT ACTIVE \n";
	msg += "     EQ.1, BIRTH OPTION ACTIVE \n";
	msg += "     EQ.2, DEATH OPTION ACTIVE \n";
	msg += "     EQ.3, DEATH AFTER BIRTH OPTION ACTIVE \n";
	msg += "\n\n";
	msg += " ELEMENT TYPE CODE . . . . . . . . . . .( NPAR(5) ). . =    0 \n";
	msg += "     EQ.0, GENERAL 3-D TRUSS \n";
	msg += "     EQ.1, RING ELEMENT \n";
	msg += "\n";
	msg += " SKEW COORDINATE SYSTEM \n";
	msg += "     REFERENCE INDICATOR . . . . . . . .( NPAR(6) ). . =    0 \n";
	msg += "     EQ.0, ALL ELEMENT NODES \n";
	msg += "           USE THE GLOBAL SYSTEM ONLY \n";
	msg += "     EQ.1, ELEMENT NODES REFER \n";
	msg += "           TO SKEW COORDINATE SYSTEM \n";
	msg += "\n";
	msg += " MAXIMUM NUMBER OF NODES USED TO DESCRIBE \n";
	msg += "     ANY ONE ELEMENT . . . . . . . . . .( NPAR(7) ). . =    2 \n";
	msg += "\n";
	msg += " FLAG FOR ELEMENT GAP OPTION . . . . . .( NPAR(8) ). . =    0 \n";
	msg += "    EQ.0, ELEMENTS HAVE NO GAPS \n";
	msg += "    EQ.1, ELEMENTS HAVE GAPS \n";
	msg += "\n";
	msg += " INTEGRATION ORDER FOR STIFFNESS \n";
	msg += "     CALCULATION . . . . . . . . . . . .( NPAR(10)). . =    1 \n";
	msg += "\n\n\n";
	msg += " M A T E R I A L   D E F I N I T I O N \n";
	msg += "\n\n";
	msg += " MATERIAL MODEL. . . . . . . . . . . . .( NPAR(15)). . =    1 \n";
	msg += "    EQ.1,   LINEAR ELASTIC \n";
	msg += "    EQ.2,   NONLINEAR ELASTIC \n";
	msg += "            (STRESS-STRAIN LAW SPECIFIED) \n";
	msg += "    EQ.3,   THERMOELASTIC \n";
	msg += "    EQ.4,   ELASTIC-PLASTIC (ISOTROPIC) \n";
	msg += "    EQ.5,   ELASTIC-PLASTIC (KINEMATIC) \n";
	msg += "    EQ.6,   ELASTIC-PLASTIC-CREEP (ISOTROPIC) \n";
	msg += "    EQ.7,   ELASTIC-PLASTIC-CREEP (KINEMATIC) \n";
	msg += "\n\n";
	msg += " NUMBER OF DIFFERENT SETS OF MATERIAL \n";
	msg += "     CONSTANTS . . . . . . . . . . . . .( NPAR(16)). . =    1 \n";
	msg += "\n";
	msg += " NUMBER OF MATERIAL CONSTANTS PER SET. .( NPAR(17)). . =    1 \n";
	msg += "\n\n\n\n";
	msg += " LENGTH OF ARRAY NEEDED FOR STORING ELEMENT GROUP \n";
	msg += " DATA (GROUP " + BltForamt::blank(GId,3) + "). . . . . . . . . . . . .( MIDEST ). . =    1226 \n";
	msg += "\n\n\n\n\n\n";
	msg += " M A T E R I A L    C O N S T A N T S \n";
	msg += "\n\n\n";
	msg += "  SET           AREA            DEN              E \n";
	msg += "\n";
	msg += "    1   0.000000E+00   0.000000E+00   0.000000E+00 \n";	// TODO
	msg += "\n\n\n";

	return msg;
}
// TODO NPAR 参数
QString MBltResultHeaderExportorImpl::Data::ShellEleMsg(int NEle, int GId){
	QString msg = " E L E M E N T  D E F I N I T I O N \n";
	msg += "\n\n";
	msg += " ELEMENT TYPE  . . . . . . . . . . . . .( NPAR(1) ). . =    6 \n";
	msg += "     EQ.1,      TRUSS ELEMENTS \n";
	msg += "     EQ.2,      2-DIM ELEMENTS \n";
	msg += "     EQ.3,      3-DIM ELEMENTS \n";
	msg += "     EQ.4,      BEAM  ELEMENTS \n";
	msg += "     EQ.5,      ISO/BEAM ELEMENTS \n";
	msg += "     EQ.6,      PLATE ELEMENTS \n";
	msg += "     EQ.7,      SHELL ELEMENTS \n";
	msg += "     EQ.8,      PIPE ELEMENTS \n";
	msg += "     EQ.9,      GENERAL ELEMENTS \n";
	msg += "     EQ.10,     EMPTY \n";
	msg += "     EQ.11,     2-DIM FLUID ELEMENTS \n";
	msg += "     EQ.12,     3-DIM FLUID ELEMENTS \n";
	msg += "\n";
	msg += " NUMBER OF ELEMENTS. . . . . . . . . . .( NPAR(2) ). . =   " + BltForamt::blank(NEle, 5) + " \n";
	msg += "\n\n";
	msg += " TYPE OF ANALYSIS  . . . . . . . . . . .( NPAR(3) ). . =    0 \n";
	msg += "     EQ.0, LINEAR \n";
	msg += "     EQ.1, MATERIAL NONLINEARITY ONLY \n";
	msg += "     EQ.2, LARGE DISPLACEMENTS AND ROTATIONS \n";
	msg += "           WITH SMALL STRAINS \n";
	msg += "\n";
	msg += " ELEMENT BIRTH AND DEATH OPTION  . . . .( NPAR(4) ). . =    0 \n";
	msg += "     EQ.0, OPTION NOT ACTIVE \n";
	msg += "     EQ.1, BIRTH OPTION ACTIVE \n";
	msg += "     EQ.2, DEATH OPTION ACTIVE \n";
	msg += "     EQ.3, DEATH AFTER BIRTH OPTION ACTIVE \n";
	msg += "\n\n";
	msg += " SKEW COORDINATE SYSTEM \n";
	msg += " REFERENCE INDICATOR . . . . . . . . . .( NPAR(6) ). . =    0 \n";
	msg += "     EQ.0, GLOBAL COORDINATE SYSTEM \n";
	msg += "     EQ.1, SKEW   COORDINATE SYSTEM \n";
	msg += "\n";
	msg += " INTEGRATION SCHEME FOR \n";
	msg += " STIFFNESS CALCULATION . . . . . . . . .( NPAR(10)). . =    4 \n";
	msg += "     EQ.1, AT CENTROID ONLY        (1 POINT ) \n";
	msg += "     EQ.2, AT 3 INTERIOR LOCATIONS (3 POINTS) \n";
	msg += "     EQ.3, AT 3 MID-SIDE LOCATIONS (3 POINTS) \n";
	msg += "     EQ.4, AT 7 INTERIOR LOCATIONS (7 POINTS) \n";
	msg += "\n\n";
	msg += " NUMBER OF STRESS OUTPUT TABLES  . . . .( NPAR(13)). . =    1 \n";
	msg += "     EQ.-1, PRINT NODAL FORCES \n";
	msg += "     EQ. 0, PRINT AT INTEGRATION POINTS \n";
	msg += "     GT. 0, PRINT AT OUTPUT POINTS \n";
	msg += "\n";
	msg += " MATERIAL MODEL. . . . . . . . . . . . .( NPAR(15)). . =    1 \n";
	msg += "     EQ.1, HOMOGENEOUS LINEAR ISOTROPIC \n";
	msg += "     EQ.2, HOMOGENEOUS LINEAR ORTHOTROPIC \n";
	msg += "     EQ.3, ISOTROPIC ELASTIC-PLASTIC \n";
	msg += "     EQ.4, EMPTY MODEL \n";
	msg += "\n";
	msg += " NUMBER OF DIFFERENT SETS OF MATERIAL \n";
	msg += "     CONSTANTS . . . . . . . . . . . . .( NPAR(16)). . =    1 \n";
	msg += "\n\n\n\n\n";
	msg += " S T O R A G E   I N F O R M A T I O N \n";
	msg += "\n\n\n";
	msg += " LENGTH OF ARRAY NEEDED FOR STORING ELEMENT GROUP \n";
	msg += " DATA (GROUP  " + BltForamt::blank(GId,3) + "). . . . . . . . . . . . .( MIDEST ). . =     693 \n";
	msg += "\n\n\n\n";
	msg += " M A T E R I A L   C O N S T A N T S \n";
	msg += "\n\n";
	msg += " MATERIAL PROPERTY SET NO.    1 \n";
	msg += "\n";
	msg += "      DENSITY =    0.000000E+00 \n";	// TODO
	msg += "\n\n";
	msg += " ISOTROPIC PROPERTIES \n";
	msg += "\n";
	msg += " YOUNG*S MODULUS   E =    0.000000E+00 \n";	// TODO
	msg += " POISSON*S RATIO VNU =    0.000000E+00 \n";	// TODO
	msg += "\n\n\n\n\n";
	msg += " S T R E S S   O U T P U T   T A B L E \n";
	msg += "\n";
	msg += "     TABLE         1         2         3         4         5         6         7 \n";
	msg += "\n";
	msg += "         1         1         2         3         0         0         0         0 \n";
	msg += "\n\n\n";

	return msg;
}
