#include "ppdpp.h"

int PPDPP::dtoi(char c,int sigma){
  switch(sigma){
  case 2: return (c - '0');
  case 4:
    {
      switch(c){
      case 'A': return 0;
      case 'C': return 1;
      case 'G': return 2;
      case 'T': return 3;
      default : std::cerr << "error1 : " << c << std::endl;
	return -1;
      }
    }
  case 26: return (c - 'a');
  }
}

/*サーバ・クライアント両者が使用
  一回のイテレーションで計算するセルすべての座標をベクトルとして作成
  最後に足し合わせられるセルの位置を返す*/
int PPDPP::makePairVec(int turn,int cl,int sl,int threshold,std::vector< std::pair<int,int> >& cells, int &cells_len){
  int ret = -1;
  cells_len = 0;
  for(int i=0;i<=turn;i++){
    int j = turn-i;
    if(i>=cl || j>=sl || abs(i-j) >= threshold) continue;
    std::pair<int,int> p = std::make_pair(i,j);
    cells[cells_len++] = p;
#ifdef DEBUG
    std::cerr << "(" << i << "," << j << ") " << i+j << std::endl;
#endif
    //lindexの条件必要
    if(i == cl-1 || i-j == threshold-1) ret = cells_len-1;
  }
#ifdef DEBUG
  if(ret != -1) std::cerr << "lindex : " <<  "(" << cells[ret].first << "," << cells[ret].second << ") " << std::endl;
#endif
  
  return ret;
}

/*クライアント文字列の長さと閾値を受け取る
  計算に必要なベクトルのメモリを確保*/
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
  ansarray = (Elgamal::CipherText*)malloc(len_server*sizeof(Elgamal::CipherText));
  ran_x_a = (int*)malloc(len_server*sizeof(int));
  ran_y_a = (int*)malloc(len_server*sizeof(int));
  if(ansarray == NULL || ran_x_a == NULL || ran_y_a == NULL) std::cerr << "error elgamalarray1" << std::endl;
  tablevecsize = sigma * LBLOCK;
  querysize = floor(sqrt(tablevecsize));
  resultsize = tablevecsize / querysize + (tablevecsize % querysize != 0);
  tablesize = querysize * resultsize;
  querysize++;
  table = (int*)malloc(tablesize*sizeof(int));
  l_table = (int*)malloc(tablesize*sizeof(int));
  lqueryarray = (Elgamal::CipherText*)malloc(tablevecsize*len_server*sizeof(Elgamal::CipherText));
  queryarray = (Elgamal::CipherText*)malloc(mem*querysize*sizeof(Elgamal::CipherText));
  resultarray = (Elgamal::CipherText*)malloc(mem*resultsize*sizeof(Elgamal::CipherText));
  if(queryarray == NULL || resultarray == NULL || table == NULL || l_table == NULL || lqueryarray == NULL) std::cerr << "error table" << std::endl;
  for(int i=0;i<tablesize;i++){
    if(i < LBLOCK){
      table[i] = Table[i];
      l_table[i] = L_Table[i];
    }else if(i < tablevecsize){
      table[i] = Table[LBLOCK + i % LBLOCK];
      l_table[i] = L_Table[LBLOCK + i % LBLOCK];
    }else{
      table[i] = l_table[i] = 0;
    }
  }
}

/*最後に足し合わせるセルを記録しておく
  セルに対応する乱数を記録しておくため*/
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

/*一回のループ内で計算するセルに関する情報をすべて配列に書き出す
  各セル毎に並列でOTを行う*/
void PPDPP::Server::parallelDP(std::string& queryfile,std::string& resultfile,std::vector< std::pair<int,int> >& cells,int &cells_len){
  std::ifstream ifs(queryfile.c_str(), std::ios::binary);
  std::ofstream ofs(resultfile.c_str(), std::ios::binary);
  int loopnum = cells_len;
  for(int i=0;i<querysize*loopnum;i++){
    ifs >> queryarray[i];
  }
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopnum;i++){
    calcInnerProduct(queryarray+(querysize*i),resultarray+(resultsize*i),cells[i].first,cells[i].second);
  }
  for(int i=0;i<loopnum*resultsize;i++){
    ofs << resultarray[i] << "\n";
  }
}

