win32 {
TEMPLATE=vcapp
CINDER="../cinder_0.8.2_vc2008"
}

macx {
FRAMEWORKS=/System/Library/Frameworks
}

# glm (header only) is used for vector math. The copy bundled with Cinder is
# fine; override with qmake GLM_DIR=/path/containing/glm to use another.
isEmpty(GLM_DIR): GLM_DIR = /Users/dal/Documents/cinder/Cinder/include
INCLUDEPATH += $${GLM_DIR}
DEFINES += GLM_FORCE_CTOR_INIT
INCLUDEPATH += ../src ../src/commands ../src/fitsdk ../include

win32 {
VPATH += $${CINDER}/include/
INCLUDEPATH += $${CINDER}/include/
INCLUDEPATH += $${CINDER}/boost/
INCLUDEPATH += $${CINDER}/boost/boost/
}

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
CONFIG += c++17
LIBS += -framework OpenGL
LIBS += -framework Cocoa
LIBS += -framework AppKit
LIBS += -framework Foundation
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
# AGL was removed from the macOS SDK; older Qt mkspecs still link it
QMAKE_LIBS_OPENGL = -framework OpenGL
QMAKE_INCDIR_OPENGL = /System/Library/Frameworks/OpenGL.framework/Headers
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
          ../src/GLUtil.h \
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
          ../src/commands/AddLayersCommand.h \
          ../src/commands/EditLayerCommand.h \
          ../src/commands/RemoveLayersCommand.h \
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
          ../src/GLUtil.cpp \
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
          ../src/Types.cpp \
          ../src/ViewCtx.cpp \
          ../src/Util.cpp \
          ../src/commands/AddLayersCommand.cpp \
          ../src/commands/EditLayerCommand.cpp \
          ../src/commands/RemoveLayersCommand.cpp \
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

OTHER_FILES += ../resources/about.html \
               ../resources/track1.frag \
               ../resources/track1.geom \
               ../resources/track1.vert \
               ../resources/viewportGlow.frag \
               ../resources/quad.vert \
               ../resources/osmTile.frag \
               ../resources/label.frag \
               ../resources/flat.vert \
               ../resources/flat.frag

RESOURCES = ../resources/resources.qrc
          
QT += widgets opengl openglwidgets network xml core5compat
