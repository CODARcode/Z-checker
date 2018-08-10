#include "zserver.h"
#include <mutex>
#include <list>
#include <fstream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <json.hh>
#include "zc.h"
#include "ZC_DataProperty.h"

typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;

static std::mutex mutex;
static server wss;
static std::thread *thread;
static std::map<std::string, std::string> map;

static std::map<std::string, std::list<double> > valueLists;
static std::map<std::string, std::list<std::vector<double> > > vectorLists;

std::list<std::string> listResults;

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
  } else if (query.find("/realtime") == 0) {
    // const std::string key = query.substr(query.find("?") + 1);
    std::stringstream buffer;

    std::unique_lock<std::mutex> lock(mutex);
    valueLists2json(buffer, valueLists);
    con->set_body(buffer.str());
    succ = true;
  } else if (query.find("/all") == 0) {
    std::unique_lock<std::mutex> lock(mutex);
    
    std::stringstream buffer;
    buffer << "[";
    for (auto str : listResults)
      buffer << str << ",";
    buffer.seekp(-1, std::ios_base::end);
    buffer << "]";

    con->set_body(buffer.str());
    succ = true;
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
  
  std::list<double>& list = valueLists[key];
  list.push_back(val);
  if (list.size() > limit)
    list.pop_front();
}

void zserver_commit_vec(const char* key, int length, double *val) {
  const int limit = 500;
  std::unique_lock<std::mutex> lock(mutex);

  std::list<std::vector<double> > vectorList = vectorLists[key];
  std::vector<double> vector;
  vector.assign(val, val + length);
  vectorList.push_back(vector);

  if (vectorList.size() > limit)
    vectorList.pop_front();
}

void zserver_commit(int timestep, struct ZC_DataProperty *d, struct ZC_CompareData *c)
{
  const int limit = 500;
  nlohmann::json j;
  std::unique_lock<std::mutex> lock(mutex);

  // data properties
  j["timestep"] = timestep;
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

  listResults.push_back(j.dump());
  if (listResults.size() > limit) 
    listResults.pop_front();
}

} // extern "C"
