if (DEFINED TYPRO_LIBS_ONLY)
	cmake_minimum_required(VERSION 3.4.3)
	project(TYPRO_LIBS LANGUAGES C CXX)
	set(ignoreMe "${TYPRO_LIBS_ONLY}")
	include_directories("${CMAKE_SOURCE_DIR}/../../include" "${CMAKE_SOURCE_DIR}")
	set(TUNE_ARGS "")
else()
	set(TUNE_ARGS "-march=native -mtune=native")
endif ()

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp PROPERTIES COMPILE_FLAGS "-frtti -fexceptions ${OpenMP_CXX_FLAGS} -O3 ${TUNE_ARGS}")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp PROPERTIES COMPILE_DEFINITIONS "__EMBEDDED_SOUFFLE__")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses_simple.cpp PROPERTIES COMPILE_FLAGS "-frtti -fexceptions ${OpenMP_CXX_FLAGS} -O3 ${TUNE_ARGS}")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses_simple.cpp PROPERTIES COMPILE_DEFINITIONS "__EMBEDDED_SOUFFLE__")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_equivalence.cpp PROPERTIES COMPILE_FLAGS "-frtti -fexceptions ${OpenMP_CXX_FLAGS} -O3 ${TUNE_ARGS}")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/compute_equivalence.cpp PROPERTIES COMPILE_DEFINITIONS "__EMBEDDED_SOUFFLE__")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set_source_files_properties(Typegraph.cpp PROPERTIES COMPILE_FLAGS "${OpenMP_CXX_FLAGS} -Og ${TUNE_ARGS}")
else()
	set_source_files_properties(Typegraph.cpp PROPERTIES COMPILE_FLAGS "${OpenMP_CXX_FLAGS} -O3 ${TUNE_ARGS}")
endif()

add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp
		COMMAND souffle -j 8 -g ${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp ${CMAKE_CURRENT_SOURCE_DIR}/compute_function_uses.dl && ${CMAKE_CURRENT_SOURCE_DIR}/patch_souffle_datastructures.py ${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp
		DEPENDS compute_function_uses.dl patch_souffle_datastructures.py
)

add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses_simple.cpp
		COMMAND souffle -j 8 -g ${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses_simple.cpp ${CMAKE_CURRENT_SOURCE_DIR}/compute_function_uses_simple.dl
		DEPENDS compute_function_uses_simple.dl
)

add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/compute_equivalence.cpp
		COMMAND souffle -j 8 -g ${CMAKE_CURRENT_BINARY_DIR}/compute_equivalence.cpp ${CMAKE_CURRENT_SOURCE_DIR}/compute_equivalence.dl
		DEPENDS compute_equivalence.dl
)

if (NOT DEFINED TYPRO_LIBS_ONLY)
add_llvm_component_library(LLVMTypegraph
		TGCallGraph.cpp
		Typegraph.cpp
		TypegraphSettings.cpp
		typegraph_llvm_utils.cpp
		typegraph_layering.cpp
		typegraph_function_uses.cpp
		abstract_map.h
		algorithms.h
		color_map.h
		scc_algorithms.h
		dotwriter.h
		TypegraphFormatters.h
		TGDispatcherBuilder.cpp
		TGDispatcherBuilder.h
		${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp
		${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses_simple.cpp
		${CMAKE_CURRENT_BINARY_DIR}/compute_equivalence.cpp

		ADDITIONAL_HEADER_DIRS
		${LLVM_MAIN_INCLUDE_DIR}/llvm/Typegraph
		#${LLVM_MAIN_INCLUDE_DIR}/llvm/ExecutionEngine/Orc

		#DEPENDS
		#intrinsics_gen
		)

target_link_libraries(LLVMTypegraph
		PRIVATE
		LLVMCore
		LLVMObject
		LLVMSupport
		)
target_link_options(LLVMTypegraph PRIVATE "${OpenMP_CXX_FLAGS}")
endif()

include(FetchContent)
FetchContent_Declare(json GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git GIT_TAG v3.9.1)
FetchContent_GetProperties(json)
if (NOT json_POPULATED)
	FetchContent_Populate(json)
	add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
	message("Retrieved nlohmann::json to \"${json_SOURCE_DIR}\"")
	include_directories("${json_SOURCE_DIR}/include")
endif ()

add_library(typro-instrumentation SHARED runtime/instrumentation_collect_calltargets.cpp)
target_compile_definitions(typro-instrumentation PUBLIC WITHOUT_LLVM)
target_link_options(typro-instrumentation PRIVATE "${OpenMP_CXX_FLAGS}")


set(TYPRO_RT_SOURCES runtime/enforcing_rt.cpp
		runtime/enforcing_rt.h
		runtime/enforcing_lazy.cpp
		runtime/typegraph_binary_loader.cpp
		runtime/typegraph_binary_loader.h
		runtime/AssemblyBuilder.cpp
		runtime/AssemblyBuilder.h
		runtime/ProtectedAllocator.cpp
		runtime/ProtectedAllocator.h
		Typegraph.cpp
		TGCallGraph.cpp
		abstract_map.h
		algorithms.h
		color_map.h
		scc_algorithms.h
		${CMAKE_CURRENT_BINARY_DIR}/compute_function_uses.cpp)

add_library(typro-rt SHARED ${TYPRO_RT_SOURCES})
target_compile_definitions(typro-rt PUBLIC WITHOUT_LLVM RUNTIME_CUSTOM_CLASSES)
target_link_options(typro-rt PRIVATE "${OpenMP_CXX_FLAGS}")
# for the optimized tail call in tgLazyDispatcher<>, this function must not use stack variables.
# that's true for -O2.
set_source_files_properties(runtime/enforcing_lazy.cpp PROPERTIES COMPILE_FLAGS "-O2")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
	target_link_options(typro-rt PRIVATE "${OpenMP_CXX_FLAGS}" "-s")
endif()

if (DEFINED TYPRO_LIBS_ONLY)
	add_library(typro-instrumentation-static STATIC runtime/instrumentation_collect_calltargets.cpp)
	target_compile_definitions(typro-instrumentation-static PUBLIC WITHOUT_LLVM)
	target_link_options(typro-instrumentation-static PRIVATE "${OpenMP_CXX_FLAGS}")
	set_target_properties(typro-instrumentation-static PROPERTIES POSITION_INDEPENDENT_CODE true)

	add_library(typro-rt-static STATIC ${TYPRO_RT_SOURCES})
	target_compile_definitions(typro-rt-static PUBLIC WITHOUT_LLVM)
	target_link_options(typro-rt-static PRIVATE "${OpenMP_CXX_FLAGS}")
	set_target_properties(typro-rt-static PROPERTIES POSITION_INDEPENDENT_CODE true)
endif()
