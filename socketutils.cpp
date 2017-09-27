#include "socketutils.h"
#include "encryptutils.h"
#include "abstractedserver.h"
#include "fakeinput.h"
#include <QEventLoop>
#include <QTime>
#include <QDebug>
#include <QTextCodec>
#include <QMutex>
#include <QWaitCondition>

// 30 kb max chunk size
#define IO_MAX_CHUNK (1024*30)

qint64 MIN(qint64 a, qint64 b) { return a<b?a:b; }
qint64 MAX(qint64 a, qint64 b) { return a>b?a:b; }

QByteArray intToBytes(unsigned int n) {
    QByteArray bytes(4, 0);
    for(int i=0; i<bytes.length(); i++)
        *(bytes.data() + i) = 0xFF & (n >> 8*i);
    return bytes;
}

unsigned int bytesToInt(QByteArray bytes) {
    if(bytes.length() < 4)
        return 0;
    int n = int((unsigned char)(bytes.at(0)) << 0 |
                (unsigned char)(bytes.at(1)) << 8 |
                (unsigned char)(bytes.at(2)) << 16 |
                (unsigned char)(bytes.at(3)) << 24);
    return n;
}

namespace SocketUtils
{

QIODevice *socket;
bool socketIsBluetooth;

long sessionIV;
QByteArray sessionPasswordHash;

void setGlobalSocket(QIODevice *socket, bool bluetooth)
{
    SocketUtils::socket = socket;
    socketIsBluetooth = bluetooth;
}

void initSession(long sessionIV, QByteArray sessionPasswordHash)
{
    SocketUtils::sessionIV = sessionIV;
    SocketUtils::sessionPasswordHash = sessionPasswordHash;
}

QByteArray getSessionHash()
{
    return sessionPasswordHash;
}

bool bytesAvailable() { return socket->bytesAvailable() > 0; }

// wait functions that will work for both TCP & Bluetooth IODevice
bool waitForBytesWritten(int msecs)
{
    QEventLoop eventLoop;
    QTime stopWatch;
    stopWatch.start();

    eventLoop.processEvents();
    while(stopWatch.elapsed() < msecs && socket->bytesToWrite() && socket->isOpen()) {
        FakeInput::platformIndependentSleepMs(10); // sleep for cpu
        eventLoop.processEvents();
    }
    return socket->bytesToWrite() == false;
}
bool waitForReadyRead(int msecs)
{
    QEventLoop eventLoop;
    QTime stopWatch;
    stopWatch.start();

    eventLoop.processEvents();
    while(stopWatch.elapsed() < msecs && socket->bytesAvailable() == false && socket->isOpen()) {
        FakeInput::platformIndependentSleepMs(10); // sleep for cpu
        eventLoop.processEvents();
    }
    return bytesAvailable();
}

bool writeAllData(QByteArray data) {
    int wroteSoFar = 0;
    while(wroteSoFar < data.size()) {
        int bytesLeft = data.size() - wroteSoFar;
        int wroteThisTime = socket->write(data.data() + wroteSoFar, MIN(bytesLeft, IO_MAX_CHUNK));

        if(wroteThisTime <= 0 || !waitForBytesWritten(1000))
            break;
        else
            wroteSoFar += wroteThisTime;
    }

    if(wroteSoFar != data.size()) {
        socket->close();
        return false;
    }
    else
        return socket->bytesToWrite() == 0 || waitForBytesWritten(1000);
}

// write & write encrypted
bool writeDataUnencrypted(QByteArray data) {
    // Note: bluetooth fucks up if you send data size and data in 2 writes.
    return writeAllData(intToBytes(data.size()) + data);
}

bool writeDataEncrypted(QByteArray data) {
    if(socketIsBluetooth)
        return writeDataUnencrypted(data); // bluetooth already encrypted

    // AES encryption requires 16 byte blocks
    int padding = 16-(data.size()%16);
    data.resize(data.size() + padding);

    sessionIV = ( sessionIV + 1 ) % JAVA_INT_MAX_VAL;
    QByteArray iv = EncryptUtils::makeHash16(QString::number(sessionIV).toUtf8());
    QByteArray encrypted = EncryptUtils::encryptBytes(data, sessionPasswordHash, iv);

    return writeDataUnencrypted(encrypted);
}

bool readAllData(QByteArray *data) {
    if(!bytesAvailable() && !waitForReadyRead(2500))
        return false;
    int readSoFar = 0;
    while(readSoFar < data->size()) {
        qint64 bytesRead;
        do {
            int bytesLeft = data->size() - readSoFar;
            // server doesn't seem to need max chunk read size. mobile glitches out for reading tho
            bytesRead = socket->read(data->data() + readSoFar, bytesLeft);

            if(bytesRead < 0)
                return false;
            else
                readSoFar += bytesRead;
        }
        while(bytesRead > 0 && readSoFar < data->size());

        // No bytes left to read. If Block hasn't finished reading, try wait
        if(readSoFar < data->size() && !waitForReadyRead(2500)) {
            socket->close();
            return false;
        }
    }
    return true;
}

// read & read encrypted
QByteArray readDataUnencrypted() {
    // Get data size
    QByteArray dataLengthBytes(4, 0);
    if( readAllData(&dataLengthBytes) == false )
        return QByteArray();
    int dataLength = bytesToInt(dataLengthBytes);

    // Read data
    QByteArray data(dataLength, 0);
    if( readAllData(&data) )
        return data;
    else
        return QByteArray();
}

QByteArray readDataEncrypted() {
    QByteArray data = readDataUnencrypted();
    if(socketIsBluetooth)
        return data; // bluetooth protocol automatically encrypts/decrypts

    sessionIV = ( sessionIV + 1 ) % JAVA_INT_MAX_VAL;
    QByteArray iv = EncryptUtils::makeHash16(QString::number(sessionIV).toUtf8());

    if(data.size() % 16 == 0)
        data = EncryptUtils::decryptBytes(data, sessionPasswordHash, iv);
    else
        return QByteArray();

    return data;
}

// read & write strings
bool writeString(QString str, bool encrypt) {
    if(encrypt)
        return writeDataEncrypted(str.toUtf8());
    else
        return writeDataUnencrypted(str.toUtf8());
}
QString readString(bool decrypt) {
    QByteArray data;
    if(decrypt)
        data = readDataEncrypted();
    else
        data = readDataUnencrypted();
    return QString::fromUtf8(data);
}

}
