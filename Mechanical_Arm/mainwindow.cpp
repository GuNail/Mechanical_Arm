#include "mainwindow.h"
#include "calculateangles.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

extern QSettings *settings;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "main Thread runing:" << QThread::currentThread();

    updataSerialComBox();
    updataCameraDeviceBox();

    serialInit();
    netInit();

    mainWindowInit();
    serialTabWidgetInit();
    netTabWidgetInit();
    mechanicalArmControlTabWidgetInit();
    mechanicalArmutolTabWidgetInit();
    settingTabWidgetInit();

    taskInit();
}

MainWindow::~MainWindow()
{
    taskDeInit();
    serialDeInit();
    netDeInit();
    delete ui;
}

void MainWindow::mainWindowInit()
{
    ui->net_IPAddr_Edit->setText("192.168.1.52");
    ui->net_COMPort_Edit->setText("8086");
    typewriteInit();
}

void MainWindow::serialTabWidgetInit() // 串口助手页面
{
    connect(ui->rush_serial_button, &QPushButton::clicked, this, [&]() { updataSerialComBox(); }); //串口刷新

    connect(ui->serial_connect_button, &QPushButton::clicked, this,
            &MainWindow::openSerial); // 串口连接按钮

    connect(ui->clear_rx_display, &QPushButton::clicked, this,
            [this]() { ui->rx_display->clear(); }); // 串口接收显示框清空

    connect(ui->tx_data_Button, &QPushButton::clicked, this,
            &MainWindow::sendSerialTxData); // 发送按钮绑定

    connect(this->serialManage, &SerialManage::readFinished, this,
            &MainWindow::showSerialRxData); //串口接收数据框
}

void MainWindow::netTabWidgetInit()
{
    connect(ui->net_TCP_Insten_Button, &QPushButton::released, this, &MainWindow::openNet);

    connect(ui->clear_net_rx_display, &QPushButton::released, this, [&]() { ui->net_rx_display->clear(); });

    connect(ui->net_tx_data_Button, &QPushButton::released, this,
            &MainWindow::sendNetTxData); // 发送按钮绑定

    connect(this->socketManage, &SocketManage::readFinished, this,
            &MainWindow::showNetRxData); // 网络接收数据框

    connect(socketManage, &SocketManage::updateClient, this, [&](QList<QTcpSocket *> clients) {
        ui->client_comboBox->clear();

        ui->client_comboBox->addItem("All Client");
        for (const QTcpSocket *temp : clients)
        {
            ui->client_comboBox->addItem(temp->peerAddress().toString() + ":" + QString::number(temp->peerPort()),
                                         QVariant::fromValue(temp));
        }
    });
}

void MainWindow::mechanicalArmControlTabWidgetInit() // 机械臂控制页面
{
    buttonGroupInit();

    connect(ui->serial_DataScr_Button, &QRadioButton::toggled, this, [&](bool isCheack) {
        if (isCheack)
        {
            serialConnectMechanicalArm();
            task.setSerialProtocol(this->serialProtocol);
        }
        else
        {
            serialDisConnectMechanicalArm();
        }
    });

    connect(ui->net_DataScr_Button, &QRadioButton::toggled, this, [&](bool isCheack) {
        if (isCheack)
        {
            netConnectMechanicalArm();
            task.setSocketProtocol(this->socketProtocol);
        }
        else
        {
            netDisConnectMechanicalArm();
        }
    });

    connect(ui->send_servodata_Button, &QPushButton::clicked, this,
            &MainWindow::sendSrtvoData); // 发送舵机数据按钮绑定

    connect(ui->marm_restButton, &QPushButton::clicked, this, [this]() {
        QList<int> data = {90, 90, 90, 90, 90, 90};
        emit mSendServoQListData(data);
    }); // 舵机复位按钮绑定

    connect(ui->moveButton, &QPushButton::clicked, this, [=]() {
        QList<int> angle = Inverse_Kinematics(ui->X_lineEdit->text().toInt(), ui->Y_lineEdit->text().toInt(),
                                              ui->Z_lineEdit->text().toInt());

        qDebug() << angle;
        emit mSendServoQListData(angle);
    }); // 空间坐标角度转换

    connect(ui->Gripping_pushButton, &QPushButton::clicked, this, [=]() { emit mSendServoData(5, 95); }); // 夹取

    connect(ui->release_pushButton, &QPushButton::clicked, this, [=]() { emit mSendServoData(5, 40); }); // 释放

    connect(ui->moveToPlaceArea_Button, &QPushButton::clicked, this, [=]() {
        // QList<int> data ={237,104,231,191};
        QList<int> data = {90, 30, 180, 160};
        emit mSendServoQListData(data);
    }); // 移动至放置区

    connect(ui->workingmode_2_Button, &QPushButton::clicked, this, [=]() {
        QList<int> data = {90, 30, 180, 160};
        emit mSendServoQListData(data);
    }); // 舵机工作模式按钮绑定
}

