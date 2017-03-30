#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double ans[10000][10];
double ans1[10000][10];
double t1, t2;
bool flag;
int main(){
    freopen("TMNR-machine2-rev.txt", "w", stdout);
    FILE *fp;
    int k = 0;
    if( (fp=fopen("TMNR-1-30-1000-nsga2-machine2-全部原解.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            flag = true;
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf],", &t1, &t2);
                for(int i = 0 ; i < k ; i ++) {
                    if(t1 == ans[i][0] && t2 == ans[i][1]) {
                        flag = false;
                        break;
                    }
                }
                if(flag) {
                    sscanf(str,"[%lf,%lf],", &ans[k][0], &ans[k][1]);
                    ans[k][0] = t1;
                    ans[k][1] = t2;
                    k++;
                }
            }
        }
        fclose(fp);
    }
    int k1 = 0;
    if( (fp=fopen("TMNR-1-30-1000-machine2.txt","rt"))!=NULL) {
        while( !feof(fp)) {
            fscanf(fp,"%s",str);
            flag = true;
            if(str[0] == '['){
                sscanf(str,"[%lf,%lf],", &t1, &t2);
                for(int i = 0 ; i < k1 ; i ++) {
                    if(t1 == ans1[i][0] && t2 == ans1[i][1]) {
                        flag = false;
                        break;
                    }
                }
                if(flag) {
                    ans1[k1][0] = t1;
                    ans1[k1][1] = t2;
                    k1++;
                }
            }
        }
        fclose(fp);
    }

    for(int i = 0 ; i < k ; i ++){
        printf("i = %d:  ", i);
        for(int j = 0 ; j < 2 ; j ++){
            printf("%.2lf ", ans[i][j]);
        }
        printf("\n");
    }
    for(int i = 0 ; i < k1 ; i ++){
        printf("i = %d:", i);
        for(int j = 0 ; j < 2 ; j ++){
            printf("%.2lf ", ans1[i][j]);
        }
        printf("\n");
    }
        printf("k1 = %d k = %d\n", k1, k);
    int all = 0;
    for(int i = 0 ; i < k1 ; i ++){
        for(int j = 0 ; j < k ; j ++){
            if(ans[j][0] <= ans1[i][0] && ans[j][1] <= ans1[i][1]){
                if(ans1[i][0] != ans[j][0] || ans1[i][1] != ans[j][1]){
                    all ++;
                    printf("%d支配%d\n", j , i);
                    break;
                }
            }
        }
    }
    printf("all = %d k1 = %d\n", all, k1);
    double ret = all * 1.0 / k1 * 1.0;
    printf("c = %.2lf\n", ret);
    return 0;
}

