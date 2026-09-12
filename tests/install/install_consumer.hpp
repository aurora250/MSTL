#ifndef NEXUSFORCE_TESTS_INSTALL_INSTALL_CONSUMER_HPP__
#define NEXUSFORCE_TESTS_INSTALL_INSTALL_CONSUMER_HPP__
#include <NeForce/core/async/signals.hpp>
#include <NeForce/core/reflect/reflect_macros.hpp>
#include <NeForce/core/string/string.hpp>

using namespace neforce;

enum class InstallConsumerColor {
    Red,
    Green,
    Blue
};
NEFORCE_REFLECT_ENUM(InstallConsumerColor, int)
NEFORCE_REFLECT_ENUM_VAL(InstallConsumerColor, Red)
NEFORCE_REFLECT_ENUM_VAL(InstallConsumerColor, Green)
NEFORCE_REFLECT_ENUM_VAL(InstallConsumerColor, Blue)

struct InstallConsumerOrder {
    NEFORCE_REFLECT_OBJ(InstallConsumerOrder)
    NEFORCE_REFLECT_PROP(int, id)
    NEFORCE_REFLECT_PROP(string, name)
    NEFORCE_REFLECT_PROP_ATTR(double, amount, PROP_OPTIONAL)

    int id = 0;
    string name;
    double amount = 0.0;
};

struct InstallConsumerBase {
    NEFORCE_REFLECT_OBJ(InstallConsumerBase)
    NEFORCE_REFLECT_PROP(int, base_field)

    int base_field = 42;
};

struct InstallConsumerDerived : public InstallConsumerBase {
    NEFORCE_REFLECT_OBJ(InstallConsumerDerived)
    NEFORCE_REFLECT_PROP(int, extra)

    int extra = 0;
};

struct InstallConsumerWorker {
    NEFORCE_REFLECT_OBJ(InstallConsumerWorker)
    NEFORCE_REFLECT_PROP(int, count)
    NEFORCE_REFLECT_FUNC(void, reset)
    NEFORCE_REFLECT_FUNC(int, compute, int, int)
    NEFORCE_REFLECT_FUNC(void, setCount, int)

    int count = 0;

    void reset() { count = 0; }

    int compute(const int left, const int right) { return left + right; }

    void setCount(const int value) { count = value; }
};

struct InstallConsumerEmitter {
    NEFORCE_REFLECT_OBJ(InstallConsumerEmitter)
    NEFORCE_REFLECT_PROP(int, value)
    NEFORCE_REFLECT_SIGNAL(signal<int>, onChanged)

    int value = 0;
    neforce::signal<int> onChanged;
};

#endif // NEXUSFORCE_TESTS_INSTALL_INSTALL_CONSUMER_HPP__
