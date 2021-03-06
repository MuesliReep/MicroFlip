#include "downloader.h"

Downloader::Downloader(QObject *parent) :
QObject(parent) {

}

Downloader::~Downloader() = default;

// Generates a network request based on the given url
QNetworkRequest Downloader::generateRequest(const QUrl &url) {

  return QNetworkRequest(url);
}

// Adds a custuom header to the given request
void Downloader::addHeaderToRequest(QNetworkRequest *request, const QByteArray& headerName, const QByteArray& headerValue) {

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

QNetworkAccessManager* Downloader::doDownload(const QNetworkRequest& request, QObject * receiver, const char * method) {

    auto *manager = new QNetworkAccessManager(this);

    manager->get(request);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    return manager;
}

void Downloader::doDownload(const QNetworkRequest& request, QNetworkAccessManager *manager, QObject * receiver, const char * method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->get(request);
}

QNetworkAccessManager* Downloader::doPostDownload(const QNetworkRequest& request, const QByteArray& data, QObject * receiver, const char * method) {

    auto *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->post(request, data);

    return manager;
}

void Downloader::doPostDownload(const QNetworkRequest& request, QNetworkAccessManager *manager, const QByteArray& data, QObject * receiver, const char * method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            receiver, method);

    manager->post(request, data);
}

QNetworkAccessManager *Downloader::doDeleteRequest(const QNetworkRequest& request, QObject *receiver, const char *method) {

    auto *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
    receiver, method);

    manager->deleteResource(request);

    return manager;
}

void Downloader::doDeleteRequest(const QNetworkRequest& request, QNetworkAccessManager *manager, QObject *receiver, const char *method) {

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            receiver, method);

    manager->deleteResource(request);
}
