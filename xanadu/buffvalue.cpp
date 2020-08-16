//

#include "buffvalue.hpp"

#include "constants.hpp"

// constructor

Value::Value(unsigned long long stat, int value, int position) :
value_(value),
position_(position),
stat_(stat)
{
}

int Value::get_value()
{
	return value_;
}

int Value::get_position()
{
	return position_;
}

unsigned long long Value::get_stat()
{
	return stat_;
}
