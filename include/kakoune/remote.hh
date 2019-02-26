#ifndef remote_hh_INCLUDED
#define remote_hh_INCLUDED

#include "color.hh"
#include "display_buffer.hh"
#include "env_vars.hh"
#include "exception.hh"
#include "keys.hh"
#include "memory.hh"
#include "optional.hh"
#include "utils.hh"
#include "value.hh"
#include "vector.hh"

#include <memory>

namespace Kakoune
{

struct disconnected : runtime_error
{
    using runtime_error::runtime_error;
};

class FDWatcher;
class UserInterface;

template<typename T> struct Optional;
struct BufferCoord;

using RemoteBuffer = Vector<char, MemoryDomain::Remote>;


enum class MessageType : uint8_t
{
    Unknown,
    Connect,
    Command,
    MenuShow,
    MenuSelect,
    MenuHide,
    InfoShow,
    InfoHide,
    Draw,
    DrawStatus,
    SetCursor,
    Refresh,
    SetOptions,
    Exit,
    Key,
};


class MsgWriter
{
public:
    MsgWriter(RemoteBuffer& buffer, MessageType type);

    ~MsgWriter();

    void write(const char* val, size_t size);

    template<typename T>
    void write(const T& val);
    void write(StringView str);
    void write(const String& str);

    template<typename T>
    void write(ConstArrayView<T> view);

    template<typename T, MemoryDomain domain>
    void write(const Vector<T, domain>& vec);
    template<typename Key, typename Val, MemoryDomain domain>
    void write(const HashMap<Key, Val, domain>& map);
    template<typename T>
    void write(const Optional<T>& val);
    void write(Color color);
    void write(const DisplayAtom& atom);
    void write(const DisplayLine& line);
    void write(const DisplayBuffer& display_buffer);

private:
    RemoteBuffer& m_buffer;
    uint32_t m_start;
};



class MsgReader
{
public:
    void read_available(int sock);
    bool ready() const;
    uint32_t size() const;
    MessageType type() const;

    void read(char* buffer, size_t size);

    template<typename T>
    T read();


    template<typename T>
    Vector<T> read_vector();
    template<typename Key, typename Val, MemoryDomain domain>
    HashMap<Key, Val, domain> read_hash_map();

    template<typename T>
    Optional<T> read_optional();

    void reset();

private:
    void read_from_socket(int sock, size_t size);

    static constexpr uint32_t header_size = sizeof(MessageType) + sizeof(uint32_t);
    Vector<char, MemoryDomain::Remote> m_stream;
    uint32_t m_write_pos = 0;
    uint32_t m_read_pos = header_size;
};

// A remote client handle communication between a client running on the server
// and a user interface running on the local process.
class RemoteClient
{
public:
    RemoteClient(StringView session, StringView name, std::unique_ptr<UserInterface>&& ui,
                 int pid, const EnvVarMap& env_vars, StringView init_command,
                 Optional<BufferCoord> init_coord);

    bool is_ui_ok() const;
    const Optional<int>& exit_status() const { return m_exit_status; }
private:
    std::unique_ptr<UserInterface> m_ui;
    std::unique_ptr<FDWatcher>     m_socket_watcher;
    RemoteBuffer                   m_send_buffer;
    Optional<int>                  m_exit_status;
};

void send_command(StringView session, StringView command);
String get_user_name();

struct Server : public Singleton<Server>
{
    Server(String session_name);
    ~Server();
    const String& session() const { return m_session; }

    bool rename_session(StringView name);
    void close_session(bool do_unlink = true);

    bool negotiating() const { return not m_accepters.empty(); }

private:
    class Accepter;
    void remove_accepter(Accepter* accepter);

    String m_session;
    std::unique_ptr<FDWatcher> m_listener;
    Vector<std::unique_ptr<Accepter>, MemoryDomain::Remote> m_accepters;
};

bool check_session(StringView session);

}

#endif // remote_hh_INCLUDED
