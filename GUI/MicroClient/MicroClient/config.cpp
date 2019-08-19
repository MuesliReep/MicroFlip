#include "config.h"

#include <QFile>
#include <QSettings>

Config::Config()
{

}

bool Config::loadConfigFromFile(const QString& fileName) {

    static const QString  DEFAULT_KEY_SERVER     = "";
    static const QString  DEFAULT_KEY_PRIVATE    = "";
    static const uint16_t DEFAULT_SERVER_PORT    = 33500;
    static const QString  DEFAULT_SERVER_ADDRESS = "127.0.0.1";

    bool result = false;

    QFile file(fileName);

    // If config file does not exist, create a default file and exit
    if(!file.exists()) {

        // Create file
        QSettings settings(fileName, QSettings::IniFormat);

        // Set default values
        settings.setValue("Keys/Server",    DEFAULT_KEY_SERVER     );
        settings.setValue("Keys/Private",   DEFAULT_KEY_PRIVATE    );
        settings.setValue("Server/Port",    DEFAULT_SERVER_PORT    );
        settings.setValue("Server/Address", DEFAULT_SERVER_ADDRESS );

        // Save settings
        settings.sync();

        return result;
    }

    QSettings settings(fileName, QSettings::IniFormat);

    this->serverKey    = settings.value("Keys/Server",    DEFAULT_KEY_SERVER     ).toString ();
    this->privateKey   = settings.value("Keys/Private",   DEFAULT_KEY_PRIVATE    ).toString ();
    this->serverPort   = settings.value("Server/Port",    DEFAULT_SERVER_PORT    ).toUInt   ();
    this->serverAdress = settings.value("Server/Address", DEFAULT_SERVER_ADDRESS ).toString ();

    result = true;

    settings.sync();

    return result;
}

void Config::saveConfigToFile  (const QString& fileName) {

    QSettings settings(fileName, QSettings::IniFormat);

    settings.setValue("Keys/Server",    this->serverKey    );
    settings.setValue("Keys/Private",   this->privateKey   );
    settings.setValue("Server/Port",    this->serverPort   );
    settings.setValue("Server/Address", this->serverAdress );

    settings.sync();
}

QString  Config::getServerKey   () const { return serverKey;    }
QString  Config::getPrivateKey  () const { return privateKey;   }
uint16_t Config::getServerPort  () const { return serverPort;   }
QString  Config::getServerAdress() const { return serverAdress; }

