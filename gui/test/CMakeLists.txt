set ( SRCS main.cpp ../hexgui/hexgui.cpp ../hexgui/manager.cpp ../hexgui/board_inc.cpp ../hexgui/game_view.cpp ../hexgui/mcts_view.cpp )
set ( MOC_HDRS ../hexgui/hexgui.h ../hexgui/manager.h ../hexgui/board_inc.h  ../hexgui/game_view.h ../hexgui/mcts_view.h )
set ( RCS )
set ( UIS )

set ( BOOST_ROOT /usr/local/include/boost-1_39 )
set ( HEXGUI_ROOT ../hexgui )
set ( BOARD_MAIN ../../main )
set ( BOARD_IMPL ../../implementation )
set ( BOARD_LIB ../../lib )
set ( PATTERNS ../../patterns )
add_definitions ( -Wall )

find_package(Qt4 REQUIRED)
include ( ${QT_USE_FILE} )
qt4_add_resources ( RC_SRCS ${RCS} )
qt4_wrap_ui ( UI_HDRS ${UIS} )
qt4_wrap_cpp ( MOC_SRCS ${MOC_HDRS} )

include_directories (
  ${HEXGUI_ROOT}
  ${gogui_SOURCE_DIR}/libgogui
  ${CMAKE_BINARY_DIR}
  ${BOOST_ROOT}
  ${BOARD_MAIN}
  ${BOARD_IMPL}
  ${BOARD_LIB}
  ${PATTERNS}
)  


link_directories ( 
  ${gogui_BINARY_DIR}/libgogui
)

add_executable (test  ${SRCS} ${RC_SRCS} ${UI_HDRS} ${MOC_SRCS} )

target_link_libraries(test gogui ${QT_LIBRARIES})

