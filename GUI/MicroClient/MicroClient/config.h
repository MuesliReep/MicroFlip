#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

class Config
{
public:
    Config();

    bool loadConfigFromFile(const QString& fileName = "config.ini");
    void saveConfigToFile  (const QString& fileName = "config.ini");

    QString  getServerKey   () const;
    QString  getPrivateKey  () const;
    uint16_t getServerPort  () const;
    QString  getServerAdress() const;

private:
    QString  serverKey    {};
    QString  privateKey   {};
    uint16_t serverPort   {};
    QString  serverAdress {};
};

#endif // CONFIG_H
