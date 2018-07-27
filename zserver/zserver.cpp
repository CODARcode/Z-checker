#include "zserver.h"
#include <mutex>
#include <fstream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

static std::mutex mutex;
static server wss;
static std::thread *thread;
static std::map<std::string, std::string> map;

static void on_http(server *s, websocketpp::connection_hdl hdl) 
{
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  con->append_header("Access-Control-Allow-Origin", "*");
  con->append_header("Access-Control-Allow-Headers", "*");
  
  std::string query = con->get_resource();

  bool found = false;

  if (query.find("/get?") == 0) {
    const std::string key = query.substr(query.find("?") + 1);

    if (map.find(key) != map.end()) { // found
      mutex.lock();
      std::ifstream ifs(map[key]);
      mutex.unlock();
     
      std::stringstream buffer;
      buffer << ifs.rdbuf();
      ifs.close();

      con->set_body(buffer.str());
      con->set_status(websocketpp::http::status_code::ok);
      found = true;
    } 
  }
  
  if (!found) {
    std::string response = "<html><body>404 not found</body></html>";
    con->set_body(response);
    con->set_status(websocketpp::http::status_code::not_found);
  }
}

static void start_server_thread(int port)
{
  using websocketpp::lib::placeholders::_1;
  using websocketpp::lib::placeholders::_2;
  using websocketpp::lib::bind;
  
  try {
    // Set logging settings
    wss.set_access_channels(websocketpp::log::alevel::all);
    wss.clear_access_channels(websocketpp::log::alevel::frame_payload);
    wss.set_open_handshake_timeout(0); // disable timer

    // Initialize Asio
    wss.init_asio();

    // Register our message handler
    // wss.set_message_handler(bind(&onMessage, &wss, _1, _2));
    wss.set_http_handler(bind(&on_http, &wss, _1));

    // Listen on port 9002
    wss.listen(port);

    // Start the server accept loop
    wss.start_accept();
    
    // Start the ASIO io_service run loop
    wss.run();
  } catch (websocketpp::exception const & e) {
    std::cerr << e.what() << std::endl;
    // exit(EXIT_FAILURE);
  } catch (...) {
    std::cerr << "[wss] other exception" << std::endl;
    // exit(EXIT_FAILURE);
  }
}


///////////////////////////////////
extern "C" {

void zserver_start(int port) 
{
  thread = new std::thread(start_server_thread, port);
}

void zserver_stop()
{
  wss.stop();
  thread->join();
  delete thread;
  // TODO
}

void zserver_commit_file(const char *key, const char *filename)
{
  // fprintf(stderr, "new file committed: key=%s, filename=%s\n", key, filename);

  mutex.lock();
  map[key] = filename;
  mutex.unlock();
}

} // extern "C"
