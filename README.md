bbs-mud
=======

BBS MUD server and client

English abstract, translated by Google Translator

This MUD available via ssh!

Installation: make ; mkdir messages ; mkdir bbs

Create in Linux server user name 'bbs' with password 'bbs' and adjust his shell to 'bbs'

Run program 'server' as daemon

Login to 'bbs': ssh -l bbs

Enjoy ;-)

Communication between server and client occur in directories 'cmd' and 'messages'

You can not create a new user 'bbs', and run the program './bbs' in the console

Yet realized only moving and chat

---

Введение в BBS MUD

Давно я хотел написать свой мад, но необходимость работы с портами, кодировками
и очередями событий пугали меня. И я во время пути на работу и с работы по улице
Механизаторской придумал собственную архитектуру.

1. Мад представляет собой процесс по имени server (процесс в смысле ОС UNIX)

2. Клиенты тоже представляют собой процессы по имени bbs (на одного человека
игрока один экземпляр процесса)

3. Общение клиентов с серверов осуществляется путем помещения файликов в каталоги
cmd (команды от клиента к серверу) и messages (ответные сообщения сервера),
имена файлов начинаются с uid клиента, так достигается уникальность. В общем,
многозадачность и семафорность я возложил на ядро UNIX (Linux, FreeBSD) и на
его файловую систему.

Вот собственно и всё :)

Процесс-сервер можно запустить демоном и он будет висеть, обслуживать запросы,
а клиенты можно запускать вручную из консоли или сделать на UNIX-машине пользователя
по имени bbs, у которого в качестве командного интерпретатора будет указан
процесс bbs. Этим я убил еще одного зайца: коммуникацию тоже я возложил на UNIX,
а так как входить в мад надо будет по протоколу ssh, то это будет кроме всего 
прочего еще и защищенный мад!

Название BBS (bulletin board system) возникло у меня в голове случайно и
прижилось. На данный момент в маде реализован только минимальный чат
(механизм общения игроков), то есть мад вполне можно использовать в виде
простейшей BBS. Если вы считаете название дурацким, то по вашему "реки грязи"
или "кружок грязи" - более умные наименования? :)

Простейшая инсталляция

1. Все собрать командою make

2. Проверить, чтобы там, где бинарники, были каталоги cmd и messages

3. Запустить в одной консоли сервер ./server

4. Запустить в другой консоли клиента ./bbs

5. Радоваться (рыдать или поражаться идиотизму автора мада)

Пруль, Харьков, 23 октября 2014 года

proolix@gmail.com
http://prool.kharkov.org
http://mud.kharkov.org
