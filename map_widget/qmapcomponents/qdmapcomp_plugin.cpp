/******* QDMAPCOMP_PLUGIN.CPP **************************************
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
*                      PLUGIN'S  ŒÃœŒÕ≈Õ“                          *
*                                                                  *
*******************************************************************/

#include <QtCore/qplugin.h>
#include <qmessagebox.h>

#include "qdmapcomp_plugin.h"
#include <qdmwinw.h>
#include <qdmcmp.h>
#include <qdmpoint.h>
#include <qdmobj.h>
#include <qdmfind.h>
#include <qdmdsel.h>
#include <qdmselob.h>
#include <qdmconv.h>
#include <qdmdlist.h>
#include <qdmrect.h>
#include <qdmactpt.h>

#ifndef HIDE_PRINTSUPPORT
  #include <qdmdprn.h>
#endif

#include <qdmdcmtw.h>
#include <qdmdgtob.h>

#ifndef HIDE_GISSERVER_SUPPORT
  #include <qdmgsrv.h>
#endif

#ifndef HIDE_3DSUPPORT
  #include <qdmd3d.h>
#endif

#include <qdmnet.h>
#include <qdmdwmspopup.h>
#include <qdmactline.h>

const char * mapview_xpm[] = {
  "24 24 47 1 ",
  "  c #313131",
  ". c #464646",
  "X c #1D639A",
  "o c #24689D",
  "O c #2871AF",
  "+ c #2278CA",
  "@ c #237CD4",
  "# c #4383B8",
  "$ c #558BB6",
  "% c #3382C3",
  "& c #2A84D8",
  "* c #338CDC",
  "= c #3791DF",
  "- c #3A95E2",
  "; c #4795DD",
  ": c #599AD4",
  "> c #689CC5",
  ", c #76A6D3",
  "< c #459AE2",
  "1 c #57A2E4",
  "2 c #66A9E5",
  "3 c #77B0E5",
  "4 c #929292",
  "5 c #ACACAC",
  "6 c #A8AEB2",
  "7 c #AAB3BA",
  "8 c #B2B2B2",
  "9 c #8BB1D2",
  "0 c #94BADD",
  "q c #89BAE6",
  "w c #93BFE8",
  "e c #A7C2DE",
  "r c #99C3E7",
  "t c #A8CCEB",
  "y c #B8D1E7",
  "u c #B5D1E9",
  "i c #CACACA",
  "p c #D5D5D5",
  "a c #C8D8E6",
  "s c #D3DEE8",
  "d c #C6DDF2",
  "f c #D9E2E9",
  "g c #CFE3F3",
  "h c #D8E7F4",
  "j c #E6E6E6",
  "k c #EBF1F6",
  "l c #FEFEFE",
  /* pixels */
  "jjjjjjjjjjjjjjjjjjjjjjjj",
  "jjllllllllllkkljjjjjjjjj",
  "jl4 . il8 l8  8lljjjjjjj",
  "l8 5i5 l8 l ii iljjjjjjj",
  "l.4lll5l8 l jlllljjjjjjj",
  "l iliiil8 l6.  7l:sjjjjj",
  "l.8l   l8 lllli l>o0jjjj",
  "l4 llp.l5 l ll5 lkfoejjj",
  "jl5   4l6 l6   7lkkhoujj",
  "jjllllylllllllllgwhw0$sj",
  "jjjjjgX-------2udq<2g#tj",
  "jjjjg>%------1hkkkhhgu>j",
  "jjjjuX---=-=-tkkkkkktfXa",
  "jjjjtX--==*=*tkkkkkkgwXa",
  "jjjjqXy1****-3kkkkkkktXa",
  "jjjjrokk2***&&2<;hkkkqXa",
  "jjjjyXkkkt-**&*@&rkkk=Xj",
  "jjjjjXfkkk3@*@&&@3kku;Xj",
  "jjjjj>#kkk;@@&@&@rkk:#9j",
  "jjjjjjO9kw@@@@@@@3ku#Xjj",
  "jjjjjjsO$@@@@@@@@@q#Xjjj",
  "jjjjjjja#XO@+@+@+O#Xfjjj",
  "jjjjjjjjjqOXXXoXo+ejjjjj",
  "jjjjjjjjjjjy03,0ajjjjjjj"
};

const char* mapwindow_xpm[] = {
  "24 24 45 1 ",
  "  c #313131",
  ". c #3F4040",
  "X c #464646",
  "o c gray34",
  "O c #1D639A",
  "+ c #226AA4",
  "@ c #2B76B7",
  "# c #2278CA",
  "$ c #237CD4",
  "% c #4383B8",
  "& c #558BB6",
  "* c #2A84D8",
  "= c #338CDC",
  "- c #3691DF",
  "; c #4795DD",
  ": c #5399DD",
  "> c #6D9BC0",
  ", c #7AA8D5",
  "< c #56A1E3",
  "1 c #68A9E4",
  "2 c #76AFE4",
  "3 c #929292",
  "4 c #8092A3",
  "5 c #ACACAC",
  "6 c #A8AEB2",
  "7 c #AAB3BA",
  "8 c #B2B2B2",
  "9 c #8FB4D6",
  "0 c #88B8E4",
  "q c #93BFE8",
  "w c #A7C2DE",
  "e c #98C2E7",
  "r c #A8CBE9",
  "t c #B4CEE6",
  "y c #B9D1E7",
  "u c #CACACA",
  "i c #D5D5D5",
  "p c #C8D6E4",
  "a c #D5DEE7",
  "s c #C6DEF2",
  "d c #DAE1E6",
  "f c #D9E7F3",
  "g c #E6E6E6",
  "h c #EBF1F6",
  "j c #FFFFFF",
  /* pixels */
  "oooooooooooooooooooggggg",
  "ojjjjjjjjjjjjjjjjjoggggg",
  "ojj3 X uj8 j8  8jjoooggg",
  "oj8 5u5 h8 j uu ujojoggg",
  "ojX3jjj5j8 j hhjjjojoggg",
  "oj ujuiuj5 j8.  7jojoggg",
  "ojX8h   j5 jjjju jojoggg",
  "oj3 jjiXj8 j jj5 jojoggg",
  "ojj5   3j6 j6   7jojoygg",
  "ojjjjjjjjjjjjjjjjjojo&ag",
  "ooooooooooooooooooojo&rg",
  "gdojjjjjjjjjjjjjjjjjot>g",
  "ggoooooooooooooooooo4fOp",
  "ggggrO-----==rhhhhhhsqOp",
  "gggg0Os<=====2hhhhhhhtOp",
  "ggggeOhh1=====1;;fhhh0Op",
  "ggggyOhhfr;=*****ehhh;Og",
  "gggggOfhhh2******2hhr;Og",
  "ggggg>%hhh;****$$ehh:%9g",
  "gggggg@,hq$$$$$$$1hy%Ogg",
  "gggggga+&$$$$$$$$$0%Oggg",
  "gggggggp%O@#$$#$#@@Odggg",
  "ggggggggg9@+OOOO+@wggggg",
  "gggggggggggy0,99aggggggg"
};

