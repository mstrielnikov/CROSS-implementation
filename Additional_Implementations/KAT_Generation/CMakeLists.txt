cmake_minimum_required(VERSION 3.7)

project(CROSS C)
set(CMAKE_C_STANDARD 11)

set(CC gcc)
# set(CC clang)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -pedantic -Wuninitialized -march=native -O3 -g3")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${SANITIZE} ${TEST_FLAGS}")
message("Compilation flags:" ${CMAKE_C_FLAGS})

# default compilation picks reference codebase
if (NOT DEFINED REFERENCE)
    set(REFERENCE 1)
endif()

if (NOT DEFINED UNIFIED)
    set(UNIFIED 0)
endif()

if(UNIFIED EQUAL 1)
    message("Compilation mode: UNIFIED (Runtime Dispatch)")
else()
    if(REFERENCE EQUAL 1)
        message("Compilation mode: REFERENCE")
    else()
        message("Compilation mode: OPTIMIZED")
    endif()
endif()

#generate by default KATs with standalone keccak library
set(STANDALONE_KECCAK 1)

# selection of specialized compilation units differing between ref and opt
# implementations.
set(REFERENCE_CODE_DIR ../../Reference_Implementation)
set(OPTIMIZED_CODE_DIR ../../Optimized_Implementation)
set(COMMON_DIR ${REFERENCE_CODE_DIR})

if(UNIFIED EQUAL 1)
    set(BASE_DIR_REF ${REFERENCE_CODE_DIR})
    set(BASE_DIR_OPT ${OPTIMIZED_CODE_DIR})
    set(SPEC_HEADERS
            ${BASE_DIR_REF}/include/sha3.h
            ${BASE_DIR_REF}/include/csprng_hash.h
            ${BASE_DIR_REF}/include/restr_arith.h
            ${BASE_DIR_REF}/include/fp_arith.h
            ${BASE_DIR_OPT}/include/align.h
            ${BASE_DIR_OPT}/include/fips202x4.h
            ${BASE_DIR_OPT}/include/KeccakP-1600-times4-SnP.h
            ${BASE_DIR_OPT}/include/SIMD256-config.h
            ${BASE_DIR_OPT}/include/architecture_detect.h )
    
    # Shared sources that need to be namespaced
    set(SHARED_SOURCES
            ${COMMON_DIR}/lib/csprng_hash.c
            ${COMMON_DIR}/lib/pack_unpack.c )
            
    set(SPEC_SOURCES_REF
            ${BASE_DIR_REF}/lib/merkle.c
            ${BASE_DIR_REF}/lib/seedtree.c
            ${BASE_DIR_REF}/lib/CROSS.c
            ${SHARED_SOURCES} )
    set(SPEC_SOURCES_OPT
            ${BASE_DIR_OPT}/lib/fips202x4.c
            ${BASE_DIR_OPT}/lib/KeccakP-1600-times4-SIMD256.c
            ${BASE_DIR_OPT}/lib/merkle.c
            ${BASE_DIR_OPT}/lib/seedtree.c
            ${BASE_DIR_OPT}/lib/CROSS.c
            ${SHARED_SOURCES} )
    set(DISPATCH_SOURCES
            ../../Unified_Implementation/lib/dispatch.c )
elseif(REFERENCE EQUAL 1)
    message("Compiling portable reference code")
    set(BASE_DIR ${REFERENCE_CODE_DIR})
    set(SPEC_HEADERS
            ${BASE_DIR}/include/sha3.h
            ${BASE_DIR}/include/csprng_hash.h
            ${BASE_DIR}/include/restr_arith.h
            ${BASE_DIR}/include/fp_arith.h )
    set(SPEC_SOURCES
            ${BASE_DIR}/lib/merkle.c
            ${BASE_DIR}/lib/seedtree.c
            ${BASE_DIR}/lib/CROSS.c
    )
