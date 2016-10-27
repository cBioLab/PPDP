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
	double start_sec = get_wall_time();
	double s_time,e_time,calc_time=0,total_time=0;
	s_time = start_sec;

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
  
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sock = prepCSock((char *)hostname.c_str(),port);

	s_time = get_wall_time();
  client.makeParam(cparam);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sendsize+=sendFile(sock,(char *)cparam.c_str());
	recvsize+=recvFile(sock,(char *)sparam.c_str());

	s_time = get_wall_time();
  client.setParam(sparam);
  client.setKeys(prvfile, pubfile);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sendsize+=sendFile(sock,(char *)pubfile.c_str());

	s_time = get_wall_time(); 
  int sl = client.len_server;
  int cl = client.len_client;
  int epsilon = client.epsilon;
  std::vector< std::pair<int,int> > cells;
	int li;
	e_time = get_wall_time();
	calc_time += e_time-s_time;  

  for(int i=0;i<sl+cl-1;i++){
		s_time = get_wall_time(); 
    li = PPDPP::makePairVec(i,cl,sl,epsilon,cells);
    client.setLindex(li,cells);
    client.makeQuerySet(queryfile,cells);
		e_time = get_wall_time();
		calc_time += e_time-s_time;

		sendsize+=sendFile(sock,(char *)queryfile.c_str());
		recvsize+=recvFile(sock,(char *)resultfile.c_str());

		s_time = get_wall_time();
    client.decResultSet(resultfile,cells);
		e_time = get_wall_time();
		calc_time += e_time-s_time;
    if(li != -1){
			s_time = get_wall_time();
      client.makeLQuery(l_queryfile);
			e_time = get_wall_time();
			calc_time += e_time-s_time;

			sendsize+=sendFile(sock,(char *)l_queryfile.c_str());
    }
  }
	recvsize+=recvFile(sock,(char *)ansfile.c_str());

	s_time = get_wall_time();
  int ans = client.decEditD(ansfile);
	e_time = get_wall_time();
	calc_time += e_time-s_time;
	total_time = get_wall_time() - start_sec;
 
	std::cout << "Edit distance : " << ans << std::endl;
	std::cout << "client calc_time : " << calc_time << "s" << std::endl;
	std::cout << "total time : " << total_time << "s" << std::endl;
	std::cout << "comsize : " << (double)(recvsize+sendsize)/(1024*1024) << "MB" << std::endl;

  return 0;
}