const char * mappoint_xpm[] = {
  "24 24 31 1 ",
  "  c #333333",
  ". c #353535",
  "X c #3B3B3B",
  "o c #3F3F3F",
  "O c #454545",
  "+ c #4B4B4B",
  "@ c #4E4E4E",
  "# c gray33",
  "$ c #585858",
  "% c #5E5E5E",
  "& c #666666",
  "* c #6D6D6D",
  "= c #737373",
  "- c #7D7D7D",
  "; c gray51",
  ": c gray52",
  "> c gray55",
  ", c #959595",
  "< c #9E9E9E",
  "1 c #A5A5A5",
  "2 c #AAAAAA",
  "3 c #B3B3B3",
  "4 c gray73",
  "5 c gray74",
  "6 c #C5C5C5",
  "7 c #CCCCCC",
  "8 c #D3D3D3",
  "9 c #DADADA",
  "0 c #DEDEDE",
  "q c #E2E2E2",
  "w c #E6E6E6",
  /* pixels */
  "wwwwwwwwwwwwwwwwwwwwwwww",
  "ww&qwwwwwwwwwwwwwwwwwwww",
  "wq 7wwwwwwwwwwwwwwwwwwww",
  "w1 >wwwwwwwwwwwwwwwwwwww",
  "w& +qwwwwwwwwwwwwwwwwwww",
  "wwo5wwwwwwwwwwwwwwwwwwww",
  "wwo5wwwwwwwwwqwwwwwwwwww",
  "wwo5ww&-wwww< *3wwww2=ww",
  "wwo5ww3 ,ww2.,;.0ww8.<ww",
  "wwo5www& 89.&wwO%ww##www",
  "wwo5wwwwX+@Xwww6.1,.9www",
  "wwo4wwww6..6wwwq=X.1wwww",
  "wwo5wwww9..0wwww9#:wwwww",
  "wwo5wwww%X+%wwwww$:wwwww",
  "wwo5www3.76 4wwww$:wwwww",
  "wwo5ww0.=ww&Oqwww$:wwwww",
  "wwo5ww-+qww0.,www#:wwwww",
  "wwo5wwwwwwwwwwwwwwwwwwww",
  "wwo5wwwwwwwwwwwwwwwwwwww",
  "wwo5wwwwwwwwwwwwwwwwwwww",
  "wwo5555555555555541-6www",
  "ww.XXXXXXXXXXXXXXX.  .3w",
  "ww0qqwwqqwwqqwwwwq<&3www",
  "wwwwwwwwwwwwwwwwwwqwwwww"
};

const char * mapobj_xpm[] = {
  "24 24 29 1 ",
  "  c gray10",
  ". c #212121",
  "X c gray14",
  "o c #2C2C2C",
  "O c #393939",
  "+ c gray24",
  "@ c gray25",
  "# c #4B4B4B",
  "$ c #5A5A5A",
  "% c #6A6A6A",
  "& c #797979",
  "* c #848484",
  "= c gray56",
  "- c #949494",
  "; c #9B9B9B",
  ": c #A5A5A5",
  "> c #ACACAC",
  ", c #B2B2B2",
  "< c #B6B6B6",
  "1 c gray72",
  "2 c gray74",
  "3 c #C4C4C4",
  "4 c #CCCCCC",
  "5 c #D1D1D1",
  "6 c #D5D5D5",
  "7 c #DADADA",
  "8 c gray89",
  "9 c #E6E6E6",
  "0 c white",
  /* pixels */
  "999999999999999989999999",
  "9999993799999999 :999999",
  "99999  #5999999  o999999",
  "9999O6  o49991 , :+19999",
  "999 60   *999119 :119999",
  "9992 0  .3999999 :999999",
  "99999@ .<9999999 ,999999",
  "9999    $499999   999999",
  "997 00   o5999 > : 89999",
  "99 ;00    =9, <9 :, ,999",
  "99 500    :9,,99 :9,,999",
  "998 60   %999999 :999999",
  "9999 X  %999999   999999",
  "999 60   #5999 < - 99999",
  "99 500    O7, ,9 ;, 9999",
  "9$;000     ; ,99 :9, ,99",
  "9$>000     &,899 ;99< 99",
  "9,.000    .49999 :999999",
  "99< *7   O<99999 ;999999",
  "9991<  &39999999 ;999999",
  "999999 ,99999999 :999999",
  "999999 ,>9999999 ;>99999",
  "999999   9999999   99999",
  "999999999999999999999999"
};

const char * mapfind_xpm[] = {
  "24 24 36 1 ",
  "  c #1B1B1B",
  ". c #222222",
  "X c #333333",
  "o c gray26",
  "O c gray49",
  "+ c #0A70AB",
  "@ c #197DB7",
  "# c #458BB4",
  "$ c #1A87C6",
  "% c #1E91D4",
  "& c #268CC9",
  "* c #2991CD",
  "= c #3596CC",
  "- c #2996D6",
  "; c #2EA3DD",
  ": c #39B7E7",
  "> c #479FCE",
  ", c #67A8CD",
  "< c #79A9C4",
  "1 c #66B8E4",
  "2 c #74C1E6",
  "3 c #9B9B9B",
  "4 c gray70",
  "5 c #9BBDD0",
  "6 c #9DC4DB",
  "7 c #A6C5D7",
  "8 c #BAD3DB",
  "9 c #ABD3E4",
  "0 c #B7D6E6",
  "q c #C6C6C6",
  "w c #CBD9DE",
  "e c gainsboro",
  "r c #D8DEE2",
  "t c #DDE2E5",
  "y c #E6E6E6",
  "u c white",
  /* pixels */
  "yyyyyyyyyyyyyyyyyyyyyyyy",
  "yyyXyyyyy.yyyyyyyyyyyyyy",
  "yy...yyy...yyyyyyyyyyyyy",
  "yy..Xyyyy.yyyyyyyyyyyyyy",
  "y4..o4yy...yyyyyyyyyyyyy",
  "y.y. .y.y.y yyyyyyyyyyyy",
  "y.q. Xyey.yyyyyyyyyyyyyy",
  "yy..X4yy...yyyyyyyyyyyyy",
  "e.u.  q.y.y.%tyyyyyyyyyy",
  "4.u.. Oyy.yy%%0yyyyyyyyy",
  "yy...3yyy.yy%:-9yyyyyyyy",
  "yye.yyyyy.ey%::-9yyyyyyy",
  "yyy..yyyy..y$:::%2wyyyyy",
  "yyyyyyyyyyyy@;;;;$19yyyy",
  "yyyyyyyyyyyy@*---$$=8yyy",
  "yyyyyyyyyyyy@**-$@,67yyy",
  "yyyyyyyyyyyy@-$$$>yyyyyy",
  "yyyyyyyyyyyy@&@@&$7yyyyy",
  "yyyyyyyyyyyy@@r@@$*wyyyy",
  "yyyyyyyyyyyy@yy8@@$,yyyy",
  "yyyyyyyyyyyyyyyy<@@&<yyy",
  "yyyyyyyyyyyyyyyyy#+@7yyy",
  "yyyyyyyyyyyyyyyyy7@5eyyy",
  "yyyyyyyyyyyyyyyyyywyyyyy"
};

const char * mapdlgsel_xpm[] = {
  "24 24 36 1 ",
  "  c #020202",
  ". c #121212",
  "X c #3F3F3F",
  "o c #213966",
  "O c #585858",
  "+ c #197DB7",
  "@ c #4479C1",
  "# c #1C83BE",
  "$ c #1A87C6",
  "% c #1E91D4",
  "& c #268CC9",
  "* c #2991CD",
  "= c #3596CC",
  "- c #2996D6",
  "; c #2EA3DD",
  ": c #30A7DF",
  "> c #39B7E7",
  ", c #479FCE",
  "< c #67A8CD",
  "1 c #79A9C4",
  "2 c #66B8E4",
  "3 c #74C1E6",
  "4 c #A9A9A9",
  "5 c #BBBBBB",
  "6 c #9DC4DB",
  "7 c #A4C7DB",
  "8 c #BCCED9",
  "9 c #B7D8DC",
  "0 c #ABD3E4",
  "q c #B7D6E6",
  "w c #CCDBDF",
  "e c #D2D2D2",
  "r c #D8DEE2",
  "t c #DDE2E5",
  "y c #E6E6E6",
  "u c #FFFFFF",
  /* pixels */
  "yyyyyyyyyyyyyyyyyyyyyyyy",
  "yooooooooooooooooooooooy",
  "yo@@@@@@@@@@@@@@@@@@@@oy",
  "yo@@@@@@@@@@@@@@@@@@@@oy",
  "youuuuuuuuuuuuuuuuuuuuoy",
  "youu5 uuu uuuuuuuuuuuuoy",
  "youe  eu   uuuuuuuuuuuoy",
  "you4   ue euuuuuuuuuuuoy",
  "youu  4uy yuuuuuuuuuuuoy",
  "you u  e   u-tuuuuuuuuoy",
  "youu  Oue eu%%quuuuuuuoy",
  "yoeOu .O   u%>%quuuuuuoy",
  "you.O . u u %>>%0uuuuuoy",
  "youuX yuu uu$>>>%3wuuuoy",
  "youuu  uu  u#-;::$29uuoy",
  "youuuuuuuuuu+----$#=0uoy",
  "youuuuuuuuuu+**-$+<67uoy",
  "youuuuuuuuuu+*$$$,uuuuoy",
  "youuuuuuuuuu+$++$$7uuuoy",
  "youuuuuuuuuu#+r++$=wuuoy",
  "youuuuuuuuuu+uu8++$<yuoy",
  "youuuuuuuuuuuuuu1++&1uoy",
  "yooooooooooooooooooooooy",
  "yyyyyyyyyyyyyyyyyyyyyyyy"
};

