#include "ppdpp.h"

int PPDPP::dtoi(char c){
  switch(c){
  case 'A': return 0;
  case 'C': return 1;
  case 'G': return 2;
  case 'T': return 3;
  default : std::cerr << "error1 : " << c << std::endl;
    return -1;
  }
}

int PPDPP::makePairVec(int turn,int cl,int sl,int threshold,std::vector< std::pair<int,int> >& cells){
  cells.resize(0);
  int ret = -1;
  for(int i=0;i<=turn;i++){
    int j = turn-i;
    if(i>=cl || j>=sl || abs(i-j) >= threshold) continue;
    std::pair<int,int> p = std::make_pair(i,j);
    cells.push_back(p);
#ifdef DEBUG
    std::cerr << "(" << i << "," << j << ") " << i+j << std::endl;
#endif
    //lindexの条件必要
    if(i == cl-1 || i-j == threshold-1) ret = cells.size()-1;
  }
#ifdef DEBUG
  if(ret != -1) std::cerr << "lindex : " <<  "(" << cells[ret].first << "," << cells[ret].second << ") " << std::endl;
#endif
  
  return ret;
}


void PPDPP::Server::setParam(std::string& cparam){
  std::ifstream ifs(cparam.c_str(), std::ios::binary);
  ifs >> len_client;
  ifs >> epsilon;
  if(epsilon == 0) epsilon = max(sequence.size(),len_client);
  if(epsilon < abs(len_server-len_client)) std::cerr << "no answer s " << abs(len_server-len_client) << std::endl;
  ran_x = (int*)malloc(len_server*len_client*sizeof(int));
  if(ran_x == NULL) std::cerr << "error ranx" << std::endl;
  ran_y = (int*)malloc(len_server*len_client*sizeof(int));
  if(ran_y == NULL) std::cerr << "error rany" << std::endl;
  //ranx,ranyの初期化
  for(int i=0;i<len_client;i++){
    for(int j=0;j<len_server;j++){
      ran_x[i*len_server+j] = ran_y[i*len_server+j] = 0;
    }
  }
  //ファイル入出力用配列の初期化
  int mem = min3((epsilon*2+1),len_client,len_server);
#ifdef DEBUG
  std::cerr << "mem : " << mem << std::endl;
#endif
  queryarray = (Elgamal::CipherText*)malloc(mem*7*sizeof(Elgamal::CipherText));
  resultarray = (Elgamal::CipherText*)malloc(mem*6*sizeof(Elgamal::CipherText));
  lqueryarray = (Elgamal::CipherText*)malloc(36*len_server*sizeof(Elgamal::CipherText));
  ansarray = (Elgamal::CipherText*)malloc(len_server*sizeof(Elgamal::CipherText));
  ran_x_a = (int*)malloc(len_server*sizeof(int));
  ran_y_a = (int*)malloc(len_server*sizeof(int));
  if(queryarray == NULL || resultarray == NULL || lqueryarray == NULL || ansarray == NULL || ran_x_a == NULL || ran_y_a == NULL) std::cerr << "error elgamalarray1" << std::endl;
}

void PPDPP::Server::setLindex(int l,std::vector< std::pair<int,int> >& cells){
  if(l >= 0) lindex = cells[l].second;
  else lindex = -1;
}

void PPDPP::Server::makeParam(std::string& sparam){
  std::ofstream ofs(sparam.c_str(), std::ios::binary);
  ofs << sequence.size() << "\n";
}

void PPDPP::Server::readPubkey(std::string& pubFile){
  ROT::Load(pub, pubFile);
  std::cerr << "Server received pubKey" << std::endl;
}

void PPDPP::Server::parallelDP(std::string& queryfile,std::string& resultfile,std::vector< std::pair<int,int> >& cells){
  std::ifstream ifs(queryfile.c_str(), std::ios::binary);
  std::ofstream ofs(resultfile.c_str(), std::ios::binary);
  int loopnum = cells.size();
  for(int i=0;i<7*loopnum;i++){
    ifs >> queryarray[i];
  }
	omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopnum;i++){
    calcInnerProduct(queryarray+(7*i),resultarray+(6*i),cells[i].first,cells[i].second);
  }
  for(int i=0;i<loopnum*6;i++){
    ofs << resultarray[i] << "\n";
  }
}

