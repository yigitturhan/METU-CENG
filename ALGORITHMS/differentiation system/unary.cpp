#include "nullary.h"
#include "unary.h"
#include "binary.h"
#include <math.h>

namespace sym 
{
	bool NegOp::is_neg() const {return true; }

	__expr_t* NegOp::eval(const var_map_t& vars) const {
	    if(!operand) return nullptr;
	    __expr_t* r = operand->eval(vars);
	    Const* c = dynamic_cast<Const*>(r);
        if (c) return new Const(-1*(c->get_value()));
        else return new NegOp(r);
    }
	__expr_t* NegOp::diff(const std::string& v) const {
        if(!operand) return nullptr;
        return new NegOp(operand->diff(v));
	}

	std::ostream& NegOp::operator<< (std::ostream &out) const {
	    if(!operand) return out;
	    out << "-";
        if (operand->is_nullary()) operand->operator<<(out);
        else {
            out << "(";
            operand->operator<<(out);
            out << ")";
        }
        return out;
	}

	bool NegOp::operator==(const __expr_t& other_) const {

	    const NegOp* ot = dynamic_cast<const NegOp*>(&other_);
	    if (!ot) return false;
	    if(!operand && !ot->operand) return true;
	    if(!operand || !ot->operand) return false;
        
        return *operand == *ot->operand;
	}
}

namespace sym 
{
	bool ExpOp::is_exp() const { return true;}

	__expr_t* ExpOp::eval(const var_map_t& vars) const {
	    if(!operand) return nullptr;
	    __expr_t* op = operand->eval(vars);
        Const* cop = dynamic_cast<Const*>(op);
        if (cop) return new Const(std::exp(cop->get_value()));
        else return new ExpOp(op);
    }

	__expr_t* ExpOp::diff(const std::string& v) const {
	    if(!operand) return nullptr;
	    Const* cop = dynamic_cast<Const*>(operand->diff(v));
        return MulOp(operand->diff(v), this).eval();
	}

	std::ostream& ExpOp::operator<< (std::ostream &out) const {
	    if(!operand) return out;
	    out << "e^";
        if (operand->is_nullary()) operand->operator<<(out);
        else {
            out << "(";
            operand->operator<<(out);
            out << ")";
        }
        return out;}

	bool ExpOp::operator==(const __expr_t& other_) const {
	    
	    const ExpOp* ot = dynamic_cast<const ExpOp*>(&other_);
	    if (!ot) return false;
	    if(!operand && !ot->operand) return true;
	    if(!operand || !ot->operand) return false;
        return *operand == *ot->operand;
	}
}