const char * objinfodialog_xpm[] = {
  "24 24 53 1 ",
  "  c gray10",
  ". c #1B1B1B",
  "X c gray11",
  "o c gray12",
  "O c #323232",
  "+ c gray23",
  "@ c #213966",
  "# c gray26",
  "$ c #434343",
  "% c gray38",
  "& c #727272",
  "* c #7B7B7B",
  "= c gray49",
  "- c #707F9C",
  "; c #508EDF",
  ": c #818181",
  "> c #868686",
  ", c gray53",
  "< c gray54",
  "1 c #929292",
  "2 c #9A9A9A",
  "3 c #A4A4A4",
  "4 c #ACACAC",
  "5 c #AEAEAE",
  "6 c #B4B4B4",
  "7 c gray71",
  "8 c #B6B6B6",
  "9 c #B7B7B7",
  "0 c #B9B9B9",
  "q c #BBBBBB",
  "w c gray",
  "e c gray75",
  "r c #C1C1C1",
  "t c #C3C3C3",
  "y c gray77",
  "u c gray78",
  "i c gray80",
  "p c #CDCDCD",
  "a c gray81",
  "s c #D0D0D0",
  "d c gray83",
  "f c gray84",
  "g c #D8D8D8",
  "h c #DADADA",
  "j c gray88",
  "k c #E1E1E1",
  "l c gray89",
  "z c #E6E6E6",
  "x c #F6F6F6",
  "c c #F9F9F9",
  "v c gray98",
  "b c #FEFEFE",
  "n c white",
  /* pixels */
  "zzzzzzzzzzzzzzzzzzzzzzzz",
  "zzzzzzzzzzzzzzzzzzzzzzzz",
  "zzzzzz83zzzzzzz4 4zzzzzz",
  "zzzzz  o<zzzzz4   4zzzzz",
  "zzzz$74  >zzzzXz z zzzzz",
  "zzzz x5  %zzzzzz zzzzzzz",
  "zzzzz$, Opzzzzf2 2zzzzzz",
  "zzzzz   *kzzzz     zzzzz",
  "zzzj v6  #gzz 2z z2 zzzz",
  "zzz$hn9   =zz2zz zz2zzzz",
  "zzz ln0   :zzzzz zzzzzzz",
  "zzzw vq  #hzzz2   2zzzzz",
  "zzzzg   +szzz2 zzz 2zzzz",
  "zzzd ne  zzzzzzzzzzzzzzz",
  "zzz nnr  z@@@@@@@@@@@@@z",
  "zz$innt  z@;;;;;;;;;;;;z",
  "zz rcny  z@;;;;;;;;;;;;z",
  "zzz avu  z@bbbbbbbbbbbbz",
  "zzzz$   &z@b-----bbb---z",
  "zzzzzz1 zz@bbbbbbbbbbbbz",
  "zzzzzz1 1z@b-----bbb---z",
  "zzzzzz1..z@bbbbbbbbbbbbz",
  "zzzzzzzzzz@@@@@@@@@@@@@z",
  "zzzzzzzzzzzzzzzzzzzzzzzz"
};

const char * qdmconv_xpm[]={
  "24 24 53 1 ",
  "  c #1A1A1A",
  ". c #262626",
  "X c #373737",
  "o c #484848",
  "O c #575757",
  "+ c #656565",
  "@ c #7A7A7A",
  "# c #1D6399",
  "$ c #2A6EA2",
  "% c #3273A6",
  "& c #3179B4",
  "* c #427CAA",
  "= c #136BC5",
  "- c #1B73CA",
  "; c #297CC6",
  ": c #247CD0",
  "> c #3A83BB",
  ", c #5488B2",
  "< c #3884C8",
  "1 c #2B87D7",
  "2 c #338CD9",
  "3 c #3592DE",
  "4 c #3997E1",
  "5 c #4A99DD",
  "6 c #589AD8",
  "7 c #669ECF",
  "8 c #73A6D5",
  "9 c #469CE1",
  "0 c #58A5E3",
  "q c #6DACE3",
  "w c #7AB3E5",
  "e c #848484",
  "r c #989898",
  "t c #A9A9A9",
  "y c gray72",
  "u c #91B3CF",
  "i c #83B4E2",
  "p c #96BCE3",
  "a c #A8C5DD",
  "s c #99C5EB",
  "d c #A6C9EA",
  "f c #B5D1EA",
  "g c #B6D2EC",
  "h c #C6CBCF",
  "j c #D6D6D6",
  "k c #C5DAEA",
  "l c #D1DDE9",
  "z c #D7E1E9",
  "x c #D7E6F3",
  "c c #E6E6E6",
  "v c #E5EDF4",
  "b c #EBF1F6",
  "n c #F7F8F9",
  /* pixels */
  "ccccccccccvccccccccccccc",
  "ccccccci>####18ccccccccc",
  "ccccci<&axg944>%iccccccc",
  "cccci#60q49430xa$icccjcc",
  "cczi$k9433334sxbk#iccccc",
  "cch$lq433333sxnbxk%icccc",
  "cc%f93213230fsngidu,cccc",
  "cwu523221211sfs32wc7iccc",
  "c#922211111wbnnnnxbd#ccc",
  "c#211111125nj@oXoejb#ccc",
  "c#32111112n@  Xo.  @xcnb",
  "c#gx611:1n+ +jnnnj+ O cc",
  "c#fvb0:1nr @xqdbbbnr  hc",
  "c%dbbbq1x.on1-2vbbn   jc",
  "ci<vbnbkn y6--;fbbkkjtcc",
  "ck%ibbbdknb----xbv:=cnnc",
  "cc,&bbv6bnnn:==vbp=#c@nc",
  "ccz#7kw=b   b7=pf:#br jc",
  "ccch#%-=n  ydn=:=#zj.onc",
  "cccck%&;j .Xygnnnbjo.jcc",
  "ccccccu*##jX Oryt@ .yccc",
  "cccccccccccce.    Ojnccc",
  "cccccccccccccnhtycnccccc",
  "cccccccccccccccccccccccc"
};

const char* qdmdlist_xpm[] = {
  "24 24 36 1 ",
  "  c #015958",
  ". c #27754E",
  "X c #065F60",
  "o c #176B6B",
  "O c #007979",
  "+ c #535862",
  "@ c #727272",
  "# c #75AB34",
  "$ c #8C850E",
  "% c #FFFF00",
  "& c #008080",
  "* c #009494",
  "= c #119999",
  "- c #3A8C8F",
  "; c #00A0A0",
  ": c #00B3B3",
  "> c #38B3B3",
  ", c #499191",
  "< c #609797",
  "1 c #00C5C5",
  "2 c #00EFEF",
  "3 c #00FEFE",
  "4 c #76CFCF",
  "5 c #77E7E7",
  "6 c #B1CECE",
  "7 c #B6D1D1",
  "8 c #A8DBF0",
  "9 c #C1C5CB",
  "0 c #D8EFC0",
  "q c #FFFFCB",
  "w c #CFEFEF",
  "e c #DCE2E2",
  "r c #C0FAFA",
  "t c #E6E6E6",
  "y c #EDFCFE",
  "u c #FEFEFE",
  /* pixels */
  "tttttttttttttttttttttttt",
  "tt+++++++++++++9tttttttt",
  "tt+uuuuuuuuuuu++9ttttttt",
  "tt+uuuuuuuuuuu+u+9tttttt",
  "tt+u@u@@@@@@@u+uu+9ttttt",
  "tt+uuuuuuuuuuu+++++ttttt",
  "tt+u@u@@@@@@@uuuu8+ttttt",
  "tt+uuuuuuuuuuuuuu8+ttttt",
  "tt+u@u@@@@@@@@@@u8+ttttt",
  "tt+uuuuuuuuuuuuuu8+ttttt",
  "tt+u@u@@@@@@@uuuu8+ttttt",
  "tt+uuuuuuuuyuq$$$$$$$$$t",
  "tt+u@u@@uu5Owq$%%%%%%%$t",
  "tt+uuuuuurO1O0$%%%%%%%$t",
  "tt+u@u@uy&:32O    #%%%$t",
  "tt+uuuuu=*33O OO&O .%%$t",
  "tt+u@uu>&33;X&&&&&&X#%$t",
  "tt+uuu4O333 O&&&&&&O $$t",
  "tt+yuy&**** &&&&&&&& ttt",
  "tt+yyyyyyyu O&&&&&&O ttt",
  "tt+y8888888-X&&&&&& <ttt",
  "tt++++++++++X OO&O otttt",
  "tttttttttttte-    ,ttttt",
  "tttttttttttttte67etttttt"
};

