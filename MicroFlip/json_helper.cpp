#include "json_helper.h"

#include <QJsonParseError>
#include <QJsonValue>

bool JSON_Helper::getDocumentFromNetworkReply(QNetworkReply *reply, QJsonDocument *document) {

    QJsonParseError parseResult{};

    QString str = QString::fromUtf8(reply->readAll());

    *document = QJsonDocument::fromJson(str.toLocal8Bit(), &parseResult);

    if(parseResult.error == QJsonParseError::NoError) {

        return true;
    }

    qDebug() << "Error parsing JSON Document: " << parseResult.errorString();

    return false;
}

bool JSON_Helper::getObjectFromString(QJsonObject *object, QString *string) {

    // TODO
    (void) object;
    (void) string;
//    QJsonObject val(&string);
    return false;
}

bool JSON_Helper::getObjectFromDocument(QJsonDocument *document, QJsonObject *object) {

    if(document->isObject()) {
        *object = document->object();

        return true;
    }

    qDebug("JSON Document does not contain object");

    return false;
}

bool JSON_Helper::getArrayFromDocument(QJsonDocument *document, QJsonArray *array) {

    if(document->isArray()) {
        *array  = document->array();

        return true;
    }

    qDebug() << QString("JSON Document does not contain array");

    return false;
}
