add_compile_options(-Wno-attributes)
include_directories(.)

# generate the grammar cpp file
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_LIST_DIR}/MITScript.cpp
        COMMAND java -Xmx500M -cp "bin/antlr-4.7.2-complete.jar:${CLASSPATH}" org.antlr.v4.Tool -Dlanguage=Cpp MITScript.g
        COMMAND sed -i 's/u8//g' MITScript.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        DEPENDS MITScript.g
)

# Grammar library
add_library(MITScriptGrammar MITScript.cpp)
target_link_libraries(MITScriptGrammar ${CMAKE_CURRENT_LIST_DIR}/lib/libantlr4-runtime.a)
target_include_directories(MITScriptGrammar PUBLIC include)

# Parser sources
add_library(ParserSources OBJECT
        src/empty.cpp # only here because the source list can't be empty.
        # TODO add your parser files
        )
target_link_libraries(ParserSources MITScriptGrammar)

# Parser targets
add_executable(mitscript-parser parser.cpp)
target_link_libraries(mitscript-parser ParserSources)

add_executable(mitscript-parse-tree parse-tree.cpp)
target_link_libraries(mitscript-parse-tree MITScriptGrammar)

add_executable(mitscript-parser-unique-ptr parser-unique-ptr.cpp)
target_link_libraries(mitscript-parser-unique-ptr MITScriptGrammar)


# Interpreter sources
add_library(InterpreterSources OBJECT
        src/empty.cpp # only here because the source list can't be empty.
        # TODO(phase2) add your interpreter files
        )

add_executable(mitscript interpreter.cpp)
target_link_libraries(mitscript InterpreterSources ParserSources)