const char* qdmrect_xpm[] = {
  "24 24 30 1 ",
  "  c #317062",
  ". c #0A70AB",
  "X c #197DB7",
  "o c #1C83BE",
  "O c #458BB4",
  "+ c #5BB7A5",
  "@ c #1A87C6",
  "# c #1E91D4",
  "$ c #268CC8",
  "% c #2991CD",
  "& c #3596CC",
  "* c #2996D6",
  "= c #2EA3DD",
  "- c #30A7DF",
  "; c #39B7E7",
  ": c #479FCE",
  "> c #67A8CD",
  ", c #79A9C4",
  "< c #66B8E4",
  "1 c #74C1E6",
  "2 c #9BBDD0",
  "3 c #9DC4DB",
  "4 c #A6C5D7",
  "5 c #BCCED9",
  "6 c #ABD3E4",
  "7 c #B7D6E6",
  "8 c #CBD9DE",
  "9 c #D5DDE1",
  "0 c #DEE2E5",
  "q c #E6E6E6",
  /* pixels */
  "qqqqqqqqqqqqqqqqqqqqqqqq",
  "qqqqqqqqqqqqqqqqqqqqqqqq",
  "qqqqqqqqqqqqqqqqqqqqqqqq",
  "qq                    qq",
  "qq q+q+q+q+q+q+q+q+q+ qq",
  "qq +q+q+q+q+q+q+q+q+q qq",
  "qq q+q+q+q+q+q+q+q+q+ qq",
  "qq +q+q+q+qqqqq+q+q+q qq",
  "qq q+q+q+q+q*0qq+q+q+ qq",
  "qq +q+q+q+qq##7qq+q+q qq",
  "qq q+q+q+q+q#;#7qq+q+ qq",
  "qq +q+q+q+qq#;;#6qq+q qq",
  "qq q+q+q+q+q@-;;#18q+ qq",
  "qq +q+q+q+qqo==--@<6q qq",
  "qq q+q+q+q+qX%***@@&5qqq",
  "qq +q+q+q+qqX%%*@X>340qq",
  "qq         qX%@@@:qqqqqq",
  "qqqqqqqqqqqqX$XX@@4qqqqq",
  "qqqqqqqqqqqqoX9Xo@&8qqqq",
  "qqqqqqqqqqqqXqq5XX@>0qqq",
  "qqqqqqqqqqqqqqqq,XXo,qqq",
  "qqqqqqqqqqqqqqqq9O.X4qqq",
  "qqqqqqqqqqqqqqqqq4X20qqq",
  "qqqqqqqqqqqqqqqqqq8qqqqq"
};

const char* qdmactpt_xpm[] = {
  "24 24 7 1 ",
  "  c #7C9F59",
  ". c #084887",
  "X c #0D7DEB",
  "o c #2C9097",
  "O c #A8C18F",
  "+ c #A1DFE4",
  "@ c #E6E6E6",
  /* pixels */
  "@@@@@@@@@@@@@@@@@@@@@@@@",
  "@@@@@@@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@X.X@@              @@",
  "@@@...@@OOOOOOOOOOOOOO@@",
  "@@@X.X@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@+o+@@@@@@@@@@@@@@@@@@",
  "@@@@@@@@@@@@@@@@@@@@@@@@",
  "@@@@@@@@@@@@@@@@@@@@@@@@"
};

const char* qdmdcmtw_xpm[] = {
  "24 24 50 1 ",
  "  c #595959",
  ". c #5E725E",
  "X c #696969",
  "o c #6F7672",
  "O c #E15611",
  "+ c #E3661C",
  "@ c #D4772E",
  "# c #E9752D",
  "$ c #519453",
  "% c #57A54A",
  "& c #65A94D",
  "* c #68AE52",
  "= c #7CB759",
  "- c #6E9365",
  "; c #77C54A",
  ": c #D7972B",
  "> c #D48737",
  ", c #E49131",
  "< c #DEB137",
  "1 c #E4A537",
  "2 c #E2BA3B",
  "3 c #DEDD24",
  "4 c #DFC533",
  "5 c #E3D62A",
  "6 c #E3C636",
  "7 c #E7D636",
  "8 c #E1E129",
  "9 c #87967B",
  "0 c #DFB34B",
  "q c #E5B444",
  "w c #E1A35E",
  "e c #8BC84C",
  "r c #A9D458",
  "t c #B3D650",
  "y c #8DC363",
  "u c #ACCE63",
  "i c #CBD852",
  "p c #E8E647",
  "a c #7D7D80",
  "s c #87868A",
  "d c #98989C",
  "f c #9F9EA3",
  "g c #B9B8BA",
  "h c #DBBB9F",
  "j c #E2B496",
  "k c #E5CEAC",
  "l c #BCBAC1",
  "z c #CFCFD1",
  "x c #E7DACE",
  "c c #E6E6E6",
  /* pixels */
  "cccccccccccccccccccccccc",
  "cccccccccccccccccccccccc",
  "cccccccccccccccccccccccc",
  "ccccccccccxjjkxccccccccc",
  "ccccccccch++OO#jcccccccc",
  "ccccccccw#OOOOO+wccccccc",
  "cccccccw#>@@>@@>++hccccc",
  "ccccc0,,1qqq000wq::kcccc",
  "cccw217766556666222,:xcc",
  "cc;468888838833554622:xc",
  "ce46888888883333342422:c",
  "c;4iipp8p777777764642<:k",
  "l-9ertetttiiiittruuy*9X ",
  "lso-%*;;eeurrryyy=*-.   ",
  "lssaX$%;;====yy=%$.    X",
  "ccdsso.$%*&&&&%$.     og",
  "ccclssoo$$%%$%$     Xgcc",
  "cccccdsao.$.      Xgcccc",
  "cccccczssX       gcccccc",
  "ccccccccfsX    fcccccccc",
  "cccccccccza sgcccccccccc",
  "cccccccccccccccccccccccc",
  "cccccccccccccccccccccccc",
  "cccccccccccccccccccccccc"
};

const char* qdmdprn_xpm[] = {
  "24 24 37 1 ",
  "  c #132C3B",
  ". c #213967",
  "X c #164672",
  "o c #424F59",
  "O c #566774",
  "+ c #295983",
  "@ c #1D619D",
  "# c #1A63A5",
  "$ c #1969B4",
  "% c #1F73BD",
  "& c #2669A2",
  "* c #2776BD",
  "= c #4B769B",
  "- c #2378C2",
  "; c #4479C1",
  ": c #3B81C2",
  "> c #4B90CC",
  ", c #5394CD",
  "< c #6297CA",
  "1 c #7CB1DD",
  "2 c #829CB0",
  "3 c #9CA0AA",
  "4 c #92B4D2",
  "5 c #A3B6C8",
  "6 c #99BFE0",
  "7 c #9EC3E3",
  "8 c #B0CEE8",
  "9 c #BDDCF3",
  "0 c gray79",
  "q c #CED3D6",
  "w c gainsboro",
  "e c #C8D8E6",
  "r c #D1E1EF",
  "t c #E6E6E6",
  "y c #E4EDF5",
  "u c #EBF1F6",
  "i c #FFFFFF",
  /* pixels */
  "tttttttttttttttttttttttt",
  "t......................t",
  "t.;;;;;;;;;;;;;;;;;;;;.t",
  "t.;;;;;;;;;;;;;;;;;;;;.t",
  "t.iiiiiiiiiiiiiiiiiiii.t",
  "t.iiiOOOOOOOOiiiiiiiii.t",
  "t.iiiOwwwwwwOiiiiiiiii.t",
  "t.iiiOwwwwwwOiiiiiiiii.t",
  "t.ii OwqwwwwO iiie555q.t",
  "t.ii O000000O r4@X+XXX.t",
  "t.i           XX=66&#&.t",
  "t.i qiiiiiiii2 &6&@##1.t",
  "t.i qiiiiiiui5 6@@@&,u.t",
  "t.i            @@@@,8y.t",
  "t.i  o333333o  @@@-6>>.t",
  "t.iiio999999o@@@@@-6yr.t",
  "t.iiio999999o@@@@%4uur.t",
  "t.iiioooooooo@@@#:uuur.t",
  "t.iiiii8X@@@@@@@$:uuur.t",
  "t.iiiii7Xu:@@@$$$*uuur.t",
  "t.iiiii7Xuy4$$$$$$<9u6.t",
  "t.iiiiieXiuy,$$$#$##$*.t",
  "t......................t",
  "ttttttttttuttttttrtttttt"
};

