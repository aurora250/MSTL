#include <NeForce/core/async/io_context.hpp>
#include <NeForce/core/async/strand.hpp>
#include <NeForce/tui/component/collapsible.hpp>
#include <NeForce/tui/component/component.hpp>
#include <NeForce/tui/component/hoverable.hpp>
#include <NeForce/tui/component/resizable_split.hpp>
#include <NeForce/tui/component/scroll_view.hpp>
#include <NeForce/tui/component/slider.hpp>
#include <NeForce/tui/component/text_input.hpp>
#include <NeForce/tui/component/window.hpp>
#include <NeForce/tui/dom/element.hpp>
#include <NeForce/tui/dom/state.hpp>
#include <NeForce/tui/events.hpp>
#include <gtest/gtest.h>
using namespace neforce;
using namespace neforce::tui;
using namespace neforce::tui::components;

namespace {
    struct leaf_component : component<> {
        string label = "leaf";
        int key_calls = 0;
        int mouse_calls = 0;
        bool consume_keys = false;
        bool leaf_focusable = true;
        key_event::type last_key = key_event::type::unknown;
        int last_mouse_x = -1;

        element render() override { return element::text(label); }

        bool on_key(const key_event& e) override {
            ++key_calls;
            last_key = e.key;
            return consume_keys;
        }

        bool on_mouse(const mouse_event& e) override {
            ++mouse_calls;
            last_mouse_x = e.x;
            return true;
        }

        NEFORCE_NODISCARD bool focusable() const override { return leaf_focusable; }

        void bind(strand& s, io_context& c) {
            strand_ = &s;
            ctx_ = &c;
        }

        component_base* adopt(unique_ptr<component_base> child) {
            component_base* raw = child.get();
            add_child(move(child));
            return raw;
        }

        template <typename T>
        state<T>& make_state(T initial) {
            return this->create_state<T>(_NEFORCE move(initial));
        }
    };

    key_event make_key(const key_event::type t, const key_modifier mods = key_modifier::none) {
        key_event e;
        e.key = t;
        e.mods = mods;
        return e;
    }

    key_event make_printable(const char ch) {
        key_event e;
        e.key = key_event::type::printable;
        e.cp = codepoint(static_cast<uint32_t>(ch));
        return e;
    }

    mouse_event make_mouse(const mouse_action action) {
        mouse_event e;
        e.x = 4;
        e.y = 7;
        e.button = mouse_button::left;
        e.action = action;
        return e;
    }
} // namespace


TEST(TuiComponentBaseTest, AddChildRegistersChild) {
    leaf_component parent;
    parent.add_child(make_unique<leaf_component>());

    EXPECT_EQ(parent.child_count(), 1U);
    EXPECT_NE(parent.child_at(0), nullptr);
    EXPECT_EQ(parent.child_at(0)->parent(), &parent);
}

TEST(TuiComponentBaseTest, AddChildClaimsActiveChildWhenUnset) {
    leaf_component parent;
    parent.add_child(make_unique<leaf_component>());

    ASSERT_NE(parent.active_child(), nullptr);
    EXPECT_EQ(parent.active_child(), parent.child_at(0));
}

TEST(TuiComponentBaseTest, AddChildDoesNotStealExistingActiveChild) {
    leaf_component parent;
    parent.add_child(make_unique<leaf_component>());
    component_base* first = parent.active_child();
    ASSERT_NE(first, nullptr);

    parent.add_child(make_unique<leaf_component>());

    EXPECT_EQ(parent.active_child(), first);
    EXPECT_EQ(parent.child_count(), 2U);
}

TEST(TuiComponentBaseTest, AddChildIgnoresNullChild) {
    leaf_component parent;
    parent.add_child(nullptr);

    EXPECT_EQ(parent.child_count(), 0U);
    EXPECT_EQ(parent.active_child(), nullptr);
}