void MainWindow::mechanicalArmutolTabWidgetInit() // 机械臂自动化页面
{
    connect(ui->opencamera_Button, &QPushButton::clicked, this, &MainWindow::openCamera);

    connect(ui->rush_camera_button, &QPushButton::clicked, this, [&]() { updataCameraDeviceBox(); });

    connect(ui->workingmode, &QPushButton::clicked, this, [&]() {
        QList<int> data = {90, 30, 180, 160};
        emit mSendServoQListData(data);
    }); // 舵机工作模式按钮绑定

    connect(ui->startWork_Button, &QPushButton::clicked, &task, &autoMechanicalArm::start);

    connect(ui->startWork_Button, &QPushButton::clicked, this, [&]() {
        ui->startWork_Button->setDisabled(true);
        ui->Continuous_Mode->setDisabled(true);
    });

    connect(ui->stopWork_Button, &QPushButton::clicked, this, [&]() {
        this->condition.wakeAll();
        ui->startWork_Button->setEnabled(true);
        ui->Continuous_Mode->setEnabled(true);
    });

    connect(ui->takePhotoButton, &QPushButton::clicked, this, [&]() {

    });

    connect(ui->stopWork_Button, &QPushButton::released, &task, &autoMechanicalArm::stop);

    connect(ui->Continuous_Mode, &QCheckBox::toggled, &task, &autoMechanicalArm::setCycleMode);
}

void MainWindow::settingTabWidgetInit()
{
    settingConfig();
    //    ui->clip_openAngle->setText(settings->value("cilp_OpenAngle").toString());
    //    ui->clip_closeAngle->setText(settings->value("cilp_CloseAngle").toString());
}

void MainWindow::cameraInit(void)
{
    connect(ui->opencamera_Button, &QPushButton::clicked, this, &MainWindow::openCamera);
}

void MainWindow::cameraDeInit()
{
}

void MainWindow::taskInit()
{
    task.setMutex(&this->mutex);
    task.setWaitCondition(&this->condition);

    taskThread.moveToThread(&taskThread);
    task.moveToThread(&taskThread);
    taskThread.start();

    connect(this, &MainWindow::mSendQImage, &task, &autoMechanicalArm::recvQImage);

    connect(&task, &autoMechanicalArm::mNotThing, this, &MainWindow::doWarning);

    connect(&task, &autoMechanicalArm::mGetImage, this, [&]() { ui->videoPlayer->Capture(); });

    connect(ui->videoPlayer, &OpenCVWidget::imageCaptured, this, &MainWindow::doImageCapture);

    connect(&task, &autoMechanicalArm::returnResultsImage, this,
            [&](QImage Image) { ui->resultDisplay->setPixmap(QPixmap::fromImage(Image)); });

    connect(&task, &autoMechanicalArm::mFinish, this, [&]() {
        ui->startWork_Button->setEnabled(true);
        ui->Continuous_Mode->setEnabled(true);
    });
}

void MainWindow::taskDeInit()
{
    taskThread.quit();
    taskThread.wait();
}

void MainWindow::serialControlEnable()
{
    ui->serial_com_Box->setEnabled(true);
    ui->serialcom_baud_box->setEnabled(true);

    ui->tx_data_Button->setDisabled(true); // 发送按钮失能
    ui->send_servodata_Button->setDisabled(true);
    ui->marm_restButton->setDisabled(true);
}

void MainWindow::serialControlDisable()
{
    ui->serial_com_Box->setDisabled(true);
    ui->serialcom_baud_box->setDisabled(true);

    ui->tx_data_Button->setEnabled(true); // 发送按钮使能
    ui->send_servodata_Button->setEnabled(true);
    ui->marm_restButton->setEnabled(true);
}

