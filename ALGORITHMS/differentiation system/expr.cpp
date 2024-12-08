#include "expr.h"

namespace sym 
{
	Expr::Expr(const Const& c) {expr_ = new Const(c);}
	Expr::Expr(const Var& v) {expr_ = new Var(v); }
	Expr::Expr(const __expr_t* e) {
	    expr_ = e;
	}
	Expr::Expr(const __expr_t& e) {
	    expr_ = &e;

	}
	Expr::Expr(const Expr& e) {expr_ = e.expr_; }
		
	Expr::~Expr() {delete expr_;}

	__expr_t* Expr::eval(const var_map_t& var_map) const { 
	    if (!expr_) return nullptr;
        return expr_->eval(var_map);
	}
	__expr_t* Expr::diff(const std::string& v) const {
	    if (!expr_) return nullptr;
        return expr_->diff(v);
	}
	std::ostream& Expr::operator<< (std::ostream &out) const {
	    if (expr_) return expr_->operator<<(out);
        else return out;
	}
	bool Expr::operator==(const Expr& other) const {
	    if (!expr_ && !other.expr_) return true;
        if (!expr_ || !other.expr_) return false;
        return (*expr_ == *(other.expr_));
	}
	bool Expr::operator==(const __expr_t& other) const {
	    if(!expr_)
	    return *expr_ == other;
	}
}                                                                  	
