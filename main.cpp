#include <cmath>
#include <iostream>
#include <memory>

namespace Fncs {
template <typename T>
struct Neg {
  T operator()(const T &x) const { return -x; };
};
template <typename T>
struct Exp {
  T operator()(const T &x) const { return exp(x); };
};
template <typename T>
struct Log {
  T operator()(const T &x) const { return log(x); };
};
template <typename T>
struct Sin {
  T operator()(const T &x) const { return sin(x); };
};
template <typename T>
struct Cos {
  T operator()(const T &x) const { return cos(x); };
};
}

struct Expression;

using ExpressionPtr = std::shared_ptr<Expression>;

struct Expression {
  virtual double eval() const = 0;
  virtual ExpressionPtr deriv(const std::string &s) const = 0;
  virtual ~Expression(){};
  virtual std::string to_string() const = 0;
};

using ExpressionPtr = std::shared_ptr<Expression>;

struct Numeric : public Expression {
  Numeric(double v) : value(v){};
  double value;
  virtual double eval() const { return value; }
  virtual ExpressionPtr deriv(const std::string &s) const { return 0; }
  virtual std::string to_string() const { return std::to_string(value); }
};

ExpressionPtr numeric(double x) { return std::make_shared<Numeric>(x); }

struct Variable : public Expression {
  Variable(const std::string &v) : variable(v){};
  std::string variable;
  virtual double eval() const {
    throw std::runtime_error("Error trying to evaluate a variable.");
  }
  virtual ExpressionPtr deriv(const std::string &s) const {
    if (s == variable)
      return numeric(1);
    else
      return numeric(0);
  }
  virtual std::string to_string() const { return variable; }
};

ExpressionPtr variable(const std::string &s) {
  return std::make_shared<Variable>(s);
}

template <typename Fnc>
struct UnaryOperation : public Expression {
 private:
  ExpressionPtr operand;

 public:
  UnaryOperation(ExpressionPtr operand_) : operand(operand_){};
  virtual double eval() const { return Fnc()(operand->eval()); }
  virtual ExpressionPtr deriv(const std::string &s) const;
  virtual std::string to_string() const;
};

using Neg = UnaryOperation<Fncs::Neg<double>>;
using Exp = UnaryOperation<Fncs::Exp<double>>;
using Log = UnaryOperation<Fncs::Log<double>>;
using Sin = UnaryOperation<Fncs::Sin<double>>;
using Cos = UnaryOperation<Fncs::Cos<double>>;

template <>
std::string Neg::to_string() const {
  return "-" + operand->to_string();
}
template <>
std::string Exp::to_string() const {
  return "exp(" + operand->to_string() + ")";
}
template <>
std::string Log::to_string() const {
  return "log(" + operand->to_string() + ")";
}
template <>
std::string Sin::to_string() const {
  return "sin(" + operand->to_string() + ")";
}
template <>
std::string Cos::to_string() const {
  return "cos(" + operand->to_string() + ")";
}

ExpressionPtr neg(ExpressionPtr x) { return std::make_shared<Neg>(x); }
ExpressionPtr exp(ExpressionPtr x) { return std::make_shared<Exp>(x); }
ExpressionPtr log(ExpressionPtr x) { return std::make_shared<Log>(x); }
ExpressionPtr sin(ExpressionPtr x) { return std::make_shared<Sin>(x); }
ExpressionPtr cos(ExpressionPtr x) { return std::make_shared<Cos>(x); }

template <typename Fnc>
struct BinaryOperation : public Expression {
 private:
  std::pair<ExpressionPtr, ExpressionPtr> operands;

 public:
  BinaryOperation(ExpressionPtr fst, ExpressionPtr snd) : operands(fst, snd){};
  virtual double eval() const {
    return Fnc()(operands.first->eval(), operands.second->eval());
  }

  virtual ExpressionPtr deriv(const std::string &s) const {
    throw std::runtime_error("bar");
  }
  virtual std::string to_string() const;
};

using Sum = BinaryOperation<std::plus<double>>;
using Difference = BinaryOperation<std::minus<double>>;
using Product = BinaryOperation<std::multiplies<double>>;
using Division = BinaryOperation<std::divides<double>>;

template <>
std::string Sum::to_string() const {
  return "(" + operands.first->to_string() + " + " +
         operands.second->to_string() + ")";
}
template <>
std::string Difference::to_string() const {
  return "(" + operands.first->to_string() + " - " +
         operands.second->to_string() + ")";
}
template <>
std::string Product::to_string() const {
  return "(" + operands.first->to_string() + " * " +
         operands.second->to_string() + ")";
}
template <>
std::string Division::to_string() const {
  return "(" + operands.first->to_string() + " / " +
         operands.second->to_string() + ")";
}

ExpressionPtr sum(ExpressionPtr a, ExpressionPtr b) {
  return std::make_shared<Sum>(a, b);
}
ExpressionPtr difference(ExpressionPtr a, ExpressionPtr b) {
  return std::make_shared<Difference>(a, b);
}
ExpressionPtr product(ExpressionPtr a, ExpressionPtr b) {
  return std::make_shared<Product>(a, b);
}
ExpressionPtr division(ExpressionPtr a, ExpressionPtr b) {
  return std::make_shared<Division>(a, b);
}

template <>
ExpressionPtr Neg::deriv(const std::string &s) const {
  return neg(operand->deriv(s));
}

template <>
ExpressionPtr Exp::deriv(const std::string &s) const {
  return sum(exp(operand), operand->deriv(s));
}

template <>
ExpressionPtr Log::deriv(const std::string &s) const {
  return division(operand->deriv(s), operand);
}

template <>
ExpressionPtr Sin::deriv(const std::string &s) const {
  return product(operand->deriv(s), cos(operand));
}

template <>
ExpressionPtr Cos::deriv(const std::string &s) const {
  return product(operand->deriv(s), neg(sin(operand)));
}

std::string to_string(const Numeric &x) { return to_string(x.value); }

ExpressionPtr operator+(const ExpressionPtr &a, const ExpressionPtr &b) {
  return sum(a, b);
}
ExpressionPtr operator*(const ExpressionPtr &a, const ExpressionPtr &b) {
  return product(a, b);
}
ExpressionPtr operator-(const ExpressionPtr &a, const ExpressionPtr &b) {
  return difference(a, b);
}
ExpressionPtr operator/(const ExpressionPtr &a, const ExpressionPtr &b) {
  return division(a, b);
}

int main(int argc, char **argv) {
  try {
    using namespace std;
    auto foo_a = numeric(2.0);
    auto foo_b = numeric(3.0);
    auto expr3 = log(difference(
        product(foo_a, sum(foo_a, sum(division(sin(foo_b), foo_a), foo_b))),
        foo_b));
    auto expr4 = log(variable("foo"));
    auto expr5 = expr4->deriv("a");
    auto expr6 = expr4->deriv("foo");
    cout << "expr3 = " << expr3->to_string()
         << " expr3.eval() = " << expr3->eval() << endl;
    cout << "expr4 = " << expr4->to_string() << endl;
    cout << "expr5 = " << expr5->to_string() << endl;
    cout << "expr6 = " << expr6->to_string() << endl;
    cout << "expr3 + expr4 + expr5 + expr6 = "
         << (expr3 + expr4 + expr5 + expr6)->to_string() << endl;
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    throw e;
  }
  return EXIT_SUCCESS;
}