void PPDPP::Server::calcInnerProduct(Elgamal::CipherText *query,Elgamal::CipherText *ret,int turn_c,int turn_s){
  CipherTextVec queryvec;
  queryvec.resize(7);
#ifdef DEBUG
  bool b;
  int iip;
#endif
  for(int i=0;i<7;i++){
    queryvec[i] = query[i];
#ifdef DEBUG
    iip = prv.dec(queryvec[i],&b);
    if(b) std::cerr << iip << " ";
    else std::cerr << "bad query";
#endif
  }
#ifdef DEBUG
  std::cerr << std::endl;
#endif
  CipherTextVec ctv;
  Elgamal::CipherText tmp;
  Zn rn;
  int rx = rand();
  int ry = rand();
  int index = turn_c * len_server + turn_s;
#ifdef NRAND
  rx = ry = 0;
#endif
#ifdef DEBUG
  std::cerr << "ranx : " << ran_x[index] << " rany : " << ran_y[index] << std::endl;
#endif
  for(int i=0;i<6;i++){
    pub.enc(ret[i],0,rg);
    ctv = queryvec;
    for(int j=0;j<6;j++){
      int t_index = ( ( (3-ran_y[index]+(6*i+j))%3 + (9-3*ran_x[index]+((6*i+j)/3)*3))%9 + (36-9*dtoi(sequence[turn_s])+((6*i+j)/9)*9) ) % 36;
#ifdef DEBUG
      std::cerr << ((table[t_index]/3+rx)%3)*3 + (table[t_index] + ry)%3 << " ";
#endif
      ctv[j].mul(((table[t_index]/3+rx)%3)*3 + (table[t_index] + ry)%3);
    }
    for(int j=0;j<6;j++)  ret[i].add(ctv[j]);
#ifdef DEBUG
    std::cerr << std::endl;
    iip = prv.dec(ret[i],&b);
    if(b) std::cerr << "ip" << i << " : "<< iip << std::endl;
    else std::cerr << "error ret" << std::endl;
#endif
    rn.setRand(rg);
    pub.enc(tmp,i,rg);
    tmp.neg();
    tmp.add(queryvec[6]);
    tmp.mul(rn);
    ret[i].add(tmp);
#ifdef DEBUG
    iip = prv.dec(ret[i],&b);
    if(b) std::cout << "ip_ans" << i << " : "<< iip << std::endl; 
#endif
  }
  if(lindex == turn_s){
    ran_x_a[lindex] = ran_x[index];
    ran_y_a[lindex] = ran_y[index];
  }
  if(turn_s != len_server-1) ran_x[turn_c*len_server+(turn_s+1)] = rx % 3;
  if(turn_c != len_client-1) ran_y[(turn_c+1)*len_server+turn_s] = ry % 3;
}

void PPDPP::Server::calcLInnerProduct(Elgamal::CipherText *lqueryvec,Elgamal::CipherText *lret,int s_index){
 pub.enc((*lret),0,rg);
 for(int i=0;i<36;i++){
    lqueryvec[( ( (ran_x_a[s_index]+i)%3 + (3*ran_y_a[s_index]+(i/3)*3))%9 + (9*dtoi(sequence[s_index])+(i/9)*9) ) % 36].mul(l_table[i]);
  }
  for(int i=0;i<36;i++){
    (*lret).add(lqueryvec[i]);
  }
}