TEST(TuiComponentBaseTest, SetActiveChildOverridesSelection) {
    leaf_component parent;
    parent.add_child(make_unique<leaf_component>());
    parent.add_child(make_unique<leaf_component>());

    parent.set_active_child(parent.child_at(1));

    EXPECT_EQ(parent.active_child(), parent.child_at(1));
}

TEST(TuiComponentBaseTest, DetachAllChildrenClearsSelection) {
    leaf_component parent;
    parent.add_child(make_unique<leaf_component>());
    parent.add_child(make_unique<leaf_component>());

    parent.detach_all_children();

    EXPECT_EQ(parent.child_count(), 0U);
}


TEST(TuiSliderTest, RendersLabelAndBar) {
    int value = 5;
    slider_options opt;
    opt.label = "level";
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    auto comp = slider(opt);
    const element el = comp->render();

    ASSERT_EQ(el.kind(), element::kind::hbox);
    ASSERT_EQ(el.children().size(), 2U);
    EXPECT_EQ(el.children()[0].text(), "level ");
    EXPECT_EQ(el.children()[1].text(), "[========        ] 5");
}

TEST(TuiSliderTest, EmptyBarAtMinimum) {
    int value = 0;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    const element el = slider(opt)->render();
    EXPECT_EQ(el.children()[1].text(), "[                ] 0");
}

TEST(TuiSliderTest, FullBarAtMaximum) {
    int value = 10;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    const element el = slider(opt)->render();
    EXPECT_EQ(el.children()[1].text(), "[================] 10");
}

TEST(TuiSliderTest, NonZeroMinimumIsRespected) {
    int value = 15;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 10;
    opt.max_value = 20;

    const element el = slider(opt)->render();
    EXPECT_EQ(el.children()[1].text(), "[========        ] 15");
}

TEST(TuiSliderTest, DegenerateRangeDoesNotDivideByZero) {
    int value = 5;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 5;
    opt.max_value = 5;

    const element el = slider(opt)->render();
    EXPECT_EQ(el.children()[1].text(), "[                ] 5");
}

TEST(TuiSliderTest, NullValueRendersErrorText) {
    slider_options opt;
    opt.label = "broken";

    auto comp = slider(opt);
    const element el = comp->render();

    EXPECT_EQ(el.kind(), element::kind::text);
    EXPECT_NE(el.text().find("error"), string::npos);
}

TEST(TuiSliderTest, RightAndUpIncreaseValue) {
    int value = 5;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    auto comp = slider(opt);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::right)));
    EXPECT_EQ(value, 6);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::up)));
    EXPECT_EQ(value, 7);
}

TEST(TuiSliderTest, LeftAndDownDecreaseValue) {
    int value = 5;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    auto comp = slider(opt);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::left)));
    EXPECT_EQ(value, 4);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::down)));
    EXPECT_EQ(value, 3);
}

TEST(TuiSliderTest, IncrementIsApplied) {
    int value = 0;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 100;
    opt.increment = 25;

    auto comp = slider(opt);
    comp->on_key(make_key(key_event::type::right));
    EXPECT_EQ(value, 25);
}

TEST(TuiSliderTest, ClampsAtMaximum) {
    int value = 9;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;
    opt.increment = 5;

    auto comp = slider(opt);
    comp->on_key(make_key(key_event::type::right));
    EXPECT_EQ(value, 10);
}

TEST(TuiSliderTest, ClampsAtMinimum) {
    int value = 2;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;
    opt.increment = 5;

    auto comp = slider(opt);
    comp->on_key(make_key(key_event::type::left));
    EXPECT_EQ(value, 0);
}

TEST(TuiSliderTest, UnrelatedKeysAreNotConsumed) {
    int value = 5;
    slider_options opt;
    opt.value = &value;
    opt.min_value = 0;
    opt.max_value = 10;

    auto comp = slider(opt);
    EXPECT_FALSE(comp->on_key(make_key(key_event::type::enter)));
    EXPECT_FALSE(comp->on_key(make_printable('a')));
    EXPECT_EQ(value, 5);
}

TEST(TuiSliderTest, NullValueRejectsKeys) {
    slider_options opt;
    auto comp = slider(opt);

    EXPECT_FALSE(comp->on_key(make_key(key_event::type::right)));
}