else()
    message("Compiling optimized AVX2 code")
    set(BASE_DIR ${OPTIMIZED_CODE_DIR})
    set(SPEC_HEADERS
            ${BASE_DIR}/include/align.h
            ${BASE_DIR}/include/fips202x4.h
            ${BASE_DIR}/include/KeccakP-1600-times4-SnP.h
            ${BASE_DIR}/include/SIMD256-config.h
            ${BASE_DIR}/include/sha3.h
            ${BASE_DIR}/include/csprng_hash.h
            ${BASE_DIR}/include/architecture_detect.h
            ${BASE_DIR}/include/restr_arith.h
            ${BASE_DIR}/include/fp_arith.h )
    set(SPEC_SOURCES
            ${BASE_DIR}/lib/fips202x4.c
            ${BASE_DIR}/lib/KeccakP-1600-times4-SIMD256.c
            ${BASE_DIR}/lib/merkle.c
            ${BASE_DIR}/lib/seedtree.c
            ${BASE_DIR}/lib/CROSS.c
    )
endif()

set(COMMON_DIR ${REFERENCE_CODE_DIR})
set(HEADERS
    ${SPEC_HEADERS}
    ${COMMON_DIR}/include/api.h
    ${COMMON_DIR}/include/CROSS.h
    ${COMMON_DIR}/include/pack_unpack.h
    ${COMMON_DIR}/include/fips202.h
    ${COMMON_DIR}/include/keccakf1600.h
    ${COMMON_DIR}/include/parameters.h
    ${COMMON_DIR}/include/seedtree.h
    ${COMMON_DIR}/include/merkle_tree.h
    ../../Unified_Implementation/include/CROSS_namespace.h
    ../../Unified_Implementation/include/cpu_features.h
    ../../Unified_Implementation/include/CROSS_dispatch.h
)

if(STANDALONE_KECCAK)
  message("Employing standalone SHA-3")
  set(KECCAK_EXTERNAL_LIB "")
  set(KECCAK_EXTERNAL_ENABLE "")
  list(APPEND FALLBACK_SOURCES ${COMMON_DIR}/lib/keccakf1600.c)
  list(APPEND FALLBACK_SOURCES ${COMMON_DIR}/lib/fips202.c)
else()
  message("Employing libkeccak")
  set(KECCAK_EXTERNAL_LIB keccak)
  set(KECCAK_EXTERNAL_ENABLE "-DSHA_3_LIBKECCAK")
endif()


if(UNIFIED EQUAL 1)
    set(SOURCES
        ${DISPATCH_SOURCES}
        ${FALLBACK_SOURCES}
        ${COMMON_DIR}/lib/keccakf1600.c
        ${COMMON_DIR}/lib/fips202.c
    )
else()
    set(SOURCES
        ${SPEC_SOURCES}
        ${FALLBACK_SOURCES}
        ${COMMON_DIR}/lib/csprng_hash.c
        ${COMMON_DIR}/lib/pack_unpack.c
        ${COMMON_DIR}/lib/keccakf1600.c
        ${COMMON_DIR}/lib/fips202.c
        ${COMMON_DIR}/lib/sign.c
    )
endif()

