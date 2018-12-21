#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class JSON_Helper
{
public:
    static bool getDocumentFromNetworkReply(QNetworkReply *reply,    QJsonDocument *document);
    static bool getObjectFromDocument      (QJsonDocument *document, QJsonObject *object);
    static bool getArrayFromDocument       (QJsonDocument *document, QJsonArray *array);
};

#endif // JSON_HELPER_H