TEST(TuiSliderTest, IsFocusable) {
    slider_options opt;
    EXPECT_TRUE(slider(opt)->focusable());
}


TEST(TuiCollapsibleTest, RendersHeaderAndChildWhenExpanded) {
    bool show = true;
    auto child = make_unique<leaf_component>();
    auto comp = collapsible("section", move(child), &show);

    const element el = comp->render();
    ASSERT_EQ(el.kind(), element::kind::vbox);
    ASSERT_EQ(el.children().size(), 2U);
    EXPECT_EQ(el.children()[0].text(), "v section");
    EXPECT_EQ(el.children()[1].text(), "leaf");
}

TEST(TuiCollapsibleTest, HidesChildWhenCollapsed) {
    bool show = false;
    auto comp = collapsible("section", make_unique<leaf_component>(), &show);

    const element el = comp->render();
    ASSERT_EQ(el.children().size(), 1U);
    EXPECT_EQ(el.children()[0].text(), "> section");
}

TEST(TuiCollapsibleTest, TogglesOnEnter) {
    bool show = true;
    auto comp = collapsible("section", make_unique<leaf_component>(), &show);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::enter)));
    EXPECT_FALSE(show);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::enter)));
    EXPECT_TRUE(show);
}

TEST(TuiCollapsibleTest, TogglesOnSpace) {
    bool show = true;
    auto comp = collapsible("section", make_unique<leaf_component>(), &show);

    EXPECT_TRUE(comp->on_key(make_printable(' ')));
    EXPECT_FALSE(show);
}

TEST(TuiCollapsibleTest, IgnoresOtherPrintableKeys) {
    bool show = true;
    auto comp = collapsible("section", make_unique<leaf_component>(), &show);

    EXPECT_FALSE(comp->on_key(make_printable('x')));
    EXPECT_TRUE(show);
}

TEST(TuiCollapsibleTest, DelegatesKeysToChildWhenExpanded) {
    bool show = true;
    auto* raw = new leaf_component();
    raw->consume_keys = true;
    auto comp = collapsible("section", unique_ptr<component_base>(raw), &show);

    EXPECT_TRUE(comp->on_key(make_printable('x')));
    EXPECT_EQ(raw->key_calls, 1);
}

TEST(TuiCollapsibleTest, DoesNotDelegateKeysWhenCollapsed) {
    bool show = false;
    auto* raw = new leaf_component();
    auto comp = collapsible("section", unique_ptr<component_base>(raw), &show);

    EXPECT_FALSE(comp->on_key(make_printable('x')));
    EXPECT_EQ(raw->key_calls, 0);
}

TEST(TuiCollapsibleTest, UsesInternalStateWithoutSharedFlag) {
    auto comp = collapsible("section", make_unique<leaf_component>(), nullptr);

    EXPECT_EQ(comp->render().children().size(), 2U);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::enter)));
    EXPECT_EQ(comp->render().children().size(), 1U);
}

TEST(TuiCollapsibleTest, IsFocusable) {
    EXPECT_TRUE(collapsible("section", make_unique<leaf_component>(), nullptr)->focusable());
}


TEST(TuiHoverableTest, RendersChildElement) {
    auto comp = hoverable(make_unique<leaf_component>(), [] {}, [] {});
    EXPECT_EQ(comp->render().text(), "leaf");
}

TEST(TuiHoverableTest, FocusabilityFollowsChild) {
    auto focusable_child = make_unique<leaf_component>();
    focusable_child->leaf_focusable = true;
    EXPECT_TRUE(hoverable(move(focusable_child), [] {}, [] {})->focusable());

    auto plain_child = make_unique<leaf_component>();
    plain_child->leaf_focusable = false;
    EXPECT_FALSE(hoverable(move(plain_child), [] {}, [] {})->focusable());
}

