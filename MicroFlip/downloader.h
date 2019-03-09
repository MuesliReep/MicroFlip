#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QDateTime>


class Downloader : public QObject {

    Q_OBJECT

public:
  explicit Downloader(QObject *parent = nullptr);
  ~Downloader();

  QNetworkRequest generateRequest(const QUrl& url);
  void addHeaderToRequest(QNetworkRequest *request, QByteArray headerName, QByteArray headerValue);

  // QNetworkRequest generateGetRequest(QUrl url, QByteArray headerName, QByteArray headerValue);
  // QNetworkRequest generatePostRequest(QUrl url);

  QNetworkAccessManager* doDownload     (QNetworkRequest request, QObject* receiver, const char * method);
  QNetworkAccessManager* doPostDownload (QNetworkRequest request, QByteArray data, QObject* receiver, const char * method);
  QNetworkAccessManager* doDeleteRequest(QNetworkRequest request, QObject* receiver, const char * method);

  void doDownload     (QNetworkRequest request, QNetworkAccessManager *manager, QObject * receiver, const char * method);
  void doPostDownload (QNetworkRequest request, QNetworkAccessManager *manager, QByteArray data, QObject * receiver, const char * method);
  void doDeleteRequest(QNetworkRequest request, QNetworkAccessManager *manager, QObject * receiver, const char * method);

};

#endif // DOWNLOADER_H
