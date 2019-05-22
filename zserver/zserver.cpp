#include "zserver.h"
#include "base64.h"
#include <mutex>
#include <list>
#include <set>
#include <fstream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <json.hh>
#include "zc.h"
#include "ZC_DataProperty.h"
#include "png_utils.hh"
#include <cmrc/cmrc.hpp>

#ifdef HAVE_ROCKSDB
#include <rocksdb/db.h>
static rocksdb::DB* db = NULL;
#elif defined HAVE_LEVELDB
#include <leveldb/db.h>
static leveldb::DB* db = NULL;
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

CMRC_DECLARE(zserver_public);
auto webfs = cmrc::zserver_public::get_filesystem();

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

// static std::mutex mutex;
static server wss;
static std::thread *thread, *thread1;
static std::map<std::string, std::string> map;

// std::list<std::string> listResults;




// actions
enum {ACTION_SUBSCRIBE=0, ACTION_UNSUBSCRIBE, 
  ACTION_BROADCAST, ACTION_MESSAGE, ACTION_EXIT};

struct Action {
  Action(int t) : type(t) {}
  Action(int t, const std::string& str_bcast) : type(t), str(str_bcast) {}
  Action(int t, websocketpp::connection_hdl h) : type(t), hdl(h) {}
  Action(int t, websocketpp::connection_hdl h, server::message_ptr m) : type(t), hdl(h), msg(m) {}

  int type;
  std::string str;
  websocketpp::connection_hdl hdl;
  server::message_ptr msg;
};

std::mutex mutex_actions;
std::condition_variable cond_actions;
std::queue<Action> actions;

std::mutex mutex_connections;
std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl> > connections;

static void valueList2json(std::stringstream &buf, const std::list<double>& list) {
  buf << "[";
  for (auto val : list) 
    buf << val << ',';
  buf.seekp(-1, std::ios_base::end);
  buf << "]";
}

static void valueLists2json(std::stringstream &buf, std::map<std::string, std::list<double> > &valueLists)
{
  buf << "{";
  for (auto kv : valueLists) {
    buf << "\"" << kv.first << "\":";
    valueList2json(buf, kv.second);
    buf << ",\n";
  }
  buf.seekp(-2, std::ios_base::end);
  buf << "}";
}

void on_open(server* s, websocketpp::connection_hdl hdl) {
  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_SUBSCRIBE, hdl));
  }
  cond_actions.notify_one();
}

void on_close(server* s, websocketpp::connection_hdl hdl) {
  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_UNSUBSCRIBE, hdl));
  }
  cond_actions.notify_one();
}

void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_MESSAGE, hdl, msg));
  }
  cond_actions.notify_one();
}

static void on_http(server *s, websocketpp::connection_hdl hdl) 
{
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  con->append_header("Access-Control-Allow-Origin", "*");
  con->append_header("Access-Control-Allow-Headers", "*");
  
  std::string query = con->get_resource();
  if (query == "/") query = "/index.html";

  bool succ = false;
#if 0
  if (query.find("/all") == 0) {
    // std::unique_lock<std::mutex> lock(mutex);
    
    std::stringstream buffer;
    buffer << "[";
    // for (auto str : listResults)
    //   buffer << str << ",";
    // buffer.seekp(-1, std::ios_base::end);
    buffer << "]";
    con->set_body(buffer.str());
    succ = true;
  }
#endif

  const std::string filename = "/public" + query;
  try {
    auto file = webfs.open(filename);
    std::string str(file.begin(), file.end());
    con->set_body(str);
    succ = true;
  } catch (...) {
    succ = false;
  }
 
  if (succ) {
    con->set_status(websocketpp::http::status_code::ok);
  } else {
    std::string response = "<html><body>404 not found</body></html>";
    con->set_body(response);
    con->set_status(websocketpp::http::status_code::not_found);
  }
}

static void start_process_message()
{
  while (1) {
    std::unique_lock<std::mutex> lock(mutex_actions);
    while (actions.empty()) 
      cond_actions.wait(lock);

    Action a = actions.front();
    actions.pop();
    lock.unlock();

    if (a.type == ACTION_SUBSCRIBE) {
      std::unique_lock<std::mutex> lock(mutex_connections);
      connections.insert(a.hdl);
    } else if (a.type == ACTION_UNSUBSCRIBE) {
      std::unique_lock<std::mutex> lock(mutex_connections);
      connections.erase(a.hdl);
    } else if (a.type == ACTION_MESSAGE) {
      // fprintf(stderr, "MESSAGE!\n");
    } else if (a.type == ACTION_BROADCAST) {
      for (auto conn : connections) {
        wss.send(conn, a.str, websocketpp::frame::opcode::text);
      }
    } else if (a.type == ACTION_EXIT) {
      break;
    }
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
    // wss.clear_access_channels(websocketpp::log::alevel::frame_payload);
    wss.clear_access_channels(websocketpp::log::alevel::all);
    wss.set_open_handshake_timeout(0); // disable timer

    // Initialize Asio
    wss.init_asio();

    // Register our message handler
    wss.set_http_handler(bind(&on_http, &wss, _1));
    wss.set_open_handler(bind(&on_open, &wss, _1));
    wss.set_close_handler(bind(&on_close, &wss, _1));
    wss.set_message_handler(bind(&on_message, &wss, _1, _2));

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

static void open_db(const std::string& dbname)
{
#if HAVE_ROCKSDB
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::Status status = rocksdb::DB::Open(options, dbname.c_str(), &db);
#elif HAVE_LEVELDB
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, dbname.c_str(), &db);
#endif
}

///////////////////////////////////
extern "C" {

void zserver_start(int port) 
{
  fprintf(stderr, "listening to port %d!\n", port);
  const std::string dbname = "/tmp/zchecker";
  open_db(dbname);

  thread = new std::thread(start_server_thread, port);
  thread1 = new std::thread(start_process_message);
}

void zserver_stop()
{
#if HAVE_LEVELDB
  delete db;
#endif

  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_EXIT));
  }
  cond_actions.notify_one();

  thread1->join();
  delete thread1;

  wss.stop();
  thread->join();
  delete thread;
  // TODO
}

