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

int PPDPP::makePairVec(int turn,int cl,int sl,int threshold,std::vector<std::pair<int,int>> &cells){
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
    if(j == sl-1 || j == turn + threshold) ret = cells.size()-1;
  }
  return ret;
}

void PPDPP::Server::setParam(std::string& cparam){
  std::ifstream ifs(cparam.c_str(), std::ios::binary);
  ifs >> len_client;
  ifs >> epsilon;
  if(epsilon == 0) epsilon = max(sequence.size(),len_client);
  if(epsilon < abs(sequence.size()-len_client)) std::cerr << "no answer s " << abs((int)(sequence.size()-len_client)) << std::endl;
  ran_x = (int*)malloc(len_client*sizeof(int));
  if(ran_x == NULL) std::cerr << "error ranx" << std::endl;
  for(int i=0;i<len_client;i++) ran_x[i] = 0;
  ran_y = (int*)malloc(len_server*sizeof(int));
  if(ran_y == NULL) std::cerr << "error rany" << std::endl;
  for(int i=0;i<len_server;i++) ran_y[i] = 0;
  //ファイル入出力用配列の初期化
  queryarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*7*sizeof(Elgamal::CipherText));
  resultarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*6*sizeof(Elgamal::CipherText));
  if(queryarray == NULL || resultarray == NULL) std::cerr << "error elgamalarray1" << std::endl;
}

void PPDPP::Server::setParam(std::string& cparam){
  std::ifstream ifs(cparam.c_str(), std::ios::binary);
  ifs >> len_client;
  ifs >> epsilon;
  if(epsilon == 0) epsilon = max(sequence.size(),len_client);
  if(epsilon < abs(sequence.size()-len_client)) std::cerr << "no answer s " << abs((int)(sequence.size()-len_client)) << std::endl;
  ran_x = (int*)malloc(len_client*sizeof(int));
  if(ran_x == NULL) std::cerr << "error ranx" << std::endl;
  for(int i=0;i<len_client;i++) ran_x[i] = 0;
  ran_y = (int*)malloc(len_server*sizeof(int));
  if(ran_y == NULL) std::cerr << "error rany" << std::endl;
  for(int i=0;i<len_server;i++) ran_y[i] = 0;
  //ファイル入出力用配列の初期化
  queryarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*7*sizeof(Elgamal::CipherText));
  resultarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*6*sizeof(Elgamal::CipherText));
  if(queryarray == NULL || resultarray == NULL) std::cerr << "error elgamalarray1" << std::endl;
}

void PPDPP::Server::setLindex(int l,std::vector<std::pair<int,int>> &cells){
  lindex = cells[l].second();
}

void PPDPP::Server::makeParam(std::string& sparam){
  std::ofstream ofs(sparam.c_str(), std::ios::binary);
  ofs << sequence.size() << "\n";
}

void PPDPP::Server::readPubkey(std::string& pubFile){
  ROT::Load(pub, pubFile);
  std::cerr << "Server received pubKey" << std::endl;
}

void PPDPP::Server::parallelDP(std::string& queryfile,std::string& resultfile,std::vector<pair<int,int>> &cells){
  std::ifstream ifs(queryfile.c_str(), std::ios::binary);
  std::ofstream ofs(resultfile.c_str(), std::ios::binary);
  int loopnum = cells.size();
  for(int i=0;i<7*loopnum;i++){
    ifs >> queryarray[i];
  }
  for(int i=0;i<loopnum;i++){
    calcInnerProduct(queryarray+(7*i),resultarray+(6*i),cells[i].first(),cells[i].second());
  }
  for(int i=0;i<loopnum*6;i++){
    ofs << resultarray[i] << "\n";
  }
}

void PPDPP::Server::calcInnerProduct(Elgamal::CipherText *query,Elgamal::CipherText *ret,int turn_c,int turn_s){
  CipherTextVec queryvec(7);
  for(int i=0;i<7;i++) queryvec[i] = query[i];
  CipherTextVec ctv;
  Elgamal::CipherText tmp;
  resvec.resize(0);
  Zn rn;
  int rx = rand();
  int ry = rand();
#ifdef NRAND
  rx = ry = 0;
#endif
#ifdef DEBUG
  std::cerr << "ranx : " << ran_x[turn_c] << " rany : " << ran_y[turn_s] << std::endl;
#endif
  for(int i=0;i<6;i++){
    pub.enc(ret[i],0,rg);
    ctv = queryvec;
    for(int j=0;j<6;j++){
      int index = ( ( (3-ran_y[turn_s]+(6*i+j))%3 + (9-3*ran_x[turn_c]+((6*i+j)/3)*3))%9 + (36-9*dtoi(sequence[turn_s])+((6*i+j)/9)*9) ) % 36;
#ifdef DEBUG
      std::cerr << ((table[index]/3+rx)%3)*3 + (table[index] + ry)%3 << " ";
#endif
      ctv[j].mul(((table[index]/3+rx)%3)*3 + (table[index] + ry)%3);
    }
    for(int j=0;j<6;j++)  ret[i].add(ctv[j]);
#ifdef DEBUG
    std::cerr << std::endl;
    bool b;
    int iip = prv.dec(ip,&b);
    if(b) std::cout << "ip" << i << " : "<< iip << std::endl; 
#endif
    rn.setRand(rg);
    pub.enc(tmp,i,rg);
    tmp.neg();
    tmp.add(queryvec[6]);
    tmp.mul(rn);
    ret[i].add(tmp);
    resvec.push_back(ip);
#ifdef DEBUG
    iip = prv.dec(ret[i],&b);
    if(b) std::cout << "ip_ans" << i << " : "<< iip << std::endl; 
#endif
  }
  if(lindex = turn_s){
    ran_x_a = ran_x[turn_c];
    ran_y_a = ran_y[turn_s];
  }
  ran_x[turn_c] = rx % 3;
  ran_y[turn_s] = ry % 3;
}

