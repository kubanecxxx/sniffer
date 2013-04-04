#include "dialogfilterfiller.h"
#include "ui_dialogfilterfiller.h"
#include <QColorDialog>

dialogFilterFiller::dialogFilterFiller(QWidget *parent, packet_t &Packet) :
    QDialog(parent),
    ui(new Ui::dialogFilterFiller),
    packet(Packet)
{
    ui->setupUi(this);
    QStringList list;
    list << TYPES;

    //fill dialog from packet
    QString sheet;
    sheet = QString("QPushButton {background-color: %1;}").arg(packet.color.name());
    ui->butColor->setProperty("color",packet.color);
    ui->butColor->setStyleSheet(sheet);
    ui->editName->setText(packet.name);
    ui->spinDataAddr->setValue(packet.address);
    ui->spinDestAddr->setValue(packet.dest_addr);
    ui->spinSourceAddr->setValue(packet.source_addr);
    ui->spinType->setValue(packet.type);
    ui->spinType->setSuffix(QString(" (%1)").arg(list.at(packet.type)));
}

dialogFilterFiller::~dialogFilterFiller()
{
    delete ui;
}

void dialogFilterFiller::on_butColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->butColor->property("color").value<QColor>(),this);

    if (color.isValid())
    {
        ui->butColor->setProperty("color",color);
        QString sheet;
        sheet = QString("QPushButton {background-color: %1;}").arg(color.name());
        ui->butColor->setStyleSheet(sheet);
    }
}

void dialogFilterFiller::on_spinType_valueChanged(int arg1)
{
    QStringList list;
    list << TYPES;

    QSpinBox * box = qobject_cast<QSpinBox *>(sender());
    box->setSuffix(QString(" (%1)").arg(list.at(arg1)));
}

void dialogFilterFiller::accept()
{
    packet.color = ui->butColor->property("color").value<QColor>();
    packet.name = ui->editName->text();
    packet.address = ui->spinDataAddr->value();
    packet.dest_addr = ui->spinDestAddr->value();
    packet.source_addr = ui->spinSourceAddr->value();
    packet.type = ui->spinType->value();

    QDialog::accept();
}