foreach(category RANGE 1 5 2)
    set(RSDP_VARIANTS RSDP RSDPG)
    foreach(RSDP_VARIANT ${RSDP_VARIANTS})
        set(PARAM_TARGETS SIG_SIZE BALANCED SPEED)
        foreach(optimiz_target ${PARAM_TARGETS})
             if(optimiz_target STREQUAL SPEED)
                set(OMIT_SEED_TREE "-DNO_TREES=1")
             else()
                set(OMIT_SEED_TREE "")
             endif()
             # settings for benchmarking binary
             set(TARGET_BINARY_NAME CROSS_KATgen_cat_${category}_${RSDP_VARIANT}_${optimiz_target})
             
             if(UNIFIED EQUAL 1)
                 # Create object libraries for this specific configuration
                 set(REF_OBJ_LIB ${TARGET_BINARY_NAME}_ref_objs)
                 set(OPT_OBJ_LIB ${TARGET_BINARY_NAME}_opt_objs)
                 
                 add_library(${REF_OBJ_LIB} OBJECT ${SPEC_SOURCES_REF})
                 target_compile_definitions(${REF_OBJ_LIB} PRIVATE 
                     ${OMIT_SEED_TREE} CATEGORY_${category}=1 ${optimiz_target}=1 ${RSDP_VARIANT}=1 ${KECCAK_EXTERNAL_ENABLE}
                     CROSS_NAMESPACE=ref_)
                 target_include_directories(${REF_OBJ_LIB} PRIVATE 
                     ${BASE_DIR_REF}/include ${COMMON_DIR}/include ../../Unified_Implementation/include)
                 
                 add_library(${OPT_OBJ_LIB} OBJECT ${SPEC_SOURCES_OPT})
                 target_compile_definitions(${OPT_OBJ_LIB} PRIVATE 
                     ${OMIT_SEED_TREE} CATEGORY_${category}=1 ${optimiz_target}=1 ${RSDP_VARIANT}=1 ${KECCAK_EXTERNAL_ENABLE}
                     CROSS_NAMESPACE=opt_)
                 target_include_directories(${OPT_OBJ_LIB} PRIVATE 
                     ${BASE_DIR_OPT}/include ${COMMON_DIR}/include ../../Unified_Implementation/include)
                 # Add -march=native for optimized part if needed, but architecture_detect.h handles it
                 
                 add_executable(${TARGET_BINARY_NAME} ${HEADERS} ${SOURCES}
                                    ./lib/rng.c ./lib/PQCgenKAT_sign.c
                                    $<TARGET_OBJECTS:${REF_OBJ_LIB}>
                                    $<TARGET_OBJECTS:${OPT_OBJ_LIB}>)
                 target_include_directories(${TARGET_BINARY_NAME} PRIVATE
                                            ../../Unified_Implementation/include
                                            ${BASE_DIR_REF}/include
                                            ${BASE_DIR_OPT}/include
                                            ${COMMON_DIR}/include
                                            ./include)
             else()
                 add_executable(${TARGET_BINARY_NAME} ${HEADERS} ${SOURCES}
                                    ./lib/rng.c ./lib/PQCgenKAT_sign.c)
                 target_include_directories(${TARGET_BINARY_NAME} PRIVATE
                                            ${BASE_DIR}/include
                                            ${COMMON_DIR}/include
                                            ./include)
             endif()
             
             target_link_libraries(${TARGET_BINARY_NAME} m ssl crypto ${SANITIZE} ${KECCAK_EXTERNAL_LIB})
             set_target_properties(${TARGET_BINARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./bin)
             
             target_compile_definitions(${TARGET_BINARY_NAME} PRIVATE 
                 ${OMIT_SEED_TREE} CATEGORY_${category}=1 ${optimiz_target}=1 ${RSDP_VARIANT}=1 ${KECCAK_EXTERNAL_ENABLE})
             if(UNIFIED EQUAL 1)
                # Create a dedicated test_dispatch executable for Category 1 RSDP Balanced
                if(${category} EQUAL 1 AND ${RSDP_VARIANT} STREQUAL "RSDP" AND ${optimiz_target} STREQUAL "BALANCED")
                    set(TEST_NAME test_dispatch)
                    add_executable(${TEST_NAME} ../../Unified_Implementation/lib/test_dispatch.c ./lib/rng.c ${SOURCES})
                    target_include_directories(${TEST_NAME} PRIVATE 
                         ${BASE_DIR_REF}/include
                         ${BASE_DIR_OPT}/include
                         ../../Unified_Implementation/include
                         ./include)
                    target_link_libraries(${TEST_NAME}
                         ${REF_OBJ_LIB}
                         ${OPT_OBJ_LIB}
                         m ssl crypto ${SANITIZE} ${KECCAK_EXTERNAL_LIB})
                    target_compile_definitions(${TEST_NAME} PRIVATE 
                         CATEGORY_1=1 RSDP=1 BALANCED=1 ${KECCAK_EXTERNAL_ENABLE})
                    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ./bin)
                endif()
             endif()
        endforeach(optimiz_target)
    endforeach(RSDP_VARIANT)
endforeach(category)
