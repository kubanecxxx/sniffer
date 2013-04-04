#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialenumerator.h"
#include <QDateTime>
#include "qcustomplot.h"
#include "qextserialport.h"

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


    ui->widget->addGraph();
    ui->widget->addGraph()->setPen(QPen(Qt::red));

#define plot widget
    ui->plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->plot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->plot->xAxis->setLabel(trUtf8("Čas"));
    ui->plot->yAxis->setLabel(trUtf8("Teplota °C"));
    ui->plot->legend->setVisible(true);
    ui->plot->legend->setPositionStyle(QCPLegend::psTopLeft);

    on_butRefresh_clicked();
    disable(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#define PACKET_SIZE 10

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

        item->setData(0,Qt::DisplayRole,QTime::currentTime());
        item->setText(1,message.toHex());
        ParseMessage(*item,message);
    }
}

void MainWindow::ParseMessage(QTreeWidgetItem &item, const QByteArray &message)
{
    QStringList list;
    list << "IDLE" << "GET" << "SET" << "OKSET" << "OKGET" << "NOKSET" << "NOKGET" << "IDLEOK";

    quint8 check2 = Checksum(message);
    int sourceAddr = message.at(0);
    int destAddr = message.at(1);
    int type = message.at(2);
    quint16 address;
    quint32 data;
    int checksum = message.at(9);
    memcpy(&address,message.constData() + 3,2);
    memcpy(&data,message.constData()+5,4);

    QTreeWidgetItem * it;

    it = new QTreeWidgetItem;
    it->setText(0,"Source Address");
    it->setText(1,QString("%1").arg(sourceAddr));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Dest Address");
    it->setText(1,QString("%1").arg(destAddr));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Type");
    it->setText(1,QString("%1 - %2").arg(type).arg(list.at(type)));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Address");
    it->setText(1,QString("%1").arg(address));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Data");
    it->setText(1,QString("%1").arg(data));
    item.addChild(it);

    it = new QTreeWidgetItem;
    it->setText(0,"Checksum");
    it->setText(1,QString("%1 (vypocteno: %2)").arg(checksum).arg(check2));
    item.addChild(it);
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
