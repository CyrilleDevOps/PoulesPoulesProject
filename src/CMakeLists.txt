# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
FILE(GLOB certificats_sources ${CMAKE_SOURCE_DIR}/Certificat/*.pem)
message("All sources: ${app_sources}")
message("All Certificats: ${certificats_sources}")
idf_component_register(SRCS ${app_sources}
                            EMBED_TXTFILES ${certificats_sources})


