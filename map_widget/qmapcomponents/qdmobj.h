/******* QDMOBJ.H  *************************************************
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2018              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                       FOR Qt-DESIGNER                            *
*                                                                  *
********************************************************************
*                                                                  *
*     ОПИСАНИЕ КЛАССА "КОМПОНЕНТ - ОБЪЕКТ ЭЛЕКТРОННОЙ КАРТЫ"       *
*                                                                  *
*******************************************************************/

#ifndef QDMOBJ_H
#define QDMOBJ_H

#include "qdmobja.h"
#include "qdmcmp.h"
#include "qdmpoint.h"
#include "mapapi.h"

class QDMapSemantic;
class QDMapMetric;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ КЛАСС "КОМПОНЕНТ - ОБЪЕКТ ЭЛЕКТРОННОЙ КАРТЫ"                ++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class QDMapObj : public QDMapObjAcces
{
  Q_OBJECT

  public :
    QDMapObj(QWidget * parent = 0, const char * name = 0);
    ~QDMapObj();

    // Создать объект карты по его описанию (для совместимости со старыми версиями)
    void CreateObject(int site, int kind, bool text, int list, int excode, int local);
    // Создать пустой объект
    void CreateNullObj(int sitenumber);
    // Записать изменения в карту
    void Commit();
    // Удалить объект из карты
    void Delete();
    // Найти объект карты по его уникальным параметрам
    int  SeekObject(char *listname, int key);
    // Переместить отображение объекта карты в центр окна
    void Center();

    enum  TOBJECTSTYLE     // Способ визуализации объектов карты
    {
      OS_NOVIEW = 1,       // Объект не отображается
      OS_NORMAL = 2,       // Отображаются в нормально виде
      OS_SELECT = 3        // Объект выделяется заданным способом
    };

    enum TOBJECTSTYLESELECT   // Тип Специального (выделенного) отображения объекта карты
    {
      SEL_LINE     = 1     // Выделение линией. Параметры Image, Mode, Parm игнорируются
                           // пользователем параметров Image, Mode, Parm
                           // рисуются утолщенной линией в 2 пиксела (функция IMGLINE)
    };


    int            GetKey();
    int            GetExCode();
    int            GetInCode();
    int            GetLocal();
    const char *   GetObjName();
    int            GetImage();
    int            GetMode();
    const char *   GetParm();
    char *         GetListName();
    char *         GetText(int subobj);
    bool           GetTextType();
    double         GetSquare();
    double         GetPerimeter();
    double         GetLength();
    int            GetSiteNumber();
    // Возвращает идентификатор пользовательской карты
    HSITE          GetSiteHandle();
    int            GetListNumber();
    int            GetPlaceOut();
    // Возвращает габариты объекта в указанной системе координат
    MAPDFRAME      GetObjFrame(int place);
    int            GetLayerNumber();
    void           SetStyle(int value);
    void           SetColorImage(int value);
    // Установить тип выделенного отображения объекта карты
    void           SetStyleSelect(int value);
    void           SetText(int subobj, char * value);
    void           SetObjHandle(HOBJ value);
    // Установить/запросить верхнюю и нижнюю границу масштаба
    void           SetBotTopScale(int bot, int top);
    void           GetBotTopScale(int *bot, int *top);
    virtual void   SetMapView(QDMapView *value);
    QDMapView      *GetMapView() const;

    /*!
     * \brief GetPointCount - запросить количеств о точек объекта
     * \param subject - номер подобъекта
     * \return 0 при ошибке
     */
    long int GetPointCount(long int subject);

    /*!
     * \brief GetPlanePoint - Запрос координаты точки в системе координат документа
     * \param subject - номер подобъекта
     * \param number - номер точки в подобъекте
     * \param outPoint - указатель на DOUBLEPOINT для заполнения
     * \return 0 при ошибке
     */
    long int GetPlanePoint(long int subject, long int number, DOUBLEPOINT* outPoint);

    /*!
     * \brief AppendPlanePoint - Добавить точку в объект
     * \param subject - номер подобъекта
     * \param inPoint - указатель на DOUBLEPOINT для копирования координат
     * \return
     */
    long int AppendPlanePoint(long int subject, DOUBLEPOINT* inPoint);

    // Установить описание объекта используя диалог
    // Вход    acreate - надо ли после успешного выполнения диалога сформировать описание
    //                QDMapObj (вид объекта, метрика, семантика) Заново или же установить
    //                только вид
    // Выход = true - вид объекта и карта установлены
    bool           SetObjectByDLg(bool acreate);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Блок функций для внутреннего использования
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Перерисовать объект карты
    void           Repaint();
    void           MapAction(int acttype);
    void           Paint(HDC dc, RECT updateRect);
    void           RepaintPrivate(QDMapViewAcces *mapview);
    // Освободить текущий объект карты
    void           FreeCurrentObj();

public :
    // Пользователю только на чтение !!!!!!!!
    QDMapSemantic    *VarSemantic;
    QDMapMetric      *VarMetric;
    MAPDFRAME        VarObjFrame;
    PAINTPARM        VarImage;
    int              VarStyle;
    int              VarPlace;           // система координат по MapApi PP_
    int              VarColorImage;
    int              VarColorImageLin;
    int              VarInCodeImage;
    int              VarStyleSelect;     // тип выделения
    bool             VarOldFrame;
    char             StrVal[256];

private:
    HPAINT PaintControl;
};

#endif // QDMOBJ_H