const char * getobjdialog_xpm[] = {
  "24 24 36 1 ",
  "  c black",
  ". c #010101",
  "X c #020202",
  "o c #040404",
  "O c #070707",
  "+ c gray5",
  "@ c #111111",
  "# c #161616",
  "$ c #1B1B1B",
  "% c #1E1E1E",
  "& c #222222",
  "* c #213966",
  "= c #434343",
  "- c #626262",
  "; c #4479C1",
  ": c gray60",
  "> c #B2B2B2",
  ", c #B6B6B6",
  "< c #C0C0C0",
  "1 c gray77",
  "2 c gray78",
  "3 c #C8C8C8",
  "4 c #CBCBCB",
  "5 c #D0D0D0",
  "6 c LightGray",
  "7 c gray85",
  "8 c #E6E6E6",
  "9 c gray93",
  "0 c gray96",
  "q c gray97",
  "w c #F7FCFB",
  "e c #FBFBFB",
  "r c gray99",
  "t c #FDFDFD",
  "y c #FEFEFE",
  "u c white",
  /* pixels */
  "888888888888888888888888",
  "8**********************8",
  "8*;;;;;;;;;;;;;;;;;;;;*8",
  "8*;;;;;;;;;;;;;;;;;;;;*8",
  "8*uuuuuuuwuuuuuuuuuuuu*8",
  "8*uuuu:  4uuuuu uuuuuu*8",
  "8*uuu4 9X$4uu5   5uuuu*8",
  "8*uuu4 9XX4u5 > , 5uuu*8",
  "8*uuuu=9.%uuuue uuuuuu*8",
  "8*uuu1Xo.&tuuu   uuuuu*8",
  "8*uuu=:9X =re > > uuuu*8",
  "8*uuu 99  $7 <r u< 5uu*8",
  "8*uuu=:1X#=uuuq uuuuuu*8",
  "8*uuu:    :uuu   quuuu*8",
  "8*uu: u9X  -u > > uuuu*8",
  "8*uu 0u9X   @<t u< 3uu*8",
  "8*uu :u9X   5ut uu<2uu*8",
  "8*uu6+  XO 6uut uuuuuu*8",
  "8*uuutuuXuryuuy uuuuuu*8",
  "8*uuuuuuXuuuuut uuuuuu*8",
  "8*uuuuuuXXuuuue  uuuuu*8",
  "8*uuuuuuuuuuuuuuuuuuuu*8",
  "8**********************8",
  "888888888888888888888888"
};

const char *qdmgsrv_xpm[] = {
  "24 24 35 1 ",
  "  c #375278",
  ". c #F20D27",
  "X c #3E5D88",
  "o c #44638F",
  "O c #486A9A",
  "+ c #526A95",
  "@ c #587399",
  "# c #597CAD",
  "$ c #5A7EB3",
  "% c #B16F96",
  "& c #B27097",
  "* c #5E81B5",
  "= c #6587B8",
  "- c #708FBD",
  "; c #7493BF",
  ": c #5F88C0",
  "> c #668DC3",
  ", c #6F95C8",
  "< c #7D9DC9",
  "1 c #86A4CD",
  "2 c #89AAD8",
  "3 c #91AFD8",
  "4 c #97B6DC",
  "5 c #9BBBE1",
  "6 c #A7C4E5",
  "7 c #B1CAE5",
  "8 c #B5D0EE",
  "9 c #B8D3F0",
  "0 c #C2CEDE",
  "q c #CDD3DD",
  "w c #D2D7DE",
  "e c #C6D4E4",
  "r c #D5DDE6",
  "t c #DCE1E7",
  "y c #E6E6E6",
  /* pixels */
  "yyyyyyyyyyyyyttyyyyyyyyy",
  "yyyyyyyyyyye<<<2ryyyyyyy",
  "yyyyyyyyy7,:298<:1tyyyyy",
  "yyyyyyy5>>49999981<ryyyy",
  "yyyyr3*,6999999987@ yyyy",
  "yyyyO$2989999998@ + yyyy",
  "yyyyO<O#689996@ O-- yyyy",
  "yyyyO44;O*99@ O---- yyyy",
  "yyyyO4254-* +=---=- yyyy",
  "yyyyO4OX<45 ==-=-=- yyyy",
  "yyyyO45<XX5 =====-= yyyy",
  "yyyyO4XX<45 =====-= yyyy",
  "yyyyO551oo5 ======= yyyy",
  "yyyyO225414 **=*$== yyyy",
  "yyyyO322444 *=**=== yyyy",
  "yyyyO222444 ***==$= yyyy",
  "yyyyO222.&4 *=*=$== yyyy",
  "yyyyO322..3 $$=*$=$ yyyy",
  "yyyyO222%.3 $***$$OXyyyy",
  "yyyyO*22223 $$$$o @yyyyy",
  "yyyyy=O$223 $$o @wyyyyyy",
  "yyyyyyw*O#3 o @qyyyyyyyy",
  "yyyyyyyy0=O +qyyyyyyyyyy",
  "yyyyyyyyyy0ryyyyyyyyyyyy"
};

const char* qdmd3d_xpm[] = {
  "24 24 45 1 ",
  "  c #5F5D54",
  ". c #68675F",
  "X c #615C6A",
  "o c #BE9412",
  "O c #D3AF30",
  "+ c #A8A24D",
  "@ c #ECCE54",
  "# c #D1CA6F",
  "$ c #FADE61",
  "% c #E7CE70",
  "& c #543A8F",
  "* c #0F7D92",
  "= c #564182",
  "- c #5B4395",
  "; c #6C5297",
  ": c #73668D",
  "> c #7157AB",
  ", c #957A8A",
  "< c #846CBC",
  "1 c #896FC4",
  "2 c #198698",
  "3 c #238890",
  "4 c #1996A7",
  "5 c #2A94A2",
  "6 c #31A8B7",
  "7 c #49908F",
  "8 c #63A89C",
  "9 c #1DBFCB",
  "0 c #2ABBD0",
  "q c #79B9C2",
  "w c #1CC7D1",
  "e c #27CADB",
  "r c #32C0D2",
  "t c #2ECCE1",
  "y c #34CEE5",
  "u c #3AD1EB",
  "i c #41D3F1",
  "p c #9D93A6",
  "a c #ECDB96",
  "s c #EDE3B8",
  "d c #A5CAD0",
  "f c #D0D3D3",
  "g c #E9E5D2",
  "h c #D3DCE2",
  "j c #E6E6E6",
  /* pixels */
  "jjjjjjjjjjjjjjjjjjjjjjjj",
  "jjjjjjjjjjjj jjjjjjjjjjj",
  "jjjjjjjjjjjj jjjjjjjjjjj",
  "jjjjjjjjjjjj gjjjjjjjjjj",
  "jjjjjjjjjg@o o%gjjjjjjjj",
  "jjjjjjgg%oO@ $Ooagjjjjjj",
  "jjjjjjaOo@$$ $$@o@gjjjjj",
  "jjjjsOo@$$$$$$$@$Oo%gjjj",
  "jjjj++$$$$$$ $$$$$@+&jjj",
  "jjjj252#$$$$ $$$$%;;&jjj",
  "jjjj2uu57%$$$$$$,->1&jjj",
  "jjjj*uuur38$$$#=>111&jjj",
  "jjjj*yuuuu6*#;-11111&jjj",
  "jjjj2uuyyyyt&>111111&jjj",
  "jjjj2tttyytt&1111111&jjj",
  "jjjj2ut: tte&11X:111&jjj",
  "jjjj*7iu0eee&11<11:1&jjj",
  "jjgf  6teeee&111<< .=jjj",
  "jh. *4teeeew&111<11=  fj",
  "j 7jj8*0ewww&111<;&pgp j",
  "jjjjjjh549ew&11>&phjjjjj",
  "jjjjjjjjd24w&>&<hjjjjjjj",
  "jjjjjjjjjjq*&-hjjjjjjjjj",
  "jjjjjjjjjjjjhjjjjjjjjjjj"
};

