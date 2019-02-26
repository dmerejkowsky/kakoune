#include <gtest/gtest.h>

#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <kakoune/display_buffer.hh>
#include <kakoune/env_vars.hh>
#include <kakoune/event_manager.hh>
#include <kakoune/exception.hh>
#include <kakoune/face.hh>
#include <kakoune/remote.hh>
#include <kakoune/string.hh>
#include <kakoune/user_interface.hh>


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

// FIXME: this segfaults and leave an open socket
TEST(Remote, DISABLED_CreateClientAndServer)  {
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

TEST(Remote, ReadAndWrite) {
    try {
        unlink("test.sock"); // make sure the test socket does not exist
        int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        ASSERT_GT(server_sock, 0) <<  "could not create listen socket";
        sockaddr_un saun;
        saun.sun_family = AF_UNIX;
        strcpy(saun.sun_path, "test.sock");

        int len = sizeof(saun.sun_family) + ::strlen(saun.sun_path);
        int bind_rc = bind(server_sock, (sockaddr *)&saun, len);
        ASSERT_EQ(bind_rc, 0) << "could not bind server socket: " << strerror(errno);

        int listen_rc = listen(server_sock, 5);
        ASSERT_EQ(listen_rc, 0) << "could not listen to socket" << strerror(errno);

        int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        ASSERT_GT(client_sock, 0) << "could not create client socket" << strerror(errno);

        int connect_rc = connect(client_sock, (sockaddr*)&saun, len);
        ASSERT_EQ(connect_rc, 0) << "could not connect to socket" << strerror(errno);

        unsigned int fromlen;
        sockaddr fsaun;
        int accepted_sock = accept(server_sock, &fsaun, &fromlen);
        ASSERT_GT(accepted_sock, 0) << "could not accept connection" << strerror(errno);

        const auto red = str_to_color("red");
        RemoteBuffer buffer;
        MsgWriter writer(buffer, MessageType::Command);
        writer.write(red);
        bool send_ok = send_data(accepted_sock, buffer);
        ASSERT_TRUE(send_ok);

        MsgReader reader;
        reader.read_available(client_sock);
        const auto actual = reader.read<Color>();
        EXPECT_EQ(actual, red);

        close(accepted_sock);
        close(client_sock);
        close(server_sock);
    } catch (Kakoune::exception &e) {
       std::cerr << "caught: " << e.what().data() << std::endl;
       FAIL();
    }

}
