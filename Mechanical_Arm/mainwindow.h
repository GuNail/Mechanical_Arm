#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QMainWindow>
#include <QMutex>
#include <QPushButton>
#include <QVideoWidget>
#include <QWaitCondition>

#include "Serial/serialprotocol.h"
#include "Socket/socketprotocol.h"
#include "automechanicalarm.h"
#include "typewrite.h"

typedef struct
{
    int clip_oc_angle;
    int clip_rotate_angle;
    int frist_joint_angle;
    int second_joint_angle;
    int main_arm_angle;
    int chassis_angle;
} mechanical_arm_angle_typedef;

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /*****界面相关******/
    void mainWindowInit(void);                    // 主窗口初始化
    void serialTabWidgetInit(void);               // 串口助手页面初始化
    void netTabWidgetInit(void);                  // 网络助手页面初始化
    void mechanicalArmControlTabWidgetInit(void); // 机械臂控制页面初始化
    void buttonGroupInit();                       // 按钮组初始化
    void mechanicalArmutolTabWidgetInit(void);    // 机械臂自动化页面初始化
    void settingTabWidgetInit(void);

    void serialControlEnable(void);  // 控件使能
    void serialControlDisable(void); // 控件失能

    void typewriteInit(void);
    void typewriteClicked(void);

    void settingConfig(void);

    //*******外部连接相关*****/

    //****串口***//
    void serialInit(void);                    // 串口初始化
    void serialDeInit(void);                  // 串口断开
    void readSerialConfig(void);              // 读取串口配置
    void serialSignalsInit(void);             // 串口信号初始化
    void serialConnectMechanicalArm(void);    // 串口连接机械臂
    void serialDisConnectMechanicalArm(void); // 串口断开机械臂

    //****网络***//
    void netInit(void);                    // 网络初始化
    void netDeInit(void);                  // 网络断开
    void readNetConfig(void);              // 读取IP和端口
    void netSignalsInit(void);             // 网络信号初始化
    void netConnectMechanicalArm(void);    // 网络连接机械臂
    void netDisConnectMechanicalArm(void); // 网络断开机械臂

    //****摄像头****/
    void readCameraConfig();
    void cameraInit(void); // 摄像头初始化
    void cameraDeInit(void);

    void taskInit(void); // 任务初始化
    void taskDeInit(void);

  public slots:
    void updataSerialComBox(void);
    void openSerial(void);
    void showSerialRxData(const QByteArray &data);
    void sendSerialTxData(void);

    void openNet(void);
    void showNetRxData(QTcpSocket *client, const QByteArray &data);
    void sendNetTxData(void);

    void showAngle(uint8_t num, uint16_t angle);

    void sendSrtvoData(void);

    void buttonCallBack(QPushButton *Button);

    void updataCameraDeviceBox(void);
    void openCamera(void);
    void doImageCapture(QImage image);

    void settingSaved(void);

  signals:
    void mOpenSerial(bool &isOpen);
    void mCloseSerial(void);
    void mSendSerialData(QByteArray data);

    void mOpenNet(void);
    void mCloseNet(void);
    void mSendNetData(QByteArray data);

    void mSendServoData(uint8_t num, int angle);
    void mSendServoQListData(QList<int> data);
    void mSendQImage(QImage image);
    void mCloseWindow(void);

  protected:
    void doWarning(const QString &str);
    void closeEvent(QCloseEvent *event) override;

  private:
    Ui::MainWindow *ui; // 主窗口

  private:
    bool serialcom_connect_button_seate = true;
    bool net_connect_button_seate = true;
    bool camera_isOpen = false;
    mechanical_arm_angle_typedef mechanical_arm_angle = {0, 0, 0, 0, 0, 0};

  private:
    typewrite *inputUI = nullptr;

    QWaitCondition condition;
    QMutex mutex;

    SocketManage *socketManage;
    SocketProtocol *socketProtocol;
    SocketThread *socketThread;

    SerialManage *serialManage;
    SerialProtocol *serialProtocol;
    SerialThread *serialThread;
    SerialManage::serialConfigStruct serialConfig;

    autoMechanicalArm task;
    autoMechanicalArmThread taskThread;
};

#endif // MAINWINDOW_H
