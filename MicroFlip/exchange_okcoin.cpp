#include "exchange_okcoin.h"

Exchange_OKCoin::Exchange_OKCoin()
{

  currentTask = ExchangeTask();

  // Initiate download managers
  tickerDownloadManager      = new QNetworkAccessManager(this);
  createTradeDownloadManager = new QNetworkAccessManager(this);
  orderInfoDownloadManager   = new QNetworkAccessManager(this);
}

void Exchange_OKCoin::startWork() {

apiKey = c->getApiKey();
apiSecret = c->getApiSecret();

timer->start(c->getCoolDownTime()*1000);
//timer2->start(1*1100); // TODO: determine correct amount
}

void Exchange_OKCoin::createNonce(QByteArray *nonce) {

  uint now = QDateTime::currentDateTime().toMSecsSinceEpoch() / 250;

  if(lastNonce == now) {
    lastNonce+=2;
  }
  else
    lastNonce = now;

  nonce->setNum(lastNonce);
}

//----------------------------------//
//              Tasks               //
//----------------------------------//

void Exchange_OKCoin::updateMarketTicker(QString pair) {

  // Create the request to download new data
  QNetworkRequest request = d.generateRequest(QUrl("https://btc-e.com/api/3/ticker/"+pair));

  // Execute the download
  d.doDownload(request, tickerDownloadManager, this, SLOT(UpdateMarketTickerReply(QNetworkReply*)));
}

void Exchange_OKCoin::updateMarketDepth(QString pair) {

    (void)pair;
  // TODO
}

void Exchange_OKCoin::updateMarketTrades(QString pair) {

    (void)pair;
  // TODO
}

void Exchange_OKCoin::updateBalances() {

  // TODO
}

void Exchange_OKCoin::createOrder(QString Pair, int Type, double Rate, double Amount) {

  // Create POST data from method and nonce
  QByteArray method("method=Trade");
  QByteArray nonce;
  createNonce(&nonce);
  nonce.prepend("nonce=");

  QByteArray pair("pair=");
  pair.append(Pair);

  QByteArray type("type=");
  if(Type == 0)
    type.append("buy");
  else
    type.append("sell");

  QByteArray price("rate=");
  //QString sPrice; sPrice.setNum(Price,'f',3);
  price.append(QString::number(Rate,'f',3));

  QByteArray amount("amount=");
  //QString sAmount; sAmount.setNum(Amount,'f',8);
  amount.append(QString::number(Amount,'f',8));

  QByteArray data(method +"&"+ nonce +"&"+ pair +"&"+ type +"&"+ price +"&"+ amount); //qDebug() << "data: " << data;

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = d.generateRequest(QUrl("https://btc-e.com/tapi/3/"));

  // Add headers
  d.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  d.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  d.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  d.doPostDownload(request, createTradeDownloadManager, data, this, SLOT(CreateOrderReply(QNetworkReply*)));
}

void Exchange_OKCoin::cancelOrder(uint orderID) {

    (void)orderID;
  // TODO
}

void Exchange_OKCoin::updateActiveOrders(QString pair) {

    (void)pair;
  // TODO
}

void Exchange_OKCoin::updateOrderInfo(uint OrderID) {

  // Create POST data from method and nonce
  QByteArray method("method=OrderInfo");
  QByteArray nonce;
  createNonce(&nonce);
  nonce.prepend("nonce=");

  QByteArray orderID("order_id=");
  orderID.append(QString::number(OrderID));

  QByteArray data(method +"&"+ nonce + "&" + orderID);  //qDebug() << "data: " << data;

  // Sign the data
  QByteArray sign = QMessageAuthenticationCode::hash(data, apiSecret.toUtf8(), QCryptographicHash::Sha512).toHex();

  // Create request
  QNetworkRequest request = d.generateRequest(QUrl("https://btc-e.com/tapi/3/"));

  // Add headers
  d.addHeaderToRequest(&request, QByteArray("Content-type"), QByteArray("application/x-www-form-urlencoded"));
  d.addHeaderToRequest(&request, QByteArray("Key"), apiKey.toUtf8());
  d.addHeaderToRequest(&request, QByteArray("Sign"), sign);

  // Execute the download
  d.doPostDownload(request, orderInfoDownloadManager, data, this, SLOT(UpdateOrderInfoReply(QNetworkReply*)));
}

void Exchange_OKCoin::executeExchangeTask(ExchangeTask *exchangeTask) {

  switch(exchangeTask->getTask()) {

    case 0: updateMarketTicker(exchangeTask->getAttributes().at(0)); break;
    case 1: updateMarketDepth(exchangeTask->getAttributes().at(0));  break;
    case 2: updateMarketDepth(exchangeTask->getAttributes().at(0));  break;
    case 3: updateBalances(); break;
    case 4:
      createOrder(exchangeTask->getAttributes().at(0),
                  exchangeTask->getAttributes().at(1).toInt(),
                  exchangeTask->getAttributes().at(2).toDouble(),
                  exchangeTask->getAttributes().at(3).toDouble());
      break;
    case 5: cancelOrder(exchangeTask->getAttributes().at(0).toUInt()); break;
    case 6: updateActiveOrders(exchangeTask->getAttributes().at(0));   break;
    case 7: updateOrderInfo(exchangeTask->getAttributes().at(0).toUInt()); break;
    default: qDebug() << "Bad exchange task received: " << exchangeTask->getTask(); break;
  }
}
