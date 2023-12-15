cmake_minimum_required (VERSION 3.15)

project (Mebius VERSION 0.1.0 LANGUAGES CXX)

file (GLOB src src/*.cpp src/*.c)
file (GLOB include include/*.hpp include/*.h)
file (GLOB include include/*.def)

add_library (
	${PROJECT_NAME} SHARED
	${src}
	${include}
)
add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_subdirectory(preloader/DINPUT)

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
	
target_compile_features (${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions (
	${PROJECT_NAME}
	PRIVATE
	MEBIUS_EXPORT
	WIN32
	_WINDOWS
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

IF (CMAKE_CXX_FLAGS MATCHES "/GR")
	STRING (REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ELSE (CMAKE_CXX_FLAGS MATCHES "/GR")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
ENDIF (CMAKE_CXX_FLAGS MATCHES "/GR")

target_link_options (
	${PROJECT_NAME}
	PRIVATE
	/LTCG
	/nologo
	$<$<CONFIG:Debug>:
		/nodefaultlib:libucrtd.lib
		/debug
		/opt:noref
	>
	$<$<CONFIG:Release>:
		/nodefaultlib:libucrt.lib
		/opt:ref
	>
)

target_link_libraries (
	${PROJECT_NAME}
	PRIVATE
	$<$<CONFIG:Debug>:
		ucrtd.lib
	>
	$<$<CONFIG:Release>:
		ucrt.lib
	>
)

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
