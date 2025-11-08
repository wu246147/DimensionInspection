#ifndef QWAITING_H
#define QWAITING_H

#include <QWidget>

#include <QMovie>

namespace Ui
{
    class QWaiting;
}

class QWaiting : public QWidget
{
    Q_OBJECT

public:
    explicit QWaiting(QWidget *parent = nullptr);
    ~QWaiting();

    QMovie *m_Move;

private:
    Ui::QWaiting *ui;
};

#endif // QWAITING_H