void PPDPP::Server::makeEditDFile(std::string& l_query,std::string& ans){
  std::ifstream ifs(l_query.c_str(), std::ios::binary);
  for(int i=0;i<36*len_server;i++){
    ifs >> lqueryarray[i];
  }
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<len_server;i++){
    calcLInnerProduct(lqueryarray+(i*36),ansarray+i,i);
  }
  Elgamal::CipherText editD;
  pub.enc(editD,0,rg);
  for(int i=0;i<len_server;i++){
#ifdef DEBUG
  bool b;
  std::cout << "ans" << i << " : "<< prv.dec(ansarray[i],&b) << std::endl; 
#endif
    editD.add(ansarray[i]);
  }
  std::ofstream ofs(ans.c_str(), std::ios::binary);
  ofs << editD << "\n";
}

void PPDPP::Client::setKeys(std::string& prvFile, std::string& pubFile){
  const mcl::EcParam& para = mcl::ecparam::secp192k1;
  const Fp x0(para.gx);
  const Fp y0(para.gy);
  const Ec P(x0, y0);
  const size_t bitLen = para.bitSize;
  
  Elgamal::PrivateKey prvt;
  prvt.init(P, bitLen, rg);
  const Elgamal::PublicKey& pubt = prvt.getPublicKey();
  
  ROT::Save(prvFile, prvt);
  ROT::Save(pubFile, pubt);
  
  ROT::Load(pub, pubFile);
  ROT::Load(prv, prvFile);
  
  if(sequence.size()<len_server)
    prv.setCache(0, max((len_server+1)*2,10)); // set cache for prv
  else
    prv.setCache(0, max((sequence.size()+1)*2,10));
  
  std::cerr << "Client created prv and pub" << std::endl; 
}

void PPDPP::Client::setParam(std::string& sparam){
  std::ifstream ifs(sparam.c_str(), std::ios::binary);
  ifs >> len_server;
  if(epsilon == 0) epsilon = max(sequence.size(),len_server);
  if(epsilon < abs((int)(sequence.size()-len_server))) std::cerr << "no answer c " << std::endl;
  //x,yの初期化
  x = (int*)malloc(len_server*len_client*sizeof(int));
  y = (int*)malloc(len_server*len_client*sizeof(int));
  if(x == NULL || y == NULL) std::cerr << "error x,y" << std::endl;
  for(int i=0;i<len_client;i++){
    for(int j=0;j<len_server;j++){
      x[i*len_server+j] = y[i*len_server+j] = 2;
    }
  }
  //入出力用配列の初期化
  queryarray = (Elgamal::CipherText*)malloc(min3((epsilon*2+1),len_client,len_server)*7*sizeof(Elgamal::CipherText));
  resultarray = (Elgamal::CipherText*)malloc(min3((epsilon*2+1),len_client,len_server)*6*sizeof(Elgamal::CipherText));
  lqueryarray = (Elgamal::CipherText*)malloc(36*len_server*sizeof(Elgamal::CipherText));
  lx = (int*)malloc(len_server*sizeof(int));
  ly = (int*)malloc(len_server*sizeof(int));
  ll = (int*)malloc(len_server*sizeof(int));
  if(queryarray == NULL || resultarray == NULL || lqueryarray == NULL || lx == NULL || ly == NULL || ll == NULL) std::cerr << "error array2" << std::endl;
}

void PPDPP::Client::setLindex(int l,std::vector< std::pair<int,int> > &cells){
  if(l >= 0) lindex = cells[l].first;
  else lindex = -1;
}

void PPDPP::Client::makeParam(std::string& cparam){
  std::ofstream ofs(cparam.c_str(), std::ios::binary);
  ofs << sequence.size() << "\n";
  ofs << epsilon << "\n";
}

void PPDPP::Client::makeQuerySet(std::string& query,std::vector< std::pair<int,int> >& cells){
  std::ofstream ofs(query.c_str(), std::ios::binary);
  int loopnum = cells.size();
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopnum;i++){
    makeQuery(cells[i].first,cells[i].second,queryarray+(i*7));
  }
  for(int i=0;i<loopnum*7;i++){
    ofs << queryarray[i] << "\n";
#ifdef DEBUG
    bool b;
    int iip;
    iip = prv.dec(queryarray[i],&b);
    if(b) std::cerr << iip << " ";
    else std::cerr << "error query";
#endif
  }
