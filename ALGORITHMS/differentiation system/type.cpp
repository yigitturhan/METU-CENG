#include "type.h"

namespace sym 
{
	__nullary_op_t::~__nullary_op_t() { }

	bool __nullary_op_t::is_nullary() const {return true;}

	bool __nullary_op_t::is_con() const {if (this->is_nullary()) return this->is_con();}
	bool __nullary_op_t::is_var() const {if (this->is_nullary()) return this->is_var();}

}

namespace sym 
{
	__unary_op_t::~__unary_op_t() { }

	bool __unary_op_t::is_unary() const {return true;}

	bool __unary_op_t::is_neg() const {if (this->is_unary()) return this->is_neg();}
	bool __unary_op_t::is_exp() const {if (this->is_unary()) return this->is_exp();}

}

namespace sym 
{
	__binary_op_t::~__binary_op_t() { }

	bool __binary_op_t::is_binary() const {return true; }

	bool __binary_op_t::is_add() const { if (this->is_binary()) return this->is_add();}
	bool __binary_op_t::is_mul() const {if (this->is_binary()) return this->is_mul();}

}
