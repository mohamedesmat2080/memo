
set(SOURCE_FILES
        src/DllMain.cpp
        src/Util.cpp src/Util.h)

add_library(OutPutGS SHARED ${SOURCE_FILES})
target_link_libraries(OutPutGS GameServer memory winmm)

set_target_properties(OutPutGS
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "D:/VsroServerFiles/ServerFiles/RelWithDebInfo/"
        LIBRARY_OUTPUT_DIRECTORY "D:/VsroServerFiles/ServerFiles/RelWithDebInfo/"
        RUNTIME_OUTPUT_DIRECTORY "D:/VsroServerFiles/ServerFiles/RelWithDebInfo/"
        OUTPUT_NAME "JTGameServer"
        )
