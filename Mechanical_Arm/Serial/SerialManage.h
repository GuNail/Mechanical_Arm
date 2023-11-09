#ifndef SERIALMANAGE_H
#define SERIALMANAGE_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "logInfo.h"

class SerialManage : public QObject
{
    Q_OBJECT

  public:
    typedef struct
    {
        QString vSerialPortName;                     // 串口名称
        qint32 vSerialBaudRate;                      // 串口波特率
        QSerialPort::StopBits vSerialStopBits;       // 串口停止位
        QSerialPort::DataBits vSerialDataBits;       // 串口数据位
        QSerialPort::Parity vSerialParrity;          // 设置奇偶校验
        QSerialPort::FlowControl vSerialFlowControl; // 数据流控
    } serialConfigStruct;

  public:
    explicit SerialManage(QObject *parent = nullptr);
    ~SerialManage(void);
    void setSerialConfig(serialConfigStruct SerialConfig);

  public slots:
    void connectSerial(bool &isOpen);
    void disconnectSerial(void);
    void writeData(QByteArray sendBuffer);
    void readData(void);

  private:
    QSerialPort *serialPort;
    SerialManage::serialConfigStruct serialConfig;

  signals:
    void openSuccess(void);
    void openFail(void);
    void close(void);
    void writeDataError(void);
    void readFinished(QByteArray &data);
    void readError(void);
};

#endif // SERIALMANAGE_H
