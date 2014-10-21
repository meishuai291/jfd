#include <stdio.h>
#include <debugger.h>
#include <mobjectmanager.h>

Debugger::Debugger()
  {
  }

Debugger::~Debugger()
  {
  }

void Debugger::onLoadingPlugin(const QString& name)
  {
  printf("Loading plugin: %s\n", qPrintable(name));
  }

void Debugger::onUnloadingPlugin(const QString& name)
  {
//  if(name=="org.sipesc.core.utility_2.0.0")
//    {
//    MObjectManager m=MObjectManager::getManager();
//    QStringList names=m.getAvailableObjects();
//    if(!names.isEmpty())
//      {
//      printf("Objects not unregistered:\n");
//      foreach(QString n, names) printf("  %s\n", qPrintable(n));
//      }
//    }
  printf("Unloading plugin: %s\n", qPrintable(name));
  }
