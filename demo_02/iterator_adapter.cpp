#include "iterator_adapter.h"

namespace MSTL {
	namespace iterator {
		struct input_iterator_tag {};
		struct output_iterator_tag {};
		struct forward_iterator_tag : public input_iterator_tag {};
		struct bidirectional_iterator_tag : public forward_iterator_tag {};
		struct random_access_iterator_tag : public bidirectional_iterator_tag {};
	}
	struct __true_type {};
	struct __false_type {};
}