TEST(TuiHoverableTest, MouseMoveInvokesEnterOnce) {
    int enters = 0;
    int leaves = 0;
    auto comp = hoverable(make_unique<leaf_component>(), [&enters] { ++enters; }, [&leaves] { ++leaves; });

    comp->on_mouse(make_mouse(mouse_action::move));
    comp->on_mouse(make_mouse(mouse_action::move));

    EXPECT_EQ(enters, 1);
    EXPECT_EQ(leaves, 0);
}

TEST(TuiHoverableTest, MouseLeaveInvokesLeaveCallback) {
    int enters = 0;
    int leaves = 0;
    auto comp = hoverable(make_unique<leaf_component>(), [&enters] { ++enters; }, [&leaves] { ++leaves; });

    comp->on_mouse(make_mouse(mouse_action::move));
    comp->on_mouse_leave();

    EXPECT_EQ(enters, 1);
    EXPECT_EQ(leaves, 1);
}

TEST(TuiHoverableTest, LeaveWithoutEnterDoesNotInvokeCallback) {
    int leaves = 0;
    auto comp = hoverable(make_unique<leaf_component>(), [] {}, [&leaves] { ++leaves; });

    comp->on_mouse_leave();

    EXPECT_EQ(leaves, 0);
}

TEST(TuiHoverableTest, WritesHoveredFlag) {
    bool hovered = false;
    auto comp = hoverable(make_unique<leaf_component>(), &hovered);

    comp->on_mouse(make_mouse(mouse_action::move));
    EXPECT_TRUE(hovered);

    comp->on_mouse_leave();
    EXPECT_FALSE(hovered);
}

TEST(TuiHoverableTest, ForwardsEventsToChild) {
    auto* raw = new leaf_component();
    auto comp = hoverable(unique_ptr<component_base>(raw), [] {}, [] {});

    comp->on_key(make_key(key_event::type::enter));
    comp->on_mouse(make_mouse(mouse_action::press));

    EXPECT_EQ(raw->key_calls, 1);
    EXPECT_EQ(raw->mouse_calls, 1);
    EXPECT_EQ(raw->last_mouse_x, 4);
}


TEST(TuiResizableSplitTest, HorizontalSplitRendersHboxWithSeparator) {
    int main_size = 10;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    const element el = comp->render();
    ASSERT_EQ(el.kind(), element::kind::hbox);
    ASSERT_EQ(el.children().size(), 3U);
    EXPECT_EQ(el.children()[1].text(), "│");
}

TEST(TuiResizableSplitTest, VerticalSplitRendersVboxWithSeparator) {
    int main_size = 10;
    auto comp = resizable_split_top(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    const element el = comp->render();
    ASSERT_EQ(el.kind(), element::kind::vbox);
    EXPECT_EQ(el.children()[1].text(), "─");
}

TEST(TuiResizableSplitTest, LeftPutsMainFirst) {
    int main_size = 10;
    auto* main = new leaf_component();
    main->label = "main";
    auto* back = new leaf_component();
    back->label = "back";

    auto comp = resizable_split_left(unique_ptr<component_base>(main), unique_ptr<component_base>(back), &main_size);
    const element el = comp->render();

    EXPECT_EQ(el.children()[0].text(), "main");
    EXPECT_EQ(el.children()[2].text(), "back");
}

TEST(TuiResizableSplitTest, RightPutsMainLast) {
    int main_size = 10;
    auto* main = new leaf_component();
    main->label = "main";
    auto* back = new leaf_component();
    back->label = "back";

    auto comp = resizable_split_right(unique_ptr<component_base>(main), unique_ptr<component_base>(back), &main_size);
    const element el = comp->render();

    EXPECT_EQ(el.children()[0].text(), "back");
    EXPECT_EQ(el.children()[2].text(), "main");
}

TEST(TuiResizableSplitTest, TopPutsMainFirstAndBottomPutsMainLast) {
    int main_size = 10;
    auto* main_top = new leaf_component();
    main_top->label = "main";
    auto* back_bottom = new leaf_component();
    back_bottom->label = "back";
    auto top = resizable_split_top(unique_ptr<component_base>(main_top), unique_ptr<component_base>(back_bottom),
                                   &main_size);
    EXPECT_EQ(top->render().children()[0].text(), "main");

    auto* main_bottom = new leaf_component();
    main_bottom->label = "main2";
    auto* back_top = new leaf_component();
    back_top->label = "back2";
    auto bottom = resizable_split_bottom(unique_ptr<component_base>(main_bottom), unique_ptr<component_base>(back_top),
                                         &main_size);
    EXPECT_EQ(bottom->render().children()[2].text(), "main2");
}

TEST(TuiResizableSplitTest, TabCyclesActiveChild) {
    int main_size = 10;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    component_base* first = comp->active_child();
    ASSERT_NE(first, nullptr);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::tab)));
    EXPECT_NE(comp->active_child(), first);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::tab)));
    EXPECT_EQ(comp->active_child(), first);
}

