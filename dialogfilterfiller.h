#ifndef DIALOGFILTERFILLER_H
#define DIALOGFILTERFILLER_H

#include <QDialog>
#include "paket.h"

namespace Ui {
class dialogFilterFiller;
}

class dialogFilterFiller : public QDialog
{
    Q_OBJECT
    
public:
    explicit dialogFilterFiller(QWidget *parent, packet_t &packet);
    ~dialogFilterFiller();
    
private slots:
    void on_butColor_clicked();
    void on_spinType_valueChanged(int arg1);


private:
    Ui::dialogFilterFiller *ui;
    packet_t & packet;
    void accept();
};

#endif // DIALOGFILTERFILLER_H