void PPDPP::Server::addAnsVec(std::string& l_query){
  Elgamal::CipherText ip;
  pub.enc(ip,0,rg);
  std::ifstream ifs(l_query.c_str(), std::ios::binary);
  for(int i=0;i<36;i++){
    ifs >> l_queryvec[i];
  }
  for(int i=0;i<36;i++){
    l_queryvec[( ( (ran_x_a+i)%3 + (3*ran_y_a+(i/3)*3))%9 + (9*dtoi(sequence[lindex])+(i/9)*9) ) % 36].mul(l_table[i]);
  }
  for(int i=0;i<36;i++){
    ip.add(l_queryvec[i]);
  }
  ansvec.push_back(ip);
}

void PPDPP::Server::makeEditDFile(std::string& ans){
  Elgamal::CipherText editD;
  pub.enc(editD,0,rg);
  for(int i=0;i<ansvec.size();i++){
		#ifdef DEBUG
    bool b;
    std::cout << "ans" << i << " : "<< prv.dec(ansvec[i],&b) << std::endl; 
#endif
    editD.add(ansvec[i]);
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
  x = (*int)malloc(len_client*sizeof(int));
  y = (*int)malloc(len_server*sizeof(int));
  if(x == NULL || y == NULL) std::cerr << "error x,y" << std::endl;
  //入出力用配列の初期化
  queryarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*7*sizeof(Elgamal::CipherText));
  resultarray = (*Elgamal::CipherText)malloc(min3(epsilon,len_client,len_server)*6*sizeof(Elgamal::CipherText));
  if(queryarray == NULL || resultarray == NULL) std::cerr << "error array" << std::endl;
}

void PPDPP::Client::setLindex(int l,std::vector<std::pair<int,int>> &cells){
  lindex = cells[l].first();
}

void PPDPP::Client::makeParam(std::string& cparam){
  std::ofstream ofs(cparam.c_str(), std::ios::binary);
  ofs << sequence.size() << "\n";
  ofs << epsilon << "\n";
}

void PPDPP::Client::makeQuerySet(std::string& query,std::vector<pair<int,int>> &cells){
  std::ofstream ofs(query.c_str(), std::ios::binary);
  int loopnum = cells.size();
  for(int i=0;i<loopnum;i++){
    makeQuery(cells[i].first(),cells[i].second(),queryarray+(i*7));
  }
  for(int i=0;i<loopnum*7;i++){
    ofs << queryarray[i] << "\n";
  }
}

void PPDPP::Client::makeQuery(int turn_c,int turn_s,Elgamal::CipherText *queryvec){
  Elgamal::CipherText ct;
  int t = 3*x[turn_c]+y[turn_s]+9*dtoi(sequence[turn]);
#ifdef DEBUG
  std::cout << x[turn] << " " << y[turn_s] << " "<< t <<std::endl; 
#endif
  for(int i=0;i<6;i++){
    pub.enc(queryvec[i],(t%6)==i,rg);
  }
  pub.enc(queryvec[6],t/6,rg);
}

void PPDPP::Client::makeLQuery(std::string& l_query){
  Elgamal::CipherText ct;
  CipherTextVec queryvec;
  std::ofstream ofs(l_query.c_str(), std::ios::binary);
  int t = lx+3*ly+9*dtoi(sequence[lindex]);
  for(int i=0;i<36;i++){
    pub.enc(ct,t==i,rg);
    queryvec.push_back(ct);
  }
  for(int i=0;i<queryvec.size();i++){
    ofs << queryvec[i];
    ofs << "\n";
  }
}

void PPDPP::Client::decResultSet(std::string& result,std::vector<pair<int,int>> &cells){
  std::ifstream ifs(result.c_str(), std::ios::binary);
  int loopmain = cells.size();
  for(int i=0;i<6*loopmain;i++){
    ifs >> resultarray[i];
  }
  for(int i=0;i<loopmain;i++){
    decResult(resultarray+(i*6),cells[i].first(),cells[i].second());
  }
}

void PPDPP::Client::decResult(Elgamal::CipherText *resultvec,int turn_c,int turn_s){
  bool b;
  int ret = prv.dec(resultvec[(3*x[turn_c]+y[turn_s]+9*dtoi(sequence[turn_c]))/6], &b);
#ifdef DEBUG
  if(b) std::cerr << "ret : " << ret << std::endl;
  else std::cerr << "ret:error " << std::endl;
#endif
  if(lindex == turn_c){
    lx = x[turn_c];
    ly = y[turn_s];
  }
  x[turn_c] = ret / 3;
  y[turn_s] = ret % 3;
}

int PPDPP::Client::decEditD(std::string& Ans){
  Elgamal::CipherText ans;
  bool b;
  std::ifstream ifs(Ans.c_str(), std::ios::binary);
  ifs >> ans;
  int editD = prv.dec(ans, &b);
  if(!b) std::cerr << "error4" << std::endl;
  //std::cout << editD + sequence.size() - len_server << std::endl;
  return editD + sequence.size() - len_server;
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
