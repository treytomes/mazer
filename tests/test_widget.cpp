#include <catch2/catch_test_macros.hpp>
#include "SystemFixture.h"
#include "MockGraphicsMode.h"
#include "UI/Widget.h"

// Concrete widget that records update/draw calls.
struct TrackingWidget : public UI::Widget
{
    int update_count = 0;
    int draw_count   = 0;
    int last_draw_x  = -1;
    int last_draw_y  = -1;

    TrackingWidget(int x, int y) : Widget(x, y) {}

    void updateSelf(Uint32 /*dt*/) override { update_count++; }
    void drawSelf(CGA::GraphicsMode* /*mode*/, int abs_x, int abs_y) override
    {
        draw_count++;
        last_draw_x = abs_x;
        last_draw_y = abs_y;
    }
};

TEST_CASE("Widget: starts visible, not wanting close", "[widget]")
{
    SystemFixture sys;
    UI::Widget w(0, 0);
    REQUIRE(w.isVisible());
    REQUIRE_FALSE(w.wantsClose());
}

TEST_CASE("Widget: setVisible / isVisible", "[widget]")
{
    SystemFixture sys;
    UI::Widget w(0, 0);
    w.setVisible(false);
    REQUIRE_FALSE(w.isVisible());
    w.setVisible(true);
    REQUIRE(w.isVisible());
}

TEST_CASE("Widget: close sets wantsClose", "[widget]")
{
    SystemFixture sys;
    UI::Widget w(0, 0);
    w.close();
    REQUIRE(w.wantsClose());
}

TEST_CASE("Widget: update does not call updateSelf when invisible", "[widget]")
{
    SystemFixture sys;
    auto* tw = new TrackingWidget(0, 0);
    UI::Widget parent(0, 0);
    parent.addChild(tw);
    parent.setVisible(false);
    parent.update(16);
    // parent invisible → nothing updated
    REQUIRE(tw->update_count == 0);
}

TEST_CASE("Widget: draw does not call drawSelf when invisible", "[widget]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    auto* tw = new TrackingWidget(0, 0);
    UI::Widget parent(0, 0);
    parent.addChild(tw);
    parent.setVisible(false);
    parent.draw(&mode);
    REQUIRE(tw->draw_count == 0);
}

TEST_CASE("Widget: child draw position accumulates parent offset", "[widget]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    auto* child = new TrackingWidget(10, 20);
    UI::Widget parent(5, 3);
    parent.addChild(child);
    parent.draw(&mode, 0, 0);
    // child abs_x = 0 + 5 + 10 = 15; abs_y = 0 + 3 + 20 = 23
    REQUIRE(child->last_draw_x == 15);
    REQUIRE(child->last_draw_y == 23);
}

TEST_CASE("Widget: addChild up to MAX_CHILDREN", "[widget]")
{
    SystemFixture sys;
    UI::Widget parent(0, 0);
    for (int i = 0; i < UI::MAX_CHILDREN; i++)
        parent.addChild(new TrackingWidget(0, 0));
    // One more should be silently ignored (no crash).
    parent.addChild(new TrackingWidget(0, 0));
    // We can't query child count directly; just verify update doesn't crash.
    parent.update(1);
}

TEST_CASE("Widget: child is removed and deleted after close()", "[widget]")
{
    SystemFixture sys;
    auto* child = new TrackingWidget(0, 0);
    UI::Widget parent(0, 0);
    parent.addChild(child);
    child->close();

    parent.update(1); // should remove child

    // child has been deleted — verify parent updates without it.
    // If parent still held a dangling pointer this would crash under ASan.
    parent.update(1);
}

TEST_CASE("Widget: multiple children — only the closed one is removed", "[widget]")
{
    SystemFixture sys;
    auto* a = new TrackingWidget(0, 0);
    auto* b = new TrackingWidget(0, 0);
    UI::Widget parent(0, 0);
    parent.addChild(a);
    parent.addChild(b);

    b->close();
    parent.update(1); // removes b

    // a is still alive and gets updated.
    parent.update(1);
    REQUIRE(a->update_count == 2);
}

TEST_CASE("Widget: removeChild deletes the child", "[widget]")
{
    SystemFixture sys;
    auto* child = new TrackingWidget(0, 0);
    UI::Widget parent(0, 0);
    parent.addChild(child);
    parent.removeChild(child);
    // child pointer is now dangling — don't dereference.
    // A second update should not crash.
    parent.update(1);
}