void MainWindow::typewriteInit()
{
    inputUI = new typewrite(this);
    inputUI->setAttribute(Qt::WA_DeleteOnClose);

    QList<CustomLineEdit *> LineEdits = {ui->servoangle_Edit, ui->X_lineEdit,       ui->Y_lineEdit,
                                         ui->Z_lineEdit,      ui->net_COMPort_Edit, ui->net_IPAddr_Edit};

    for (const CustomLineEdit *enit : LineEdits)
    {
        connect(enit, &CustomLineEdit::lineEditClicked, this, &MainWindow::typewriteClicked);
    }
}

void MainWindow::typewriteClicked()
{
    CustomLineEdit *senderLineEdit = qobject_cast<CustomLineEdit *>(sender());
    if (senderLineEdit)
    {
        inputUI->clearnEdit();
        inputUI->show();

        inputUI->setEditText(senderLineEdit->text());

        disconnect(inputUI, &typewrite::mSendValue, nullptr, nullptr);

        connect(inputUI, &typewrite::mSendValue, this, [=](QString str) {
            ;
            senderLineEdit->setText(str);
            disconnect(inputUI, &typewrite::mSendValue, nullptr, nullptr);
        });
    }
}

void MainWindow::settingConfig()
{
    connect(ui->settingSave_pushButton, &QPushButton::clicked, this, &MainWindow::settingSaved);
}

void MainWindow::settingSaved()
{
    settings->setValue("cilp_OpenAngle", ui->clip_openAngle->text());
    settings->setValue("cilp_CloseAngle", ui->clip_closeAngle->text());
}

void MainWindow::serialInit()
{
    this->serialManage = new SerialManage;
    this->serialProtocol = new SerialProtocol;
    this->serialThread = new SerialThread;

    this->serialProtocol->setSerialManage(this->serialManage);

    this->serialManage->moveToThread(this->serialThread);
    this->serialProtocol->moveToThread(this->serialThread);
    this->serialThread->start();

    serialSignalsInit();
}

void MainWindow::serialDeInit()
{
    emit mCloseSerial();
    this->serialThread->quit();
    this->serialThread->wait();
    this->serialThread->deleteLater();
}

void MainWindow::buttonGroupInit()
{
#define TRUGGER_TIME 200
#define TRUGGER_TNTERVAL 100

    ui->servoangle_Edit->setValidator(new QIntValidator(0, 180, ui->servoangle_Edit));

    QList<QPushButton *> PushBtton_List = {ui->pushButton_1, ui->pushButton_2,  ui->pushButton_3,  ui->pushButton_4,
                                           ui->pushButton_5, ui->pushButton_6,  ui->pushButton_7,  ui->pushButton_8,
                                           ui->pushButton_9, ui->pushButton_10, ui->pushButton_11, ui->pushButton_12};

    uint8_t count = 1;
    foreach (QPushButton *tempButton, PushBtton_List)
    {
        tempButton->setProperty("ID", count++);
        tempButton->setAutoRepeat(true);
        tempButton->setAutoRepeatDelay(TRUGGER_TIME);
        tempButton->setAutoRepeatInterval(TRUGGER_TNTERVAL);
        connect(tempButton, &QPushButton::clicked, this, [=]() { buttonCallBack(tempButton); });
    }
}

void MainWindow::readCameraConfig()
{
    QVariant var = ui->camera_comboBox->itemData(ui->camera_comboBox->currentIndex());
    ui->videoPlayer->setCameraDevice(var.value<QCameraInfo>().deviceName());
}

void MainWindow::readSerialConfig()
{
    serialConfig.vSerialPortName = ui->serial_com_Box->currentText();
    serialConfig.vSerialBaudRate = ui->serialcom_baud_box->currentText().toInt();
    serialConfig.vSerialDataBits = QSerialPort::Data8;
    serialConfig.vSerialStopBits = QSerialPort::OneStop;
    serialConfig.vSerialParrity = QSerialPort::NoParity;
    serialConfig.vSerialFlowControl = QSerialPort::NoFlowControl;

    this->serialManage->setSerialConfig(serialConfig);
}

void MainWindow::serialSignalsInit()
{
    connect(this->serialManage, &SerialManage::openSuccess, this,
            [&]() { this->serialcom_connect_button_seate = false; }); // 串口打开成功

    connect(this->serialManage, &SerialManage::openFail, this, [&]() {
        this->serialcom_connect_button_seate = true;
        this->doWarning("串口打开失败");
    }); // 串口打开失败

    connect(this->serialManage, &SerialManage::close, this,
            [&]() { this->serialcom_connect_button_seate = true; }); // 串口关闭成功

    connect(this, &MainWindow::mOpenSerial, this->serialManage,
            &SerialManage::connectSerial); // 串口连接信号

    connect(this, &MainWindow::mCloseSerial, this->serialManage,
            &SerialManage::disconnectSerial); // 串口断开信号

    connect(this, &MainWindow::mSendSerialData, this->serialManage,
            &SerialManage::writeData); // 串口发送数据框信号
}

