#include "rot.h"

//#define DEBUG
//#define NRAND

extern cybozu::RandomGenerator rg;

#define max(a,b) ((a>b) ? a : b)
#define min(a,b) ((a>b) ? b : a)
#define min3(a,b,c) ((min(a,b) > c) ? c : min(a,b))
#define abs(a) (((a) > 0) ? (a) : (-(a)))

namespace PPDPP{

	int dtoi(char c);

	const int l_table[36] = {2,1,0,2,1,0,2,1,0,2,1,0,2,2,1,2,2,1,2,1,0,2,2,1,2,2,1,2,1,0,2,2,1,2,2,1};
	const int table[36] = {8,5,2,7,4,1,6,3,0,8,5,2,7,8,5,6,7,4,8,5,2,7,8,5,6,7,4,8,5,2,7,8,5,6,7,4};

	int edit_check(std::string &s,std::string &c);

	int makePairVec(int turn,int threshold,std::vector<std::pair<int,int>> &cells); //総和と閾値を入力し、位置ペアのベクトルを返す.lindexのペアの番号を返す.

	class Server{
		Elgamal::PublicKey pub;
		Elgamal::CipherText *queryarray; //入出力ベクトルのメモリ確保に関しては要検証
		Elgamal::CipherText *resultarray;
		CipherTextVec l_queryvec;
		std::string sequence;
		int *ran_x;
		int *ran_y;
		int ran_x_a;
		int ran_y_a;
		ChiperTextVec ansvec;
	public:
		int core;
		int len_client;
		int len_server;
		int epsilon;
		int lindex;
		void setParam(std::string& cparam); //文字列長,閾値セット
		void makeParam(std::string& sparam); //相手側,文字列長セット,乱数メモリ確保と初期化,配列メモリ確保
		void readPubkey(std::string& pubfile);
		void loopmain(int turn,std::string& queryfile,std::string& resultfile); //map生成→paraDP→(addAnsV). M+N-1回呼び出し
		void parallelDP(std::string& queryfile,std::string& resultfile,); //入力ファイルと通信回数を受け取り,出力ファイル生成.必要な位置をマップで確保.lindexのセットも.
		void calcInnerProduct(Elgamal::CipherText *query,Elgamal::CipherText *ret,int turn_c,int turn_s); //parallelDPから呼び出される.セルの位置と入力ベクトルを渡され出力ベクトルを返す.
		void addAnsVec(std::string& lqueryfile); //セルの位置はlindexで知らせる.
		void makeEditDFile(std::string& ans); //返答用ベクトルの要素を足しこんでファイルに書き込み.
		Server(std::string &str){
			sequence = str;
			l_queryvec.resize(36);
			std::cerr << "make server" << std::endl;
			len_server = sequence.size();
		}
		#ifdef DEBUG
			Elgamal::PrivateKey prv;
		#endif
	};

	class Client{
		std::string sequence;
		int lx,ly;
		int *x,*y;
		Elgamal::CipherText *queryarray;
		Elgamal::CipherText *resultarray;
		Elgamal::PublicKey pub;
		Elgamal::PrivateKey prv;
	public:
		int core;
		int len_client;
		int len_server;
		int epsilon;
		int lindex;
		void setKeys(std::string& prvFile, std::string& pubFile);
		void setParam(std::string& sparam); //文字列長セット,閾値セット
		void makeParam(std::string& cparam); //相手側文字列セット,x,yメモリ確保と初期化,配列メモリ確保
		void loopmain(int turn,std::string& query,std::string& result); //map生成→makeQS→decRS→(makeLQ)
		void makeQuerySet(std::string& query,std::vector<pair<int,int> &cells>); //必要なセルの位置をマップで確保.
		void makeQuery(int turn_c,int turn_s,Elgamal::CipherText *queryvec); //makeQuerySetから呼び出される.格納先のベクトルのメモリ確保は要検証
		void makeLQuery(std::string& l_query);
		void decResultSet(std::string& result,std::vector<pair<int,int> &cells>);
		void decResult(Elgamal::CipherText *resultvec,int turn_c,int turn_s); //decResultSetからの呼び出し.位置を知らせてx,yを更新する 
		int decEditD(std::string& Ans);
		Client(std::string &str){
			sequence = str;
			std::cerr << "make client" << std::endl;
			epsilon = 0;
			len_client = sequence.size();
		}
	};
}