const char *qdmnet_xpm[] = {
  "24 24 58 1 ",
  "  c black",
  ". c gray9",
  "X c #252525",
  "o c #2C2C2C",
  "O c #2F2F2F",
  "+ c gray21",
  "@ c gray25",
  "# c #4B4B4B",
  "$ c gray33",
  "% c #5A5A5A",
  "& c #5F5F5F",
  "* c #634545",
  "= c gray43",
  "- c #747474",
  "; c gray49",
  ": c #E32222",
  "> c #E32929",
  ", c #E33030",
  "< c #EA5858",
  "1 c #E47F7F",
  "2 c #084887",
  "3 c #0E80F1",
  "4 c #838383",
  "5 c #888888",
  "6 c #8E8E8E",
  "7 c gray56",
  "8 c #909090",
  "9 c gray58",
  "0 c #9D9D9D",
  "q c #A2A2A2",
  "w c gray68",
  "e c #AEAEAE",
  "r c gray70",
  "t c gray71",
  "y c #B9B9B9",
  "u c gray74",
  "i c #E58A8A",
  "p c #E58E8E",
  "a c #F08888",
  "s c #F29A9A",
  "d c #C5C5C5",
  "f c gray80",
  "g c #CECECE",
  "h c #CAD3DB",
  "j c #D0D0D0",
  "k c gray83",
  "l c gray86",
  "z c gray87",
  "x c #E5D0D0",
  "c c #E6D5D5",
  "v c #E6DBDB",
  "b c #E5DEDE",
  "n c gray88",
  "m c #E1E1E1",
  "M c #E2E2E2",
  "N c gray90",
  "B c #E6E5E5",
  "V c #E6E6E6",
  /* pixels */
  "VVVVVVVVVVVVVVVVVVVVVVVV",
  "VVVVVnetVVVVVVVVVVVVVvcV",
  "VVVjd@@@.&VVVVVVVVVVs>:s",
  "V322@Vl9  9ds::sVVVV::::",
  "V2222VV    @::::::::::::",
  "V2222VV    V::::VVVVs::s",
  "V322+8V9  9Vs::sVVVVV$gV",
  "VV+VV+8VVVVVV:VVVVVVVOmV",
  "VV+VVV++VVVVV:VVVVVVMoVV",
  "VV+VVVV6+VVVV:VVVVVVy-VV",
  "VV+VVVVV8+VVV:VVVVVV09VV",
  "VV+VVVVVV8+VV:VVVVVV;rVV",
  "VV+VVVVVVV7s::sVVVVV%fVV",
  "Vs::sVVVVVx::::VVVVV+zVV",
  "V,:::VVVVVB::::VVVVNXVVV",
  "V::::VVVVVV<::*VVVVk#VVV",
  "Vs::<pVVVVb:VV5+VVVu=VVV",
  "VVVVV:iVVVaaVVV8+VVq8VVV",
  "VVVVVV:1VV:VVVVV8+V4wVVV",
  "VVVVVVV:<::sVVVVV8+223VV",
  "VVVVVVVV::::VVVVVV2222VV",
  "VVVVVVVV::::++++++2222VV",
  "VVVVVVVVs::sVVVVVV3223VV",
  "VVVVVVVVVVVVVVVVVVVhVVVV"
};

const char *qdmdwmspopup_xpm[]={
  "24 24 51 1 ",
  "  c #0A155B",
  ". c #3E4149",
  "X c #056263",
  "o c #156B6B",
  "O c #727272",
  "+ c red",
  "@ c #66D12E",
  "# c #1F5B88",
  "$ c #1E649A",
  "% c #23679D",
  "& c #3374A6",
  "* c #2C7FBF",
  "= c #377FBB",
  "- c #1936E6",
  "; c #2097B4",
  ": c #508CAF",
  "> c #6399B6",
  ", c #2A84D8",
  "< c #348DDD",
  "1 c #3791DF",
  "2 c #3C96E3",
  "3 c #4E9BDB",
  "4 c #7EAFDA",
  "5 c #439CE5",
  "6 c #559EE1",
  "7 c #47A0E7",
  "8 c #57A8E9",
  "9 c #6BABE5",
  "0 c #66B1EC",
  "q c #77B5E5",
  "w c #8FB3B3",
  "e c #A1BDBD",
  "r c #92B1C7",
  "t c #88BDEA",
  "y c #99C0D5",
  "u c #AAC4C4",
  "i c #B3C9C9",
  "p c #94C2E9",
  "a c #AACDEB",
  "s c #A8DBF0",
  "d c #BBD8F1",
  "f c #C8D3D8",
  "g c #D6DCDE",
  "h c #C9DDEE",
  "j c #D9E0E5",
  "k c #CDE1F2",
  "l c #DBE8F4",
  "z c #E6E6E6",
  "x c #E5EEF6",
  "c c #EBF1F6",
  "v c #FEFEFE",
  /* pixels */
  "zzzzzzzzzzxzzzzzzzzzzzzz",
  "zzzzzzq*####*qjzzzzzzzzz",
  "zzzzs%50xdq73,%tzzzzzzzz",
  "zzza&qq885720xj&tzzzzzzz",
  "zzz$*hq75778lccl%ggzzzzz",
  "zzr*h852222k9qdp>>Xezzzz",
  "zz%l5222227kxa9pa:uozzzz",
  "zg&7222222kcccxlk;zXwzzz",
  "jf:1221<12cccccca%zXezzz",
  "zf$tq<<<<1lccccck%vX;jzz",
  "zg$hct<<<<3...........jz",
  "zg$dcch6,,,.vvvvvvvvv.jz",
  "zXr=xxc9,,,.vvvvvvvvs.zz",
  "uXz&qxl1,,=.v+vOOOOOs.zz",
  "fXij%:*:*=9.vvvvvvvvs.zz",
  "feXivcvchy:.v@vOOOOOs.zz",
  "zzuoXXXXXXX.vvvvvvvvs.zz",
  "zzzzzgzaq4q.v-vOOOOOs.zz",
  "zzzzzzzzzzz.vvvvvvvvs.zz",
  "zzzzzzzzzzz.v vOOOOOs.zz",
  "zzzzzzzzzzz.vvvvvvvvs.zz",
  "zzzzzzzzzzz.vssasssss.zz",
  "zzzzzzzzzzz...........zz",
  "zzzzzzzzzzzzzzzzzzzzzzzz"
};


QDMapViewPlugin::QDMapViewPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapViewPlugin::~QDMapViewPlugin()
{
}


QString QDMapViewPlugin::name() const
{
    return QLatin1String("QDMapView");
}

QString QDMapViewPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapViewPlugin::toolTip() const
{
    return QString();
}

QString QDMapViewPlugin::whatsThis() const
{
    return QString();
}

QString QDMapViewPlugin::includeFile() const
{
    return QLatin1String("qdmcmp.h");
}

QIcon QDMapViewPlugin::icon() const
{
    return QIcon(QPixmap(mapview_xpm));
}

bool QDMapViewPlugin::isContainer() const
{
    return true;
}


QWidget *QDMapViewPlugin::createWidget(QWidget *parent)
{
    QWidget* w = 0;


      w = new QDMapView(parent);

    return w;
}

bool QDMapViewPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapViewPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapViewPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapView\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapWindowPlugin::QDMapWindowPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapWindowPlugin::~QDMapWindowPlugin()
{
}


QString QDMapWindowPlugin::name() const
{
    return QLatin1String("QDMapWindow");
}

QString QDMapWindowPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapWindowPlugin::toolTip() const
{
    return QString();
}

QString QDMapWindowPlugin::whatsThis() const
{
    return QString();
}

QString QDMapWindowPlugin::includeFile() const
{
    return QLatin1String("qdmwinw.h");
}

QIcon QDMapWindowPlugin::icon() const
{
    return QIcon(QPixmap(mapwindow_xpm));
}

