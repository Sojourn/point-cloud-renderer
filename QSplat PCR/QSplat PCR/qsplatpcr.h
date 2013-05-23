#ifndef QSPLATPCR_H
#define QSPLATPCR_H

#include <QtWidgets/QMainWindow>
#include "ui_qsplatpcr.h"

class QSplatPCR : public QMainWindow
{
	Q_OBJECT

public:
	QSplatPCR(QWidget *parent = 0);
	~QSplatPCR();

private:
	Ui::QSplatPCRClass ui;
};

#endif // QSPLATPCR_H
