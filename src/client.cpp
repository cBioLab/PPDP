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
	std::string hostname = "localhost";
	int sendsize = 0;
	int recvsize = 0;
	
  if(argc == 1){
    str = "ATGTATAT";
  }else if(argc == 2){
    std::string stringfile = argv[1];
    std::ifstream readfile(stringfile.c_str(),std::ios::in);
    std::getline(readfile,str);
  }

  ROT::SysInit();
  
  std::string sparam = "../comm/client/sparam.dat";
  std::string cparam = "../comm/client/cparam.dat";
  std::string prvfile = "../comm/client/prv.dat";
  std::string pubfile = "../comm/client/pub.dat";
  std::string queryfile = "../comm/client/query.dat";
  std::string resultfile = "../comm/client/result.dat";
  std::string l_queryfile = "../comm/client/l_query.dat";
  std::string ansfile = "../comm/client/ans.dat";
	
  std::cerr << str << std::endl;

  PPDPP::Client client(str);	
  client.core = 1;
  
	sock = prepCSock((char *)hostname.c_str(),port);

  client.makeParam(cparam);
  
	sendsize+=sendFile(sock,(char *)cparam.c_str());
	recvsize+=recvFile(sock,(char *)sparam.c_str());

  client.setParam(sparam);
  
  client.setKeys(prvfile, pubfile);
	sendsize+=sendFile(sock,(char *)pubfile.c_str());
  
  int sl = client.len_server;
  int cl = client.len_client;
  int epsilon = client.epsilon;

  std::vector< std::pair<int,int> > cells;
	int li;
    
  for(int i=0;i<sl+cl-1;i++){
    li = PPDPP::makePairVec(i,cl,sl,epsilon,cells);
    client.setLindex(li,cells);
    client.makeQuerySet(queryfile,cells);

		sendsize+=sendFile(sock,(char *)queryfile.c_str());
		recvsize+=recvFile(sock,(char *)resultfile.c_str());

    client.decResultSet(resultfile,cells);
    if(li != -1){
      client.makeLQuery(l_queryfile);

			sendsize+=sendFile(sock,(char *)l_queryfile.c_str());
    }
  }
	recvsize+=recvFile(sock,(char *)ansfile.c_str());
  int ans = client.decEditD(ansfile);
  std::cout << "Edit distance : " << ans << std::endl;

  return 0;
}
