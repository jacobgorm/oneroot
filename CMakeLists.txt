cmake_minimum_required(VERSION 3.4)

project("mindcastle" C)
option(BUILD_PYKV "build pykv python3 extension" OFF)

find_package(CURL)
find_package(OpenSSL)

set(CMAKE_C_FLAGS "-Wall -Werror -Ofast -Wfatal-errors -Wno-unused-result -D_POSIX_C_SOURCE=200809L -g")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#set (SANITIZE 1)
if (SANITIZE)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

include_directories(
    "${CMAKE_SOURCE_DIR}"
)

add_library(swap
    block-swap.c
    aio.c
    ioh.c
    tinyuuid.c
    block-swap/crypto.c
    block-swap/dubtree.c
    block-swap/hashtable.c
    block-swap/simpletree.c
    )

install(TARGETS swap
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

add_executable(mindcastle nbd.c)
target_link_libraries(mindcastle swap -lpthread -llz4 ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})

add_executable(img-test img-test.c mt19937-64.c)
target_link_libraries(img-test swap -lpthread -llz4 ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})

add_executable(img-fsck img-fsck.c)
target_link_libraries(img-fsck swap -lpthread -llz4 ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})

add_executable(img-heat img-heat.c mt19937-64.c)
target_link_libraries(img-heat swap -lpthread -llz4 ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})

install(TARGETS mindcastle img-fsck img-test DESTINATION bin)

if (BUILD_PYKV)
    add_library(kv
        kv.c
        aio.c
        ioh.c
        tinyuuid.c
        block-swap/crypto.c
        block-swap/dubtree.c
        block-swap/hashtable.c
        block-swap/simpletree.c
        )

    find_package(PythonInterp 3.0 REQUIRED)
    find_package(PythonLibs 3.0 REQUIRED)
    add_library(pykv SHARED pykv.c)
    set_target_properties(
        pykv
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME pykv
            LINKER_LANGUAGE C
    )
    target_include_directories(pykv PUBLIC
        ${PYTHON_INCLUDE_DIRS}
        ${NUMPY_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/headers
        )
    target_link_libraries(pykv
        ${PYTHON_LIBRARIES}
        kv -lpthread -llz4 -luuid ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES})
endif()
