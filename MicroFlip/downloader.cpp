#include "downloader.h"

Downloader::Downloader(QObject *parent) :
QObject(parent) {

}

Downloader::~Downloader() {

}

// Generates a network request based on the given url
QNetworkRequest Downloader::generateRequest(QUrl url) {

  return QNetworkRequest(url);
}

// Adds a custuom header to the given request
void Downloader::addHeaderToRequest(QNetworkRequest *request, QByteArray headerName, QByteArray headerValue) {

    request->setRawHeader(headerName, headerValue);
}

// // Generates a HTTP GET request
// QNetworkRequest Downloader::generateGetRequest(QUrl url, QByteArray headerName, QByteArray headerValue) {
//
//   QNetworkRequest request(url);
//
//   request.setRawHeader(headerName, headerValue);
//
//   return request;
// }
//
// // Generates a HTTP POST request
// QNetworkRequest Downloader::generatePostRequest(QUrl url) {
//
//   return QNetworkRequest(url);
// }

//
QNetworkAccessManager* Downloader::doDownload(QNetworkRequest request, QObject * receiver, const char * method) {

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    manager->get(request);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    return manager;
}

void Downloader::doDownload(QNetworkRequest request, QNetworkAccessManager *manager, QObject * receiver, const char * method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->get(request);
}

QNetworkAccessManager* Downloader::doPostDownload(QNetworkRequest request, QByteArray data, QObject * receiver, const char * method) {

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->post(request, data);

    return manager;
}

void Downloader::doPostDownload(QNetworkRequest request, QNetworkAccessManager *manager, QByteArray data, QObject * receiver, const char * method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            receiver, method);

    manager->post(request, data);
}

QNetworkAccessManager *Downloader::doDeleteRequest(QNetworkRequest request, QObject *receiver, const char *method) {

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->deleteResource(request);

    return manager;
}

void Downloader::doDeleteRequest(QNetworkRequest request, QNetworkAccessManager *manager, QObject *receiver, const char *method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            receiver, method);

    manager->deleteResource(request);
}

// Checks the validity of a network reply
bool Downloader::checkReply(QNetworkReply *reply) {

  if(reply->error()) {

    qDebug() << "ERROR!";
    qDebug() << reply->errorString();

    return false;
  }
  else {

    qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
    qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
    qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

    return true;
  }
}

// //
// void Downloader::doDownload(QNetworkRequest request) {
//
//   manager = new QNetworkAccessManager(this);
//
//   connect(manager, SIGNAL(finished(QNetworkReply*)),
//   this, SLOT(replyFinished(QNetworkReply*)));
//
//   manager->get(request);
// }
//
// //
// void Downloader::replyFinished (QNetworkReply *reply) {
//
//   if(reply->error())
//   {
//     qDebug() << "ERROR!";
//     qDebug() << reply->errorString();
//   }
//   else
//   {
//     qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
//     qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
//     qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
//     qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//     qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
//
//     QFile *file = new QFile("downloadedData.json");
//     if(file->open(QFile::WriteOnly))
//     {
//       file->write(reply->readAll());
//       file->flush();
//       file->close();
//     }
//     delete file;
//   }
//
//   reply->deleteLater();
//   // delete reply;
// }
