#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double ans[10000][10];
double ans1[10000][10];
double t1, t2,t3;
bool flag;
int main(){
    freopen("h264-10-40.txt", "w", stdout);
    FILE *fp;
    int k = 0;
    if( (fp=fopen("h264_14-1-30-400-MACHINE4-3object-10.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            flag = true;
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf,%lf],", &t1, &t2,&t3);
                for(int i = 0 ; i < k ; i ++) {
                    if(t1 == ans[i][0] && t2 == ans[i][1]&& t3 == ans[i][2]) {
                        flag = false;
                        break;
                    }
                }
                if(flag) {
                    //sscanf(str,"[%lf,%lf,%lf],", &ans[k][0], &ans[k][1],&ans[k][2]);
                    ans[k][0] = t1;
                    ans[k][1] = t2;
                    ans[k][2] = t3;
                    k++;
                }
            }
        }
        fclose(fp);
    }
    int k1 = 0;
    if( (fp=fopen("h264_14-1-30-400-MACHINE4-3object-40.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            flag = true;
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf,%lf],", &t1, &t2,&t3);
                for(int i = 0 ; i < k1 ; i ++) {
                    if(t1 == ans1[i][0] && t2 == ans1[i][1]&& t3 == ans1[i][2]) {
                        flag = false;
                        break;
                    }
                }
                if(flag) {
                    ans1[k1][0] = t1;
                    ans1[k1][1] = t2;
                    ans1[k1][2] = t3;
                    k1++;
                }
            }
        }
        fclose(fp);
    }

    for(int i = 0 ; i < k ; i ++){
        printf("i = %d:  ", i);
        for(int j = 0 ; j < 3 ; j ++){
            printf("%.2lf ", ans[i][j]);
        }
        printf("\n");
    }
    for(int i = 0 ; i < k1 ; i ++){
        printf("i = %d:", i);
        for(int j = 0 ; j < 3 ; j ++){
            printf("%.2lf ", ans1[i][j]);
        }
        printf("\n");
    }
        printf("k1 = %d k = %d\n", k1, k);
    int all1 = 0,all2=0;
    for(int i = 0 ; i < k1 ; i ++){
        for(int j = 0 ; j < k ; j ++){
            if(ans[j][0] <= ans1[i][0] && ans[j][1] <= ans1[i][1]&& ans[j][2] <= ans1[i][2]){
                if(ans1[i][0] != ans[j][0] || ans1[i][1] != ans[j][1]|| ans1[i][2] != ans[j][2]){
                    all1 ++;
                    printf("%d÷ß≈‰%d\n", j , i);
                    break;
                }
            }
            if(ans[j][0] >= ans1[i][0] && ans[j][1] >= ans1[i][1]&& ans[j][2] >= ans1[i][2]){
                if(ans1[i][0] != ans[j][0] || ans1[i][1] != ans[j][1]|| ans1[i][2] != ans[j][2]){
                    all2 ++;
                    printf("%d÷ß≈‰%d\n", i , j);
                    break;
                }
            }
        }
    }
    printf("all1 = %d k1 = %d\n", all1, k1);
    double ret1 = all1 * 1.0 / k1 * 1.0;
    printf("c1 = %.2lf\n", ret1);
    printf("all2 = %d k = %d\n", all2, k);
    double ret2 = all2 * 1.0 / k1 * 1.0;
    printf("c2 = %.2lf\n", ret2);
    return 0;
}

