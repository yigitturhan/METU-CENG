# automatic-differentiation-system

I will be using object-oriented paradigm (polymorphism, encapsulation, abstraction, and inheritance) to construct an automatic-differentiation system (auto-grad),
which takes expression trees of polynomials  and produces another expression tree representing the result of few complex operations like the gradient with respect to a variable or the evaluation under some constrains. To this end, I will be implementing given C++ classes inherited from an abstract class and overload it’s four pure methods.


1. sym::__nullary_op_t
The sym::__nullary_op_t is an abstract class directly derived from sym::__expr_t that represents nullary operators,
i.e. operators that take no arguments. It’s a baseline to inherit sym::Const and sym::Var.


1.1. sym::Const
- sym::Const::Const: (constructor) takes a double as an argument and sets it to its attribute (let’s say) variable.
- sym::Const::get value: (getter) returns the variable.
- sym::Const::operator double: (cast operator) returns the variable.
- sym::Const::eval: (overridden method) returns a pointer to a new sym::Const instance with the same variable.
- sym::Const::diff: (overridden method) returns a pointer to a new expression tree which is the derivative of this expression tree with respect to its input v.
- sym::Const::operator<<: (overridden method) writes variable to its input “output stream” and returns it.
- sym::Const::operator==: (overridden method) compares if its input (say other)
  is a sym::Const instance with the same variable as it’s variable.


1.2. sym::Var
- sym::Var::Var: (constructor) takes a std::string as an argument and sets it to its attribute (let’s say) variable.
- sym::Var::get variable: (getter) returns the variable.
- sym::Var::operator std::string: (cast operator) returns the variable.
- sym::Var::eval: (overridden method) returns a pointer to a new sym::Var with the same variable unless it’s variable is in the input “variable map”,
  otherwise it returns a constant with a value which is mapped by the variable in the input “variable map”.
- sym::Var::diff: (overridden method) returns a pointer to a new expression tree which is the derivative of this expression tree with respect to its input v,
  i.e. it’s a one-constant if variable equals v, zero-constant otherwise.
- sym::Var::operator<<: (overridden method) writes variable to its input “output stream” and returns it.
- sym::Var::operator==: (overridden method) compares if its input (say other) is a sym::Var instance with the same variable as it’s variable.


2. sym::__unary_op_t
The sym::__unary_op_t is an abstract class directly derived from sym::__expr_t that represents unary operators, i.e. operators that take one argument.
It’s a baseline to inherit sym::NegOp and sym::ExpOp.


2.1. sym::NegOp
- sym::NegOp::NegOp: (constructor) takes an expression sub-tree pointed to by a root class pointer (i.e. sym::__expr_t) as an argument and sets it to its attribute (let’s say) operand.
- sym::NegOp::eval: (overridden method) returns a pointer to a new sym::Const
  with a negative “variable” if its’ operand is evaluated as constant with “variable”.
  Otherwise, it returns a pointer to a new negation instance whose operand points to a sub-tree that is equal to one pointed to by operand.
- sym::NegOp::diff: (overridden method) returns a pointer to a new expression tree that is equal to the differentiation of the tree pointed to by operand.
  ∇(NegOp(op)) = NegOp(∇(op))
- sym::NegOp::operator<<: (overridden method) writes “-operand” to its input
  “output stream” and returns it in top-to-down fashion. Additionally, if it’s operand is not nullary, encloses it with parenthesis i.e. “-(operand)”.
- sym::NegOp::operator==: (overridden method) compares if its input argument
  (say other) is a sym::NegOp instance with a sub-tree that is (recursively) equal to the one pointed to by operand.

2.2. sym::ExpOp

- sym::ExpOp::ExpOp: (constructor) takes an expression sub-tree pointed to by a
  root class pointer (i.e. sym::__expr_t) as an argument and sets it to its attribute
  (let’s say) operand.
- sym::ExpOp::eval: (overridden method) returns a pointer to a new sym::Const
  with an exponatiation of “variable” if its’ operand is evaluated as constant with
  “variable”. Otherwise, it returns a pointer to a new exponentiation instance whose
  operand points to a sub-tree that is equal to one pointed to by operand.
- sym::ExpOp::diff: (overridden method) returns a pointer to a new expression tree
  that is equal to the differentiation of the tree pointed to by operand.
  ∇(ExpOp(op)) = MulOp(∇(op), ExpOp(op))
- sym::ExpOp::operator<<: (overridden method) writes “eˆoperand” to its input
  “output stream” and returns it in top-to-down fashion. Additionally, if it’s operand
  is not nullary, encloses it with parenthesis i.e. “eˆ(operand)”.
- sym::ExpOp::operator==: (overridden method) compares if its input argument
  (say other) is a sym::ExpOp instance with a sub-tree that is (recursively) equal to
  the one pointed to by operand.

3. sym::__binary_op_t
The sym:: binary op t is an abstract class directly derived from sym::__expr_t that
represents binary operators, i.e. operators that take two arguments. It’s a baseline to inherit sym::AddOp and sym::MulOp.

3.1. sym::AddOp