/*OTを行い、セルのoutputを計算*/
void PPDPP::Server::calcInnerProduct(Elgamal::CipherText *query,Elgamal::CipherText *ret,int turn_c,int turn_s){
  CipherTextVec queryvec;
  queryvec.resize(querysize);
#ifdef DEBUG
  bool b;
  int iip;
#endif
  for(int i=0;i<querysize;i++){
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
  for(int i=0;i<resultsize;i++){
    pub.enc(ret[i],0,rg);
    ctv = queryvec;
    for(int j=0;j<(querysize-1);j++){
      int t_index = ( ( (SBLOCK-ran_y[index]+((querysize-1)*i+j))%SBLOCK + (LBLOCK-SBLOCK*ran_x[index]+(((querysize-1)*i+j)/SBLOCK)*SBLOCK))%LBLOCK + (tablevecsize-LBLOCK*dtoi(sequence[turn_s],sigma)+(((querysize-1)*i+j)/LBLOCK)*LBLOCK) ) % tablevecsize;
#ifdef DEBUG
      std::cerr << ((table[t_index]/SBLOCK+rx)%SBLOCK)*SBLOCK + (table[t_index] + ry)%SBLOCK << " ";
#endif
      ctv[j].mul(((table[t_index]/SBLOCK+rx)%SBLOCK)*SBLOCK + (table[t_index] + ry)%SBLOCK);
    }
    for(int j=0;j<(querysize-1);j++)  ret[i].add(ctv[j]);
#ifdef DEBUG
    std::cerr << std::endl;
    iip = prv.dec(ret[i],&b);
    if(b) std::cerr << "ip" << i << " : "<< iip << std::endl;
    else std::cerr << "error ret" << std::endl;
#endif
    rn.setRand(rg);
    pub.enc(tmp,i,rg);
    tmp.neg();
    tmp.add(queryvec[querysize-1]);
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
  if(turn_s != len_server-1) ran_x[turn_c*len_server+(turn_s+1)] = rx % SBLOCK;
  if(turn_c != len_client-1) ran_y[(turn_c+1)*len_server+turn_s] = ry % SBLOCK;
}

/*端の部分のセルの計算
 足し合わせて編集距離となる部分の計算*/
void PPDPP::Server::calcLInnerProduct(Elgamal::CipherText *lqueryvec,Elgamal::CipherText *lret,int s_index){
  pub.enc((*lret),0,rg);
  for(int i=0;i<tablevecsize;i++){
    lqueryvec[( ( (ran_x_a[s_index]+i)%SBLOCK + (SBLOCK*ran_y_a[s_index]+(i/SBLOCK)*SBLOCK))%LBLOCK + (LBLOCK*dtoi(sequence[s_index],sigma)+(i/LBLOCK)*LBLOCK) ) % tablevecsize].mul(l_table[i]);
  }
  for(int i=0;i<tablevecsize;i++){
    (*lret).add(lqueryvec[i]);
  }
}

/*編集距離計算に必要な端のセルの計算を並列で行う*/
void PPDPP::Server::makeEditDFile(std::string& l_query,std::string& ans){
  std::ifstream ifs(l_query.c_str(), std::ios::binary);
  for(int i=0;i<tablevecsize*len_server;i++){
    ifs >> lqueryarray[i];
  }
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<len_server;i++){
    calcLInnerProduct(lqueryarray+(i*tablevecsize),ansarray+i,i);
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
  
  if(len_client<len_server)
    prv.setCache(0, max((len_server+1)*2,10*sigma));
  else
    prv.setCache(0, max((len_client+1)*2,10*sigma));
  
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
  lx = (int*)malloc(len_server*sizeof(int));
  ly = (int*)malloc(len_server*sizeof(int));
  ll = (int*)malloc(len_server*sizeof(int));
  if(lx == NULL || ly == NULL || ll == NULL) std::cerr << "error array2" << std::endl;
  tablevecsize = sigma * LBLOCK;
  querysize = floor(sqrt(tablevecsize));
  resultsize = tablevecsize / querysize + (tablevecsize % querysize != 0);
  tablesize = querysize * resultsize;
  querysize++;
  lqueryarray = (Elgamal::CipherText*)malloc(tablevecsize*len_server*sizeof(Elgamal::CipherText));
  queryarray = (Elgamal::CipherText*)malloc(min3((epsilon*2+1),len_client,len_server)*querysize*sizeof(Elgamal::CipherText));
  resultarray = (Elgamal::CipherText*)malloc(min3((epsilon*2+1),len_client,len_server)*resultsize*sizeof(Elgamal::CipherText));
  if(queryarray == NULL || resultarray == NULL || lqueryarray == NULL) std::cerr << "error lqarray" << std::endl;
}

/*端のセルの記録*/
void PPDPP::Client::setLindex(int l,std::vector< std::pair<int,int> > &cells){
  if(l >= 0) lindex = cells[l].first;
  else lindex = -1;
}

void PPDPP::Client::makeParam(std::string& cparam){
  std::ofstream ofs(cparam.c_str(), std::ios::binary);
  ofs << sequence.size() << "\n";
  ofs << epsilon << "\n";
}

/*一回のloopで計算するセルすべて分のクエリをファイルにまとめる*/
void PPDPP::Client::makeQuerySet(std::string& query,std::vector< std::pair<int,int> >& cells,int &cells_len){
  std::ofstream ofs(query.c_str(), std::ios::binary);
  int loopnum = cells_len;
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopnum;i++){
    makeQuery(cells[i].first,cells[i].second,queryarray+(i*querysize));
  }
  for(int i=0;i<loopnum*querysize;i++){
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

/*各クエリをベクトルに書き込む*/
void PPDPP::Client::makeQuery(int turn_c,int turn_s,Elgamal::CipherText *queryvec){
  Elgamal::CipherText ct;
  int index = turn_c * len_server + turn_s;
  int t = SBLOCK*x[index]+y[index]+LBLOCK*dtoi(sequence[turn_c],sigma);
  for(int i=0;i<(querysize-1);i++){
    pub.enc(queryvec[i],(t%(querysize-1))==i,rg);
  }
  pub.enc(queryvec[querysize-1],t/(querysize-1),rg);
}

/*編集距離計算に必要な端のセルのクエリを並列でファイルにまとめる*/
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
    makeLQuery(lqueryarray+(i*tablevecsize),i);
  }
  for(int i=0;i<len_server*tablevecsize;i++){
    ofs << lqueryarray[i];
    ofs << "\n";
  }
}

/*端の各セルについてクエリを作成*/
void PPDPP::Client::makeLQuery(Elgamal::CipherText *lqueryvec,int index){
  int t = lx[index]+SBLOCK*ly[index]+LBLOCK*dtoi(sequence[ll[index]],sigma);
  for(int i=0;i<tablevecsize;i++){
    pub.enc(lqueryvec[i],t==i,rg);
  }
}

/*loop内で計算される各セルの計算結果を並列で復号する*/
void PPDPP::Client::decResultSet(std::string& result,std::vector< std::pair<int,int> >& cells,int &cells_len){
  std::ifstream ifs(result.c_str(), std::ios::binary);
  int loopmain = cells_len;
  for(int i=0;i<resultsize*loopmain;i++){
    ifs >> resultarray[i];
  }
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<loopmain;i++){
    decResult(resultarray+(i*resultsize),cells[i].first,cells[i].second);
  }
}

/*計算結果の復号*/
void PPDPP::Client::decResult(Elgamal::CipherText *resultvec,int turn_c,int turn_s){
  bool b;
  int index = turn_c * len_server + turn_s;
  int ret = prv.dec(resultvec[(SBLOCK*x[index]+y[index]+LBLOCK*dtoi(sequence[turn_c],sigma))/(querysize-1)], &b);
  if(lindex == turn_c){
    lx[turn_s] = x[index];
    ly[turn_s] = y[index];
    ll[turn_s] = lindex;
  }
  if(turn_s != len_server-1) x[turn_c*len_server+(turn_s+1)] = ret / SBLOCK;
  if(turn_c != len_client-1) y[(turn_c+1)*len_server+turn_s] = ret % SBLOCK;
}

/*暗号化された編集距離の復号*/
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

/*編集距離の計算(確認用)*/
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
