#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
  public:
    CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
    {
    }

  protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            emit lineEditClicked();
        }
        QLineEdit::mousePressEvent(event);
    }
  signals:
    void lineEditClicked();
};

#endif // CUSTOMLINEEDIT_H
