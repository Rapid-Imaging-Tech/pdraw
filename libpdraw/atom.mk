
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpdraw
LOCAL_DESCRIPTION := Parrot Drones Awesome Video Viewer library
LOCAL_CATEGORY_PATH := libs

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
# Public API headers - top level headers first
# This header list is currently used to generate a python binding
LOCAL_EXPORT_CUSTOM_VARIABLES := LIBPDRAW_HEADERS=$\
	$(LOCAL_PATH)/include/pdraw/pdraw.h:$\
	$(LOCAL_PATH)/include/pdraw/pdraw_defs.h;
LOCAL_EXPORT_CXXFLAGS := -std=c++11 \
	-DBOOST_NO_CXX11_SCOPED_ENUMS

LOCAL_CFLAGS := -DPDRAW_API_EXPORTS -fvisibility=hidden -D_USE_MATH_DEFINES \
	-DRESPONDAR \
	-DBOOST_NO_CXX11_SCOPED_ENUMS \
	-DHAVE_PTHREAD=1 \
	-DXML_POOR_ENTROPY \
	-Ipackages/pdraw/libpdraw/src/ \
	-F/usr/local/Cellar/qt/5.15.1/lib \
	-I/usr/local/Cellar/qt/5.15.1/lib/QtGui.framework/Versions/5/Headers/ \
	-I/usr/local/Cellar/libpng/1.6.37/include \
	-F"/Users/lafrance/dev/rit/boost_build/dist/boost.xcframework" \
	-I"/Users/lafrance/dev/rit/boost_build/dist/boost.xcframework/Headers"

