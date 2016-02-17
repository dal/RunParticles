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
INCLUDEPATH += $${CINDER}/boost/
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
CONFIG += c++11
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
#LIBS += "/usr/local/lib/libcairo.a"
#LIBS += "/usr/local/lib/libpixman-1.a"
#LIBS += "/usr/local/lib/libpng.a"
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
QMAKE_MAC_SDK = macosx
}

CONFIG += qt

TEMPLATE = app

ICON = RunParticles.icns

appIcon.files = RunParticles.icns
appIcon.path = Contents/Resources

QMAKE_INFO_PLIST = Info.plist

QMAKE_BUNDLE_DATA += appIcon

HEADERS = ../src/AboutDialog.h \
          ../src/BoundingBox.h \
          ../src/Box.h \
          ../src/FitFileReader.h \
          ../src/ExportImageDialog.h \
          ../src/GLWidget.h \
          ../src/GpxHandler.h \
          ../src/Layer.h \
          ../src/LayerListWidget.h \
          ../src/MainWindow.h \
          ../src/Map.h \
          ../src/MapFileHandler.h \
          ../src/MapFileIO.h \
          ../src/MapProjectorWorker.h \
          ../src/MapView.h \
          ../src/OsmLayer.h \
          ../src/OsmTileSource.h \
          ../src/PathUtil.h \
          ../src/PlaybackWidget.h \
          ../src/PreferencesDialog.h \
          ../src/Projection.h \
          ../src/Settings.h \
          ../src/Singleton.h \
          ../src/TableWidgetDragRows.h \
          ../src/TcxHandler.h \
          ../src/TimeCtx.h \
          ../src/TrackFileReader.h \
          ../src/TrackLayer.h \
          ../src/Types.h \
          ../src/Util.h \
          ../src/ViewCtx.h \
          ../src/fitsdk/fit.hpp \
          ../src/fitsdk/fit_accumulator.hpp \
          ../src/fitsdk/fit_accumulated_field.hpp \
          ../src/fitsdk/fit_activity_mesg.hpp \
          ../src/fitsdk/fit_config.hpp \
          ../src/fitsdk/fit_decode.hpp \
          ../src/fitsdk/fit_factory.hpp \
          ../src/fitsdk/fit_field.hpp \
          ../src/fitsdk/fit_field_definition.hpp \
          ../src/fitsdk/fit_file_id_mesg.hpp \
          ../src/fitsdk/fit_file_creator_mesg.hpp \
          ../src/fitsdk/fit_mesg.hpp \
          ../src/fitsdk/fit_mesg_definition.hpp \
          ../src/fitsdk/fit_mesg_definition_listener.hpp \
          ../src/fitsdk/fit_mesg_listener.hpp \
          ../src/fitsdk/fit_runtime_exception.hpp \
          ../src/fitsdk/fit_profile.hpp \
          ../src/fitsdk/fit_unicode.hpp \
          ../src/fitsdk/fit_slave_device_mesg.hpp \
          ../src/fitsdk/fit_capabilities_mesg.hpp \
          ../src/fitsdk/fit_file_capabilities_mesg.hpp \
          ../src/fitsdk/fit_mesg_capabilities_mesg.hpp \
          ../src/fitsdk/fit_field_capabilities_mesg.hpp \
          ../src/fitsdk/fit_device_settings_mesg.hpp \
          ../src/fitsdk/fit_user_profile_mesg.hpp \
          ../src/fitsdk/fit_hrm_profile_mesg.hpp \
          ../src/fitsdk/fit_sdm_profile_mesg.hpp \
          ../src/fitsdk/fit_bike_profile_mesg.hpp \
          ../src/fitsdk/fit_zones_target_mesg.hpp \
          ../src/fitsdk/fit_sport_mesg.hpp \
          ../src/fitsdk/fit_hr_zone_mesg.hpp \
          ../src/fitsdk/fit_speed_zone_mesg.hpp \
          ../src/fitsdk/fit_cadence_zone_mesg.hpp \
          ../src/fitsdk/fit_power_zone_mesg.hpp \
          ../src/fitsdk/fit_met_zone_mesg.hpp \
          ../src/fitsdk/fit_goal_mesg.hpp \
          ../src/fitsdk/fit_session_mesg.hpp \
          ../src/fitsdk/fit_lap_mesg.hpp \
          ../src/fitsdk/fit_length_mesg.hpp \
          ../src/fitsdk/fit_record_mesg.hpp \
          ../src/fitsdk/fit_event_mesg.hpp \
          ../src/fitsdk/fit_device_info_mesg.hpp \
          ../src/fitsdk/fit_training_file_mesg.hpp \
          ../src/fitsdk/fit_hrv_mesg.hpp \
          ../src/fitsdk/fit_course_mesg.hpp \
          ../src/fitsdk/fit_course_point_mesg.hpp \
          ../src/fitsdk/fit_workout_mesg.hpp \
          ../src/fitsdk/fit_workout_step_mesg.hpp \
          ../src/fitsdk/fit_schedule_mesg.hpp \
          ../src/fitsdk/fit_totals_mesg.hpp \
          ../src/fitsdk/fit_weight_scale_mesg.hpp \
          ../src/fitsdk/fit_blood_pressure_mesg.hpp \
          ../src/fitsdk/fit_monitoring_info_mesg.hpp \
          ../src/fitsdk/fit_monitoring_mesg.hpp \
          ../src/fitsdk/fit_memo_glob_mesg.hpp \
          ../src/fitsdk/fit_pad_mesg.hpp \
          ../src/fitsdk/fit_mesg_with_event.hpp \
          ../src/fitsdk/fit_crc.hpp \
          ../src/fitsdk/fit_mesg_broadcaster.hpp \
          ../src/fitsdk/fit_mesg_with_event_broadcaster.hpp \
          ../src/fitsdk/fit_mesg_with_event_listener.hpp \
          ../src/fitsdk/fit_activity_mesg_listener.hpp \
          ../src/fitsdk/fit_lap_mesg_listener.hpp \
          ../src/fitsdk/fit_file_id_mesg_listener.hpp \
          ../src/fitsdk/fit_buffered_record_mesg_broadcaster.hpp \
          ../src/fitsdk/fit_buffered_record_mesg_listener.hpp \
          ../src/fitsdk/fit_buffered_record_mesg.hpp \
          ../src/fitsdk/fit_record_mesg_listener.hpp \
          ../src/fitsdk/fit_file_creator_mesg_listener.hpp \
          ../src/fitsdk/fit_software_mesg_listener.hpp \
          ../src/fitsdk/fit_slave_device_mesg_listener.hpp \
          ../src/fitsdk/fit_capabilities_mesg_listener.hpp \
          ../src/fitsdk/fit_file_capabilities_mesg_listener.hpp \
          ../src/fitsdk/fit_mesg_capabilities_mesg_listener.hpp \
          ../src/fitsdk/fit_field_capabilities_mesg_listener.hpp \
          ../src/fitsdk/fit_device_settings_mesg_listener.hpp \
          ../src/fitsdk/fit_user_profile_mesg_listener.hpp \
          ../src/fitsdk/fit_hrm_profile_mesg_listener.hpp \
          ../src/fitsdk/fit_sdm_profile_mesg_listener.hpp \
          ../src/fitsdk/fit_bike_profile_mesg_listener.hpp \
          ../src/fitsdk/fit_zones_target_mesg_listener.hpp \
          ../src/fitsdk/fit_sport_mesg_listener.hpp \
          ../src/fitsdk/fit_hr_zone_mesg_listener.hpp \
          ../src/fitsdk/fit_speed_zone_mesg_listener.hpp \
          ../src/fitsdk/fit_cadence_zone_mesg_listener.hpp \
          ../src/fitsdk/fit_power_zone_mesg_listener.hpp \
          ../src/fitsdk/fit_met_zone_mesg_listener.hpp \
          ../src/fitsdk/fit_goal_mesg_listener.hpp \
          ../src/fitsdk/fit_session_mesg_listener.hpp \
          ../src/fitsdk/fit_length_mesg_listener.hpp \
          ../src/fitsdk/fit_event_mesg_listener.hpp \
          ../src/fitsdk/fit_device_info_mesg_listener.hpp \
          ../src/fitsdk/fit_training_file_mesg_listener.hpp \
          ../src/fitsdk/fit_hrv_mesg_listener.hpp \
          ../src/fitsdk/fit_course_mesg_listener.hpp \
          ../src/fitsdk/fit_course_point_mesg_listener.hpp \
          ../src/fitsdk/fit_workout_mesg_listener.hpp \
          ../src/fitsdk/fit_workout_step_mesg_listener.hpp \
          ../src/fitsdk/fit_schedule_mesg_listener.hpp \
          ../src/fitsdk/fit_totals_mesg_listener.hpp \
          ../src/fitsdk/fit_weight_scale_mesg_listener.hpp \
          ../src/fitsdk/fit_blood_pressure_mesg_listener.hpp \
          ../src/fitsdk/fit_monitoring_info_mesg_listener.hpp \
          ../src/fitsdk/fit_monitoring_mesg_listener.hpp \
          ../src/fitsdk/fit_memo_glob_mesg_listener.hpp \
          ../src/fitsdk/fit_pad_mesg_listener.hpp

