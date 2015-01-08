#ifndef ORG_SIPESC_FEMS_HEXA12GEOSTRESS_H
#define ORG_SIPESC_FEMS_HEXA12GEOSTRESS_H

#include <mplugin.h>

class OrgSipescFemsHexa12geostressPlugin:public MPluginObject
  {
  public:
    OrgSipescFemsHexa12geostressPlugin();
    virtual ~OrgSipescFemsHexa12geostressPlugin();
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
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleStressStiffCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12GMatrixFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12SMatrixFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleStressStiffGMatrix();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleInterForceCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleDeformGradTensorCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleJaccobianMatrixFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMHexaBrick12EleJaccobianMatrix();
    MExtensionObject* createOrgSipescFemsHexa12geostressMBrickEleGreenStrainCalculatorFactory();
    MExtensionObject* createOrgSipescFemsHexa12geostressMBrickEleCauchyStressCalculatorFactory();
  private:
    bool _running;
  };

#endif /* ORG_SIPESC_FEMS_HEXA12GEOSTRESS_H */
