all: parser parse-tree interpreter

parser: MITScriptLexer.cpp MITScriptParser.cpp parser.cpp
	g++ -std=c++20 -Wno-attributes -Iinclude MITScript.cpp parser.cpp lib/libantlr4-runtime.a -o mitscript-parser

parse-tree: MITScriptLexer.cpp MITScriptParser.cpp parse-tree.cpp
	g++ -std=c++20 -Wno-attributes -Iinclude MITScript.cpp parse-tree.cpp lib/libantlr4-runtime.a -o mitscript-parse-tree

parser-unique-ptr: MITScriptLexer.cpp MITScriptParser.cpp parser-unique-ptr.cpp
	g++ -std=c++20 -Wno-attributes -Iinclude MITScript.cpp parser-unique-ptr.cpp lib/libantlr4-runtime.a -o mitscript-parser-unique-ptr

interpreter: MITScriptLexer.cpp MITScriptParser.cpp interpreter.cpp
	g++ -std=c++20 -Wno-attributes -Iinclude MITScript.cpp interpreter.cpp lib/libantlr4-runtime.a -o mitscript


MITScriptLexer.cpp MITScriptParser.cpp: MITScript.g.intermediate
	@# Empty recipe

.INTERMEDIATE: MITScript.g.intermediate

MITScript.g.intermediate: MITScript.g
	java -Xmx500M -cp "bin/antlr-4.7.2-complete.jar:${CLASSPATH}" org.antlr.v4.Tool -Dlanguage=Cpp MITScript.g
	sed -i 's/u8//g' MITScript.cpp

clean:
	rm -f MITScript.h MITScript.cpp mitscript-parser mitscript-parse-tree mitscript MITScript.interp  MITScript.tokens
