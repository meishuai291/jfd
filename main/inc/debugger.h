#ifndef DEBUGER_H_
#define DEBUGER_H_

#include <qobject.h>

class Debugger:public QObject
  {
    Q_OBJECT
  public:
    Debugger();
    virtual ~Debugger();
  private slots:
    void onLoadingPlugin(const QString&);
    void onUnloadingPlugin(const QString&);
  };
#endif /* DEBUGER_H_ */
