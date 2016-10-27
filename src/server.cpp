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
	double s_time,e_time,calc_time=0;
	s_time = start_sec;

  std::string str;
	std::string stringfile;
	int sock;
	int port;
	int core;
  
  ROT::SysInit();
  
  std::string sparam = "../comm/server/sparam.dat";
  std::string cparam = "../comm/server/cparam.dat";
  std::string prvfile = "../comm/server/prv.dat";
  std::string pubfile = "../comm/server/pub.dat";
  std::string queryfile = "../comm/server/query.dat";
  std::string resultfile = "../comm/server/result.dat";
  std::string l_queryfile = "../comm/server/l_query.dat";
  std::string ansfile = "../comm/server/ans.dat";

	//-p port -f WMfile -c core -e epsilon
	int opt;
	while((opt = getopt(argc, argv, "p:f:c:")) != -1){
		switch(opt){
			case 'p':
				{
					port = atoi(optarg);
					break;
				}
			case 'f':
				{ 
					stringfile = optarg;
					break;
				}
			case 'c':
				{ 
					core = atoi(optarg);
					break;
				}
			default:
				{
					fprintf(stderr,"error! \'%c\' \'%c\'\n", opt, optopt);
					return -1;
					break;
				}
		}
	}

  std::ifstream readfile(stringfile.c_str(),std::ios::in);
  std::getline(readfile,str);

  PPDPP::Server server(str);
  server.core = core;

  std::cerr << str << std::endl;

	e_time = get_wall_time();
	calc_time += e_time-s_time;

	sock = prepSSock(port);
	sock = acceptSSock(sock);

	s_time = get_wall_time();
  server.makeParam(sparam);
	e_time = get_wall_time();
	calc_time += e_time-s_time;

	recvFile(sock,(char *)cparam.c_str());
	sendFile(sock,(char *)sparam.c_str());

	s_time = get_wall_time();
  server.setParam(cparam);
	e_time = get_wall_time();
	calc_time += e_time-s_time;
  
	recvFile(sock,(char *)pubfile.c_str());

	s_time = get_wall_time();
  server.readPubkey(pubfile);
  
  int sl = server.len_server;
  int cl = server.len_client;
  int epsilon = server.epsilon;

  std::vector< std::pair<int,int> > cells;
	int li;
	e_time = get_wall_time();
	calc_time += e_time-s_time;  
  
  for(int i=0;i<sl+cl-1;i++){
		s_time = get_wall_time();
    li = PPDPP::makePairVec(i,cl,sl,epsilon,cells);
    server.setLindex(li,cells);
		e_time = get_wall_time();
		calc_time += e_time-s_time; 

		recvFile(sock,(char *)queryfile.c_str());

		s_time = get_wall_time();
    server.parallelDP(queryfile,resultfile,cells);
		e_time = get_wall_time();
		calc_time += e_time-s_time; 

		sendFile(sock,(char *)resultfile.c_str());
    if(li != -1){
			recvFile(sock,(char *)l_queryfile.c_str());
    	
			s_time = get_wall_time();
  		server.addAnsVec(l_queryfile);
			e_time = get_wall_time();
			calc_time += e_time-s_time; 
    }
  }
	s_time = get_wall_time();
  server.makeEditDFile(ansfile);
	e_time = get_wall_time();
	calc_time += e_time-s_time; 

	sendFile(sock,(char *)ansfile.c_str());

	std::cout << "server calc_time : " << calc_time << "s" << std::endl;

  return 0;
}
