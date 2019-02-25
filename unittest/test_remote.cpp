#include <gtest/gtest.h>
#include <unistd.h>

#include "display_buffer.hh"
#include "env_vars.hh"
#include "event_manager.hh"
#include "face.hh"
#include "remote.hh"
#include "string.hh"
#include "user_interface.hh"


using namespace Kakoune;


struct TestUI : UserInterface
{
    TestUI() { set_signal_handler(SIGINT, SIG_DFL); }
    bool is_ok() const override { return true; }
    void menu_show(ConstArrayView<DisplayLine>, DisplayCoord,
                   Face, Face, MenuStyle) override {}
    void menu_select(int) override {}
    void menu_hide() override {}

    void info_show(StringView, StringView, DisplayCoord, Face, InfoStyle) override {}
    void info_hide() override {}

    void draw(const DisplayBuffer&, const Face&, const Face&) override {}
    void draw_status(const DisplayLine&, const DisplayLine&, const Face&) override {}
    DisplayCoord dimensions() override { return {24,80}; }
    void set_cursor(CursorMode, DisplayCoord) override {}
    void refresh(bool) override {}
    void set_on_key(OnKeyCallback) override {}
    void set_ui_options(const Options&) override {}
};

TEST(Remote, CreateClientAndServer)  {
    const auto session_name = "test-session"_sv;

    Server server("test-session");

    const auto client_name = "test_client"_sv;
    std::unique_ptr<UserInterface> ui = std::make_unique<TestUI>();
    const auto pid = getpid();
    const auto env_vars = get_env_vars();
    const auto init_command = ""_sv;
    const auto init_coords = Optional<BufferCoord>();

    RemoteClient client(session_name, client_name, std::move(ui), pid, env_vars, ""_sv, std::move(init_coords));
}