LOCAL_SRC_FILES := \
	src/pdraw_avcdecoder.cpp \
	src/pdraw_channel.cpp \
	src/pdraw_demuxer_record.cpp \
	src/pdraw_demuxer_stream.cpp \
	src/pdraw_demuxer_stream_mux.cpp \
	src/pdraw_demuxer_stream_net.cpp \
	src/pdraw_element.cpp \
	src/pdraw_gles2_hmd.cpp \
	src/pdraw_gles2_hmd_colors.cpp \
	src/pdraw_gles2_hmd_indices.cpp \
	src/pdraw_gles2_hmd_positions_cockpitglasses.cpp \
	src/pdraw_gles2_hmd_positions_cockpitglasses2.cpp \
	src/pdraw_gles2_hmd_shaders.cpp \
	src/pdraw_gles2_hmd_texcoords.cpp \
	src/pdraw_gles2_hmd_texcoords_cockpitglasses_blue.cpp \
	src/pdraw_gles2_hmd_texcoords_cockpitglasses_red.cpp \
	src/pdraw_gles2_video.cpp \
	src/pdraw_media.cpp \
	src/pdraw_metadata_session.cpp \
	src/pdraw_renderer.cpp \
	src/pdraw_renderer_gles2.cpp \
	src/pdraw_renderer_videocoreegl.cpp \
	src/pdraw_session.cpp \
	src/pdraw_settings.cpp \
	src/pdraw_sink.cpp \
	src/pdraw_sink_video.cpp \
	src/pdraw_socket_inet.cpp \
	src/pdraw_source.cpp \
	src/pdraw_utils.cpp \
	src/pdraw_wrapper.cpp \
	src/ClientFactory.cpp \
	src/HardCodedSettings.cpp \
	src/KMLEntities.cpp \
	src/KMLParser.cpp \
	src/LFClientEngine.cpp \
	src/LoggableObject.cpp \
	src/RotationTK.cpp \
	src/StatusLogger.cpp \
	src/StringTrimmingRoutines.cpp \
	src/TextFileUserSettings.cpp \
	src/coordinateconverter.cpp \
	src/customkmlfunctions.cpp \
	src/dted.cpp \
	src/elevationmanagerlocal.cpp \
	src/geotileservergl.cpp \
	src/geovectileparser.cpp \
	src/metadata.cpp \
	src/objcUtiltiyWrapper.mm \
	src/openglalignedtext.cpp \
	src/openglautoalignedtext.cpp \
	src/openglfont.cpp \
	src/opengltext.cpp \
	src/pathway.cpp \
	src/placemarkcollection.cpp \
	src/rafootprint.cpp \
	src/rastertile.cpp \
	src/rimatrixmath.cpp \
	src/streetlabelline.cpp \
	src/streetlinemanager.cpp \
	src/streetlinetriangulation.cpp \
	src/streetsegment.cpp \
	src/symbol.cpp \
	src/tile.cpp \
	src/utilities.cpp \
	src/vectortile.cpp \
	src/vector_tile.cc \
	src/vector_tile_compression.cpp \
	src/kml/base/attributes.cc src/kml/base/csv_splitter.cc src/kml/base/date_time.cc src/kml/base/expat_handler_ns.cc src/kml/base/expat_parser.cc src/kml/base/file.cc src/kml/base/file_posix.cc src/kml/base/math_util.cc src/kml/base/mimetypes.cc src/kml/base/referent.cc src/kml/base/string_util.cc src/kml/base/time_util.cc src/kml/base/uri_parser.cc src/kml/base/version.cc src/kml/base/xml_namespaces.cc src/kml/base/zip_file.cc src/kml/convenience/atom_util.cc src/kml/convenience/convenience.cc src/kml/convenience/csv_file.cc src/kml/convenience/csv_parser.cc src/kml/convenience/feature_list.cc src/kml/convenience/google_doc_list.cc src/kml/convenience/google_maps_data.cc src/kml/convenience/google_picasa_web.cc src/kml/convenience/google_spreadsheets.cc src/kml/convenience/http_client.cc src/kml/convenience/kmz_check_links.cc src/kml/dom/abstractlatlonbox.cc src/kml/dom/abstractview.cc src/kml/dom/atom.cc src/kml/dom/balloonstyle.cc src/kml/dom/colorstyle.cc src/kml/dom/container.cc src/kml/dom/document.cc src/kml/dom/element.cc src/kml/dom/extendeddata.cc src/kml/dom/feature.cc src/kml/dom/folder.cc src/kml/dom/geometry.cc  src/kml/dom/gx_tour.cc src/kml/dom/hotspot.cc src/kml/dom/iconstyle.cc src/kml/dom/kml.cc src/kml/dom/kml22.cc src/kml/dom/kml_cast.cc src/kml/dom/kml_factory.cc src/kml/dom/kml_handler.cc src/kml/dom/kml_handler_ns.cc src/kml/dom/labelstyle.cc src/kml/dom/linestyle.cc src/kml/dom/link.cc src/kml/dom/liststyle.cc src/kml/dom/model.cc src/kml/dom/networklink.cc src/kml/dom/networklinkcontrol.cc src/kml/dom/object.cc src/kml/dom/overlay.cc src/kml/dom/parser.cc src/kml/dom/placemark.cc src/kml/dom/polystyle.cc src/kml/dom/region.cc src/kml/dom/schema.cc src/kml/dom/serializer.cc src/kml/dom/snippet.cc src/kml/dom/style.cc src/kml/dom/stylemap.cc src/kml/dom/styleselector.cc src/kml/dom/substyle.cc src/kml/dom/timeprimitive.cc src/kml/dom/vec2.cc src/kml/dom/visitor.cc src/kml/dom/visitor_driver.cc src/kml/dom/xal.cc src/kml/dom/xml_serializer.cc src/kml/dom/xsd.cc src/kml/engine/clone.cc src/kml/engine/entity_mapper.cc src/kml/engine/feature_balloon.cc src/kml/engine/feature_view.cc src/kml/engine/feature_visitor.cc src/kml/engine/find.cc src/kml/engine/find_xml_namespaces.cc src/kml/engine/get_link_parents.cc src/kml/engine/get_links.cc src/kml/engine/href.cc src/kml/engine/id_mapper.cc src/kml/engine/kml_cache.cc src/kml/engine/kml_file.cc src/kml/engine/kml_stream.cc src/kml/engine/kml_uri.cc src/kml/engine/kmz_cache.cc src/kml/engine/kmz_file.cc src/kml/engine/link_util.cc src/kml/engine/location_util.cc src/kml/engine/merge.cc src/kml/engine/parse_old_schema.cc src/kml/engine/style_inliner.cc src/kml/engine/style_merger.cc src/kml/engine/style_resolver.cc src/kml/engine/style_splitter.cc src/kml/engine/update.cc src/kml/engine/update_processor.cc src/kml/regionator/feature_list_region_handler.cc src/kml/regionator/regionator.cc src/kml/regionator/regionator_util.cc src/kml/xsd/xsd_complex_type.cc src/kml/xsd/xsd_element.cc src/kml/xsd/xsd_file.cc src/kml/xsd/xsd_handler.cc src/kml/xsd/xsd_primitive_type.cc src/kml/xsd/xsd_util.cc src/kml/xsd/xst_parser.cc \
	src/uriparser/UriCommon.c src/uriparser/UriCompare.c src/uriparser/UriEscape.c src/uriparser/UriFile.c src/uriparser/UriIp4.c src/uriparser/UriIp4Base.c src/uriparser/UriNormalize.c src/uriparser/UriNormalizeBase.c src/uriparser/UriParse.c src/uriparser/UriParseBase.c src/uriparser/UriQuery.c src/uriparser/UriRecompose.c src/uriparser/UriResolve.c src/uriparser/UriShorten.c \
	src/minizip/ioapi.c src/minizip/iomem_simple.c src/minizip/mztools.c src/minizip/unzip.c src/minizip/zip.c \
	src/google/protobuf/any.cc src/google/protobuf/stubs/common.cc src/google/protobuf/any.pb.cc src/google/protobuf/api.pb.cc src/google/protobuf/arena.cc src/google/protobuf/arena_test_util.cc src/google/protobuf/arenastring.cc src/google/protobuf/compiler/code_generator.cc src/google/protobuf/compiler/command_line_interface.cc src/google/protobuf/compiler/cpp/cpp_enum.cc src/google/protobuf/compiler/cpp/cpp_enum_field.cc src/google/protobuf/compiler/cpp/cpp_extension.cc src/google/protobuf/compiler/cpp/cpp_field.cc src/google/protobuf/compiler/cpp/cpp_file.cc src/google/protobuf/compiler/cpp/cpp_generator.cc src/google/protobuf/compiler/cpp/cpp_helpers.cc src/google/protobuf/compiler/cpp/cpp_map_field.cc src/google/protobuf/compiler/cpp/cpp_message.cc src/google/protobuf/compiler/cpp/cpp_message_field.cc src/google/protobuf/compiler/cpp/cpp_primitive_field.cc src/google/protobuf/compiler/cpp/cpp_service.cc src/google/protobuf/compiler/cpp/cpp_string_field.cc src/google/protobuf/compiler/csharp/csharp_doc_comment.cc src/google/protobuf/compiler/csharp/csharp_enum.cc src/google/protobuf/compiler/csharp/csharp_enum_field.cc src/google/protobuf/compiler/csharp/csharp_field_base.cc src/google/protobuf/compiler/csharp/csharp_generator.cc src/google/protobuf/compiler/csharp/csharp_helpers.cc src/google/protobuf/compiler/csharp/csharp_map_field.cc src/google/protobuf/compiler/csharp/csharp_message.cc src/google/protobuf/compiler/csharp/csharp_message_field.cc src/google/protobuf/compiler/csharp/csharp_primitive_field.cc src/google/protobuf/compiler/csharp/csharp_reflection_class.cc src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc src/google/protobuf/compiler/importer.cc src/google/protobuf/compiler/java/java_context.cc src/google/protobuf/compiler/java/java_doc_comment.cc src/google/protobuf/compiler/java/java_enum.cc src/google/protobuf/compiler/java/java_enum_field.cc src/google/protobuf/compiler/java/java_enum_field_lite.cc src/google/protobuf/compiler/java/java_enum_lite.cc src/google/protobuf/compiler/java/java_extension.cc src/google/protobuf/compiler/java/java_extension_lite.cc src/google/protobuf/compiler/java/java_field.cc src/google/protobuf/compiler/java/java_file.cc src/google/protobuf/compiler/java/java_generator.cc src/google/protobuf/compiler/java/java_generator_factory.cc src/google/protobuf/compiler/java/java_helpers.cc src/google/protobuf/compiler/java/java_lazy_message_field.cc src/google/protobuf/compiler/java/java_lazy_message_field_lite.cc src/google/protobuf/compiler/java/java_map_field.cc src/google/protobuf/compiler/java/java_map_field_lite.cc src/google/protobuf/compiler/java/java_message.cc src/google/protobuf/compiler/java/java_message_builder.cc src/google/protobuf/compiler/java/java_message_builder_lite.cc src/google/protobuf/compiler/java/java_message_field.cc src/google/protobuf/compiler/java/java_message_field_lite.cc src/google/protobuf/compiler/java/java_message_lite.cc src/google/protobuf/compiler/java/java_name_resolver.cc src/google/protobuf/compiler/java/java_primitive_field.cc src/google/protobuf/compiler/java/java_primitive_field_lite.cc src/google/protobuf/compiler/java/java_service.cc src/google/protobuf/compiler/java/java_shared_code_generator.cc src/google/protobuf/compiler/java/java_string_field.cc src/google/protobuf/compiler/java/java_string_field_lite.cc src/google/protobuf/compiler/javanano/javanano_enum.cc src/google/protobuf/compiler/javanano/javanano_enum_field.cc src/google/protobuf/compiler/javanano/javanano_extension.cc src/google/protobuf/compiler/javanano/javanano_field.cc src/google/protobuf/compiler/javanano/javanano_file.cc src/google/protobuf/compiler/javanano/javanano_generator.cc src/google/protobuf/compiler/javanano/javanano_helpers.cc src/google/protobuf/compiler/javanano/javanano_map_field.cc src/google/protobuf/compiler/javanano/javanano_message.cc src/google/protobuf/compiler/javanano/javanano_message_field.cc src/google/protobuf/compiler/javanano/javanano_primitive_field.cc src/google/protobuf/compiler/js/embed.cc src/google/protobuf/compiler/js/js_generator.cc src/google/protobuf/compiler/main.cc src/google/protobuf/compiler/objectivec/objectivec_enum.cc src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc src/google/protobuf/compiler/objectivec/objectivec_extension.cc src/google/protobuf/compiler/objectivec/objectivec_field.cc src/google/protobuf/compiler/objectivec/objectivec_file.cc src/google/protobuf/compiler/objectivec/objectivec_generator.cc src/google/protobuf/compiler/objectivec/objectivec_helpers.cc src/google/protobuf/compiler/objectivec/objectivec_map_field.cc src/google/protobuf/compiler/objectivec/objectivec_message.cc src/google/protobuf/compiler/objectivec/objectivec_message_field.cc src/google/protobuf/compiler/objectivec/objectivec_oneof.cc src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc src/google/protobuf/compiler/parser.cc src/google/protobuf/compiler/php/php_generator.cc src/google/protobuf/compiler/plugin.cc src/google/protobuf/compiler/plugin.pb.cc src/google/protobuf/compiler/profile.pb.cc src/google/protobuf/compiler/python/python_generator.cc src/google/protobuf/compiler/ruby/ruby_generator.cc src/google/protobuf/compiler/subprocess.cc src/google/protobuf/compiler/test_plugin.cc src/google/protobuf/compiler/zip_writer.cc src/google/protobuf/descriptor.cc src/google/protobuf/descriptor.pb.cc src/google/protobuf/descriptor_database.cc src/google/protobuf/duration.pb.cc src/google/protobuf/dynamic_message.cc src/google/protobuf/empty.pb.cc src/google/protobuf/extension_set.cc src/google/protobuf/extension_set_heavy.cc src/google/protobuf/field_mask.pb.cc src/google/protobuf/generated_message_reflection.cc src/google/protobuf/generated_message_table_driven.cc src/google/protobuf/generated_message_util.cc src/google/protobuf/io/coded_stream.cc src/google/protobuf/io/gzip_stream.cc src/google/protobuf/io/printer.cc src/google/protobuf/io/strtod.cc src/google/protobuf/io/tokenizer.cc src/google/protobuf/io/zero_copy_stream.cc src/google/protobuf/io/zero_copy_stream_impl.cc src/google/protobuf/io/zero_copy_stream_impl_lite.cc src/google/protobuf/map_field.cc  src/google/protobuf/message.cc src/google/protobuf/message_lite.cc src/google/protobuf/reflection_ops.cc src/google/protobuf/repeated_field.cc src/google/protobuf/service.cc src/google/protobuf/source_context.pb.cc src/google/protobuf/struct.pb.cc src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc src/google/protobuf/stubs/bytestream.cc  src/google/protobuf/stubs/int128.cc src/google/protobuf/stubs/mathlimits.cc src/google/protobuf/stubs/once.cc src/google/protobuf/stubs/status.cc src/google/protobuf/stubs/statusor.cc src/google/protobuf/stubs/stringpiece.cc src/google/protobuf/stubs/stringprintf.cc src/google/protobuf/stubs/structurally_valid.cc src/google/protobuf/stubs/strutil.cc src/google/protobuf/stubs/substitute.cc src/google/protobuf/stubs/time.cc src/google/protobuf/testing/file.cc src/google/protobuf/testing/zcgunzip.cc src/google/protobuf/testing/zcgzip.cc src/google/protobuf/text_format.cc src/google/protobuf/timestamp.pb.cc src/google/protobuf/type.pb.cc src/google/protobuf/unknown_field_set.cc src/google/protobuf/util/delimited_message_util.cc src/google/protobuf/util/field_comparator.cc src/google/protobuf/util/field_mask_util.cc src/google/protobuf/util/internal/datapiece.cc src/google/protobuf/util/internal/default_value_objectwriter.cc src/google/protobuf/util/internal/error_listener.cc src/google/protobuf/util/internal/field_mask_utility.cc src/google/protobuf/util/internal/json_escaping.cc src/google/protobuf/util/internal/json_objectwriter.cc src/google/protobuf/util/internal/json_stream_parser.cc src/google/protobuf/util/internal/object_writer.cc src/google/protobuf/util/internal/proto_writer.cc src/google/protobuf/util/internal/protostream_objectsource.cc src/google/protobuf/util/internal/protostream_objectwriter.cc src/google/protobuf/util/internal/type_info.cc src/google/protobuf/util/internal/type_info_test_helper.cc src/google/protobuf/util/internal/utility.cc src/google/protobuf/util/json_util.cc src/google/protobuf/util/message_differencer.cc src/google/protobuf/util/time_util.cc src/google/protobuf/util/type_resolver_util.cc src/google/protobuf/wire_format.cc src/google/protobuf/wire_format_lite.cc src/google/protobuf/wrappers.pb.cc \
	src/expat/xmlparse.c src/expat/xmltok.c src/expat/xmltok_ns.c src/expat/xmlrole.c src/expat/xmltok_impl.c

