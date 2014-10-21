#include <org.sipesc.fems.scriptassist.h>

#include <mvectorassistorimpl.h>
#include <mmatrixassistorimpl.h>
#include <mdataimportassistorimpl.h>
#include <mtransdataimportimpl.h>

OrgSipescFemsScriptassistPlugin::OrgSipescFemsScriptassistPlugin()
{
	_running = false;
}

OrgSipescFemsScriptassistPlugin::~OrgSipescFemsScriptassistPlugin()
{
}

bool OrgSipescFemsScriptassistPlugin::initialize()
{
	return true;
}

bool OrgSipescFemsScriptassistPlugin::cleanup()
{
	return true;
}

void OrgSipescFemsScriptassistPlugin::start()
{
	if (_running)
		return;
	_running = true;
}

void OrgSipescFemsScriptassistPlugin::stop()
{
	if (!_running)
		return;
	_running = false;
}

QStringList OrgSipescFemsScriptassistPlugin::getRequiredFeatures() const
{
	return QStringList();
}

MExtensionObject* OrgSipescFemsScriptassistPlugin::createOrgSipescFemsMatrixassistMVectorAssistor()
{
	return new MVectorAssistorImpl;
}
MExtensionObject* OrgSipescFemsScriptassistPlugin::createOrgSipescFemsMatrixassistMMatrixAssistor()
{
	return new MMatrixAssistorImpl;
}
MExtensionObject* OrgSipescFemsScriptassistPlugin::createOrgSipescFemsImportorassistMDataImportAssistor()
{
	return new MDataImportAssistorImpl;
}
MExtensionObject* OrgSipescFemsScriptassistPlugin::createOrgSipescFemsImportorassistMTransDataImport()
{
	return new MTransDataImportImpl;
}
