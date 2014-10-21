#ifndef ORG_SIPESC_FEMS_SCRIPTASSIST_H
#define ORG_SIPESC_FEMS_SCRIPTASSIST_H

#include <mplugin.h>

class OrgSipescFemsScriptassistPlugin:public MPluginObject
  {
  public:
    OrgSipescFemsScriptassistPlugin();
    virtual ~OrgSipescFemsScriptassistPlugin();
  public:
    bool initialize();
    bool cleanup();
    void start();
    void stop();
    void setupMetaTypes();
  public:
    QByteArray getPluginXml() const;
    QStringList getRequiredFeatures() const;
    QString getTextStringForId(qint32) const;
    QVariant getData(const QString& text) const;
    int convertData(QVariant&, const QString& type) const;
  protected:
    QVariant callMethod(MExtensionObject*, const QString&, const QString&, QList<void*>&);
    MExtensionObject* createObject(const QString&);
    MExtensionObject* createOrgSipescFemsMatrixassistMVectorAssistor();
    MExtensionObject* createOrgSipescFemsMatrixassistMMatrixAssistor();
    MExtensionObject* createOrgSipescFemsImportorassistMDataImportAssistor();
    MExtensionObject* createOrgSipescFemsImportorassistMTransDataImport();
  private:
    bool _running;
  };

#endif /* ORG_SIPESC_FEMS_SCRIPTASSIST_H */
