win32 {
TEMPLATE=vcapp
CINDER="../cinder_0.8.2_vc2008"
}

macx {
CINDER="/usr/local/cinder_0.8.5_mac"
FRAMEWORKS=/System/Library/Frameworks
}

VPATH += $${CINDER}/include/
INCLUDEPATH += $${CINDER}/include/
INCLUDEPATH += $${CINDER}/boost/boost/

win32 {
debug {
LIBS += $${CINDER}/lib/cinder_d.lib
LIBS += $${CINDER}"/lib/msw/libboost_date_time-vc90-mt-sgd-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_filesystem-vc90-mt-sgd-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_system-vc90-mt-sgd-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_thread-vc90-mt-sgd-1_44.lib"
} else {
LIBS += $${CINDER}/lib/cinder.lib
LIBS += $${CINDER}"/lib/msw/libboost_date_time-vc90-mt-s-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_filesystem-vc90-mt-s-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_system-vc90-mt-s-1_44.lib"
LIBS += $${CINDER}"/lib/msw/libboost_thread-vc90-mt-s-1_44.lib"
}
LIBS += $${CINDER}"/lib/msw/atls.lib"
LIBS += $${CINDER}"/lib/msw/atlthunk.lib"
LIBS += $${CINDER}"/lib/msw/cairo-static.lib"
LIBS += $${CINDER}"/lib/msw/pixman-1.lib"
LIBS += $${CINDER}"/lib/msw/libpng.lib"
LIBS += $${CINDER}"/lib/msw/libjpeg.lib"
LIBS += $${CINDER}"/lib/msw/Wldap32.lib"
LIBS += $${CINDER}"/lib/msw/zlib.lib"
}

macx {
QMAKE_LFLAGS += -L$${FRAMEWORKS}/Accelerate.framework
LIBS += -framework Accelerate
QMAKE_LFLAGS += -L$${FRAMEWORKS}/AudioToolbox.framework
LIBS += -framework AudioToolbox
QMAKE_LFLAGS += -L$${FRAMEWORKS}/AudioUnit.framework
LIBS += -framework AudioUnit
QMAKE_LFLAGS += -L$${FRAMEWORKS}/CoreAudio.framework
LIBS += -framework CoreAudio
#QMAKE_LFLAGS += -L$${FRAMEWORKS}/QuickTime.framework
#LIBS += -framework QuickTime
QMAKE_LFLAGS += -L$${FRAMEWORKS}/QTKit.framework
LIBS += -framework QTKit
QMAKE_LFLAGS += -L$${FRAMEWORKS}/CoreVideo.framework
LIBS += -framework CoreVideo
QMAKE_LFLAGS += -L$${FRAMEWORKS}/Carbon.framework
LIBS += -framework Carbon
QMAKE_LFLAGS += -L$${FRAMEWORKS}/OpenGL.framework
LIBS += -framework OpenGL
QMAKE_LFLAGS += -L$${FRAMEWORKS}/Cocoa.framework
LIBS += -framework Cocoa
QMAKE_LFLAGS += -L$${FRAMEWORKS}/AppKit.framework
LIBS += -framework AppKit
QMAKE_LFLAGS += -L$${FRAMEWORKS}/CoreData.framework
LIBS += -framework CoreData
QMAKE_LFLAGS += -L$${FRAMEWORKS}/Foundation.framework
LIBS += -framework Foundation
}

macx {
LIBS += $${CINDER}/lib/libcinder.a
LIBS += $${CINDER}"/lib/macosx/libboost_date_time.a"
LIBS += $${CINDER}"/lib/macosx/libboost_filesystem.a"
LIBS += $${CINDER}"/lib/macosx/libboost_system.a"
LIBS += $${CINDER}"/lib/macosx/libboost_thread.a"
LIBS += $${CINDER}"/lib/macosx/libboost_regex.a"
#LIBS += $${CINDER}"/lib/macosx/libcairo.a"
LIBS += "/usr/local/lib/libcairo.a"
#LIBS += $${CINDER}"/lib/macosx/libpixman-1.a"
LIBS += "/usr/local/lib/libpixman-1.a"
#LIBS += $${CINDER}"/lib/macosx/libpng14.a"
LIBS += "/usr/local/lib/libpng.a"
#LIBS += $${CINDER}"/lib/macosx/libz.a"
CONFIG += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -stdlib=libc++
XCODE_COMPILER_LIBRARY.name = CLANG_CXX_LIBRARY
XCODE_COMPILER_LIBRARY.value = libc++
QMAKE_MAC_XCODE_SETTINGS += XCODE_COMPILER_LIBRARY
#CONFIG -= x86_64 ppc64
#CONFIG += x86
#ARCHES -= x86_64
ARCHES -= i386
QMAKE_MAC_SDK = /Applications/Xcode.app/Contents/./Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk
}

CONFIG += qt

TEMPLATE = app

HEADERS = ../src/BoundingBox.h \
          ../src/GLWidget.h \
          ../src/GpxHandler.h \
          ../src/Layer.h \
          ../src/LayerListWidget.h \
          ../src/MainWindow.h \
          ../src/Map.h \
          ../src/MapView.h \
          ../src/PathUtil.h \
          ../src/TcxHandler.h \
          ../src/TimeCtx.h \
          ../src/TrackFileReader.h \
          ../src/TrackLayer.h \
          ../src/Types.h \
          ../src/Util.h \
          ../src/ViewCtx.h \
          ../src/XmlHandler.h

SOURCES = ../src/BoundingBox.cpp \
          ../src/GLWidget.cpp \
          ../src/GpxHandler.cpp \
          ../src/main.cpp \
          ../src/Layer.cpp \
          ../src/LayerListWidget.cpp \
          ../src/MainWindow.cpp \
          ../src/Map.cpp \
          ../src/MapView.cpp \
          ../src/PathUtil.cpp \
          ../src/TcxHandler.cpp \
          ../src/TimeCtx.cpp \
          ../src/TrackFileReader.cpp \
          ../src/TrackLayer.cpp \
          ../src/ViewCtx.cpp \
          ../src/Util.cpp \
          ../src/XmlHandler.cpp
QT += opengl xml
