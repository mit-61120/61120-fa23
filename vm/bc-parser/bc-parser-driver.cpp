
#include <iostream>

#include "bc-lexer.h"
#include "bc-parser.h"
#include "prettyprinter.h"

int main(int argc, char** argv) {
  void* scanner;
  bclex_init(&scanner);
  bcset_in(stdin, scanner);

  Function* output;
  int rvalue = bcparse(scanner, output);
  if (rvalue == 1) {
    std::cout << "Parsing failed" << std::endl;
    return 1;
  }

  PrettyPrinter printer;

  printer.print(*output, std::cout);

  return 0;
}