#ifdef DEBUG
  std::cerr << std::endl;
#endif
}

void PPDPP::Client::makeQuery(int turn_c,int turn_s,Elgamal::CipherText *queryvec){
  Elgamal::CipherText ct;
  int index = turn_c * len_server + turn_s;
  int t = 3*x[index]+y[index]+9*dtoi(sequence[turn_c]);
  for(int i=0;i<6;i++){
    pub.enc(queryvec[i],(t%6)==i,rg);
  }
  pub.enc(queryvec[6],t/6,rg);
}

void PPDPP::Client::makeLQuerySet(std::string& l_query){
#ifdef DEBUG
  for(int i=0;i<len_server;i++){
    std::cerr << lx[i] << " " << ly[i] << " " << ll[i] << std::endl;
  }
#endif
  std::ofstream ofs(l_query.c_str(), std::ios::binary);
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<len_server;i++){
    makeLQuery(lqueryarray+(i*36),i);
  }
  for(int i=0;i<len_server*36;i++){
    ofs << lqueryarray[i];
    ofs << "\n";
  }
}

void PPDPP::Client::makeLQuery(Elgamal::CipherText *lqueryvec,int index){
  int t = lx[index]+3*ly[index]+9*dtoi(sequence[ll[index]]);
  for(int i=0;i<36;i++){
    pub.enc(lqueryvec[i],t==i,rg);
  }
}

void PPDPP::Client::decResultSet(std::string& result,std::vector< std::pair<int,int> >& cells){
  std::ifstream ifs(result.c_str(), std::ios::binary);
  int loopmain = cells.size();
  for(int i=0;i<6*loopmain;i++){
    ifs >> resultarray[i];
  }
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopmain;i++){
    decResult(resultarray+(i*6),cells[i].first,cells[i].second);
  }
}

void PPDPP::Client::decResult(Elgamal::CipherText *resultvec,int turn_c,int turn_s){
  bool b;
  int index = turn_c * len_server + turn_s;
  int ret = prv.dec(resultvec[(3*x[index]+y[index]+9*dtoi(sequence[turn_c]))/6], &b);
  if(lindex == turn_c){
    lx[turn_s] = x[index];
    ly[turn_s] = y[index];
		ll[turn_s] = lindex;
  }
  if(turn_s != len_server-1) x[turn_c*len_server+(turn_s+1)] = ret / 3;
  if(turn_c != len_client-1) y[(turn_c+1)*len_server+turn_s] = ret % 3;
}

int PPDPP::Client::decEditD(std::string& Ans){
  Elgamal::CipherText ans;
  bool b;
  std::ifstream ifs(Ans.c_str(), std::ios::binary);
  ifs >> ans;
  int editD = prv.dec(ans, &b);
  if(!b) std::cerr << "error editD" << std::endl;
  //std::cout << editD + sequence.size() - len_server << std::endl;
#ifdef DEBUG
  std::cerr << "x" << std::endl;
  for(int i=0;i<len_client;i++){
    for(int j=0;j<len_server;j++){
      std::cerr << x[i*len_server+j]-1 << " ";
    }
    std::cerr << std::endl;
  }
  std::cerr << "y" << std::endl;
  for(int i=0;i<len_client;i++){
    for(int j=0;j<len_server;j++){
      std::cerr << y[i*len_server+j]-1 << " ";
    }
    std::cerr << std::endl;
  }
#endif
  return editD + len_client - len_server;
}


int PPDPP::edit_check(std::string &s,std::string &c){
  int ss = s.size()+1;
  int cs = c.size()+1;
  int dpm[ss][cs];
  for(int i=0;i<ss;i++){
    for(int j=0;j<cs;j++){
      if(i==0) dpm[i][j] = j;
      else if(j==0) dpm[i][j] = i;
      else dpm[i][j] = min3(dpm[i][j-1]+1,dpm[i-1][j]+1,dpm[i-1][j-1]+(s[i-1]!=c[j-1]));
    }
  }
  return dpm[ss-1][cs-1];
}
