find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

# Translation howto:
# - Translations are mentioned in the LANGUAGES variable below
# - To update the .ts files, cd into translations and run 'ninja translations'
# - The .qm files are created as part of a regular make command

# mailadler: Nur DE, EN, IT für den Anfang
set(LANGUAGES
    de
    en
    it
)

list(TRANSFORM LANGUAGES PREPEND "mailadler_" OUTPUT_VARIABLE TS_FILES)
list(TRANSFORM TS_FILES APPEND ".qm" OUTPUT_VARIABLE QM_FILES)
list(TRANSFORM TS_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
list(TRANSFORM TS_FILES APPEND ".ts")

# lupdate ts files
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*")
qt_add_lupdate(mailadler TS_FILES ${TS_FILES} SOURCES ${SOURCES}
    OPTIONS -locations absolute -no-obsolete -recursive)
add_custom_target(translations DEPENDS mailadler_lupdate)

# generate qm files with lrelease
qt6_add_translation(QM_FILES ${TS_FILES})
add_custom_target(qm ALL DEPENDS ${QM_FILES})

if(UNIX AND NOT APPLE)
  include(GNUInstallDirs)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/mailadler/translations
    FILES_MATCHING PATTERN "*.qm" PATTERN CMakeFiles EXCLUDE)
endif()
