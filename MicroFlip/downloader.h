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


class Downloader : public QObject
{
  Q_OBJECT

public:
  explicit Downloader(QObject *parent = 0);
  ~Downloader();


  QNetworkRequest generateRequest(QUrl url);
  void addHeaderToRequest(QNetworkRequest *request, QByteArray headerName, QByteArray headerValue);

  // QNetworkRequest generateGetRequest(QUrl url, QByteArray headerName, QByteArray headerValue);
  // QNetworkRequest generatePostRequest(QUrl url);

  // void doDownload(QNetworkRequest request);
  QNetworkAccessManager* doDownload(QNetworkRequest request, QObject* receiver, const char * method);
  void doDownload(QNetworkRequest request, QNetworkAccessManager *manager, QObject * receiver, const char * method);

  QNetworkAccessManager* doPostDownload(QNetworkRequest request, QByteArray data, QObject* receiver, const char * method);
  void doPostDownload(QNetworkRequest request, QNetworkAccessManager *manager, QByteArray data, QObject * receiver, const char * method);

  QNetworkAccessManager* doDeleteRequest(QNetworkRequest request, QObject* receiver, const char * method);
  void doDeleteRequest(QNetworkRequest request, QNetworkAccessManager *manager, QObject * receiver, const char * method);

  bool checkReply(QNetworkReply *reply);

private:

signals:

public slots:
  // void replyFinished (QNetworkReply *reply);



};

#endif // DOWNLOADER_H
