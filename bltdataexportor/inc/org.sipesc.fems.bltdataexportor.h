#ifndef ORG_SIPESC_FEMS_BLTDATAEXPORTOR_H
#define ORG_SIPESC_FEMS_BLTDATAEXPORTOR_H

#include <mplugin.h>

class OrgSipescFemsBltdataexportorPlugin:public MPluginObject
  {
  public:
    OrgSipescFemsBltdataexportorPlugin();
    virtual ~OrgSipescFemsBltdataexportorPlugin();
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
    MExtensionObject* createOrgSipescFemsBltexportMBltDataExportor();
  private:
    bool _running;
  };

#endif /* ORG_SIPESC_FEMS_BLTDATAEXPORTOR_H */