void MainWindow::serialConnectMechanicalArm()
{
    connect(this->serialProtocol, &SerialProtocol::showData, this,
            &MainWindow::showAngle); // 机械臂角度显示信号

    connect(this, &MainWindow::mSendServoData, this->serialProtocol,
            &SerialProtocol::controlSingleServo); // 控制单个舵机信号

    connect(this, &MainWindow::mSendServoQListData, this->serialProtocol,
            &SerialProtocol::controlMultipleServo); // 列表数据轮流打包发送信号

    connect(this->serialProtocol, &SerialProtocol::finish, this,
            [&]() { this->condition.wakeAll(); }); // 动作执行完成返回
}

void MainWindow::serialDisConnectMechanicalArm()
{
    disconnect(this->serialProtocol, &SerialProtocol::showData, this,
               &MainWindow::showAngle); // 机械臂角度显示信号

    disconnect(this, &MainWindow::mSendServoData, this->serialProtocol,
               &SerialProtocol::controlSingleServo); // 控制单个舵机信号

    disconnect(this, &MainWindow::mSendServoQListData, this->serialProtocol,
               &SerialProtocol::controlMultipleServo); // 列表数据轮流打包发送信号

    disconnect(this->serialProtocol, &SerialProtocol::finish, this, nullptr); // 动作执行完成返回
}

void MainWindow::netInit()
{
    this->socketManage = new SocketManage;
    this->socketProtocol = new SocketProtocol;
    this->socketThread = new SocketThread;

    this->socketProtocol->setSocketManage(this->socketManage);

    this->socketManage->moveToThread(this->socketThread);
    this->socketProtocol->moveToThread(this->socketThread);

    this->socketThread->start();

    netSignalsInit();
}

void MainWindow::netDeInit()
{
    emit mCloseNet();
    this->socketThread->quit();
    this->socketThread->wait();
    this->socketThread->deleteLater();
}

void MainWindow::readNetConfig()
{
    socketManage->setNetConfig(ui->net_IPAddr_Edit->text(),
                               static_cast<quint16>(ui->net_COMPort_Edit->text().toUInt()));
}

void MainWindow::netSignalsInit()
{
    connect(this, &MainWindow::mOpenNet, this->socketManage,
            &SocketManage::connectNet); // 网络连接信号

    connect(this, &MainWindow::mCloseNet, this->socketManage,
            &SocketManage::disConnectNet); // 网络断开信号

    connect(this->socketManage, &SocketManage::openSuccess, this,
            [&]() { this->net_connect_button_seate = false; }); // 网络打开成功

    connect(this->socketManage, &SocketManage::openFail, this, [&]() {
        this->net_connect_button_seate = true;
        this->doWarning("打开失败");
        ui->net_TCP_Insten_Button->setText("开始监听");
    }); // 网络打开失败

    connect(this->socketManage, &SocketManage::close, this,
            [&]() { this->net_connect_button_seate = true; }); // 网络关闭

    connect(this, &MainWindow::mSendNetData, this->socketManage,
            &SocketManage::broadcastData); // 发送数据信号
}

void MainWindow::netConnectMechanicalArm()
{
    connect(this->socketProtocol, &SocketProtocol::showData, this,
            &MainWindow::showAngle); // 机械臂角度显示信号

    connect(this, &MainWindow::mSendServoData, this->socketProtocol,
            &SocketProtocol::controlSingleServo); // 控制单个舵机信号

    connect(this, &MainWindow::mSendServoQListData, this->socketProtocol,
            &SocketProtocol::controlMultipleServo); // 列表数据轮流打包发送信号

    connect(this->socketProtocol, &SocketProtocol::finish, this,
            [&]() { this->condition.wakeAll(); }); // 动作执行完成返回
}

void MainWindow::netDisConnectMechanicalArm()
{
    disconnect(this->socketProtocol, &SocketProtocol::showData, this,
               &MainWindow::showAngle); // 机械臂角度显示信号

    disconnect(this, &MainWindow::mSendServoData, this->socketProtocol,
               &SocketProtocol::controlSingleServo); // 控制单个舵机信号

    disconnect(this, &MainWindow::mSendServoQListData, this->socketProtocol,
               &SocketProtocol::controlMultipleServo); // 列表数据轮流打包发送信号

    disconnect(this->socketProtocol, &SocketProtocol::finish, this, nullptr); // 动作执行完成返回
}

