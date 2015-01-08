/*
 * mfemsglobalextension.cxx
 *
 *  Created on: 2013-11-11
 *      Author: steven
 */

#include <mfemsglobalextension.h>

MExtensionManager globalExtensionManager = MExtensionManager::getManager();
MObjectManager globalObjManager = MObjectManager::getManager();

MExtension getFemsGlobal()
  {
  QLatin1String femsname("org.sipesc.fems.global.femsglobal");
  return globalObjManager.getObject(femsname);
  }

MExtension getElementGlobal()
  {
  QLatin1String elename("org.sipesc.fems.global.elementsglobal");
  return globalObjManager.getObject(elename);
  }

MExtension getExtensionFactoryManager()
  {
  QLatin1String name("org.sipesc.utilities.MExtensionFactoryManager");
  return globalExtensionManager.createExtension(name);
  }

MExtension getDataFactoryManager()
  {
  QLatin1String name("org.sipesc.core.engdbs.mdatafactorymanager");
  return globalObjManager.getObject(name);
  }

MExtension getUtilityManager()
  {
  QLatin1String name("org.sipesc.core.utility.utilitymanager");
  return globalObjManager.getObject(name);
  }

MExtension getBaseManager()
  {
  QLatin1String name("org.sipesc.core.engdbs.mdatabasemanager");
  return globalObjManager.getObject(name);
  }

MExtension getVectorFactory()
  {
  QLatin1String name("org.sipesc.fems.matrix.vectorfactory");
  return globalObjManager.getObject(name);
  }

MExtension getMatrixFactory()
  {
  QLatin1String name("org.sipesc.fems.matrix.matrixfactory");
  return globalObjManager.getObject(name);
  }
