#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialenumerator.h"
#include <QDateTime>
#include "qcustomplot.h"
#include "qextserialport.h"
#include "dialoggraphs.h"

Q_DECLARE_METATYPE(packet_t)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PortSettings nastaveni;
    nastaveni.BaudRate = BAUD1152000;
    nastaveni.DataBits = DATA_8;
    nastaveni.FlowControl = FLOW_HARDWARE;
    nastaveni.Parity = PAR_NONE;
    nastaveni.StopBits = STOP_1;
    nastaveni.Timeout_Millisec = 100;

    comport = new QextSerialPort(nastaveni,QextSerialPort::EventDriven,this);
    connect(comport,SIGNAL(readyRead()),this,SLOT(comport_new_data()));

    ui->tree->header()->setResizeMode(QHeaderView::ResizeToContents);
    ui->treeMessage->header()->setResizeMode(QHeaderView::ResizeToContents);

    on_butRefresh_clicked();
    disable(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#define PACKET_SIZE 11

void MainWindow::comport_new_data()
{
    //velikost paketu bude dycky 10bytů
    buffer += comport->readAll();

    while(buffer.count() >= PACKET_SIZE)
    {
        QByteArray message;
        message = buffer.left(PACKET_SIZE);
        buffer.remove(0,PACKET_SIZE);

        QTreeWidgetItem * item = new QTreeWidgetItem;
        ui->tree->addTopLevelItem(item);

        item->setData(0,Qt::DisplayRole,QDateTime::currentDateTime());

        packet_t pack = ParseMessage(*item,message);
        emit newPacket(pack);

        QString mes = message.toHex();
        int i = 2;
#define increm i++
#define space " "
        mes.insert(increm ,space);
        i+= 2;
        mes.insert(increm ,space);
        i+= 2;
        mes.insert(increm ,space);
        i+= 2;
        mes.insert(increm ,space);
        i+= 4;
        mes.insert(increm ,space);
        i+= 8;
        mes.insert(increm ,space);

        item->setText(1,mes);
        if (pack.checksum != pack.checksumLocal)
            item->setTextColor(1,Qt::red);
    }
}

void MainWindow::ParsePaket(packet_t &packet, const QByteArray &message)
{
    packet.checksumLocal = Checksum(message);
    packet.source_addr = message.at(0);
    packet.dest_addr = message.at(1);
    packet.type = message.at(2);
    packet.checksum = message.at(10);
    memcpy(&packet.address,message.constData() + 4,2);
    memcpy(&packet.data,message.constData()+6,4);
}

void MainWindow::FillTreeItem(QTreeWidgetItem &item, const packet_t &packet)
{
    static int neco = 0;
    neco++;
    QStringList list;
    list << TYPES;

    QTreeWidgetItem * it;

    it = new QTreeWidgetItem;
    it->setText(0,"Source Address");
    it->setText(1,QString("%1").arg(packet.source_addr));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Dest Address");
    it->setText(1,QString("%1").arg(packet.dest_addr));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Type");
    int type = packet.type;
    if (type >= 8)
        type = 8;
    it->setText(1,QString("%1 - %2").arg(packet.type).arg(list.at(type)));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Address");
    it->setText(1,QString("%1").arg(packet.address));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Data");
    it->setText(1,QString("%1").arg(packet.data));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Checksum");
    it->setText(1,QString("%1 (vypocteno: %2)").arg(packet.checksum).arg(packet.checksumLocal));
    item.addChild(it);
}

packet_t MainWindow::ParseMessage(QTreeWidgetItem &item, const QByteArray &message)
{
    packet_t pack;
    ParsePaket(pack,message);
    FillTreeItem(item,pack);
    pack.time = item.data(0,Qt::DisplayRole).value<QDateTime>();
    item.setData(0,Qt::DisplayRole,pack.time.time());

    return pack;
}

quint8 MainWindow::Checksum(const QByteArray &msg)
{
    quint8 temp = 0;
    for (int i = 0; i < PACKET_SIZE - 1 ; i++)
    {
        temp += msg.at(i);
    }
    temp ^= 0b01010101;

    return temp;
}

void MainWindow::disable(bool data)
{
    ui->pushButton->setDisabled(data);
    ui->pushButton_2->setEnabled(data);
    ui->butRefresh->setDisabled(data);
    ui->comboBox->setDisabled(data);
}

void MainWindow::on_pushButton_clicked()
{
    comport->setPortName(ui->comboBox->currentText());
    if (comport->open(QextSerialPort::ReadWrite))
    {
        comport->write("read\n\r");
        ui->textEdit->append(trUtf8("Otevřeno"));
        disable(true);
        buffer.clear();
    }
    else
    {
        ui->textEdit->append(trUtf8("Nejde připojit COM port"));
    }
}

void MainWindow::on_butRefresh_clicked()
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    QStringList list;

    for (int i = 0 ; i < ports.count() ; i++)
    {

    #ifdef Q_OS_WIN
         list << ports.at(i).portName;
    #else
        list << ports.at(i).physName;

    #endif

    }

    #ifndef Q_OS_WIN
    list << "/tmp/interceptty";
    #endif
    ui->comboBox->clear();
    ui->comboBox->addItems(list);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (comport->isOpen())
    {
        comport->close();
        ui->textEdit->append(trUtf8("Zavřeno"));
        disable(false);
    }
}

void MainWindow::on_tree_itemClicked(QTreeWidgetItem *item, int )
{
    if (item->parent())
        item = item->parent();

    ui->treeMessage->clear();

    QTreeWidgetItem * it = item->clone();
    ui->treeMessage->addTopLevelItem(it);
    ui->treeMessage->expandAll();
}

void MainWindow::on_actionGrafy_triggered()
{
    dialogGraphs * dlg = new dialogGraphs(this);
    dlg->show();
    connect(dlg,SIGNAL(finished(int)),this,SLOT(delete_dialog(int)));
    connect(this,SIGNAL(newPacket(packet_t)),dlg,SLOT(rec_paket(packet_t)));
}

void MainWindow::delete_dialog(int)
{
    sender()->deleteLater();
}