LOCAL_LIBRARIES := \
	eigen \
	libfutils \
	libh264 \
	libmp4 \
	libpomp \
	librtsp \
	libsdp \
	libulog \
	libvideo-buffers \
	libvideo-buffers-generic \
	libvideo-decode \
	libvideo-metadata \
	libvideo-streaming
LOCAL_CONDITIONAL_LIBRARIES := \
	OPTIONAL:json \
	OPTIONAL:libmux

ifeq ("$(TARGET_OS)-$(TARGET_OS_FLAVOUR)","linux-native")
  LOCAL_CFLAGS += -DUSE_GLES2
  LOCAL_LIBRARIES += \
	glfw3 \
	opengl
else ifeq ("$(TARGET_OS)-$(TARGET_OS_FLAVOUR)","linux-android")
  LOCAL_CFLAGS += -DUSE_GLES2
  LOCAL_LDLIBS += -lEGL -lGLESv2 -landroid
else ifeq ("$(TARGET_OS)-$(TARGET_OS_FLAVOUR)","darwin-native")
  LOCAL_CFLAGS += -DUSE_GLES2
  LOCAL_LDLIBS += \
	-framework OpenGL
  LOCAL_LIBRARIES += \
	glfw3
else ifeq ("$(TARGET_OS)-$(TARGET_OS_FLAVOUR)","darwin-iphoneos")
  LOCAL_CFLAGS += -DUSE_GLES2
  LOCAL_LDLIBS += \
	-framework OpenGLES
else ifeq ("$(TARGET_OS)","windows")
  LOCAL_CFLAGS += -DUSE_GLES2 -D_WIN32_WINNT=0x0600
  LOCAL_LDLIBS += -lws2_32 -lepoxy
endif

include $(BUILD_LIBRARY)