void MainWindow::updataSerialComBox()
{
    ui->serial_com_Box->clear();
    const QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &mport : port)
    {
        ui->serial_com_Box->addItem(mport.portName());
    }
}

void MainWindow::updataCameraDeviceBox()
{
    ui->camera_comboBox->clear();
    QList<QCameraInfo> cameraDevices = QCameraInfo::availableCameras();

    for (const QCameraInfo &cameraDevice : cameraDevices)
    {
        ui->camera_comboBox->addItem(cameraDevice.description(), QVariant::fromValue(cameraDevice));
    }
}

void MainWindow::openCamera(void)
{
    if (camera_isOpen)
    {
        camera_isOpen = false;
        ui->videoPlayer->close();
        ui->opencamera_Button->setText("打开摄像头");
    }
    else
    {
        camera_isOpen = true;
        readCameraConfig();
        ui->videoPlayer->open();
        ui->opencamera_Button->setText("关闭摄像头");
    }
}

void MainWindow::doImageCapture(QImage image)
{
    //    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ".jpg";
    //    if (preview.save(str))
    //    {
    //        LOG << __FUNCTION__ << preview;
    //    }

    emit mSendQImage(image);
}

void MainWindow::openSerial()
{
    if (this->serialcom_connect_button_seate)
    {
        bool isOpen = false;
        readSerialConfig();

        emit mOpenSerial(isOpen);

        if (isOpen)
        {
            this->serialcom_connect_button_seate = false;
            ui->serial_connect_button->setText("关闭串口");
            serialControlDisable();
        }
        else
        {
            doWarning("连接串口失败");
        }
    }
    else
    {
        this->serialcom_connect_button_seate = true;
        ui->serial_connect_button->setText("打开串口");
        emit mCloseSerial();
        serialControlEnable();
    }
}

void MainWindow::showSerialRxData(const QByteArray &data)
{
    if (ui->rx_ASCII_Button->isChecked())
    {
        ui->rx_display->append(data);
    }
    else if (ui->rx_HEX_Button->isChecked())
    {
        ui->rx_display->append(data.toHex());
    }
}

void MainWindow::sendSerialTxData()
{
    if (QByteArray(ui->tx_display->toPlainText().toUtf8()).isEmpty())
    {
        doWarning("请输入需要发送的数据！");
        return;
    }
    if (ui->tx_ASCII_Button->isChecked())
    {
        emit mSendSerialData(QByteArray(ui->tx_display->toPlainText().toUtf8()));
    }
    else if (ui->tx_HEX_Button->isChecked())
    {
        emit mSendSerialData(QByteArray::fromHex(ui->tx_display->toPlainText().toUtf8()));
    }
}

void MainWindow::openNet()
{
    if (this->net_connect_button_seate)
    {
        readNetConfig();
        emit mOpenNet();
        ui->net_TCP_Insten_Button->setText("停止监听");
    }
    else
    {
        emit mCloseNet();
        ui->net_TCP_Insten_Button->setText("开始监听");
    }
}

void MainWindow::showNetRxData(QTcpSocket *client, const QByteArray &data)
{
    Q_UNUSED(client);
    QString str = "<font color='blue'> Sender:" + client->peerAddress().toString() + ':' +
                  QString::number(client->peerPort()) + "</font><br>";
    if (ui->net_rx_ASCII_Button->isChecked())
    {
        ui->net_rx_display->append(str + data);
    }
    else if (ui->net_rx_HEX_Button->isChecked())
    {
        ui->net_rx_display->append(str + data.toHex());
    }
}

void MainWindow::sendNetTxData()
{
    if (QByteArray(ui->net_tx_display->toPlainText().toUtf8()).isEmpty())
    {
        doWarning("请输入需要发送的数据");
        return;
    }
    if (ui->net_tx_ASCII_Button->isChecked())
    {
        emit mSendNetData(QByteArray(ui->net_tx_display->toPlainText().toUtf8()));
    }
    else if (ui->net_tx_ASCII_Button->isChecked())
    {
        emit mSendNetData(QByteArray::fromHex(ui->net_tx_display->toPlainText().toUtf8()));
    }
}

