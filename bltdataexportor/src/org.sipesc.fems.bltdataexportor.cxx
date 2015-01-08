#include <org.sipesc.fems.bltdataexportor.h>

#include <mbltdataexportorimpl.h>

OrgSipescFemsBltdataexportorPlugin::OrgSipescFemsBltdataexportorPlugin()
  {
  _running=false;
  }

OrgSipescFemsBltdataexportorPlugin::~OrgSipescFemsBltdataexportorPlugin()
  {
  }

bool OrgSipescFemsBltdataexportorPlugin::initialize()
  {
  return true;
  }

bool OrgSipescFemsBltdataexportorPlugin::cleanup()
  {
  return true;
  }

void OrgSipescFemsBltdataexportorPlugin::start()
  {
  if(_running) return;
  _running=true;
  }

void OrgSipescFemsBltdataexportorPlugin::stop()
  {
  if(!_running) return;
  _running=false;
  }

QStringList OrgSipescFemsBltdataexportorPlugin::
getRequiredFeatures() const
  {
	  QStringList result;
	  result.append(QLatin1String("org.sipesc.utilities"));
	  result.append(QLatin1String("org.sipesc.core.utility"));
	  result.append(QLatin1String("org.sipesc.core.engdbs.data"));
	  result.append(QLatin1String("org.sipesc.fems.data"));
	  result.append(QLatin1String("org.sipesc.fems.matrix"));
	  result.append(QLatin1String("org.sipesc.fems.global"));
	  result.append(QLatin1String("org.sipesc.fems.femsutils"));

	  return result;
  }

MExtensionObject* OrgSipescFemsBltdataexportorPlugin
::createOrgSipescFemsBltexportMBltDataExportor()
  {
  return new MBltDataExportorImpl;
  }
