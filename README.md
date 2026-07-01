![banner](https://i.imgur.com/CJ2yNAE.png)

## min req
- visual studio (2022)
- msvc compiler (v143)
- directx c++ sdk

## кланг ветка
 - все тоже самое + деф кланг для vs studio

я ее ваще по приколу вкоммитил, стеш лежал с давних времен

# traumaskeet [gta:v cheat project] (og desc)
## *contains anything that used for this creation, namely:*
1) **cheat module**, based on simple dx hook and imgui menu.
2) kernelmode - basic **driver** communication.
3) usermode - cheat **loader**.<br>
   includes a python script for converting other modules into bytes and encrypting them.<br>
   basically uses only communication with the driver.
4) **protection** configs.<br>
   конфигурации для протекторов под каждый модуль (за исключением драйвера)<br>
   used protectors: [codevirtualizer (*3.1.4.0*)](https://www.52pojie.cn/thread-1752864-1-1.html), [vmprotect (*3.7.8*)](https://www.52pojie.cn/forum.php?mod=forumdisplay&fid=4&filter=typeid&typeid=125 "somewhere here, i lost the link"), [themida (*3.1.8.0*)](https://www.52pojie.cn/thread-1885788-1-1.html).