#if HAVE_PNG
static png_mem_buffer array2png(int W, int H, double *p)
{
  unsigned char *buf = (unsigned char*)malloc(W*H*4);
  double min = std::numeric_limits<double>::max(), 
         max = std::numeric_limits<double>::min();
  for (int i = 0; i < W*H; i ++) {
    min = std::min(min, p[i]);
    max = std::max(max, p[i]);
  }
  for (int i = 0; i < W*H; i ++) {
    double x = (p[i] - min) / (max - min);
    buf[i*4] = x * 255;
    buf[i*4+1] = (1-x) * 255;
    buf[i*4+2] = 0;
    buf[i*4+3] = 255;
  }
  auto png = save_png(W, H, 8, PNG_COLOR_TYPE_RGBA, buf, 4*W, PNG_TRANSFORM_IDENTITY);
  free(buf);

  return png;
  // FILE *fp = fopen("test.png", "wb");
  // fwrite(png0.buffer, 1, png0.size, fp);
  // free(png0.buffer);
  // fclose(fp);
}
#endif

void zserver_commit_field_data(int timestep, int W, int H, double *p, double *q)
{
#if HAVE_PNG
  nlohmann::json j;

  j["timestep"] = timestep;
  j["type"] = "field";
  j["width"] = W;
  j["height"] = H;
  // j["original_data"] = base64_encode((unsigned char*)p, W*H*sizeof(double));
  // j["reconstructed_data"] = base64_encode((unsigned char*)q, W*H*sizeof(double));

  auto png0 = array2png(W, H, p), 
       png1 = array2png(W, H, q);
 
  const std::string header("data:image/png;base64, ");
  j["original_data"] = header + base64_encode((unsigned char*)png0.buffer, png0.size);
  j["reconstructed_data"] = header + base64_encode((unsigned char*)png1.buffer, png1.size);

  free(png0.buffer);
  free(png1.buffer);

  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_BROADCAST, j.dump())); 
  }
  cond_actions.notify_one();
#endif
}

void zserver_commit(int timestep, struct ZC_DataProperty *d, struct ZC_CompareData *c)
{
  nlohmann::json j;
  // std::unique_lock<std::mutex> lock(mutex);

  // data properties
  j["timestep"] = timestep;
  j["type"] = "stats";
  j["varName"] = d->varName;
  j["minValue"] = d->minValue;
  j["maxValue"] = d->maxValue;
  j["avgValue"] = d->avgValue;
  j["entropy"] = d->entropy;

  if (d->fftCoeff != NULL) {
    std::vector<double> fft(FFT_SIZE*2);
    for (int i=0; i<FFT_SIZE; i++) {
      fft[i*2] = d->fftCoeff->Re; 
      fft[i*2+1] = d->fftCoeff->Im;
    }
    j["fft"] = fft;
  }

  std::vector<double> autocorr;
  autocorr.assign(d->autocorr, d->autocorr + AUTOCORR_SIZE);
  j["autocorr"] = autocorr;
  
  // compare data
  j["compressTime"] = c->compressTime;
  j["compressRate"] = c->compressRate;
  j["compressSize"] = c->compressSize;
  j["compressRatio"] = c->compressRatio;
  j["rate"] = c->rate;
  j["decompressTime"] = c->decompressTime;
  j["decompressRate"] = c->decompressRate;
  j["minAbsErr"] = c->minAbsErr;
  j["avgAbsErr"] = c->avgAbsErr;
  j["maxAbsErr"] = c->maxAbsErr;
  // autoCorrAbsErr;
  // autoCorrAbsErr3D;
  // absErrPDF;
  // pwrErrPDF;
  j["err_interval"] = c->err_interval;
  j["err_interval_rel"] = c->err_interval_rel;
  j["err_minValue"] = c->err_minValue;
  j["err_minValue_rel"] = c->err_minValue_rel;
  j["minRelErr"] = c->minRelErr;
  j["avgRelErr"] = c->avgRelErr;
  j["maxRelErr"] = c->maxRelErr;
  j["minPWRErr"] = c->minPWRErr;
  j["avgPWRErr"] = c->avgPWRErr;
  j["maxPWRErr"] = c->maxPWRErr;
  j["snr"] = c->snr;
  j["rmse"] = c->rmse;
  j["nrmse"] = c->nrmse;
  j["psnr"] = c->psnr;
  j["valErrCorr"] = c->valErrCorr;
  j["pearsonCorr"] = c->pearsonCorr;
  j["ksValue"] = c->ksValue;
  j["lum"] = c->lum;
  j["cont"] = c->cont;
  j["struc"] = c->struc;
  j["ssim"] = c->ssim;
  j["ssimImage2D_avg"] = c->ssimImage2D_avg;
  
  {
    std::unique_lock<std::mutex> lock(mutex_actions);
    actions.push(Action(ACTION_BROADCAST, j.dump()));
  }
  cond_actions.notify_one();
}

} // extern "C"
