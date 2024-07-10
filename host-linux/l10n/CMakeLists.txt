# https://gitlab.com/inkscape/inkscape/-/blob/d53ad8b/po/CMakeLists.txt

set(LOCALES "ja;zh_CN")

foreach(LOCALE IN LISTS LOCALES)
  GETTEXT_PROCESS_PO_FILES("${LOCALE}" ALL
    PO_FILES "${LOCALE}.po")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${LOCALE}.gmo"
          DESTINATION "${CMAKE_INSTALL_LOCALEDIR}/${LOCALE}/LC_MESSAGES/"
          RENAME "${PROJECT_NAME}.mo")
endforeach()
