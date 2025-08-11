cmake_minimum_required (VERSION 3.13)
set(PROJECT_NAME "sample-libwebrtc")

project (${PROJECT_NAME})
set(WEBRTC_ROOT ${SOURCE_ROOT}/include)
add_compile_definitions(
    RTC_DESKTOP_DEVICE
    NDEBUG _ITERATOR_DEBUG_LEVEL=0
    ABSL_ALLOCATOR_NOTHROW=1
    CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
    DYNAMIC_ANNOTATIONS_ENABLED=1
   
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    
    PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII=0
    PSAPI_VERSION=2
    USE_AURA=1

    RTC_ENABLE_VP9 RTC_ENABLE_H264 RTC_ENABLE_H265 RTC_DAV1D_IN_INTERNAL_DECODER_FACTORY RTC_ENABLE_WIN_WGC
    WEBRTC_ENABLE_PROTOBUF=1 WEBRTC_STRICT_FIELD_TRIALS=0 WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE WEBRTC_HAVE_SCTP WEBRTC_USE_H264 WEBRTC_USE_H265 WEBRTC_LIBRARY_IMPL WEBRTC_ENABLE_AVX2 WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1 WEBRTC_WIN 
)
set(CMAKE_CXX_STANDARD 20)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(HEADER_DIR      ${SOURCE_PATH}/include)
set(SOURCE_DIR      ${SOURCE_PATH}/src)

file(GLOB HEADER_FILES ${HEADER_DIR}/*.h)
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*.cpp ${SOURCE_DIR}/*.cc ${SOURCE_DIR}/*.h)

include_directories(${SOURCE_ROOT}/include)

include_directories(${WEBRTC_ROOT}/libwebrtc)

set(Lib_DIR ${SOURCE_ROOT}/lib/win64/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>$<$<CONFIG:MinSizeRel>:release>$<$<CONFIG:RelWithDebInfo>:debug>)
#set(Lib_DIR ${WEBRTC_ROOT}/out/win64/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>$<$<CONFIG:MinSizeRel>:release>$<$<CONFIG:RelWithDebInfo>:debug>/obj)
find_package(httplib CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
add_executable(${PROJECT_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE    
    ${Lib_DIR}/libwebrtc.dll.lib
    advapi32.lib comdlg32.lib dbghelp.lib dnsapi.lib gdi32.lib msimg32.lib odbc32.lib 
    odbccp32.lib oleaut32.lib shell32.lib shlwapi.lib user32.lib usp10.lib uuid.lib version.lib wininet.lib winmm.lib 
    winspool.lib ws2_32.lib delayimp.lib kernel32.lib ole32.lib crypt32.lib iphlpapi.lib secur32.lib 
    dmoguids.lib wmcodecdspuuid.lib amstrmid.lib msdmo.lib bcrypt.lib strmiids.lib
    d3d11.lib d3dcompiler.lib Dwmapi.lib Shcore.lib DXGI.lib
    httplib::httplib
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)  
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
