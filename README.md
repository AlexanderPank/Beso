# Client

Клиент БЭСо для создания и отображения сценария

## Getting started

sudo apt-get install libqt5websocket-dev

## Соглашение о координатах

latitude - широта  43.601735
longituda - долгота 39.574021

QPoint<lat,lon> для типов где это не определено используем так 
Todo: надо сделать свой тип, чтоб не путаться 



## Активация CLion
Простой способ активации

https://github.com/jcshan709/jetbrains-activating-tutorial

для доступа к https://search.censys.io/ нужен VPN


Запрещаем доступ к серверу активации

`sudo iptables -A OUTPUT -d 52.31.47.230 -p tcp --dport 443 -j DROP`

`sudo iptables -L OUTPUT --line-numbers` - вывести правила

`sudo iptables -D OUTPUT 1` - удалить правило

возможно 

`sudo netstat -tunacp | grep clion`

Переходим в раздел Proxy settings -> Manual proxy configuration. 
Прокси ставим localhost порт 80
В раздел No proxy for вставляем вот это: *.github.com,plugins.jetbrains.com *.niisp.int

Переходим на сайт вот к этому китайцу (https://gitee.com/superbeyone/J2_B5_A5_C4#&#25645;&#37197;&#25554;&#20214;&#25928;&#26524;&#26368;&#20339;&#20256;&#36865;&#38376;)
и копируем у него активационный код.

