#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "Visitor.h"

#define Assert(cond, msg)               \
  if (!(cond)) {                        \
    std::cerr << msg << std::endl;      \
    throw SystemException("Bad stuff"); \
  }

class SystemException {
  std::string msg_;

 public:
  SystemException(const std::string &msg) : msg_(msg) {}
};

class AST_node {
 public:
  virtual void accept(Visitor &v) = 0;
};

class Program : public AST_node {};

class Expression : public AST_node {
 public:
};

class Statement : public AST_node {};

// You need to define classes that inherit from Expression and Statement
// for all the different kinds of expressions and statements in the language.
