cmake_minimum_required(VERSION 3.10)
project(gazebo_flying_camera_plugin CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(gazebo REQUIRED)

# ---- Build targets ----------------------------------------------------------
add_library(FlyingCameraFromPoses SHARED src/FlyingCameraFromPoses.cc)
target_link_libraries(FlyingCameraFromPoses PRIVATE ${GAZEBO_LIBRARIES})
target_include_directories(FlyingCameraFromPoses PRIVATE ${GAZEBO_INCLUDE_DIRS})

add_library(CameraPoseCollector SHARED src/CameraPoseCollector.cc)
target_link_libraries(CameraPoseCollector PRIVATE ${GAZEBO_LIBRARIES})
target_include_directories(CameraPoseCollector PRIVATE ${GAZEBO_INCLUDE_DIRS})

# Se CameraPoseCollector precisar de símbolos de FlyingCameraFromPoses,
# descomente a linha abaixo (e REMOVA o link para "src/FlyingCameraFromPoses"):
# target_link_libraries(CameraPoseCollector PRIVATE FlyingCameraFromPoses)

# ---- Escolha do diretório de instalação ------------------------------------
# Permite override manual: -DPLUGIN_INSTALL_DIR=/caminho/para/plugins
set(PLUGIN_INSTALL_DIR "" CACHE PATH
  "Where to install the Gazebo plugin shared libraries (.so). Defaults to first entry of \$GAZEBO_PLUGIN_PATH or ~/.gazebo/plugins")

if(NOT PLUGIN_INSTALL_DIR)
  # Usa 1º diretório do GAZEBO_PLUGIN_PATH, se existir; senão ~/.gazebo/plugins
  set(_GP_PATH "$ENV{GAZEBO_PLUGIN_PATH}")
  if(_GP_PATH)
    string(REPLACE ":" ";" _GP_LIST "${_GP_PATH}")
    list(GET _GP_LIST 0 PLUGIN_INSTALL_DIR)
  else()
    set(PLUGIN_INSTALL_DIR "$ENV{HOME}/.gazebo/plugins")
  endif()
endif()

message(STATUS "Gazebo plugin install dir: ${PLUGIN_INSTALL_DIR}")

# ---- Instalação -------------------------------------------------------------
# Instala os .so diretamente no diretório escolhido (absoluto).
install(TARGETS
  FlyingCameraFromPoses
  CameraPoseCollector
  LIBRARY DESTINATION "${PLUGIN_INSTALL_DIR}"
)

# Opcional: mostra um lembrete pós-instalação
install(CODE
"message(\"\n[gazebo-flying-camera-plugin] Installed to: ${PLUGIN_INSTALL_DIR}\n\
Make sure this directory is listed in your GAZEBO_PLUGIN_PATH, e.g.:\n\
  export GAZEBO_PLUGIN_PATH=${PLUGIN_INSTALL_DIR}:\$GAZEBO_PLUGIN_PATH\n\")")

