#include "graphplotter.h"

GraphPlotter::GraphPlotter(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void GraphPlotter::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    // frame
    painter.setPen(QPen(QBrush(QColor(50, 50, 50)), 2));
    painter.drawRect(BORDER_SIZE, BORDER_SIZE,
        width() - 2 * BORDER_SIZE, height() - 2 * BORDER_SIZE);
}
