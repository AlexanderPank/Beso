#ifndef DIAGRAMCOLORS_H
#define DIAGRAMCOLORS_H
#include <QColor>

struct DiagramColors {
    QColor algorithmBackground{QStringLiteral("#E3E3FD")};
    QColor objectBackground{QStringLiteral("#D3D3D3")};
    QColor elementBackground{QStringLiteral("#FFFFE3")};
    QColor textBackground{Qt::white};
    QColor eventBackground{QStringLiteral("#FDE3E3")};
    QColor inputDataBackground{QStringLiteral("#E3FDE3")};
    QColor outputDataBackground{QStringLiteral("#E3E3FD")};
    QColor paramBackground{QStringLiteral("#FFF3E3")};
    QColor inputCircle{Qt::green};
    QColor outputCircle{QStringLiteral("#37a2d7")};
    QColor selfOutputCircle{QStringLiteral("#ffb400")};
    QColor arrowColor{Qt::black};
};

extern DiagramColors gDiagramColors;

#endif // DIAGRAMCOLORS_H
