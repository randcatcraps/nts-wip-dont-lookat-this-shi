# dialog has no cmake find module,
# and no .pc file in some distros

find_library(Dialog_LIBRARY
  NAMES dialog)
find_path(Dialog_INCLUDE_DIR
  NAMES dialog.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dialog
  FOUND_VAR Dialog_FOUND
  REQUIRED_VARS Dialog_LIBRARY Dialog_INCLUDE_DIR)

if (Dialog_FOUND)
  add_library(Dialog::Dialog UNKNOWN IMPORTED)
  set_target_properties(Dialog::Dialog PROPERTIES
    IMPORTED_LOCATION "${Dialog_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Dialog_INCLUDE_DIR}")
endif()

mark_as_advanced(
  Dialog_LIBRARY
  Dialog_INCLUDE_DIR
)
