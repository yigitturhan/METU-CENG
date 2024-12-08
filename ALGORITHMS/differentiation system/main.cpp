#include <iostream>
#include "expr.h"
#include "nullary.h"
#include "binary.h"
#include "unary.h"

int main() {
   \\example use
   sym::Const c = 2.0;
   sym::Var x = "x", y = "y", z= "z";

   sym::Expr e = (x*c) +  (y * z);
   sym::Expr de = e.diff(y);

   std::cout << "expression: " << e << std::endl;
   std::cout << "differention: " << de << std::endl;
   std::cout << "equality with " << c << ": " << (de == c ? "true" : "false") << std::endl;


	return 0;
}
