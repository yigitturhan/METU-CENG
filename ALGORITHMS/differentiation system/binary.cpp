#include "binary.h"
#include "nullary.h"
#include <math.h>

namespace sym 
{
	bool AddOp::is_add() const {return true; }

	__expr_t* AddOp::eval(const var_map_t& vars) const { 
	    if(!lhs_ && !rhs_) return nullptr;
	    if(!lhs_) return rhs_->eval(vars);
	    if(!rhs_) return lhs_->eval(vars);
	    __expr_t* lr = lhs_->eval(vars);
        __expr_t* rr = rhs_->eval(vars);
        Const* lc = dynamic_cast<Const*>(lr);
        Const* rc = dynamic_cast<Const*>(rr);
        if (lc && rc) return new Const(lc->get_value()+rc->get_value());
        if (lc && lc->get_value() == 0) return rr;
        if (rc && rc->get_value() == 0) return lr;
        return new AddOp(lr, rr);
    }


	__expr_t* AddOp::diff(const std::string& v) const {
	    Const* lc = dynamic_cast<Const*>(lhs_->diff(v));
	    Const* rc = dynamic_cast<Const*>(rhs_->diff(v));
	    if(((lc && lc->get_value()==0) && (rc && rc->get_value()==0)) || (lc && rc && lc->get_value()+rc->get_value()==0)) return new Const(0.0);
	    if(lc && lc->get_value()==0) return (rhs_->diff(v))->eval();
	    if(rc && rc->get_value()==0) return (lhs_->diff(v))->eval();
        return new AddOp(lhs_->diff(v), rhs_->diff(v));
	}

	std::ostream& AddOp::operator<< (std::ostream &out) const {
	    if(!lhs_ && !rhs_) return out;
	    if(!lhs_) return rhs_->operator<<(out);
	    if(!rhs_) return lhs_->operator<<(out);
	    if (!lhs_->is_nullary()) {
            out << "(";
            lhs_->operator<<(out);
            out << ")";
        } 
        else lhs_->operator<<(out);
        out << " + ";
        if (!rhs_->is_nullary()) {
            out << "(";
            rhs_->operator<<(out);
            out << ")";
        } 
        else rhs_->operator<<(out);
        return out;
	}

	bool AddOp::operator==(const __expr_t& other_) const {
	    const AddOp* oa = dynamic_cast<const AddOp*>(&other_);
        if (!oa) return false;
        if(!lhs_ && !rhs_ && !(oa->lhs_) && !(oa->rhs_)) return true;
        if(!lhs_ && !rhs_ && ((oa->lhs_) || (oa->rhs_))) return false;
        if((lhs_ || rhs_) && !(oa->lhs_) && !(oa->rhs_)) return false;
        if(!(oa->lhs_) && lhs_) return false;
        if(!(oa->rhs_) && rhs_) return false;
        if((oa->lhs_) && !lhs_) return false;
        if((oa->rhs_) && !rhs_) return false;
        if(oa->rhs_ && oa->lhs_ && lhs_ && rhs_) return ((*lhs_ == *(oa->lhs_)) && (*rhs_ == *(oa->rhs_))) ||
           ((*lhs_ == *(oa->rhs_)) && (*rhs_ == *(oa->lhs_)));
        return false;
	}
}

namespace sym 
{
	bool MulOp::is_mul() const {return true; }

	__expr_t* MulOp::eval(const var_map_t& vars) const {
	    if(!lhs_ && !rhs_) return nullptr;
	    if(!lhs_) return rhs_->eval(vars);
	    if(!rhs_) return lhs_->eval(vars);
	    __expr_t* lhse = lhs_->eval(vars);
        __expr_t* rhse = rhs_->eval(vars);
        Const* lhsec = dynamic_cast<Const*>(lhse);
        Const* rhsec = dynamic_cast<Const*>(rhse);
    if (lhsec && rhsec) {
        double res = lhsec->get_value() *rhsec->get_value();
        return new Const(res);
    }
    if((rhsec && rhsec->get_value()==0) || (lhsec && lhsec->get_value()==0)) return new Const(0.0);
    if (rhsec && rhsec->get_value() == 1.0) return lhse;
    if (lhsec && lhsec->get_value() == 1.0) return rhse;
    return new MulOp(lhse, rhse);
	}

	__expr_t* MulOp::diff(const std::string& v) const {
	    Const* le = dynamic_cast< Const*>(lhs_->eval());
        Const* re = dynamic_cast< Const*>(rhs_->eval());
	    
	    __expr_t* lhsres = lhs_->diff(v);
        __expr_t* rhsres = rhs_->diff(v);
        
        Const* lad = dynamic_cast< Const*>(lhsres->eval());
        Const* rad = dynamic_cast< Const*>(rhsres->eval());
        if ((le && le->get_value()==0) || (re && re->get_value()==0)) return new Const(0.0);
        if ((lad && lad->get_value()==0) && (rad && rad->get_value()==0)) return new Const(0.0);
        if (lad && lad->get_value()==0){
            MulOp* mm = new MulOp(lhs_->eval(),rhsres->eval());
            return mm->eval();
        }  
        if (rad && rad->get_value()==0){
            MulOp* mm = new MulOp(rhs_->eval(),lhsres->eval());
            return mm->eval();
        } 
        if (lad && rad) return new Const(lad->get_value()+rad->get_value());
        __expr_t* ltimesrres = new MulOp(lhsres->eval(), rhs_->eval());
        __expr_t* rtimeslres = new MulOp(lhs_->eval(), rhsres->eval());
        if (ltimesrres->eval()==0 && rtimeslres->eval()==0) return new Const(0.0);
        if (ltimesrres->eval()==0) return rtimeslres->eval();
        if (rtimeslres->eval()==0) return ltimesrres->eval();
        
        AddOp* aa= new AddOp(ltimesrres, rtimeslres);
        return aa->eval();
	    
	}

	std::ostream& MulOp::operator<< (std::ostream &out) const {
	    if(!lhs_ && !rhs_) return out;
	    if(!lhs_) return rhs_->operator<<(out);
	    if(!rhs_) return lhs_->operator<<(out);
	    if (!lhs_->is_nullary()) {
            out << "(";
            lhs_->operator<<(out);
            out << ")";
        } 
        else lhs_->operator<<(out);
        out << " * ";
        if (!rhs_->is_nullary()) {
            out << "(";
            rhs_->operator<<(out);
            out << ")";
        }
        else rhs_->operator<<(out);
        return out;
	}

	bool MulOp::operator==(const __expr_t& other_) const {
	    const MulOp* oa = dynamic_cast<const MulOp*>(&other_);
        if (!oa) return false;
        if(!lhs_ && !rhs_ && !(oa->lhs_) && !(oa->rhs_)) return true;
        if(!lhs_ && !rhs_ && ((oa->lhs_) || (oa->rhs_))) return false;
        if((lhs_ || rhs_) && !(oa->lhs_) && !(oa->rhs_)) return false;
        if(!(oa->lhs_) && lhs_) return false;
        if(!(oa->rhs_) && rhs_) return false;
        if((oa->lhs_) && !lhs_) return false;
        if((oa->rhs_) && !rhs_) return false;
        if(oa->rhs_ && oa->lhs_ && lhs_ && rhs_) return ((*lhs_ == *(oa->lhs_)) && (*rhs_ == *(oa->rhs_))) ||
           ((*lhs_ == *(oa->rhs_)) && (*rhs_ == *(oa->lhs_)));
        return false;
	}
}
