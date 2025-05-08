#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include "board.h"
#include <QVector>
#include <QCheckBox>

class Sidebar : public QWidget
{
	Q_OBJECT
public:
	explicit Sidebar(Board *board, QWidget *parent = nullptr);
	~Sidebar();
	QVector<QVector<QLineEdit *>>* getMatrix();
	QVector<QLineEdit *>* getNodes();
	void createMatrix();
	void drawMatrix();
	void setSize(int sizeParam);
private:
	Board *board;
	QLineEdit *SizeEdit;
	QGridLayout *grid;
	QVector<QVector<QLineEdit *>> matrix;
	QVector<QLineEdit *> nodes;
	int size = 5;

	void paintEvent(QPaintEvent *event) override;
	void Generate();
};