TARGET = main
TEMPLATE = app

SOURCES += \
    main.cpp

OTHER_FILES += \
    plugins.cfg \
    ogre.cfg \
    ogre.log \
    resources.cfg \
    models \
    textures \

LIBS += -lOgreMain -lOgreTerrain -lOIS
INCLUDEPATH += /usr/include/OGRE/
INCLUDEPATH += /usr/include/OGRE/Terrain

#MAKEFILE DEL PALAZO
#main: main.cpp
#	g++ -o main main.cpp -lOgreMain -lOgreTerrain -lOIS

#clean:
#	rm -f main