SOURCES = ../src/AboutDialog.cpp \
          ../src/ExportImageDialog.cpp \
          ../src/FitFileReader.cpp \
          ../src/GLWidget.cpp \
          ../src/GpxHandler.cpp \
          ../src/main.cpp \
          ../src/Layer.cpp \
          ../src/LayerListWidget.cpp \
          ../src/MainWindow.cpp \
          ../src/Map.cpp \
          ../src/MapFileHandler.cpp \
          ../src/MapFileIO.cpp \
          ../src/MapProjectorWorker.cpp \
          ../src/MapView.cpp \
          ../src/OsmLayer.cpp \
          ../src/OsmTileSource.cpp \
          ../src/PathUtil.cpp \
          ../src/PlaybackWidget.cpp \
          ../src/PreferencesDialog.cpp \
          ../src/Projection.cpp \
          ../src/Settings.cpp \
          ../src/TableWidgetDragRows.cpp \
          ../src/TcxHandler.cpp \
          ../src/TimeCtx.cpp \
          ../src/TrackFileReader.cpp \
          ../src/TrackLayer.cpp \
          ../src/ViewCtx.cpp \
          ../src/Util.cpp \
          ../src/fitsdk/fit.cpp \
          ../src/fitsdk/fit_accumulated_field.cpp \
          ../src/fitsdk/fit_accumulator.cpp \
          ../src/fitsdk/fit_decode.cpp \
          ../src/fitsdk/fit_factory.cpp \
          ../src/fitsdk/fit_field.cpp \
          ../src/fitsdk/fit_field_definition.cpp \
          ../src/fitsdk/fit_mesg.cpp \
          ../src/fitsdk/fit_mesg_definition.cpp \
          ../src/fitsdk/fit_profile.cpp \
          ../src/fitsdk/fit_software_mesg.hpp \
          ../src/fitsdk/fit_unicode.cpp \
          ../src/fitsdk/fit_crc.cpp \
          ../src/fitsdk/fit_mesg_broadcaster.cpp \
          ../src/fitsdk/fit_mesg_with_event_broadcaster.cpp \
          ../src/fitsdk/fit_buffered_record_mesg_broadcaster.cpp

OTHER_FILES += ../resources/about.html
               ../resources/track1.frag
               ../resources/track1.geom
               ../resources/track1.vert

RESOURCES = ../resources/resources.qrc
          
QT += opengl network xml
