#include <cassert>
#include <iostream>

#include "MITScript.h"
#include "antlr4-runtime.h"

/**
 * Term -> INT TermPrime
 * TermPrime -> * INT TermPrime
 * TermPrime -> / INT TermPrime
 * TermPrime -> \epsilon
 * */

// should probably define this inside the ast namespace or such
template <class T>
using ptr = std::unique_ptr<T>;

class TermPrime {
 public:
  virtual std::string to_string() = 0;
};

//  Term -> INT TermPrime
class Term {
 public:
  Term(int32_t value, ptr<TermPrime> rest)
      : value_(value), rest_(std::move(rest)) {}

  int32_t value_;
  ptr<TermPrime> rest_;

  std::string to_string() {
    std::stringstream ss;
    ss << "Term(" << value_ << "," << rest_->to_string() << ")";
    return ss.str();
  }
};

// TermPrime -> * INT TermPrime
class TermPrimeMul : public TermPrime {
 public:
  TermPrimeMul(int32_t value, ptr<TermPrime> rest)
      : value_(value), rest_(std::move(rest)) {}

  std::string to_string() override {
    std::stringstream ss;
    ss << "TermPrimeMul(" << value_ << "," << rest_->to_string() << ")";

    return ss.str();
  }

  int32_t value_;
  ptr<TermPrime> rest_;
};

// TermPrime -> / INT TermPrime
class TermPrimeDiv : public TermPrime {
 public:
  TermPrimeDiv(int32_t value, ptr<TermPrime> rest)
      : value_(value), rest_(std::move(rest)) {}

  std::string to_string() override {
    std::stringstream ss;
    ss << "TermPrimeDiv(" << value_ << "," << rest_->to_string() << ")";

    return ss.str();
  }

  int32_t value_;
  ptr<TermPrime> rest_;
};

// TermPrime -> \epsilon
class TermPrimeEps : public TermPrime {
 public:
  std::string to_string() override { return "TermPrimeEps"; }
};

// Helper function for reporting errors
void reportError(antlr4::Token &token) {
  std::cout << "Unexpected Symbol (" << token.getLine() << ","
            << token.getCharPositionInLine() << "): " << token.getText()
            << "\n";
}

ptr<TermPrime> ParseTermPrime(antlr4::CommonTokenStream &tokens);

ptr<Term> ParseTerm(antlr4::CommonTokenStream &tokens) {
  // Get token at current index in buffer
  antlr4::Token *token = tokens.get(tokens.index());

  if (token->getType() == MITScript::INT) {
    int32_t value = std::stoi(token->getText());

    // Move forward in token buffer
    tokens.consume();

    auto rest = ParseTermPrime(tokens);

    if (rest == nullptr) {
      return nullptr;
    }

    return std::make_unique<Term>(value, std::move(rest));
  }

  // Report a parse error
  reportError(*token);

  return nullptr;
}

ptr<TermPrime> ParseTermPrime(antlr4::CommonTokenStream &tokens) {
  antlr4::Token *token = tokens.get(tokens.index());

  switch (token->getType()) {
    case MITScript::MUL:
    case MITScript::DIV: {
      tokens.consume();

      antlr4::Token *nextToken = tokens.get(tokens.index());

      if (nextToken->getType() == MITScript::INT) {
        int32_t value = std::stoi(nextToken->getText());

        tokens.consume();

        auto rest = ParseTermPrime(tokens);

        if (rest == nullptr) {
          return nullptr;
        }

        switch (token->getType()) {
          case MITScript::MUL:
            return std::make_unique<TermPrimeMul>(value, std::move(rest));
          case MITScript::DIV:
            return std::make_unique<TermPrimeDiv>(value, std::move(rest));
          default: {
            assert(0);
          }
        }
      }

      reportError(*nextToken);

      return nullptr;
    }
    case MITScript::EOF: {
      return std::make_unique<TermPrimeEps>();
    }
    default: {
      reportError(*token);

      return nullptr;
    }
  }

  // We shouldn't reach here
  assert(0);

  return nullptr;
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: mitscript-parse-tree <filename>\n";
    return -1;
  }

  std::ifstream file;
  file.open(argv[1]);

  if (!file.is_open()) {
    std::cout << "Failed to open file: " << argv[1] << "\n";
    return -1;
  }

  // Create lexer
  antlr4::ANTLRInputStream input(file);
  MITScript lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);

  // Load all tokens within the file to a buffer
  tokens.fill();

  // Print source as text
  std::cout << "source: " << tokens.getText() << "\n";

  // Print the tokens
  std::cout << "tokens: ";

  const auto &vocab = lexer.getVocabulary();
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (i != 0) {
      std::cout << " ";
    }

    const auto token = tokens.get(i);
    std::cout << vocab.getSymbolicName(token->getType());
  }

  std::cout << "\n";

  ptr<Term> result = ParseTerm(tokens);

  bool success = result != nullptr;

  if (success) {
    std::cout << "CST:    " << result->to_string() << "\n";
  } else {
    std::cout << "Parse Failed\n";
  }

  return (success ? 0 : 1);
}
