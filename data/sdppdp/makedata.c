#include <stdio.h>

int main(){
  FILE* rf[2];
  FILE* of;
  int i,j;
  double data[4];

  of = fopen("sdp.csv","w");

  rf[0] = fopen("sdp/sdp_c_1.dat","r");
  rf[1] = fopen("sdp/sdp_s_1.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c1s1,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf",&(data[3]));
    fscanf(rf[0],"%lf",&(data[1]));
    fscanf(rf[0],"%lf",&(data[2]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");

  rf[0] = fopen("sdp/sdp_c_2.dat","r");
  rf[1] = fopen("sdp/sdp_s_8.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c2s4,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf",&(data[3]));
    fscanf(rf[0],"%lf",&(data[1]));
    fscanf(rf[0],"%lf",&(data[2]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");

  rf[0] = fopen("sdp/sdp_c_4.dat","r");
  rf[1] = fopen("sdp/sdp_s_16.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c4s16,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf",&(data[3]));
    fscanf(rf[0],"%lf",&(data[1]));
    fscanf(rf[0],"%lf",&(data[2]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");



  of = fopen("ppdp.csv","w");

  rf[0] = fopen("ppdp/ppdp_c_1.dat","r");
  rf[1] = fopen("ppdp/ppdp_s_1.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c1s1,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf,%lf,%lf",&(data[1]),&(data[2]),&(data[3]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");

  rf[0] = fopen("ppdp/ppdp_c_2.dat","r");
  rf[1] = fopen("ppdp/ppdp_s_8.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c2s8,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf,%lf,%lf",&(data[1]),&(data[2]),&(data[3]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");

  rf[0] = fopen("ppdp/ppdp_c_4.dat","r");
  rf[1] = fopen("ppdp/ppdp_s_16.dat","r");
  fprintf(of,"s_time,ctime,t_time,comsize,c4s16,\n");
  for(i=0;i<10;i++){
    fscanf(rf[0],"%lf,%lf,%lf",&(data[1]),&(data[2]),&(data[3]));
    fscanf(rf[1],"%lf",&(data[0]));
    for(j=0;j<4;j++){
      fprintf(of,"%lf,",data[j]);
    }
    fprintf(of,"\n");
  }
  fprintf(of,"\n");


  return 0;
}
