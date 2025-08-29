#ifndef MAP_ENUMS_H
#define MAP_ENUMS_H

enum CoordType { GEO,  //географические координаты.
                 PLANE, // Kоординаты в плоскости.
                 PLANE42, // координаты в плоскости 42.
                 PIC,  //координаты в формате картинки (пиксели).
               };

namespace SignLocalType {
    enum Type{
        LOCAL_LINE = 0,
        LOCAL_SQUARE = 1,
        LOCAL_POINT = 2,
        LOCAL_TITLE = 3,
        LOCAL_VECTOR = 4,
        LOCAL_MIXED = 5,
        LOCAL_COUNT = 6,
        LOCAL_UP = 6
    };
}

namespace EditMode{
    enum Type{
        None = 0 ,
        Select = 1 ,        // знак выделен и готов к изменению
        MoveObject = 2 ,
        MovePoint = 3 ,
        ScaleObject = 4 ,
        RotateObject = 5,    // знак будет вращаться
        Roulette = 6    // режим рулетки
    };
}
#endif // MAP_ENUMS_H
