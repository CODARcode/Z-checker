#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <json.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;
using json = nlohmann::json;

server wss;

void onHttp(server *s, websocketpp::connection_hdl hdl) 
{
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  con->append_header("Access-Control-Allow-Origin", "*");
  con->append_header("Access-Control-Allow-Headers", "*");
  
  std::string query = con->get_resource();

  if (query == "/requestInfo") {
    json j;
    j["hello"] = "world";
    j["number"] = 10;

    con->set_body(j.dump()); 
    con->set_status(websocketpp::http::status_code::ok);
  } else if (query == "/requestData") {
    std::string results = "hello"; 

    con->set_body(results);
    con->set_status(websocketpp::http::status_code::ok);
  } else {
    std::string response = "<html><body>404 not found</body></html>";
    con->set_body(response);
    con->set_status(websocketpp::http::status_code::not_found);
  }
}

void startServer(int port) 
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
    wss.set_http_handler(bind(&onHttp, &wss, _1));

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

int main(int argc, char **argv) 
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    return 0;
  }

  int port = atoi(argv[1]);

  std::thread *thread = new std::thread(startServer, port);
  thread->join();
  delete thread;

  return 0;
}
