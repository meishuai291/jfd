#ifndef ORG_SIPESC_FEMS_HEXABRICKELEMENT12_H
#define ORG_SIPESC_FEMS_HEXABRICKELEMENT12_H

#include <mplugin.h>

class OrgSipescFemsHexabrickelement12Plugin:public MPluginObject
  {
  public:
    OrgSipescFemsHexabrickelement12Plugin();
    virtual ~OrgSipescFemsHexabrickelement12Plugin();
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
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12StrainMatrix();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleConstiDParser();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12ShpFunction();
    MExtensionObject* createOrgSipescFemsHexaelement12MGau3D12IntegForm();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12NodeStressCalculator();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12LumpedMassMatrix();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleVolume();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleStiffCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleStrainCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleStressCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleMassCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12StrainMatrixFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleConstiDParserFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12ShpFunctionFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MGau3D12IntegFormFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12NodeStressCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12LumpedMassMatrixFactory();
    MExtensionObject* createOrgSipescFemsHexaelement12MHexaBrick12EleVolumeFactory();
  private:
    bool _running;
  };

#endif /* ORG_SIPESC_FEMS_HEXABRICKELEMENT12_H */
