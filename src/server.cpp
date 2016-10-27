#include "ppdpp.h"
#include "comm.h"

#include<sys/time.h>

double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time, NULL)){
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

extern cybozu::RandomGenerator rg;

int main(int argc,char* argv[]){
  std::string str;
	int sock;
	int port = 4444;	

  if(argc == 1){
    str = "TATATATA";
  }else if(argc == 2){
    std::string stringfile = argv[1];
    std::ifstream readfile(stringfile.c_str(),std::ios::in);
    std::getline(readfile,str);
  }
  
  ROT::SysInit();
  
  std::string sparam = "../comm/server/sparam.dat";
  std::string cparam = "../comm/server/cparam.dat";
  std::string prvfile = "../comm/server/prv.dat";
  std::string pubfile = "../comm/server/pub.dat";
  std::string queryfile = "../comm/server/query.dat";
  std::string resultfile = "../comm/server/result.dat";
  std::string l_queryfile = "../comm/server/l_query.dat";
  std::string ansfile = "../comm/server/ans.dat";
	
  std::cerr << str << std::endl;
  
  PPDPP::Server server(str);
  server.core = 2;
  
	sock = prepSSock(port);
	sock = acceptSSock(sock);

  server.makeParam(sparam);

	recvFile(sock,(char *)cparam.c_str());
	sendFile(sock,(char *)sparam.c_str());

  server.setParam(cparam);
  
	recvFile(sock,(char *)pubfile.c_str());
  server.readPubkey(pubfile);
  
  int sl = server.len_server;
  int cl = server.len_client;
  int epsilon = server.epsilon;

  std::vector< std::pair<int,int> > cells;
	int li;
    
  for(int i=0;i<sl+cl-1;i++){
    li = PPDPP::makePairVec(i,cl,sl,epsilon,cells);
    server.setLindex(li,cells);

		recvFile(sock,(char *)queryfile.c_str());

    server.parallelDP(queryfile,resultfile,cells);

		sendFile(sock,(char *)resultfile.c_str());
    if(li != -1){
			recvFile(sock,(char *)l_queryfile.c_str());
    	
  		server.addAnsVec(l_queryfile);
    }
  }
  server.makeEditDFile(ansfile);
	sendFile(sock,(char *)ansfile.c_str());

  return 0;
}
