#include "json_helper.h"

#include <QJsonParseError>

bool JSON_Helper::getDocumentFromNetworkReply(QNetworkReply *reply, QJsonDocument *document) {

    QJsonParseError parseResult;

    *document = QJsonDocument().fromJson(reply->readAll(), &parseResult);

    if(parseResult.error == QJsonParseError::NoError) {

        return true;
    }

    qDebug() << "Error parsing JSON Document: " << parseResult.errorString();

    return false;
}

bool JSON_Helper::getObjectFromDocument(QJsonDocument *document, QJsonObject *object) {

    if(document->isObject()) {
        *object = document->object();

        return true;
    }

    qDebug() << "JSON Document does not contain object";

    return false;
}

bool JSON_Helper::getArrayFromDocument(QJsonDocument *document, QJsonArray *array) {

    if(document->isArray()) {
        *array  = document->array();

        return true;
    }

    qDebug() << "JSON Document does not contain array";

    return false;
}
