set path=./bin

del zenadriver.sys.c 

cl /D"DBG" src/driver.cpp /GL  /I./ntl /W4  /Za /Ob2gty /GzyS- /Fezenadriver.sys /nologo /link /LIBPATH:./lib /align:32 /driver /machine:x86 wdm.lib /opt:icf

bin2c.pl zenadriver.sys

cl /TP src/launcher.cpp zenadriver.sys.c /GL  /I./ntl /W4 /Za /Ob2gty /GzyS- /Fezenadriver.exe /nologo /link /LIBPATH:./lib /opt:icf
