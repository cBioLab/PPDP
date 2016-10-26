#include "ppdpp.h"

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
  std::string str1,str2;
	
  if(argc == 1){
    str1 = "TATATATA";
    str2 = "ATGTATAT";
  }else{
    std::string stringfile = argv[1];
    std::ifstream readfile(stringfile.c_str(),std::ios::in);
    std::getline(readfile,str1);
    std::getline(readfile,str2);
  }
  
  ROT::SysInit();
  
  std::string sparam = "../comm/sparam.dat";
  std::string cparam = "../comm/cparam.dat";
  std::string prvfile = "../comm/prv.dat";
  std::string pubfile = "../comm/pub.dat";
  std::string queryfile = "../comm/query.dat";
  std::string resultfile = "../comm/result.dat";
  std::string l_queryfile = "../comm/l_query.dat";
  std::string ansfile = "../comm/ans.dat";
	
  std::cerr << str1 << std::endl;
  std::cerr << str2 << std::endl;
  
  PPDPP::Server server(str1);
  PPDPP::Client client(str2);	
  
  server.core = 2;
  client.core = 1;
  
  server.makeParam(sparam);
  client.makeParam(cparam);
  
  server.setParam(cparam);
  client.setParam(sparam);
  
  client.setKeys(prvfile, pubfile);
  server.readPubkey(pubfile);

#ifdef DEBUG
  ROT::Load(server.prv, prvfile);
  std::cerr << "Server received prvKey" << std::endl;
  server.prv.setCache(0, 100); // set cache for prv
#endif
  
  int sl = server.len_server;
  int cl = client.len_client;
  int epsilon = client.epsilon;

  std::vector< std::pair<int,int> > cells;
    
  for(int i=0;i<sl+cl-1;i++){
    int li = PPDPP::makePairVec(i,cl,sl,epsilon,cells);
    server.setLindex(li,cells);
    client.setLindex(li,cells);
    client.makeQuerySet(queryfile,cells);
    server.parallelDP(queryfile,resultfile,cells);
    client.decResultSet(resultfile,cells);
    if(li != -1){
      client.makeLQuery(l_queryfile);
      server.addAnsVec(l_queryfile);
    }
  }
  server.makeEditDFile(ansfile);
  int ans = client.decEditD(ansfile);
  std::cout << "Edit distance : " << ans << std::endl;
  if(ans == PPDPP::edit_check(str1,str2)) std::cerr << "ok" << std::endl;
  else std::cerr << "no" << std::endl;

  return 0;
}
