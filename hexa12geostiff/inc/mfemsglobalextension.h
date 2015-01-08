/*
 * mfemsglobalextension.h
 *
 *  Created on: 2013-11-11
 *      Author: steven
 */

#ifndef MFEMSGLOBALEXTENSION_H_
#define MFEMSGLOBALEXTENSION_H_

#include <mextensionmanager.h>
#include <mobjectmanager.h>

extern MExtensionManager globalExtensionManager;

extern MObjectManager globalObjManager;

extern MExtension getFemsGlobal();

extern MExtension getElementGlobal();

extern MExtension getExtensionFactoryManager();

extern MExtension getDataFactoryManager();

extern MExtension getUtilityManager();

extern MExtension getBaseManager();

extern MExtension getVectorFactory();

extern MExtension getMatrixFactory();

#endif /* MFEMSGLOBALEXTENSION_H_ */
