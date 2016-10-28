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

	//総和と閾値を入力し、位置ペアのベクトルを返す.lindexのペアの番号を返す.
	int makePairVec(int turn,int cl,int sl,int threshold,std::vector< std::pair<int,int> >& cells);

	class Server{
		Elgamal::PublicKey pub;
		Elgamal::CipherText *queryarray;
		Elgamal::CipherText *resultarray;
		Elgamal::CipherText *lqueryarray;
		Elgamal::CipherText *ansarray;
		std::string sequence;
		int *ran_x;
		int *ran_y;
		int *ran_x_a;
		int *ran_y_a;
	public:
		int core;
		int len_client;
		int len_server;
		int epsilon;
		int lindex;
		//文字列長,閾値セット
		void setParam(std::string& cparam);
		void setLindex(int l,std::vector< std::pair<int,int> >& cells);
		//相手側,文字列長セット,乱数メモリ確保と初期化,配列メモリ確保
		void makeParam(std::string& sparam);
		void readPubkey(std::string& pubfile);
		//入力ファイルと通信回数を受け取り,出力ファイル生成.必要な位置をマップで確保.lindexのセットも.
		void parallelDP(std::string& queryfile,std::string& resultfile,std::vector< std::pair<int,int> >& cells);
		//parallelDPから呼び出される.セルの位置と入力ベクトルを渡され出力ベクトルを返す.
		void calcInnerProduct(Elgamal::CipherText *query,Elgamal::CipherText *ret,int turn_c,int turn_s);
		void makeEditDFile(std::string& lqueryfile,std::string& ans); //返答用ベクトルの要素を足しこんでファイルに書き込み
		void calcLInnerProduct(Elgamal::CipherText *lqueryvec,Elgamal::CipherText *lretvec,int s_index);
		Server(std::string &str){
			sequence = str;
			std::cerr << "make server" << std::endl;
			len_server = sequence.size();
		}
#ifdef DEBUG
		Elgamal::PrivateKey prv;
#endif
	};

	class Client{
		std::string sequence;
		int *lx,*ly,*ll;
		int *x,*y;
		Elgamal::CipherText *queryarray;
		Elgamal::CipherText *resultarray;
		Elgamal::CipherText *lqueryarray;
		Elgamal::PublicKey pub;
		Elgamal::PrivateKey prv;
	public:
		int core;
		int len_client;
		int len_server;
		int epsilon;
		int lindex;
		void setKeys(std::string& prvFile, std::string& pubFile);
		//文字列長セット,閾値セット
		void setParam(std::string& sparam);
		void setLindex(int l,std::vector< std::pair<int,int> >& cells);
		//相手側文字列セット,x,yメモリ確保と初期化,配列メモリ確保
		void makeParam(std::string& cparam);
		//必要なセルの位置をマップで確保.
		void makeQuerySet(std::string& query,std::vector< std::pair<int,int> >& cells); 
		//makeQuerySetから呼び出される.
		void makeQuery(int turn_c,int turn_s,Elgamal::CipherText *queryvec);
		void makeLQuerySet(std::string& l_query);
		void makeLQuery(Elgamal::CipherText *lqueryvec,int index);//indexはlx,ly,llの位置
		void decResultSet(std::string& result,std::vector< std::pair<int,int> >& cells);
		//decResultSetからの呼び出し.位置を知らせてx,yを更新する
		void decResult(Elgamal::CipherText *resultvec,int turn_c,int turn_s);
		int decEditD(std::string& Ans);
		Client(std::string &str){
			sequence = str;
			std::cerr << "make client" << std::endl;
			epsilon = 0;
			len_client = sequence.size();
		}
	};
}
