#include <org.sipesc.fems.jfddataimportor.h>

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

OrgSipescFemsJfddataimportorPlugin::OrgSipescFemsJfddataimportorPlugin()
  {
  _running=false;
  }

OrgSipescFemsJfddataimportorPlugin::~OrgSipescFemsJfddataimportorPlugin()
  {
  }

bool OrgSipescFemsJfddataimportorPlugin::initialize()
  {
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	return true;
  }

bool OrgSipescFemsJfddataimportorPlugin::cleanup()
  {
  return true;
  }

void OrgSipescFemsJfddataimportorPlugin::start()
  {
  if(_running) return;
  _running=true;
  }

void OrgSipescFemsJfddataimportorPlugin::stop()
  {
  if(!_running) return;
  _running=false;
  }

QStringList OrgSipescFemsJfddataimportorPlugin::
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

MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdDataImportorFactory()
  {
  return new MJfdDataImportorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdDataImportor()
  {
  return new MJfdDataImportorImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdNodeHandlerFactory()
  {
  return new MJfdNodeHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdNodeHandler()
  {
  return new MJfdNodeHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdMPCHandlerFactory()
  {
  return new MJfdMPCHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdMPCHandler()
  {
  return new MJfdMPCHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdLineElementHandlerFactory()
  {
  return new MJfdLineElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdLineElementHandler()
  {
  return new MJfdLineElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfd3DSolidElementHandlerFactory()
  {
  return new MJfd3DSolidElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfd3DSolidElementHandler()
  {
  return new MJfd3DSolidElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdNLineElementHandlerFactory()
  {
  return new MJfdNLineElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdNLineElementHandler()
  {
  return new MJfdNLineElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdCrodElementHandlerFactory()
  {
  return new MJfdCrodElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdCrodElementHandler()
  {
  return new MJfdCrodElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdForceHandlerFactory()
  {
  return new MJfdForceHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdForceHandler()
  {
  return new MJfdForceHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdGravHandlerFactory()
  {
  return new MJfdGravHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdGravHandler()
  {
  return new MJfdGravHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMTABLED5Parser()
  {
  return new MTABLED5ParserImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdShellElementHandlerFactory()
  {
  return new MJfdShellElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdShellElementHandler()
  {
  return new MJfdShellElementHandlerImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdBeamElementHandlerFactory()
  {
  return new MJfdBeamElementHandlerFactoryImpl;
  }
MExtensionObject* OrgSipescFemsJfddataimportorPlugin
::createOrgSipescFemsJfdimportMJfdBeamElementHandler()
  {
  return new MJfdBeamElementHandlerImpl;
  }
