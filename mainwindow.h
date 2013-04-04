#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QextSerialPort;

namespace Ui {
class MainWindow;
}

class QTreeWidgetItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

private slots:
    void comport_new_data(void);

    void on_pushButton_clicked();

    void on_butRefresh_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QextSerialPort * comport;
    void disable(bool);
    QByteArray buffer;
    void ParseMessage(QTreeWidgetItem & inputItem,const QByteArray & message);
    quint8 Checksum(const QByteArray & msg);
};

#endif // MAINWINDOW_H