bool QDMapWindowPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapWindowPlugin::createWidget(QWidget *parent)
{
    return new QDMapWindow(parent);
}

bool QDMapWindowPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapWindowPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapWindowPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapWindow\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapPointPlugin::QDMapPointPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapPointPlugin::~QDMapPointPlugin()
{
}

QString QDMapPointPlugin::name() const
{
    return QLatin1String("QDMapPoint");
}

QString QDMapPointPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapPointPlugin::toolTip() const
{
    return QString();
}

QString QDMapPointPlugin::whatsThis() const
{
    return QString();
}

QString QDMapPointPlugin::includeFile() const
{
    return QLatin1String("qdmpoint.h");
}

QIcon QDMapPointPlugin::icon() const
{
    return QIcon(QPixmap(mappoint_xpm));
}

bool QDMapPointPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapPointPlugin::createWidget(QWidget *parent)
{
    return new QDMapPoint(parent);
}

bool QDMapPointPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapPointPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapPointPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapPoint\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapObjPlugin::QDMapObjPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapObjPlugin::~QDMapObjPlugin()
{
}


QString QDMapObjPlugin::name() const
{
    return QLatin1String("QDMapObj");
}

QString QDMapObjPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapObjPlugin::toolTip() const
{
    return QString();
}

QString QDMapObjPlugin::whatsThis() const
{
    return QString();
}

QString QDMapObjPlugin::includeFile() const
{
    return QLatin1String("qdmobj.h");
}

QIcon QDMapObjPlugin::icon() const
{
    return QIcon(QPixmap(mapobj_xpm));
}

bool QDMapObjPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapObjPlugin::createWidget(QWidget *parent)
{
    return new QDMapObj(parent);
}

bool QDMapObjPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapObjPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapObjPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapObj\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapFindPlugin::QDMapFindPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapFindPlugin::~QDMapFindPlugin()
{
}


QString QDMapFindPlugin::name() const
{
    return QLatin1String("QDMapFind");
}

QString QDMapFindPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapFindPlugin::toolTip() const
{
    return QString();
}

QString QDMapFindPlugin::whatsThis() const
{
    return QString();
}

QString QDMapFindPlugin::includeFile() const
{
    return QLatin1String("qdmfind.h");
}

QIcon QDMapFindPlugin::icon() const
{
    return QIcon(QPixmap(mapfind_xpm));
}

bool QDMapFindPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapFindPlugin::createWidget(QWidget *parent)
{
    return new QDMapFind(parent);
}

bool QDMapFindPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapFindPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapFindPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapFind\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapSelectDialogPlugin::QDMapSelectDialogPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapSelectDialogPlugin::~QDMapSelectDialogPlugin()
{
}


QString QDMapSelectDialogPlugin::name() const
{
    return QLatin1String("QDMapSelectDialog");
}

QString QDMapSelectDialogPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapSelectDialogPlugin::toolTip() const
{
    return QString();
}

QString QDMapSelectDialogPlugin::whatsThis() const
{
    return QString();
}

QString QDMapSelectDialogPlugin::includeFile() const
{
    return QLatin1String("qdmdsel.h");
}

QIcon QDMapSelectDialogPlugin::icon() const
{
    return QIcon(QPixmap(mapdlgsel_xpm));
}

bool QDMapSelectDialogPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapSelectDialogPlugin::createWidget(QWidget *parent)
{
    return new QDMapSelectDialog(parent);
}

bool QDMapSelectDialogPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapSelectDialogPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapSelectDialogPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapSelectDialog\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapDlgObjPlugin::QDMapDlgObjPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapDlgObjPlugin::~QDMapDlgObjPlugin()
{
}


QString QDMapDlgObjPlugin::name() const
{
    return QLatin1String("QDMapDlgObj");
}

QString QDMapDlgObjPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapDlgObjPlugin::toolTip() const
{
    return QString();
}

QString QDMapDlgObjPlugin::whatsThis() const
{
    return QString();
}

QString QDMapDlgObjPlugin::includeFile() const
{
    return QLatin1String("qdmselob.h");
}

QIcon QDMapDlgObjPlugin::icon() const
{
    return QIcon(QPixmap(objinfodialog_xpm));
}

bool QDMapDlgObjPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapDlgObjPlugin::createWidget(QWidget *parent)
{
    return new QDMapDlgObj(parent);
}

bool QDMapDlgObjPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapDlgObjPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapDlgObjPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapDlgObj\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapConvertorPlugin::QDMapConvertorPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapConvertorPlugin::~QDMapConvertorPlugin()
{
}


QString QDMapConvertorPlugin::name() const
{
    return QLatin1String("QDMapConvertor");
}

QString QDMapConvertorPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapConvertorPlugin::toolTip() const
{
    return QString();
}

QString QDMapConvertorPlugin::whatsThis() const
{
    return QString();
}

QString QDMapConvertorPlugin::includeFile() const
{
    return QLatin1String("qdmconv.h");
}

QIcon QDMapConvertorPlugin::icon() const
{
    return QIcon(QPixmap(qdmconv_xpm));
}

bool QDMapConvertorPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapConvertorPlugin::createWidget(QWidget *parent)
{
    return new QDMapConvertor(parent);
}

bool QDMapConvertorPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapConvertorPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapConvertorPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapConvertor\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapDataListPlugin::QDMapDataListPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapDataListPlugin::~QDMapDataListPlugin()
{
}

QString QDMapDataListPlugin::name() const
{
    return QLatin1String("QDMapDataList");
}

QString QDMapDataListPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapDataListPlugin::toolTip() const
{
    return QString();
}

QString QDMapDataListPlugin::whatsThis() const
{
    return QString();
}

QString QDMapDataListPlugin::includeFile() const
{
    return QLatin1String("qdmdlist.h");
}

QIcon QDMapDataListPlugin::icon() const
{
    return QIcon(QPixmap(qdmdlist_xpm));
}

bool QDMapDataListPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapDataListPlugin::createWidget(QWidget *parent)
{
    return new QDMapDataList(parent);
}

bool QDMapDataListPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapDataListPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapDataListPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapDataList\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapSelectRectPlugin::QDMapSelectRectPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapSelectRectPlugin::~QDMapSelectRectPlugin()
{
}

QString QDMapSelectRectPlugin::name() const
{
    return QLatin1String("QDMapSelectRect");
}

QString QDMapSelectRectPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapSelectRectPlugin::toolTip() const
{
    return QString();
}

QString QDMapSelectRectPlugin::whatsThis() const
{
    return QString();
}

QString QDMapSelectRectPlugin::includeFile() const
{
    return QLatin1String("qdmrect.h");
}

QIcon QDMapSelectRectPlugin::icon() const
{
    return QIcon(QPixmap(qdmrect_xpm));
}

bool QDMapSelectRectPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapSelectRectPlugin::createWidget(QWidget *parent)
{
    return new QDMapSelectRect(parent);
}

bool QDMapSelectRectPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapSelectRectPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapSelectRectPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapSelectRect\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}


QDGetMapPointPlugin::QDGetMapPointPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDGetMapPointPlugin::~QDGetMapPointPlugin()
{
}

QString QDGetMapPointPlugin::name() const
{
    return QLatin1String("QDGetMapPoint");
}

QString QDGetMapPointPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDGetMapPointPlugin::toolTip() const
{
    return QString();
}

QString QDGetMapPointPlugin::whatsThis() const
{
    return QString();
}

QString QDGetMapPointPlugin::includeFile() const
{
    return QLatin1String("qdmactpt.h");
}

QIcon QDGetMapPointPlugin::icon() const
{
    return QIcon(QPixmap(qdmactpt_xpm));
}

bool QDGetMapPointPlugin::isContainer() const
{
    return true;
}

QWidget *QDGetMapPointPlugin::createWidget(QWidget *parent)
{
    return new QDGetMapPoint(parent);
}

bool QDGetMapPointPlugin::isInitialized() const
{
    return m_initialized;
}

void QDGetMapPointPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDGetMapPointPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDGetMapPoint\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

// QDMapActionLinePlugin

QDMapActionLinePlugin::QDMapActionLinePlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapActionLinePlugin::~QDMapActionLinePlugin()
{
}

QString QDMapActionLinePlugin::name() const
{
    return QLatin1String("QDMapActionLine");
}

QString QDMapActionLinePlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapActionLinePlugin::toolTip() const
{
    return QString();
}

