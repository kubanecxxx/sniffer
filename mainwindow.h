#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "paket.h"
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
    void on_tree_itemClicked(QTreeWidgetItem *item, int column);
    void on_actionGrafy_triggered();
    void delete_dialog(int);

signals:
    void newPacket(packet_t packet);

private:
    Ui::MainWindow *ui;
    QextSerialPort * comport;
    void disable(bool);
    QByteArray buffer;
    packet_t ParseMessage(QTreeWidgetItem & inputItem,const QByteArray & message);
    void ParsePaket(packet_t & packet, const QByteArray & msg);
    void FillTreeItem(QTreeWidgetItem & item, const packet_t & packet);
    quint8 Checksum(const QByteArray & msg);
};

#endif // MAINWINDOW_H
