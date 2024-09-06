#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include <QtWidgets>

#define BORDER_SIZE 5

class GraphPlotter : public QWidget
{
    Q_OBJECT
public:
    GraphPlotter(QWidget* parent = nullptr);
    virtual void paintEvent(QPaintEvent*);
};

#endif // GRAPHPLOTTER_H
