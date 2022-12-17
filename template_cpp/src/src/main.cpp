#include <chrono>
#include <iostream>
#include <thread>

#include "parser.hpp"
#include "hello.h"
#include <signal.h>

#include "latagr.h"

std::string outputPath;
Process* p = nullptr;
Receiver* rec = nullptr;
Sender* sen = nullptr;
PerfectLink* pl = nullptr;
Beb* beb = nullptr;
LatticeAgr* lag = nullptr;

sockaddr_in getAddr(in_addr_t ip, unsigned short port);
void writeLog();

//create socketAddr from ip and port
sockaddr_in getAddr(in_addr_t ip, unsigned short port) {
  struct sockaddr_in addr;

  addr.sin_addr.s_addr = ip;
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  return addr;
}

//write log to outputPath
void writeLog() {
  std::stringstream o;
  if (p == nullptr)
    return;
  
  std::list<std::string> logs = p->getLogs();
  int pnum = static_cast<int>(logs.size());
  std::unordered_map<int, std::set<std::string>> decision;
  for (auto& log: logs){
    size_t pos = log.find('d');
    int idx = stoi(log.substr(0, pos));
    std::string pp = log.substr(pos+1, log.size());
    std::stringstream ss(pp);
    std::string item;
    std::set<std::string> tmp;
    while (std::getline(ss, item, ',')) {
        tmp.insert(item);
    }
    auto iter = decision.find(idx);
    if (iter != decision.end()){
      std::cerr << "what the fuck with the log " << log <<"\n";
    }
    //std::cout << idx << " " << log << "\n";
    decision[idx] = tmp;
  }
  for (int i = 0; i < pnum; ++i){
    bool flag = true;
    auto iter = decision.find(i);
    if (iter == decision.end()){
      std::cerr << "what the fuck cannot find prop " << "\n";
    }
    for (auto &e : decision[i]){
      if (flag){
        o << e;
        flag = false;
      }
      else{
        o << " " << e;
      }
    }
    o << "\n";
  }

  //for (auto& log: logs){
    //char logType = log[0];
    //std::cout << "the log is " << log << "\n";
    //int msgSpid;
    //std::string payload;
    //switch (logType){
    //case 'b':
      //payload = log.substr(1, log.size());
      //o << "b " << payload << "\n";
      //break;
    //case 'd':
      //msgSpid = stoi(log.substr(1, 3));
      //payload = log.substr(4, log.size());
      //o << "d " << msgSpid << " " << payload << "\n";
      //break;
    //default:
      //std::cerr << "wtf with this log " << log << "\n";
      //break;
    //}
  //}
  std::ofstream fs;
  fs.open(outputPath);
  fs << o.str(); 
  fs.close();
}


static void stop(int) {
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  if (p != nullptr)
    p->stop();
  if (beb != nullptr)
    lag->stop();
  // write/flush output file if necessary
  std::cout << "Writing output.\n";

  writeLog();

  // exit directly from signal handler
  exit(0);
}

int main(int argc, char **argv) {
  signal(SIGTERM, stop);
  signal(SIGINT, stop);

  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;

  Parser parser(argc, argv);
  parser.parse();

  hello();
  std::cout << std::endl;

  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";
  auto hosts = parser.hosts();
  for (auto &host : hosts) {
    std::cout << host.id << "\n";
    std::cout << "Human-readable IP: " << host.ipReadable() << "\n";
    std::cout << "Machine-readable IP: " << host.ip << "\n";
    std::cout << "Human-readbale Port: " << host.portReadable() << "\n";
    std::cout << "Machine-readbale Port: " << host.port << "\n";
    std::cout << "\n";
  }
  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  std::cout << "Broadcasting and delivering messages...\n\n";

  outputPath = parser.outputPath();
  unsigned long pid = parser.id();
  p = new Process(static_cast<int>(pid));
  rec = new Receiver(hosts[pid-1].ip, hosts[pid-1].port);

  std::vector<sockaddr_in> addrs;
  std::vector<int> ids;
  for (auto& host : hosts){
    if (host.id == pid){
      //senders.push_back(nullptr);
      //links.push_back(nullptr);
      //continue;
    }
    addrs.push_back(getAddr(host.ip, host.port));
    ids.push_back(static_cast<int>(host.id));
    //Sender* sen = new Sender(getAddr(host.ip, host.port));
    //PerfectLink* link = new PerfectLink(static_cast<int>(pid), static_cast<int>(host.id), rec, sen, p);
    //senders.push_back(sen);
    //links.push_back(link);
  }
  sen = new Sender(addrs);
  pl= new PerfectLink(static_cast<int>(pid), ids, rec, sen);
  beb = new Beb(static_cast<int>(pid), ids, pl, p);
  pl->setUpper(beb);
  lag = new LatticeAgr(static_cast<int>(pid), ids, beb, p);
  beb->setUpper(lag);

  int propnum, vs, ds;
  std::string line;
  std::ifstream readFile(parser.configPath());
  std::vector<std::set<std::string>> prop;
  
  //read config
  bool first = true;
  while(getline(readFile,line))   {
      std::stringstream iss(line);
      if (first){
        iss >> propnum;
        iss >> vs;
        iss >> ds;
        first = false;
      }
      else {
        std::string tmp;
        std::set<std::string> st;
        while (iss >> tmp)
          st.insert(tmp);
        prop.push_back(st);
      }
  }
  readFile.close();

  //add msg
  //unsigned long num = static_cast<unsigned long>(std::stoi(_num));
  //unsigned long tPid = static_cast<unsigned long>(std::stoi(_tPid));
  lag->start();
  lag->propose(prop);
  //int numM = static_cast<int>(num);
  //for (int i = 1; i <= numM; ++i){
    //char msg[MAX_LENGTH] = {0};
    //sprintf(msg, "%03lu%-d", static_cast<unsigned long>(pid), i);
    //std::string msg = std::to_string(i);
    //fb->broadcast(msg);
    //std::this_thread::sleep_for(std::chrono::milliseconds(5));
  //}

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
