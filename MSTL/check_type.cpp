#include "check_type.h"

MSTL_BEGIN_NAMESPACE__

bool output::check_empty(const char* val) {
    return (!val) || (val[0] == 0);
}
output::output(string& sr) : is_compact_(true), sr_(sr) {}
output& output::operator()(void) { return (*this); }
output& output::compact(void) {
    this->is_compact_ = true;
    return (*this);
}

at_destruct::at_destruct(output& out, const char* str) : out_(out), str_(str) {}
at_destruct::~at_destruct(void) { out_(str_); }
void at_destruct::set_str(const char* str) { str_ = str; }

MSTL_END_NAMESPACE__
