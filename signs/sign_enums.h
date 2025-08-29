#ifndef SIGN_ENUMS_H
#define SIGN_ENUMS_H

namespace  ESignCharacteristic  {
     enum Type {
        COURCE = 951, // Курс (дирекционный угол)
        NATIONALITY = 1100, // госпринадлежность 1-свой 2-противник 3-союзник 4-нетрал
        PARAM_ACTION = 1200, // характер действи 1-фактическое 2-планируемое
        PARAM_COLOR_SIGN = 1300, // Призн. цвета усл. знака 5 - зеленый (неопознаный), 6 красный, 8 синий, 0 - многоцветный
        PARAM_STATE= 1500,  // Призн. состояния объекта 2-поврежден, 3-уничтожен
        TEXT = 5400, // Подпись знака - символьное значение
        TEXT_BACKGROUND = 9221, // Фон подписи 1-прозрачный 2-белый
        TEXT_STYLE = 9222, // Наклон текста 1-без наклона 2-курсив
        SCALE = 31001, // масштаб отображения в процентах
        COLOR_RGB = 31002, // цвет отображения в RGB
        TEXT_HEIGHT = 31003, // ВЫСОТА ШРИФТА
        LINE_WIDTH = 31005, // ТОЛЩИНА ЛИНИИ В ММ
        LINE_END_WIDTH = 31006, // ТОЛЩИНА ОКОНЧАНИЯ ЛИНИИ В ММ
        POLYGON_COLOR = 31013 // ЦВЕТ ОТОБРАЖЕНИЯ ПОЛИГОНА RGB
    };
};

namespace ESignParamState{
    enum Type {
        NORMAL = 1, ///<  - зеленый (неопознаный),
        CORRUPTED = 2, ///<  поврежден
        DESTROYED = 3 ///<  уничтожен
    };
};
namespace EParamSignColorStates{
    enum Type {
        LIGHT_GREEN = 5, ///<  - зеленый (неопознаный),
        RED = 6, ///<  красный,
        BLUE = 8, ///< синий,
        MANY = 0 ///< - многоцветный
    };
};

namespace ESignColorStates{
    enum Type {
         BLUE = 16711680,       ///< установить цвет Синий
         LIGHT_GREEN = 65280,   ///< установить цвет Зеленый
         RED = 255,              ///< установить цвет Красный
         YELLOW = 0x00c52d,              ///< установить цвет Ж 58c0ff #00860a 00c52d
         BLACK = 0,
    };
};


namespace ENationalityStates{
    enum Type {
         NOT_SET = 0, ///< не установлено
         OWN = 1,     ///< свой
         ENEMY = 2,   ///< противник
         ALLY = 3,    ///< союзник
         NEUTRAL = 4  ///< нейтрал
    };
};

namespace EParamActionStates{
    enum Type {
      ACTUAL = 1,  ///< фактическое действие
      PLANED = 2,  ///< планируемое действие
    };
};

namespace ETextStyledStates{
  enum Type {
    NORMAL = 1,  ///< Наклон текста 1-без наклона
    ITALIC = 2,  ///< курсив
  };
};

namespace EBackgroundTextStates{
  enum Type {
    TRANSPARENT = 1,  ///< Фон подписи прозрачный
    WHITE = 2,  ///< Фон подписи белый
  };
};



#endif // SIGN_ENUMS_H