- sym::AddOp::AddOp: (constructor) takes two expression sub-trees pointed to by a
  root class pointers (i.e. sym::__expr_t) as arguments and sets them to its attributes
  (let’s say) right-hand side and left-hand side.
- sym::AddOp::eval: (overridden method)
  1- returns a pointer to a new expression tree (recursively) equal to the one-hand side
  if the other side is evaluated as zero-constant.
  2– returns a pointer to a constant with a variable summation of “value1” and “value2”
  if both attributes are evaluated as value1-constant and value2-constant respectively.
  3– returns a pointer to a new addition instance whose attributes are pointing to two
  sub-trees which are equal to ones pointed by left-hand side and right-hand
  side respectively, otherwise.
- sym::AddOp::diff: (overridden method) returns a pointer to a new expression tree
  that is equal to the differentiation of the tree pointed to by left-hand side + righthand side.
  ∇(AddOp(lhs, rhs)) = AddOp(∇(lhs), ∇(rhs))
- sym::AddOp::operator<<: (overridden method) writes “left-hand side + righthand side” to its input “output stream” and returns it in top-to-down fashion.
  Additionally, if any side is not nullary, enclose it with parenthesis e.g. “(left-hand side) + right-hand side”.
- sym::AddOp::operator==: (overridden method) compares if its input argument
  (say other) is a sym::AddOp instance with two sub-trees that are (recursively) equal
  to ones pointed to by either side reciprocally. 

3.2. sym::MulOp


- sym::MulOp::MulOp: (constructor) takes two expression sub-trees pointed to by a
  root class pointers (i.e. sym:: expr t) as arguments and sets them to its attributes
  (let’s say) right-hand side and left-hand side.
- sym::MulOp::eval: (overridden method)
  1– returns a pointer to a zero-constant if one side is evaluated as zero.
  2– returns to a new expression tree (recursively) equal to the one-hand side if the
  other side is evaluated as one-constant.
  3– returns a pointer to a constant with a variable product of “value1” and “value2”
  if both attributes are evaluated as value1-constant and a value2-constant respectively.
  4– returns a pointer to a new multiplication instance whose attributes are pointing to two sub-trees which are equal to ones pointed by left-hand side and right-hand
  side respectively, otherwise.
- sym::MulOp::diff: (overridden method) returns a pointer to a new expression tree
  that is equal to the differentiation of the tree pointed to by left-hand side times
  right-hand side.
  ∇(MulOp(lhs, rhs)) = AddOp(MulOp(∇(lhs), rhs), MulOp(lhs, ∇(rhs)))
- sym::MulOp::operator<<: (overridden method) writes “left-hand side * righthand side” to its input “output stream” and returns it in top-to-down fashion.
  Additionally, if any side is not nullary, encloses it with parenthesis e.g. “left-hand side * (right-hand side)” .
- sym::MulOp::operator==: (overridden method) compares if its input argument
  (say other) is a sym::MulOp instance with two sub-trees that are (recursively) equal
  to ones pointed to by either sides reciprocally. 

4. sym::Expr
The sym::Expr is an actual class directly derived from sym::__expr_t that represents expressions, i.e. a wrapper to contain pointers to expression tree instances.


- sym::Expr::Expr: (constructor) takes an expression tree pointed to by a root class
  pointers (i.e. sym:: expr t) as an argument and sets them to its attribute (let’s say)
  root.
- sym::Expr::eval: (overridden method) returns a pointer to a new expression tree
  (recursively) equal to one pointed to by root.
- sym::Expr::diff: (overridden method) returns a pointer to a new expression tree
  (recursively) equal to the derivative of one pointed to by root.
- sym::Expr::operator<<: (overridden method) writes “root” to the input “output
  stream” and returns it.
- sym::Expr::operator==: (overridden method) compares if its input argument (say
  other) is equal to the expression-tree pointed to by root.

5. overload.cpp
This is a separate file containing the overloading of the following operators
and methods
- exp(const sym::__expr_t &op): returns a reference to a new sym::ExpOp whose
  operand is op.
- operator-(const sym::__expr_t &op): returns a reference to a new sym::NegOp
  whose operand is op.
- operator+(const sym::__expr_t &lhs, const sym::__expr_t &rhs): returns a
  reference to a new sym::AddOp whose operands are lhs and rhs.
- operator+(const double lhs, const sym::__expr_t &rhs): returns a reference to
  a new sym::AddOp whose operands are lhs-constant and rhs.
- operator+(const sym::__expr_t &lhs, const double rhs): returns a reference to
  a new sym::AddOp whose operands are lhs and rhs-constant.
- operator*(const sym::__expr_t &lhs, const sym::__expr_t &rhs): returns a reference to a new sym::MulOp whose operands are lhs and rhs.
- operator*(const double lhs, const sym::__expr_t &rhs): returns a reference to
  a new sym::MulOp whose operands are lhs-constant and rhs.
- operator*(const sym::__expr_t &lhs, const double rhs): returns a reference to
  a new sym::MulOp whose operands are lhs and rhs-constant.



