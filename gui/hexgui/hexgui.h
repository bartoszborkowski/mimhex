#ifndef HEXGUI_H
#define HEXGUI_H

#include <QDialog>
#include <QLabel>
#include "manager.h"

class QLineEdit;

class HexGui: public QDialog
{
Q_OBJECT

public:
  HexGui(QWidget *parent = 0);

//private slots:
//  void typeIndexChanged(const QString& text);
private slots:
    void setPlayouts();
    void setTreeDepth();

    void setWinner(const Player&);
//private:
//  QLineEdit *labelEdit;
private:
    manager *m;
    QLabel *statebar;
};

#endif // HEXGUI_H