TEST(TuiResizableSplitTest, F6CyclesActiveChild) {
    int main_size = 10;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    component_base* first = comp->active_child();
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::F6)));
    EXPECT_NE(comp->active_child(), first);
}

TEST(TuiResizableSplitTest, CtrlArrowAdjustsHorizontalSplit) {
    int main_size = 10;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::right, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 11);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::left, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 10);
}

TEST(TuiResizableSplitTest, CtrlArrowAdjustsVerticalSplit) {
    int main_size = 10;
    auto comp = resizable_split_top(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::down, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 11);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::up, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 10);
}

TEST(TuiResizableSplitTest, MainSizeNeverDropsBelowOne) {
    int main_size = 1;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::left, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 1);
}

TEST(TuiResizableSplitTest, IgnoresWrongAxisCtrlArrow) {
    int main_size = 10;
    auto comp = resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size);

    EXPECT_FALSE(comp->on_key(make_key(key_event::type::up, key_modifier::ctrl)));
    EXPECT_EQ(main_size, 10);
}

TEST(TuiResizableSplitTest, ForwardsUnhandledKeysToActiveChild) {
    int main_size = 10;
    auto* main = new leaf_component();
    auto comp = resizable_split_left(unique_ptr<component_base>(main), make_unique<leaf_component>(), &main_size);

    EXPECT_FALSE(comp->on_key(make_printable('q')));
    EXPECT_EQ(main->key_calls, 1);
}

TEST(TuiResizableSplitTest, IsFocusable) {
    int main_size = 10;
    EXPECT_TRUE(resizable_split_left(make_unique<leaf_component>(), make_unique<leaf_component>(), &main_size)
                        ->focusable());
}


TEST(TuiTextInputTest, RendersConfiguredText) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("hello");

    text_input_option opt;
    opt.text = &text;
    component_base* comp = owner.adopt(text_input(opt));

    const element el = comp->render();
    EXPECT_EQ(el.kind(), element::kind::text_input);
}

TEST(TuiTextInputTest, IsFocusable) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("");
    text_input_option opt;
    opt.text = &text;

    EXPECT_TRUE(text_input(opt)->focusable());
}

TEST(TuiTextInputTest, IgnoresKeysWithoutFocus) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("ab");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));

    EXPECT_FALSE(comp->on_key(make_key(key_event::type::backspace)));
    EXPECT_EQ(text.value(), "ab");
}

TEST(TuiTextInputTest, InsertsPrintableCharacterAtCursor) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("ab");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->take_focus();

    EXPECT_TRUE(comp->on_key(make_printable('c')));
    EXPECT_EQ(text.value(), "abc");
}

TEST(TuiTextInputTest, BackspaceDeletesPreviousCharacter) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->take_focus();

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::backspace)));
    EXPECT_EQ(text.value(), "ab");
}

TEST(TuiTextInputTest, BackspaceOnEmptyTextIsNotConsumed) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->take_focus();

    EXPECT_FALSE(comp->on_key(make_key(key_event::type::backspace)));
}

TEST(TuiTextInputTest, DeleteRemovesCharacterAtCursor) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->on_key(make_key(key_event::type::home));

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::delete_)));
    EXPECT_EQ(text.value(), "bc");
}

