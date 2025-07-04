#include "lex.h"
#include <assert.h>
#include <iostream>
#include <istream>
#include <string>

using namespace lex;

void Tokenizer::consume() { m_current_token = readOneToken(); }

Tokenizer::Tokenizer(const char *filename) : m_file(filename) {
  // FIXME: handle this better!
  if (!m_file.is_open()) {
    std::cerr << "cannot open file:" << filename << "\n";
    std::exit(-1);
  }

  m_current_token = readOneToken();
}

static bool is_valid_stoi(const std::string &str) {
  for (char c : str) {
    if (!std::isdigit(c))
      return false;
  }

  return true;
}

// FIXME: since we are stack allocating, we are
// always making a copy. This gets really expensive
// because token class is quite big, and stores a
// std::string heap allocation?
const Token Tokenizer::peek() {
  // saving the location
  std::streampos current = m_file.tellg();

  Token next_token = readOneToken();

  // restoring the location
  m_file.seekg(current);
  return next_token;
}

void Tokenizer::removeWhiteSpace() {
  if (!m_file.good())
    return;

  char c;
  while ((c = m_file.peek())) {
    if (!(c == ' ' || c == '\n')) {
      break;
    }

    // consume the thing character
    m_file.get();
  }
}

Token Tokenizer::readOneToken() {
  // put this into read one
  removeWhiteSpace();

  if (!m_file.good() && m_file.peek() == std::char_traits<char>::eof()) {
    return Token(EndOfFile);
  }

  std::string buf;
  char c;
  bool is_first_time = true;
  while (m_file.get(c)) {
    if (c == ' ' || c == '\n') {
      break;
    }

    // consume if this is the first time
    // FIXME: maybe just look up the keyword map instead?
    if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
        c == ',' || c == '+' || c == ';' || c == '=' || c == '*') {
      if (is_first_time) {
        // creating the lookup term
        std::string lookup_term;
        lookup_term += c;
        // making the lookup term
        Token return_result(keyword_map.at(lookup_term));
        return return_result;
      }

      m_file.seekg(m_file.tellg() - std::streampos(1));
      break;
    }

    is_first_time = false;
    buf += c;
  }

  // this is a valid integer?
  if (is_valid_stoi(buf)) {
    long long result = std::stoll(buf);
    Token return_result(result);
    return return_result;
  }

  // keywords function, gives, etc..
  if (isKeyword(buf)) {
    return Token(getKeyword(buf));
  }

  // it must be an identifier than
  return Token(std::move(buf));
}

bool Tokenizer::isKeyword(const std::string &keyword) {
  return keyword_map.find(keyword) != keyword_map.end();
}

const Token &Tokenizer::current() { return m_current_token; }

const Token Tokenizer::next() {
  m_current_token = readOneToken();

  return m_current_token;
}

TokenType Tokenizer::getKeyword(const std::string &keyword) {
  TokenType result = keyword_map.at(keyword);
  assert(TokenType::KeywordStart < result && TokenType::KeywordEnd > result &&
         "this is the invarient");

  return result;
}

Token::Token(long long number)
    : type(IntegerLiteral), integer_literal(number) {}

const char *tokenTypeToString(TokenType type) {
  switch (type) {
  case IntegerLiteral:
    return "IntegerLiteral";
  case Identifier:
    return "Identifier";
  case EndOfFile:
    return "EndOfFile";
  case Invalid:
    return "Invalid";
  case KeywordStart:
    return "KeywordStart";
  case LeftParentheses:
    return "LeftParentheses";
  case RightParentheses:
    return "RightParentheses";
  case LeftBrace:
    return "LeftBrace";
  case RightBrace:
    return "RightBrace";
  case LeftBracket:
    return "LeftBracket";
  case RightBracket:
    return "RightBracket";
  case Comma:
    return "Comma";
  case FunctionDecl:
    return "FunctionDecl";
  case Gives:
    return "Gives";
  case SemiColon:
    return "SemiColon";
  case Equal:
    return "Equal";
  case Ret:
    return "Ret";
  case Int:
    return "Int";
  case Add:
    return "Add";
  case KeywordEnd:
    return "KeywordEnd";
  case Num:
    return "Num";
  case Multiply:
    return "Multilpy";
  default:
    return "Unknown TokenType";
  }
}

void Token::dump() const {
  std::cout << "Token type: " << tokenTypeToString(type) << std::endl;
}

Token::Token() : type(Invalid) {}

Token::Token(TokenType type) : type(type) {
  assert((type == LeftParentheses || type == RightParentheses ||
          type == EndOfFile || (type > KeywordStart && type < KeywordEnd)) &&
         "it must be parenthesis type style or keyword!");
}

Token::Token(std::string &&string) : type(Identifier), string_literal(string) {}

const std::string &Token::getStringLiteral() const {
  assert(type == Identifier);
  return string_literal;
}

long long Token::getIntegerLiteral() const {
  assert(type == IntegerLiteral);
  return integer_literal;
}

bool Token::isBinaryOperator() const {
  return type > BinaryOperatorStart && type < BinaryOperatorEnd;
}

TokenType Token::getType() const { return type; }

TokenType Tokenizer::getNextType() { return next().getType(); }

TokenType Tokenizer::getCurrentType() { return m_current_token.getType(); }
