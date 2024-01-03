# CMakeの最小バージョン
cmake_minimum_required (VERSION 3.23.2)

# プロジェクト名
project (Mebius VERSION 0.0.1)

# 使用する言語
enable_language(CXX)
enable_language(C)
enable_language(ASM_MASM)

# コンパイルするファイルを変数に格納
file (GLOB src src/*.cpp src/*.c)
file (GLOB asm src/asm/*.asm)
file (GLOB include include/*.hpp include/*.h)

# 共有ライブラリを作成
add_library (
	${PROJECT_NAME} SHARED
	${src}
	${include}
	${asm}
)

# エイリアスターゲットを定義
add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# インクルードディレクトリを指定
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories (
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/share>
	$<INSTALL_INTERFACE:include>
)

# インストールを指定
install (
	TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}-config
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)
install (
	EXPORT ${PROJECT_NAME}-config
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION cmake
)
install (
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/share/
	DESTINATION include
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file (
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	COMPATIBILITY SameMajorVersion
)
install (
	FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
	DESTINATION cmake
)

# コンパイルオプションを指定
target_compile_features (${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions (
	${PROJECT_NAME}
	PRIVATE
	MEBIUS_EXPORT
	UNICODE
	_UNICODE
	$<$<CONFIG:Debug>: 
		_DEBUG
		DEBUG
	>
	$<$<CONFIG:Release>:
		NDEBUG
	>
)
target_compile_options (
	${PROJECT_NAME}
	PRIVATE
	/W4
	/arch:AVX
	/fp:fast
	/GS
	/GT
	/MP
	/nologo
	/Oi
	/Qpar
	/utf-8
	$<$<CONFIG:Debug>:
		/JMC
		/LDd
		/Ob0
		/Od
		/RTC1
	>
	$<$<CONFIG:Release>:
		/GA
		/GL
		/Gy
		/guard:cf-
		/LD
		/Os
	>
)

# デバッグフォルダにコンパイルの産物をコピー
set(DEBUG_DIR "${CMAKE_SOURCE_DIR}/debug")
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEBUG_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" ${DEBUG_DIR}
)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" ${DEBUG_DIR}
)
ELSE(CMAKE_BUILD_TYPE MATCHES Release)
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f "${DEBUG_DIR}/${PROJECT_NAME}.pdb"
)
ENDIF()

## PreLoader
# コンパイルに追加
add_subdirectory(preloader/DINPUT)

## Zydis
# コンパイルして静的リンク
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
add_subdirectory("deps/zydis" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE Zydis)

## Lua
# コンパイルして静的リンク
set(lua_path "deps/lua")
file(GLOB lua_src ${lua_path}/*.c)
list(REMOVE_ITEM lua_src
    ${lua_path}/lua.c
    ${lua_path}/luac.c
)
add_library(lua_lib STATIC ${lua_src})
target_link_libraries(${PROJECT_NAME} PRIVATE lua_lib)

# パブリックなインクルードディレクトリにluaを追加
target_include_directories (
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/lua>
	$<INSTALL_INTERFACE:include/lua>
)

# インストール
install (
	DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/
	DESTINATION include/lua
	FILES_MATCHING PATTERN "*.h"
	PATTERN "manual" EXCLUDE
	PATTERN "testes" EXCLUDE
)

## Luaaa
# パブリックなインクルードディレクトリにluaaaを追加
target_include_directories (
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/luaaa>
	$<INSTALL_INTERFACE:include/luaaa>
)
# インストール
install (
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/deps/luaaa/luaaa.hpp
	DESTINATION include/luaaa
)

## toml++
# プライベートなインクルードディレクトリにtoml++を追加
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/deps/tomlplusplus/include/toml++
)

## cpr
# コンパイルして静的リンク
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("deps/cpr" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/cpr/include/cpr>
)

## json
# コンパイルして静的リンク
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("deps/json" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/json/include/nlohmann>
)