QString QDMapActionLinePlugin::whatsThis() const
{
    return QString();
}

QString QDMapActionLinePlugin::includeFile() const
{
    return QLatin1String("qdmactline.h");
}

QIcon QDMapActionLinePlugin::icon() const
{
    return QIcon(QPixmap(qdmactpt_xpm));
}

bool QDMapActionLinePlugin::isContainer() const
{
    return true;
}

QWidget *QDMapActionLinePlugin::createWidget(QWidget *parent)
{
    return new QDMapActionLine(parent);
}

bool QDMapActionLinePlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapActionLinePlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapActionLinePlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapActionLine\" name=\"ActionLine\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

// QDMapCreateMtwPlugin

QDMapCreateMtwPlugin::QDMapCreateMtwPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapCreateMtwPlugin::~QDMapCreateMtwPlugin()
{
}

QString QDMapCreateMtwPlugin::name() const
{
    return QLatin1String("QDMapCreateMtw");
}

QString QDMapCreateMtwPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapCreateMtwPlugin::toolTip() const
{
    return QString();
}

QString QDMapCreateMtwPlugin::whatsThis() const
{
    return QString();
}

QString QDMapCreateMtwPlugin::includeFile() const
{
    return QLatin1String("qdmdcmtw.h");
}

QIcon QDMapCreateMtwPlugin::icon() const
{
    return QIcon(QPixmap(qdmdcmtw_xpm));
}

bool QDMapCreateMtwPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapCreateMtwPlugin::createWidget(QWidget *parent)
{
    return new QDMapCreateMtw(parent);
}

bool QDMapCreateMtwPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapCreateMtwPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapCreateMtwPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapCreateMtw\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

#ifndef HIDE_PRINTSUPPORT
QDMapBuildEpsPlugin::QDMapBuildEpsPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapBuildEpsPlugin::~QDMapBuildEpsPlugin()
{
}

QString QDMapBuildEpsPlugin::name() const
{
    return QLatin1String("QDMapBuildEps");
}

QString QDMapBuildEpsPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapBuildEpsPlugin::toolTip() const
{
    return QString();
}

QString QDMapBuildEpsPlugin::whatsThis() const
{
    return QString();
}

QString QDMapBuildEpsPlugin::includeFile() const
{
    return QLatin1String("qdmdprn.h");
}

QIcon QDMapBuildEpsPlugin::icon() const
{
    return QIcon(QPixmap(qdmdprn_xpm));
}

bool QDMapBuildEpsPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapBuildEpsPlugin::createWidget(QWidget *parent)
{
    return new QDMapPrint(parent);
}

bool QDMapBuildEpsPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapBuildEpsPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapBuildEpsPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapBuildEps\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}
#endif  // HIDE_PRINTSUPPORT

QDMapSelectObjectRscPlugin::QDMapSelectObjectRscPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapSelectObjectRscPlugin::~QDMapSelectObjectRscPlugin()
{
}

QString QDMapSelectObjectRscPlugin::name() const
{
    return QLatin1String("QDMapSelectObjectRsc");
}

QString QDMapSelectObjectRscPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapSelectObjectRscPlugin::toolTip() const
{
    return QString();
}

QString QDMapSelectObjectRscPlugin::whatsThis() const
{
    return QString();
}

QString QDMapSelectObjectRscPlugin::includeFile() const
{
    return QLatin1String("qdmdgtob.h");
}

QIcon QDMapSelectObjectRscPlugin::icon() const
{
    return QIcon(QPixmap(getobjdialog_xpm));
}

bool QDMapSelectObjectRscPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapSelectObjectRscPlugin::createWidget(QWidget *parent)
{
    return new QDMapSelectObjectRsc(parent);
}

bool QDMapSelectObjectRscPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapSelectObjectRscPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapSelectObjectRscPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapSelectObjectRsc\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

#ifndef HIDE_GISSERVER_SUPPORT
QDMapGisServerPlugin::QDMapGisServerPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapGisServerPlugin::~QDMapGisServerPlugin()
{
}

QString QDMapGisServerPlugin::name() const
{
    return QLatin1String("QDMapGisServer");
}

QString QDMapGisServerPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapGisServerPlugin::toolTip() const
{
    return QString();
}

QString QDMapGisServerPlugin::whatsThis() const
{
    return QString();
}

QString QDMapGisServerPlugin::includeFile() const
{
    return QLatin1String("qdmgsrv.h");
}

QIcon QDMapGisServerPlugin::icon() const
{
    return QIcon(QPixmap(qdmgsrv_xpm));
}

bool QDMapGisServerPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapGisServerPlugin::createWidget(QWidget *parent)
{
    return new QDMapGisServer(parent);
}

bool QDMapGisServerPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapGisServerPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapGisServerPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapGisServer\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}
#endif  // HIDE_GISSERVER_SUPPORT

#ifndef HIDE_3DSUPPORT
QDMap3DPlugin::QDMap3DPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMap3DPlugin::~QDMap3DPlugin()
{
}

QString QDMap3DPlugin::name() const
{
    return QLatin1String("QDMap3D");
}

QString QDMap3DPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMap3DPlugin::toolTip() const
{
    return QString();
}

QString QDMap3DPlugin::whatsThis() const
{
    return QString();
}

QString QDMap3DPlugin::includeFile() const
{
    return QLatin1String("qdmd3d.h");
}

QIcon QDMap3DPlugin::icon() const
{
    return QIcon(QPixmap(qdmd3d_xpm));
}

bool QDMap3DPlugin::isContainer() const
{
    return true;
}

QWidget *QDMap3DPlugin::createWidget(QWidget *parent)
{
    return new QDMap3D(parent);
}

bool QDMap3DPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMap3DPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMap3DPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMap3D\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}
#endif  // HIDE_3DSUPPORT

QDMapNetPlugin::QDMapNetPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapNetPlugin::~QDMapNetPlugin()
{
}

QString QDMapNetPlugin::name() const
{
    return QLatin1String("QDMapNet");
}

QString QDMapNetPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapNetPlugin::toolTip() const
{
    return QString();
}

QString QDMapNetPlugin::whatsThis() const
{
    return QString();
}

QString QDMapNetPlugin::includeFile() const
{
    return QLatin1String("qdmnet.h");
}

QIcon QDMapNetPlugin::icon() const
{
    return QIcon(QPixmap(qdmnet_xpm));
}

bool QDMapNetPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapNetPlugin::createWidget(QWidget *parent)
{
    return new QDMapNet(parent);
}

bool QDMapNetPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapNetPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapNetPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapNet\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}

QDMapWMSPopupDialogPlugin::QDMapWMSPopupDialogPlugin(QObject *parent)
    : QObject(parent),
      m_initialized(false)
{
}

QDMapWMSPopupDialogPlugin::~QDMapWMSPopupDialogPlugin()
{
}

QString QDMapWMSPopupDialogPlugin::name() const
{
    return QLatin1String("QDMapWMSPopupDialog");
}

QString QDMapWMSPopupDialogPlugin::group() const
{
    return QLatin1String("GIS Designer");
}

QString QDMapWMSPopupDialogPlugin::toolTip() const
{
    return QString();
}

QString QDMapWMSPopupDialogPlugin::whatsThis() const
{
    return QString();
}

QString QDMapWMSPopupDialogPlugin::includeFile() const
{
    return QLatin1String("qdmdwmspopup.h");
}

QIcon QDMapWMSPopupDialogPlugin::icon() const
{
    return QIcon(QPixmap(qdmdwmspopup_xpm));
}

bool QDMapWMSPopupDialogPlugin::isContainer() const
{
    return true;
}

QWidget *QDMapWMSPopupDialogPlugin::createWidget(QWidget *parent)
{
    return new QDMapWMSPopupDialog(parent);
}

bool QDMapWMSPopupDialogPlugin::isInitialized() const
{
    return m_initialized;
}

void QDMapWMSPopupDialogPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    m_initialized = true;
}

QString QDMapWMSPopupDialogPlugin::domXml() const
{ return QLatin1String("\
        <widget class=\"QDMapWMSPopupDialog\" name=\"frame\">\
            <property name=\"geometry\">\
                <rect>\
                    <x>0</x>\
                    <y>0</y>\
                    <width>100</width>\
                    <height>80</height>\
                </rect>\
            </property>\
        </widget>\
");
}