TEST(TuiTextInputTest, HomeAndEndMoveInsertionPoint) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->take_focus();

    comp->on_key(make_key(key_event::type::home));
    comp->on_key(make_printable('X'));
    EXPECT_EQ(text.value(), "Xabc");

    comp->on_key(make_key(key_event::type::end));
    comp->on_key(make_printable('Y'));
    EXPECT_EQ(text.value(), "XabcY");
}

TEST(TuiTextInputTest, LeftAndRightMoveCursor) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);
    comp->take_focus();

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::left)));
    comp->on_key(make_printable('Z'));
    EXPECT_EQ(text.value(), "abZc");

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::left)));
    comp->on_key(make_printable('Y'));
    EXPECT_EQ(text.value(), "abYZc");

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::right)));
    comp->on_key(make_printable('W'));
    EXPECT_EQ(text.value(), "abYZWc");
}

TEST(TuiTextInputTest, EnterInvokesCallback) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    int entered = 0;
    text_input_option opt;
    opt.text = &text;
    opt.on_enter = [&entered] { ++entered; };

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::enter)));
    EXPECT_EQ(entered, 1);
}

TEST(TuiTextInputTest, CursorBlinkTogglesAfterAccumulatedDelay) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);

    const element before = comp->render();
    comp->on_animation(600);
    const element after = comp->render();

    EXPECT_NE(before.cursor_visible(), after.cursor_visible());
}

TEST(TuiTextInputTest, UnrelatedKeysAreNotConsumed) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& text = owner.make_state<string>("abc");
    text_input_option opt;
    opt.text = &text;

    component_base* comp = owner.adopt(text_input(opt));
    comp->set_has_focus(true);

    EXPECT_FALSE(comp->on_key(make_key(key_event::type::escape)));
    EXPECT_EQ(text.value(), "abc");
}


TEST(TuiScrollViewTest, RendersContentElement) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    scroll_view_option opt;
    opt.content = [] { return element::text("scrolled"); };

    component_base* comp = owner.adopt(scroll_view(opt));

    const element el = comp->render();
    EXPECT_EQ(el.kind(), element::kind::scroll_view);
    ASSERT_EQ(el.children().size(), 1U);
    EXPECT_EQ(el.children()[0].text(), "scrolled");
}

TEST(TuiScrollViewTest, IsNotFocusable) {
    scroll_view_option opt;
    opt.content = [] { return element::text("x"); };

    EXPECT_FALSE(scroll_view(opt)->focusable());
}

TEST(TuiScrollViewTest, SharesExternalScrollState) {
    io_context ctx;
    strand s{ctx};
    leaf_component owner;
    owner.bind(s, ctx);
    auto& scroll_y = owner.make_state<int>(3);

    scroll_view_option opt;
    opt.content = [] { return element::text("x"); };
    opt.external_scroll_y = &scroll_y;

    component_base* comp = owner.adopt(scroll_view(opt));

    EXPECT_EQ(comp->render().scroll_y(), 3);
    scroll_y = 7;
    EXPECT_EQ(comp->render().scroll_y(), 7);
}


TEST(TuiWindowTest, RendersTitleBarAndInnerContent) {
    window_options opt;
    opt.title = "panel";
    opt.inner = make_unique<leaf_component>();

    auto comp = window(_NEFORCE move(opt));
    const element el = comp->render();

    ASSERT_EQ(el.kind(), element::kind::vbox);
    ASSERT_EQ(el.children().size(), 3U);
    EXPECT_EQ(el.children()[0].text(), "panel");
}

TEST(TuiWindowTest, UsesProvidedGeometry) {
    int left = 4;
    int top = 3;
    int width = 20;
    int height = 6;
    window_options opt;
    opt.title = "panel";
    opt.left = &left;
    opt.top = &top;
    opt.width = &width;
    opt.height = &height;
    opt.inner = make_unique<leaf_component>();

    auto comp = window(_NEFORCE move(opt));
    comp->render();

    EXPECT_EQ(left, 4);
    EXPECT_EQ(top, 3);
    EXPECT_EQ(width, 20);
    EXPECT_EQ(height, 6);
}

