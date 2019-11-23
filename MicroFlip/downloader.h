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
  ~Downloader() override;

  QNetworkRequest generateRequest(const QUrl& url);
  void addHeaderToRequest(QNetworkRequest *request, const QByteArray& headerName, const QByteArray& headerValue);

  // QNetworkRequest generateGetRequest(QUrl url, QByteArray headerName, QByteArray headerValue);
  // QNetworkRequest generatePostRequest(QUrl url);

  QNetworkAccessManager* doDownload     (const QNetworkRequest& request, QObject* receiver, const char * method);
  QNetworkAccessManager* doPostDownload (const QNetworkRequest& request, const QByteArray& data, QObject* receiver, const char * method);
  QNetworkAccessManager* doDeleteRequest(const QNetworkRequest& request, QObject* receiver, const char * method);

  void doDownload     (const QNetworkRequest& request, QNetworkAccessManager *manager, QObject * receiver, const char * method);
  void doPostDownload (const QNetworkRequest& request, QNetworkAccessManager *manager, const QByteArray& data, QObject * receiver, const char * method);
  void doDeleteRequest(const QNetworkRequest& request, QNetworkAccessManager *manager, QObject * receiver, const char * method);

};

#endif // DOWNLOADER_H
