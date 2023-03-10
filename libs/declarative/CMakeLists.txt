include_directories(${CMAKE_SOURCE_DIR}/libs/models)

set(plasmanm_qml_plugins_SRCS
   availabledevices.cpp
   connectionicon.cpp
   enabledconnections.cpp
   enums.cpp
   networkstatus.cpp
   qmlplugins.cpp
)

add_library(plasmanm_qmlplugins SHARED ${plasmanm_qml_plugins_SRCS})

target_link_libraries(plasmanm_qmlplugins
    plasmanm_internal
    plasmanm_editor
    Qt::Quick
    KF5::I18n
    Qt5::Qml
)

if (WITH_MODEMMANAGER_SUPPORT)
    target_link_libraries(plasmanm_qmlplugins KF5::ModemManagerQt)
endif()

install(TARGETS plasmanm_qmlplugins DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/networkmanagement)