TEST(TuiWindowTest, CtrlArrowMovesWindowAndClampsAtOrigin) {
    int left = 1;
    int top = 1;
    window_options opt;
    opt.left = &left;
    opt.top = &top;
    opt.inner = make_unique<leaf_component>();

    auto comp = window(_NEFORCE move(opt));

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::right, key_modifier::ctrl)));
    EXPECT_EQ(left, 2);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::down, key_modifier::ctrl)));
    EXPECT_EQ(top, 2);

    comp->on_key(make_key(key_event::type::left, key_modifier::ctrl));
    comp->on_key(make_key(key_event::type::left, key_modifier::ctrl));
    EXPECT_EQ(left, 0);

    comp->on_key(make_key(key_event::type::up, key_modifier::ctrl));
    comp->on_key(make_key(key_event::type::up, key_modifier::ctrl));
    EXPECT_EQ(top, 0);
}

TEST(TuiWindowTest, CtrlShiftArrowResizesWithMinimumSize) {
    int width = 10;
    int height = 10;
    window_options opt;
    opt.width = &width;
    opt.height = &height;
    opt.inner = make_unique<leaf_component>();

    auto comp = window(_NEFORCE move(opt));

    EXPECT_TRUE(comp->on_key(make_key(key_event::type::right, key_modifier::ctrl | key_modifier::shift)));
    EXPECT_EQ(width, 11);
    EXPECT_TRUE(comp->on_key(make_key(key_event::type::down, key_modifier::ctrl | key_modifier::shift)));
    EXPECT_EQ(height, 11);

    for (int i = 0; i < 20; ++i) {
        comp->on_key(make_key(key_event::type::left, key_modifier::ctrl | key_modifier::shift));
        comp->on_key(make_key(key_event::type::up, key_modifier::ctrl | key_modifier::shift));
    }
    EXPECT_EQ(width, 5);
    EXPECT_EQ(height, 3);
}

TEST(TuiWindowTest, CustomRenderReceivesInnerElementAndTitle) {
    string seen_title;
    bool seen_active = true;
    window_options opt;
    opt.title = "custom";
    opt.inner = make_unique<leaf_component>();
    opt.render = [&seen_title, &seen_active](window_options::render_state state) {
        seen_title = state.title;
        seen_active = state.active;
        return state.inner;
    };

    auto comp = window(_NEFORCE move(opt));
    const element el = comp->render();

    EXPECT_EQ(seen_title, "custom");
    // focused() reports a parentless component as focused, which is how a root
    // window is presented before the reconciler takes over.
    EXPECT_TRUE(seen_active);
    EXPECT_TRUE(comp->focused());
    EXPECT_EQ(el.text(), "leaf");
}

TEST(TuiWindowTest, ForwardsEventsToInnerComponent) {
    auto* raw = new leaf_component();
    window_options opt;
    opt.inner = unique_ptr<component_base>(raw);

    auto comp = window(_NEFORCE move(opt));
    comp->on_key(make_printable('a'));
    comp->on_mouse(make_mouse(mouse_action::press));

    EXPECT_EQ(raw->key_calls, 1);
    EXPECT_EQ(raw->mouse_calls, 1);
}

TEST(TuiWindowTest, IsFocusable) {
    window_options opt;
    EXPECT_TRUE(window(window_options{})->focusable());
}

TEST(TuiWindowTest, RendersWithoutInnerComponent) {
    window_options opt;
    opt.title = "empty";
    opt.render = [](window_options::render_state state) { return state.inner; };

    auto comp = window(_NEFORCE move(opt));
    EXPECT_EQ(comp->render().kind(), element::kind::text);
    EXPECT_FALSE(comp->on_key(make_printable('a')));
    EXPECT_FALSE(comp->on_mouse(make_mouse(mouse_action::press)));
}
