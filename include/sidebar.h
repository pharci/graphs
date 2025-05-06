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
	void createMatrix();
	void drawMatrix();
	void setSize(int sizeParam);
	bool isWeighted();
	bool isOriented();
	void setWeighted(bool flag);
	void setOriented(bool flag);
private:
	Board *board;
	QLineEdit *SizeEdit;
	QGridLayout *grid;
	QVector<QVector<QLineEdit *>> matrix;
	QVector<QLineEdit *> nodes;
	QCheckBox *weighted;
	QCheckBox *oriented;
	int size = 5;

	void paintEvent(QPaintEvent *event) override;
	void onMatrixChanged();
	void onOrientedChange();
	void Generate();
};