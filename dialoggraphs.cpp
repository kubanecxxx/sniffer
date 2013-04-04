#include "dialoggraphs.h"
#include "ui_dialoggraphs.h"
#include <QMenu>
#include "dialogfilterfiller.h"

Q_DECLARE_METATYPE(QCPGraph *)
Q_DECLARE_METATYPE(QTreeWidgetItem *)

dialogGraphs::dialogGraphs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogGraphs)
{
    ui->setupUi(this);

    menu = new QMenu;
    menu->addAction(ui->actionNew_filter);
    menu->addSeparator();
    menu->addAction(ui->actionEdit_filter);
    menu->addAction(ui->actionDelete_filter);

    ui->treePakety->addAction(ui->actionClear);
    connect(ui->actionClear,SIGNAL(triggered()),ui->treePakety,SLOT(clear()));

    ui->plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->plot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->plot->xAxis->setLabel(trUtf8("Čas"));
    ui->plot->yAxis->setLabel(trUtf8("Hodnota [-]"));
    ui->plot->legend->setVisible(true);
    ui->plot->legend->setPositionStyle(QCPLegend::psTopLeft);

    ui->treePakety->header()->setResizeMode(QHeaderView::ResizeToContents);

    packet_t filter;
    filter.name = "teplota";
    filter.dest_addr = 1;
    filter.source_addr = 5;
    filter.type = 4;
    filter.address = 0;

    addTrack(filter);
}

dialogGraphs::~dialogGraphs()
{
    delete ui;
}

void dialogGraphs::rec_paket(packet_t packet)
{
    QMapIterator<QCPGraph *, packet_t> it(filters);

    while(it.hasNext())
    {
        it.next();
        quint32 value;
        QDateTime time;
        if (filterPacket(packet,value,time,it.value()))
        {
            it.key()->addData(time.toTime_t(),value);

            QTreeWidgetItem * item = new QTreeWidgetItem;
            item->setData(0,Qt::DisplayRole,time.time());
            item->setText(1,it.key()->name());
            item->setData(2,Qt::DisplayRole,value);
            ui->treePakety->addTopLevelItem(item);

            ui->plot->rescaleAxes();
            ui->plot->yAxis->setRangeLower(-1);
            ui->plot->replot();
        }
    }
}

bool dialogGraphs::filterPacket
(const packet_t & in, quint32 & retVal, QDateTime & retTime, const packet_t & filter)
{
    if (in.source_addr != filter.source_addr)
        return false;

    if (in.dest_addr != filter.dest_addr)
        return false;

    if (in.address != filter.address)
        return false;

    if (in.type != filter.type)
        return false;

    if (in.checksum != in.checksumLocal)
        return false;

    retVal = in.data;
    retTime = in.time;
    return true;
}

void dialogGraphs::on_treeFiltry_customContextMenuRequested(const QPoint & pos)
{
    QTreeWidgetItem * item = ui->treeFiltry->itemAt(pos);

    bool ok = item != NULL;

    ui->actionDelete_filter->setVisible(ok);
    ui->actionEdit_filter->setVisible(ok);

    if (ok)
    {
        QCPGraph * graph = item->data(0,Qt::UserRole).value<QCPGraph*>();
        packet_t filter = filters.value(graph);
        QString name = QString(" \"%1\"").arg(filter.name);
        ui->actionDelete_filter->setText(trUtf8("Delete filter") + name);
        ui->actionEdit_filter->setText(trUtf8("Edit filter") + name);

        foreach(QAction * act, menu->actions())
        {
            act->setProperty("graph",QVariant::fromValue(item));
        }
    }

    menu->popup(QCursor::pos());
}

void dialogGraphs::addTrack(packet_t &filter)
{
    QCPGraph * graph = ui->plot->addGraph();
    graph->setScatterStyle(QCP::ssCross);

    graph->setName(filter.name);
    graph->setPen(filter.color);

    filters.insert(graph,filter);
    QTreeWidgetItem * item = new QTreeWidgetItem;

    item->setData(0,Qt::UserRole,QVariant::fromValue(graph));
    item->setText(0,filter.name);

    ui->treeFiltry->addTopLevelItem(item);
    ui->plot->repaint();
}

void dialogGraphs::on_actionNew_filter_triggered()
{
    packet_t filter;
    dialogFilterFiller dlg(this,filter);
    if (dlg.exec() == dialogFilterFiller::Accepted)
    {
        addTrack(filter);
    }
}

void dialogGraphs::on_actionDelete_filter_triggered()
{
    QTreeWidgetItem * item = ui->actionDelete_filter->property("graph").value<QTreeWidgetItem*>();;
    QCPGraph * graph = item->data(0,Qt::UserRole).value<QCPGraph*>();
    delete item;
    filters.remove(graph);
    ui->plot->removeGraph(graph);
}

void dialogGraphs::on_actionEdit_filter_triggered()
{
    QTreeWidgetItem * item = ui->actionDelete_filter->property("graph").value<QTreeWidgetItem*>();;
    QCPGraph * graph = item->data(0,Qt::UserRole).value<QCPGraph*>();
    packet_t packet = filters.take(graph);
    dialogFilterFiller dlg(this,packet);

    if (dlg.exec() == QDialog::Accepted)
    {
        graph->setName(packet.name);
        graph->setPen(packet.color);
        item->setText(0,packet.name);
        ui->plot->repaint();
    }

    filters.insert(graph,packet);
}
