/*
 * mjfdsglobal.h
 *
 *  Created on: 2013-1-17
 *      Author: cyz
 */

#ifndef MJFDSGLOBAL_H_
#define MJFDSGLOBAL_H_

#include<qstring.h>
/*
 * 定义JFD Importor的公用变量
 */
namespace MJFDS
{

const QString CdbTempPath = "CdbTempPath";//CDB存储数据临时路径
const QString RLBLOCK = "RLBLOCK"; //存放CDB几何性质
const QString ET = "ET"; //存放单元类型Map模块数据：在*.cdb文件中以ET开头的

//const QString NodeLoad = "1"; //存放节点集中载荷
//const QString ACELLoad = "2"; //存放加速度载荷
//const QString SFELoad = "3"; //存放面载荷

const QString StruMP = "StruMP";
const QString TherMP = "TherMP";
const QString TB = "TB";



}

#endif /* MJFDSGLOBAL_H_ */
