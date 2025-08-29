/****** MWHANDLR.H ************** Savelov A.S.  ******* 08/02/17 ***
*                                                                  *
*              Copyright (c) PANORAMA Group 1991-2017              *
*                      All Rights Reserved                         *
*                                                                  *
********************************************************************
*                                                                  *
*                           FOR  LINUX                             *
*                                                                  *
********************************************************************
*                                                                  *
*            ��������� ��������� ���� ����������� �����            *
*                                                                  *
*******************************************************************/

#ifndef MWHANDLR_H
#define MWHANDLR_H

#define    DT_LOADFROMSXF         1  // ��������� �� SXF � MAP
#define    DT_GETTYPEOBJECTCREATE 2  // ������� ��� ������� ����� �� 
                                     // �������������� ��� ��������
#define    DT_OBJECTINFOBEGIN     3  // ������� ������ ���������� �� ������� �����
#define    DT_SITECREATEFORMAP    4  // ������� ����� ������������ ��� �������� �����
#define    DT_OBJECTINFODESTROY   5  // ������� ������ ���������� �� ������� �����
#define    DT_CLOSEDATAFORMAP     6  // ������� ������ �������� ������ �����
#define    DT_SETVIEWSTAFF        7  // ���������� ������ �����������
#define    DT_SETFINDOBJECTPARAM  8  // ���������� ������ ������
#define    DT_OBJECTINFOCONTINUE  9  // ��������� ������ ���������� �� ������� ����� �� ����� ��������
#define    DT_VIEWDATALIST        10 // ������ ������� ���������
#define    DT_LOADBMP             11 // ������ �������� BMP
#define    DT_OBJECTINFOBYSEEK    12 // ����� ������� �� ������
#define    DT_MEDSEEKOBJECT       13 // ������ �������� �������
#define    DT_PALETTER            14 // ������ ���������� ��������
#define    DT_GETSITENUMBER       15 // ������ ������ ������ �����       
#define    DT_ZONEFORSELECT       16 // ������ ���������� ���� ������ ����������
#define    DT_SETPARMPRINT        17 // ������ ���������� ������
#define    DT_MAP2SXF             18 // ������ �������� � sxf
#define    DT_SORT                19 // ������ ����������

#define    MainLinuxIdent 1

#define    MH_GETCURSORPOS        MainLinuxIdent       // 1 ��������� ������� �������
#define    MH_SETCURSORPOS        MainLinuxIdent + 1   // 2 ���������� ������� �������
#define    MH_SCREENTOCLIENT      MainLinuxIdent + 2   // 3
#define    MH_CLIENTTOSCREEN      MainLinuxIdent + 3   // 4
#define    MH_CLOSE               MainLinuxIdent + 4   // 5 ������� ����
#define    MH_GETKEYSTATE         MainLinuxIdent + 5   // 6 ������ � ��������� ������
#define    MH_OPENFILE            MainLinuxIdent + 6   // 7 ������� ����
#define    MH_GETXCONTEXT         MainLinuxIdent + 7   // 8 ������ ������������ ���������  // 26/03/02
                                              //   �������� ���� �����
#define    MH_AFTERSTART          MainLinuxIdent + 9   // 9 �������� ���������� ���� ��������� ��������� ���� ������ � ��������
#define    MH_SELVIEWOBJ          MainLinuxIdent +10   // 10 ������ � ������������ ��������
                                              // WPARAM = 1 - ������    LPARAM - HOBJ
                                              // WPARAM = 2 - ���������
					      
#endif  // MWHANDLR_H

