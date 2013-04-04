#ifndef DIALOGGRAPHS_H
#define DIALOGGRAPHS_H

#include <QDialog>
#include "paket.h"
#include <QMap>
#include "qcustomplot.h"

class QMenu;
namespace Ui {
class dialogGraphs;
}

class dialogGraphs : public QDialog
{
    Q_OBJECT
    
public:
    explicit dialogGraphs(QWidget *parent = 0);
    ~dialogGraphs();
    
private:
    Ui::dialogGraphs *ui;
    bool filterPacket(const packet_t & in, quint32 & retVal, QDateTime & retTime, const packet_t & filter);
    QMap<QCPGraph *,packet_t> filters;
    QMenu * menu;
    void addTrack(packet_t & filter);

private slots:
    void rec_paket(packet_t);
    void on_treeFiltry_customContextMenuRequested(const QPoint &pos);
    void on_actionNew_filter_triggered();
    void on_actionDelete_filter_triggered();
    void on_actionEdit_filter_triggered();
};

#endif // DIALOGGRAPHS_H
