#include <org.sipesc.fems.bltdataprocess.h>

#include <QTextCodec>

#include <mjfddataimportorfactoryimpl.h>
#include <mjfddataimportorimpl.h>
#include <mjfdnodehandlerfactoryimpl.h>
#include <mjfdnodehandlerimpl.h>
#include <mjfdmpchandlerfactoryimpl.h>
#include <mjfdmpchandlerimpl.h>
#include <mjfdlineelementhandlerfactoryimpl.h>
#include <mjfdlineelementhandlerimpl.h>
#include <mjfd3dsolidelementhandlerfactoryimpl.h>
#include <mjfd3dsolidelementhandlerimpl.h>
#include <mjfdnlineelementhandlerfactoryimpl.h>
#include <mjfdnlineelementhandlerimpl.h>
#include <mjfdcrodelementhandlerfactoryimpl.h>
#include <mjfdcrodelementhandlerimpl.h>
#include <mjfdforcehandlerfactoryimpl.h>
#include <mjfdforcehandlerimpl.h>
#include <mjfdgravhandlerfactoryimpl.h>
#include <mjfdgravhandlerimpl.h>
#include <mtabled5parserimpl.h>
//
#include <mjfdshellelementhandlerfactoryimpl.h>
#include <mjfdshellelementhandlerimpl.h>
#include <mjfdbeamelementhandlerimpl.h>
#include <mjfdbeamelementhandlerfactoryimpl.h>


//
#include <mbltdataexportorimpl.h>
//#include <mjfddataexportorimpl.h>
//#include <mjfdnodedataexportorimpl.h>

OrgSipescFemsBltdataprocessPlugin::OrgSipescFemsBltdataprocessPlugin()
  {
  _running=false;
  }

OrgSipescFemsBltdataprocessPlugin::~OrgSipescFemsBltdataprocessPlugin()
  {
  }

bool OrgSipescFemsBltdataprocessPlugin::initialize()
  {
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  return true;
  }

bool OrgSipescFemsBltdataprocessPlugin::cleanup()
  {
  return true;
  }

void OrgSipescFemsBltdataprocessPlugin::start()
  {
  if(_running) return;
  _running=true;
  }

void OrgSipescFemsBltdataprocessPlugin::stop()
  {
  if(!_running) return;
  _running=false;
  }

QStringList OrgSipescFemsBltdataprocessPlugin::
getRequiredFeatures() const
  {
	  QStringList result;
	  result.append(QLatin1String("org.sipesc.utilities"));
	  result.append(QLatin1String("org.sipesc.core.utility"));
	  result.append(QLatin1String("org.sipesc.core.engdbs.data"));
	  result.append(QLatin1String("org.sipesc.fems.data"));
	  result.append(QLatin1String("org.sipesc.fems.matrix"));
	  result.append(QLatin1String("org.sipesc.fems.bdfimport"));
	  result.append(QLatin1String("org.sipesc.fems.femsutils"));
	  result.append(QLatin1String("org.sipesc.fems.global"));
	  return result;
  }

MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdDataImportorFactory()
  {
	  return new MJfdDataImportorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdDataImportor()
  {
	  return new MJfdDataImportorImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdNodeHandlerFactory()
  {
	  return new MJfdNodeHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdNodeHandler()
  {
	  return new MJfdNodeHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdMPCHandlerFactory()
  {
	  return new MJfdMPCHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdMPCHandler()
  {
	  return new MJfdMPCHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdLineElementHandlerFactory()
  {
	  return new MJfdLineElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdLineElementHandler()
  {
	  return new MJfdLineElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfd3DSolidElementHandlerFactory()
  {
	  return new MJfd3DSolidElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfd3DSolidElementHandler()
  {
	  return new MJfd3DSolidElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdNLineElementHandlerFactory()
  {
	  return new MJfdNLineElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdNLineElementHandler()
  {
	  return new MJfdNLineElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdCrodElementHandlerFactory()
  {
	  return new MJfdCrodElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdCrodElementHandler()
  {
	  return new MJfdCrodElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdForceHandlerFactory()
  {
	  return new MJfdForceHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdForceHandler()
  {
	  return new MJfdForceHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdGravHandlerFactory()
  {
	  return new MJfdGravHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdGravHandler()
  {
	  return new MJfdGravHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMTABLED5Parser()
  {
	return new MTABLED5ParserImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdShellElementHandlerFactory()
  {
  return new MJfdShellElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdShellElementHandler()
  {
  return new MJfdShellElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdBeamElementHandlerFactory()
  {
  return new MJfdBeamElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsJfdimportMJfdBeamElementHandler()
  {
  return new MJfdBeamElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsBltdataprocessPlugin
::createOrgSipescFemsBltexportMBltDataExportor()
  {
  return new MBltDataExportorImpl;
  }
