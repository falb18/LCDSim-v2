add_library(
    lcdsim-defs
    INTERFACE "lcdsim-defs.h"
)

target_include_directories(
    lcdsim-defs
    INTERFACE ${PROJECT_SOURCE_DIR}/lcdsim-defs/
)