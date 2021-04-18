cmake_minimum_required(VERSION 3.17)

project (krakenapi CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_library(OpenSSL required)
#
set( SRCD ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( INCD ${CMAKE_CURRENT_SOURCE_DIR}/inc )

file(GLOB_RECURSE SRC ${SRCD}/*.cc)

add_library (krakenapi SHARED ${SRC} )
target_include_directories (krakenapi PRIVATE /usr/local/include )
target_include_directories (krakenapi PRIVATE /usr/local/opt/openssl/include )


target_include_directories (krakenapi PRIVATE ${SRCD} )

target_include_directories (krakenapi PRIVATE ${OPENSSL_INCLUDE_DIR} )
target_link_directories (krakenapi PRIVATE /usr/local/lib )
target_link_directories (krakenapi PRIVATE /usr/local/opt/openssl/lib )
target_link_libraries(krakenapi PRIVATE ssl crypto )
target_link_libraries(krakenapi PRIVATE curl curlpp )

install(TARGETS krakenapi LIBRARY DESTINATION $ENV{OBT_STAGE}/lib )
install(FILES ${SRCD}/dmbcs-kraken-api.h DESTINATION $ENV{OBT_STAGE}/include )
