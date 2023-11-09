#include "typewrite.h"
#include "ui_typewrite.h"
#include <QCloseEvent>
#include <QDebug>
#include <QPainter>

typewrite::typewrite(QWidget *parent) : QDialog(parent), ui(new Ui::typewrite)
{
    ui->setupUi(this);
    resize(210, 320);
    setWindowTitle("input");
    setModal(false);
    buttonInit();
}

typewrite::~typewrite()
{
    delete ui;
}

void typewrite::clearnEdit()
{
    ui->lineEdit->clear();
}

void typewrite::setEditText(const QString &str)
{
    ui->lineEdit->setText(str);
}

void typewrite::editDisplay(const QString &str)
{
    QString scrStr = ui->lineEdit->text();
    if (str != "del")
    {
        ui->lineEdit->setText(scrStr + str);
    }
    else
    {
        scrStr.chop(1);
        ui->lineEdit->setText(scrStr);
    }
}

void typewrite::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void typewrite::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragStartPosition);
        event->accept();
    }
}

void typewrite::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    QDialog::paintEvent(event);
}

void typewrite::buttonInit()
{
    QList<QPushButton *> buttons = {ui->pushButton_1, ui->pushButton_2,  ui->pushButton_3,  ui->pushButton_4,
                                    ui->pushButton_5, ui->pushButton_6,  ui->pushButton_7,  ui->pushButton_8,
                                    ui->pushButton_9, ui->pushButton_10, ui->pushButton_11, ui->pushButton_12};

    for (const QPushButton *button : buttons)
    {
        connect(button, &QPushButton::clicked, this, [=]() { editDisplay(button->text()); });
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&]() {
        hide();
        emit mSendValue(ui->lineEdit->text());
    });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, [&]() {
        hide();
        ui->lineEdit->clear();
    });
}

void typewrite::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}
