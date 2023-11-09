#ifndef TYPEWRITE_H
#define TYPEWRITE_H

#include <QDialog>

namespace Ui
{
class typewrite;
}

class typewrite : public QDialog
{
    Q_OBJECT

  public:
    explicit typewrite(QWidget *parent = nullptr);
    ~typewrite() override;
    void clearnEdit(void);
    void setEditText(const QString &str);

  signals:
    void mSendValue(const QString &str);
  private slots:
    void editDisplay(const QString &str);

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

  private:
    void buttonInit(void);

  private:
    Ui::typewrite *ui;

    QPoint dragStartPosition;
};

#endif // TYPEWRITE_H
