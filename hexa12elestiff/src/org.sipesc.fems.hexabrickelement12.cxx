#include <mobjectmanager.h>
#include <org.sipesc.core.engdbs.data.mdatafactorymanager.h>
#include <org.sipesc.fems.hexabrickelement12.h>

#include <mhexabrick12eleconstidparserfactoryimpl.h>
#include <mhexabrick12eleconstidparserimpl.h>
#include <mhexabrick12elemasscalculatorfactoryimpl.h>
#include <mhexabrick12elementdataimpl.h>
#include <mhexabrick12elestiffcalculatorfactoryimpl.h>
#include <mhexabrick12elestraincalculatorfactoryimpl.h>
#include <mhexabrick12elestresscalculatorfactoryimpl.h>
#include <mhexabrick12elevolumefactoryimpl.h>
#include <mhexabrick12elevolumeimpl.h>
#include <mgau3d12integformfactoryimpl.h>
#include <mgau3d12integformimpl.h>
#include <mhexabrick12lumpedmassmatrixfactoryimpl.h>
#include <mhexabrick12lumpedmassmatriximpl.h>
#include <mhexabrick12nodestresscalculatorfactoryimpl.h>
#include <mhexabrick12nodestresscalculatorimpl.h>
#include <mhexabrick12shpfunctionfactoryimpl.h>
#include <mhexabrick12shpfunctionimpl.h>
#include <mhexabrick12strainmatrixfactoryimpl.h>
#include <mhexabrick12strainmatriximpl.h>

OrgSipescFemsHexabrickelement12Plugin::OrgSipescFemsHexabrickelement12Plugin()
  {
  _running=false;
  }

OrgSipescFemsHexabrickelement12Plugin::~OrgSipescFemsHexabrickelement12Plugin()
  {
  }

bool OrgSipescFemsHexabrickelement12Plugin::initialize()
  {
  return true;
  }

bool OrgSipescFemsHexabrickelement12Plugin::cleanup()
  {
  return true;
  }

void OrgSipescFemsHexabrickelement12Plugin::start()
  {
  if(_running) return;

  MObjectManager objectManager = MObjectManager::getManager();
  MDataFactoryInterface* dataImpl;
  QString name = "org.sipesc.core.engdbs.mdatafactorymanager";
  MDataFactoryManager factoryManager = objectManager.getObject(name);

  name = "org.sipesc.fems.data.hexabrick12elementdata";
  dataImpl = new MHexaBrick12ElementDataFactoryImpl;
  factoryManager.registerFactory(name, MDataFactory(dataImpl));

  _running=true;
  }

void OrgSipescFemsHexabrickelement12Plugin::stop()
  {
  if(!_running) return;

  MObjectManager objectManager = MObjectManager::getManager();
  MDataFactoryInterface* dataImpl;
  QString name = "org.sipesc.core.engdbs.mdatafactorymanager";
  MDataFactoryManager factoryManager = objectManager.getObject(name);

  name = "org.sipesc.fems.data.hexabrick12elementdata";
  factoryManager.unregisterFactory(name);

  _running=false;
  }

QStringList OrgSipescFemsHexabrickelement12Plugin::
getRequiredFeatures() const
  {
	  QStringList result;
	  result.append(QLatin1String("org.sipesc.utilities"));
	  result.append(QLatin1String("org.sipesc.core.utility"));
	  result.append(QLatin1String("org.sipesc.core.engdbs.data"));
	  result.append(QLatin1String("org.sipesc.fems.data"));
	  result.append(QLatin1String("org.sipesc.fems.matrix"));
	  result.append(QLatin1String("org.sipesc.fems.element"));
	  result.append(QLatin1String("org.sipesc.fems.global"));
	  result.append(QLatin1String("org.sipesc.fems.femsutils"));
	  result.append(QLatin1String("org.sipesc.fems.matrictensor"));
	  return result;
  }

MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12StrainMatrix()
  {
  return new MHexaBrick12StrainMatrixImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleConstiDParser()
  {
  return new MHexaBrick12EleConstiDParserImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12ShpFunction()
  {
  return new MHexaBrick12ShpFunctionImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MGau3D12IntegForm()
  {
  return new MGau3D12IntegFormImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12NodeStressCalculator()
  {
  return new MHexaBrick12NodeStressCalculatorImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12LumpedMassMatrix()
  {
  return new MHexaBrick12LumpedMassMatrixImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleVolume()
  {
  return new MHexaBrick12EleVolumeImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleStiffCalculatorFactory()
  {
  return new MHexaBrick12EleStiffCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleStrainCalculatorFactory()
  {
  return new MHexaBrick12EleStrainCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleStressCalculatorFactory()
  {
  return new MHexaBrick12EleStressCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleMassCalculatorFactory()
  {
  return new MHexaBrick12EleMassCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12StrainMatrixFactory()
  {
  return new MHexaBrick12StrainMatrixFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleConstiDParserFactory()
  {
  return new MHexaBrick12EleConstiDParserFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12ShpFunctionFactory()
  {
  return new MHexaBrick12ShpFunctionFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MGau3D12IntegFormFactory()
  {
  return new MGau3D12IntegFormFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12NodeStressCalculatorFactory()
  {
  return new MHexaBrick12NodeStressCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12LumpedMassMatrixFactory()
  {
  return new MHexaBrick12LumpedMassMatrixFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexabrickelement12Plugin
::createOrgSipescFemsHexaelement12MHexaBrick12EleVolumeFactory()
  {
  return new MHexaBrick12EleVolumeFactoryImpl;
  }
