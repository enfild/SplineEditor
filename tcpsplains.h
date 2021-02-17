#ifndef TCPSPLAINS_H
#define TCPSPLAINS_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class TCPsplains; }
QT_END_NAMESPACE

class TCPsplains : public QMainWindow
{
    Q_OBJECT

public:
    TCPsplains(QWidget *parent = nullptr);
    ~TCPsplains();

private slots:
    void save();
    void load();
    void updateDot();

private:
    Ui::TCPsplains *ui;
};
#endif // TCPSPLAINS_H
