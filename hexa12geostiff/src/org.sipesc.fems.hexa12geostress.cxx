#include <org.sipesc.fems.hexa12geostress.h>
#include <mhexabrick12elestressstiffcalculatorfactoryimpl.h>
#include <mhexabrick12elestressstiffgmatriximpl.h>
#include <mhexabrick12gmatrixfactoryimpl.h>
#include <mhexabrick12interforcecalculatorfactoryimpl.h>
#include <mhexabrick12eledeformgradtensorcalculatorfactoryimpl.h>
#include <mhexabrick12elejaccobianmatrixfactoryimpl.h>
#include <mhexabrick12elejaccobianmatriximpl.h>
#include <mbrickelecauchystresscalculatorfactoryimpl.h>
#include <mbrickelegreenstraincalculatorfactoryimpl.h>
#include <mhexabrick12smatrixfactoryimpl.h>

OrgSipescFemsHexa12geostressPlugin::OrgSipescFemsHexa12geostressPlugin()
  {
  _running=false;
  }

OrgSipescFemsHexa12geostressPlugin::~OrgSipescFemsHexa12geostressPlugin()
  {
  }

bool OrgSipescFemsHexa12geostressPlugin::initialize()
  {
  return true;
  }

bool OrgSipescFemsHexa12geostressPlugin::cleanup()
  {
  return true;
  }

void OrgSipescFemsHexa12geostressPlugin::start()
  {
  if(_running) return;
  _running=true;
  }

void OrgSipescFemsHexa12geostressPlugin::stop()
  {
  if(!_running) return;
  _running=false;
  }

QStringList OrgSipescFemsHexa12geostressPlugin::
getRequiredFeatures() const
  {
	  QStringList result;
	  result.append(QLatin1String("org.sipesc.utilities"));
	  result.append(QLatin1String("org.sipesc.core.utility"));
	  result.append(QLatin1String("org.sipesc.core.engdbs.data"));
	  result.append(QLatin1String("org.sipesc.fems.data"));
	  result.append(QLatin1String("org.sipesc.fems.matrix"));
	  result.append(QLatin1String("org.sipesc.fems.matrictensor"));
	  result.append(QLatin1String("org.sipesc.fems.element"));
	  result.append(QLatin1String("org.sipesc.fems.global"));
	  result.append(QLatin1String("org.sipesc.fems.hexaelement12"));
	  return result;
   }

MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleStressStiffCalculatorFactory()
  {
  return new MHexaBrick12EleStressStiffCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12GMatrixFactory()
  {
  return new MHexaBrick12GMatrixFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleStressStiffGMatrix()
  {
  return new MHexaBrick12EleStressStiffGMatrixImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12SMatrixFactory()
  {
  return new MHexaBrick12SMatrixFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleInterForceCalculatorFactory()
  {
  return new MHexaBrick12EleInterForceCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleDeformGradTensorCalculatorFactory()
  {
  return new MHexaBrick12EleDeformGradTensorCalculatorFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleJaccobianMatrixFactory()
  {
  return new MHexaBrick12EleJaccobianMatrixFactoryImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMHexaBrick12EleJaccobianMatrix()
  {
  return new MHexaBrick12EleJaccobianMatrixImpl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMBrickEleGreenStrainCalculatorFactory()
  {
  return new MBrickEleGreenStrainCalculatorFactory2Impl;
  }
MExtensionObject* OrgSipescFemsHexa12geostressPlugin
::createOrgSipescFemsHexa12geostressMBrickEleCauchyStressCalculatorFactory()
  {
  return new MBrickEleCauchyStressCalculatorFactory2Impl;
  }
