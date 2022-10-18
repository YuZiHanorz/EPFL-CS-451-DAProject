#include <chrono>
#include <iostream>
#include <thread>

#include "parser.hpp"
#include "hello.h"
#include <signal.h>

#include "perfectLink.h"

std::string outputPath;
Process* p = nullptr;
Receiver* rec = nullptr;
std::vector<Sender*> senders;
std::vector<PerfectLink*> links;

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
  for (auto& log: logs){
    char logType = log[0];
    //std::cout << "the log is " << log << "\n";
    int msgSpid;
    std::string payload;
    switch (logType){
    case 'b':
      payload = log.substr(4, log.size());
      o << "b " << payload << "\n";
      break;
    case 'd':
      msgSpid = stoi(log.substr(1, 3));
      payload = log.substr(4, log.size());
      o << "d " << msgSpid << " " << payload << "\n";
      break;
    default:
      std::cerr << "wtf with this log " << log << "\n";
      break;
    }
  }
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
  if (rec != nullptr)
    rec->stop();
  for (auto& link: links) {
    if (link != nullptr) 
      link->stop();
  }

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

  for (auto& host : hosts){
    if (host.id == pid){
      senders.push_back(nullptr);
      links.push_back(nullptr);
      continue;
    }
    Sender* sen = new Sender(getAddr(host.ip, host.port));
    PerfectLink* link = new PerfectLink(static_cast<int>(pid), static_cast<int>(host.id), rec, sen, p);
    senders.push_back(sen);
    links.push_back(link);
  }

  std::string _num, _tPid;
  std::string line;
  std::ifstream readFile(parser.configPath());

  //read config
  while(getline(readFile,line))   {
      std::stringstream iss(line);
      getline(iss, _num, ' ');
      getline(iss, _tPid, '\n');
  }
  readFile.close();

  //add msg
  unsigned long num = static_cast<unsigned long>(std::stoi(_num));
  unsigned long tPid = static_cast<unsigned long>(std::stoi(_tPid));

  if (pid != tPid){
    for (unsigned int i = 1; i <= num; ++i){
      char msg[MAX_LENGTH] = {0};
      int ack = 0;
      sprintf(msg, "%-1d%03lu%-d", ack, pid, i);
      links[tPid-1]->addMsg(msg);
    }
    //std::cout << pid << " add " << num << " msgs to " << tPid << "\n";
  }

  for (auto& link: links) {
    if (link != nullptr) {
      link->start();
    }
  }

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
