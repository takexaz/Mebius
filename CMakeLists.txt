# CMakeの最小バージョン
cmake_minimum_required (VERSION 3.23.2)

# プロジェクト名
project (Mebius VERSION 0.0.7)

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

# バージョン情報をヘッダーファイルに埋め込む
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/_version.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/_version.hpp"
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
	PLOG_EXPORT 
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
set(DEBUG_DIR "${CMAKE_SOURCE_DIR}/../!MUGEN_DEBUG")
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

## Zydis
# コンパイルして静的リンク
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
add_subdirectory("deps/zydis" EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE Zydis)

## toml++
# プライベートなインクルードディレクトリにtoml++を追加
target_include_directories (
	${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/deps/tomlplusplus/include/toml++
)

## plog
# パブリックなインクルードディレクトリにplogを追加
add_subdirectory("deps/plog", EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE plog)
target_include_directories (
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/plog/include/>
	$<INSTALL_INTERFACE:include>
)
# インストール
install (
	DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/deps/plog/include/plog
	DESTINATION include
)

## その他必須ライブラリ
target_link_libraries (
	${PROJECT_NAME}
	PRIVATE
	Urlmon.lib
)