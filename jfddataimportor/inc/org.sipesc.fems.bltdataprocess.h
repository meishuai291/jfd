#ifndef ORG_SIPESC_FEMS_BLTDATAPROCESS_H
#define ORG_SIPESC_FEMS_BLTDATAPROCESS_H

#include <mplugin.h>

class OrgSipescFemsBltdataprocessPlugin:public MPluginObject
  {
  public:
    OrgSipescFemsBltdataprocessPlugin();
    virtual ~OrgSipescFemsBltdataprocessPlugin();
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
    MExtensionObject* createOrgSipescFemsJfdimportMJfdDataImportorFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdDataImportor();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdNodeHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdNodeHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdMPCHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdMPCHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdLineElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdLineElementHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfd3DSolidElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfd3DSolidElementHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdNLineElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdNLineElementHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdCrodElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdCrodElementHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdForceHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdForceHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdGravHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdGravHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMTABLED5Parser();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdShellElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdShellElementHandler();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdBeamElementHandlerFactory();
    MExtensionObject* createOrgSipescFemsJfdimportMJfdBeamElementHandler();
    MExtensionObject* createOrgSipescFemsBltexportMBltDataExportor();
  private:
    bool _running;
  };

#endif /* ORG_SIPESC_FEMS_BLTDATAPROCESS_H */
