#include <stdio.h>

int main(){
  FILE* rf[4];
  FILE* of[4];
  int i,j,k;
  double ctr[10][3];
  double ectr[10][3];
  double str[10];
  double estr[10];
  double in[3];

  for(i=0;i<10;i++){
    str[i] = estr[i] = 0;
    for(j=0;j<3;j++){
      ctr[i][j] = ectr[i][j] = 0;
    }
  }

  rf[0] = fopen("ctr.dat","r");
  rf[1] = fopen("ectr.dat","r");
  rf[2] = fopen("str.dat","r");
  rf[3] = fopen("estr.dat","r");

  for(i=0;i<5;i++){
    for(j=0;j<10;j++){
      fscanf(rf[0],"%lf,%lf,%lf",&(in[0]),&(in[1]),&(in[2]));
      for(k=0;k<3;k++){
	ctr[j][k] += in[k];
      }
      fscanf(rf[1],"%lf,%lf,%lf",&(in[0]),&(in[1]),&(in[2]));
      for(k=0;k<3;k++){
	ectr[j][k] += in[k];
      }
    }
    fscanf(rf[2],"%lf",&(in[0]));
    str[j] += in[0];
    fscanf(rf[3],"%lf",&(in[0]));
    estr[j] += in[0];
  }

  for(i=0;i<10;i++){
    for(j=0;j<3;j++){
      ectr[i][j] /= 5;
      ctr[i][j] /= 5;
      //      printf("%lf ",ectr[i][j]/5);
    }
    //printf("\n");
    estr[i] /= 5;
    str[i] /= 5;
  }

  rf[0] = fopen("totaltime.dat","w");
  rf[1] = fopen("e_totaltime.dat","w");
  rf[2] = fopen("comsize.dat","w");
  rf[3] = fopen("e_comsize.dat","w");

  for(i=0;i<10;i++){
    fprintf(rf[0],"%d %lf\n",(i+1)*10,ctr[i][1]);
    fprintf(rf[1],"%d %lf\n",(i+1)*10,ectr[i][1]);
    fprintf(rf[2],"%d %lf\n",(i+1)*10,ctr[i][2]);
    fprintf(rf[3],"%d %lf\n",(i+1)*10,ectr[i][2]);
  }

  return 0;
}