void MainWindow::showAngle(uint8_t num, uint16_t angle)
{
    switch (num)
    {
        case 0:
            mechanical_arm_angle.chassis_angle = angle;
            ui->marm_angle_display_0->setText(QString::number(angle));
            break;

        case 1:
            mechanical_arm_angle.main_arm_angle = angle;
            ui->marm_angle_display_1->setText(QString::number(angle));
            break;

        case 2:
            mechanical_arm_angle.second_joint_angle = angle;
            ui->marm_angle_display_2->setText(QString::number(angle));
            break;

        case 3:
            mechanical_arm_angle.frist_joint_angle = angle;
            ui->marm_angle_display_3->setText(QString::number(angle));
            break;

        case 4:
            mechanical_arm_angle.clip_rotate_angle = angle;
            ui->marm_angle_display_4->setText(QString::number(angle));

            break;

        case 5:
            mechanical_arm_angle.clip_oc_angle = angle;
            ui->marm_angle_display_5->setText(QString::number(angle));
            break;
    }

    double j1 = mechanical_arm_angle.chassis_angle;
    double j2 = mechanical_arm_angle.main_arm_angle - 90;
    double j3 = mechanical_arm_angle.second_joint_angle - 90;
    double j4 = mechanical_arm_angle.frist_joint_angle - 90;

    QList<int> coordinate = forward_kinematics(j1, j2, j3, j4);
    ui->X_display->setText(QString::number(coordinate.at(0)));
    ui->Y_display->setText(QString::number(coordinate.at(1)));
    ui->Z_display->setText(QString::number(coordinate.at(2)));
}

void MainWindow::sendSrtvoData()
{
    uint8_t num = static_cast<uint8_t>(ui->servoname_Box->currentIndex());
    uint16_t angle = ui->servoangle_Edit->text().toUShort();
    if (angle > 180)
    {
        doWarning("输入角度不能大于180");
        return;
    }
    emit mSendServoData(num, angle);
}

#define STEP_VALUE 2
void MainWindow::buttonCallBack(QPushButton *Button)
{
    switch (Button->property("ID").toInt())
    {
        case 1:
            mechanical_arm_angle.clip_oc_angle -= STEP_VALUE;
            emit mSendServoData(5, mechanical_arm_angle.clip_oc_angle);
            break;

        case 2:
            mechanical_arm_angle.clip_oc_angle += STEP_VALUE;
            emit mSendServoData(5, mechanical_arm_angle.clip_oc_angle);
            break;

        case 3:
            mechanical_arm_angle.clip_rotate_angle += STEP_VALUE;
            emit mSendServoData(4, mechanical_arm_angle.clip_rotate_angle);
            break;

        case 4:
            mechanical_arm_angle.clip_rotate_angle -= STEP_VALUE;
            emit mSendServoData(4, mechanical_arm_angle.clip_rotate_angle);
            break;

        case 5:
            mechanical_arm_angle.frist_joint_angle += STEP_VALUE;
            emit mSendServoData(3, mechanical_arm_angle.frist_joint_angle);
            break;

        case 6:
            mechanical_arm_angle.frist_joint_angle -= STEP_VALUE;
            emit mSendServoData(3, mechanical_arm_angle.frist_joint_angle);
            break;

        case 7:
            mechanical_arm_angle.second_joint_angle -= STEP_VALUE;
            emit mSendServoData(2, mechanical_arm_angle.second_joint_angle);
            break;

        case 8:
            mechanical_arm_angle.second_joint_angle += STEP_VALUE;
            emit mSendServoData(2, mechanical_arm_angle.second_joint_angle);
            break;

        case 9:
            mechanical_arm_angle.main_arm_angle -= STEP_VALUE;
            emit mSendServoData(1, mechanical_arm_angle.main_arm_angle);
            break;

        case 10:
            mechanical_arm_angle.main_arm_angle += STEP_VALUE;
            emit mSendServoData(1, mechanical_arm_angle.main_arm_angle);
            break;

        case 11:
            mechanical_arm_angle.chassis_angle += STEP_VALUE;
            emit mSendServoData(0, mechanical_arm_angle.chassis_angle);
            break;

        case 12:
            mechanical_arm_angle.chassis_angle -= STEP_VALUE;
            emit mSendServoData(0, mechanical_arm_angle.chassis_angle);
            break;
    }
}

/*警告*/
void MainWindow::doWarning(const QString &str)
{
    QMessageBox::warning(this, "警告", str);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit mCloseWindow();

    condition.wakeAll();

    task.stop();
    event->accept();
}
