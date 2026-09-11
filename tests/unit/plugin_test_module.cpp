#include <NeForce/plugin/plugin_entry.hpp>

using namespace neforce;

namespace {
    struct test_plugin final : iplugin {
        plugin_info info_{"neforce_test_plugin", "1.0.0", "NexusForceTestPlugin", "plugin_manager unit test fixture",
                          ""};

        NEFORCE_NODISCARD const plugin_info& get_info() const override { return info_; }

        void initialize() override { initialized = true; }

        void execute() override { executed = true; }

        void shutdown() override { shutdown_called = true; }

        bool initialized = false;
        bool executed = false;
        bool shutdown_called = false;
    };
} // namespace

NEFORCE_PLUGIN_EXPORT iplugin* create_plugin() { return new test_plugin(); }

NEFORCE_PLUGIN_EXPORT void destroy_plugin(iplugin* p) { delete p; }
