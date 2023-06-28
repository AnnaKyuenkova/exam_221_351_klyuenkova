#include "mytcpserver.h"

// Конструктор класса MyTcpServer
MyTcpServer::MyTcpServer(QObject *parent) : QTcpServer(parent), clientCount(0)
{
    // Запуск сервера на порту 33333 и вывод сообщения, если не удалось запустить сервер
    if (!listen(QHostAddress::Any, 33333)) {
        qDebug() << "Server not started!";
        return;
    }
    qDebug() << "Server started!";

    // Устанавливаем связь между сигналом newConnection и слотом slotNewConnection
    connect(this, &MyTcpServer::newConnection, this, &MyTcpServer::slotNewConnection);
}

void MyTcpServer::sendToClients(QString data, QTcpSocket* originalClient)
{
    QByteArray original;
    QByteArray thirdsymbol;
    for (int i = 0; i < data.size(); i++) {
        original.append(QByteArray(1, data[i].unicode()));
        if ((i + 1) % 3 == 0) {
            QString dataAtI = QString(data[i]);
            thirdsymbol.append(dataAtI.toUtf8());
        }
    }

    for (int i = 0; i < clientSockets.size(); i++) {
        QTcpSocket* clientSocket = clientSockets.at(i);
        if (clientSocket != originalClient) {
            clientSocket->write(original);
            clientSocket->write(thirdsymbol);
        }
    }
}

// Слот, вызываемый при новом подключении клиента
void MyTcpServer::slotNewConnection()
{
    qDebug() << "New user connected!";

    if (clientCount >= 6) {
        qDebug() << "Maximum number of clients reached!";
        return;
    }

    // Получаем указатель на сокет клиента и устанавливаем связь между сигналами readyRead и disconnected и соответствующими слотами
    QTcpSocket* clientSocket = nextPendingConnection();
    clientSockets.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    clientCount++;
}

// Слот, вызываемый при отключении клиента
void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "User disconnected!";
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    clientSockets.removeOne(clientSocket);
    clientSocket->deleteLater();
    clientCount--;
}

// Слот, вызываемый при чтении данных от клиента
void MyTcpServer::slotServerRead()
{
    // Получаем указатель на сокет клиента, от которого пришли данные
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());

    // Читаем строку из сокета и выводим сообщение о запросе в консоль
    QByteArray requestData = clientSocket->readAll();
    QString request = QString::fromUtf8(requestData);
    request = request.trimmed();
    qDebug() << "Request received from client " << clientSocket->socketDescriptor() << ": " << request;

    // Обрабатываем запрос и отправляем ответ клиенту
    sendToClients(request, clientSocket);
}
