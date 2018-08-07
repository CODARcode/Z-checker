#include "zserver.h"
#include <mutex>
#include <list>
#include <fstream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

static std::mutex mutex;
static server wss;
static std::thread *thread;
static std::map<std::string, std::string> map;

static std::map<std::string, std::list<double> > lists;

static void list2json(std::stringstream &buf, const std::list<double>& list) {
  buf << "[";
  for (auto val : list) 
    buf << val << ',';
  buf << "]";
}

static void on_http(server *s, websocketpp::connection_hdl hdl) 
{
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  con->append_header("Access-Control-Allow-Origin", "*");
  con->append_header("Access-Control-Allow-Headers", "*");
  
  std::string query = con->get_resource();

  bool succ = false;

  if (query.find("/list") == 0) {
    std::stringstream buffer;
    
    std::unique_lock<std::mutex> lock(mutex);
    for (auto &kv : map) 
      buffer << kv.first << std::endl;
    lock.unlock();

    con->set_body(buffer.str()); 
    succ = true;
  } else if (query.find("/get?") == 0) {
    const std::string key = query.substr(query.find("?") + 1);
    
    std::unique_lock<std::mutex> lock(mutex);

    if (map.find(key) != map.end()) { // found
      std::ifstream ifs(map[key]);
     
      std::stringstream buffer;
      buffer << ifs.rdbuf();
      ifs.close();

      con->set_body(buffer.str());
      succ = true;
    }
  } else if (query.find("/data?") == 0) {
    const std::string key = query.substr(query.find("?") + 1);
    std::stringstream buffer;

    std::unique_lock<std::mutex> lock(mutex);

    if (lists.find(key) != lists.end()) {
      list2json(buffer, lists[key]);
      con->set_body(buffer.str());
      succ = true;
    }
  }
 
  if (succ) {
    con->set_status(websocketpp::http::status_code::ok);
  } else {
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
  // usleep(500000); // for testing only

  // fprintf(stderr, "new file committed: key=%s, filename=%s\n", key, filename);
  std::unique_lock<std::mutex> lock(mutex);
  map[key] = filename;
}

void zserver_commit_val(const char* key, double val) {
  const int limit = 500;
  std::unique_lock<std::mutex> lock(mutex);
  
  std::list<double>& list = lists[key];
  list.push_back(val);
  if (list.size() > limit)
    list.pop_front();
}

} // extern "C"
