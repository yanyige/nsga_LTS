#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
double ans[10000][10];
double ans1[10000][10];
double t1, t2, t3;
double t4, t5, t6;
double _t1, _t2, _t3;
double _t4, _t5, _t6;
int all = 0;
bool flag;
int main(){
    freopen("GAO-MACHINE4-RAND3.txt", "w", stdout);
    FILE *fp;
    int k = 0;
    t1 = t2 = t3 = t4 = t5 = t6 = 0;
    all = 0;
    if( (fp=fopen("fine-grainedH264-3-30-400-MOHA-3object-MACHINE4-3600s-RAND3.txt","rt"))!=NULL) {
        while( !feof(fp)) {
//            fscanf(fp,"%s",str);
            fgets(str,200,fp);
            flag = true;
            _t1 = _t2 = _t3 = _t4 = _t5 = _t6 = 0;
//            printf("%s\n", str);
//            printf("%c\n", str[12]);
            if(str[12] == 'e'){
                sscanf(str,"computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf", &_t1, &_t2, &_t3);
//                printf("%lf\n", _t1);
                all ++;
            }
            if(str[12] == 't') {
                sscanf(str,"computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf", &_t4, &_t5, &_t6);
            }
            t1 += _t1;
            t2 += _t2;
            t3 += _t3;
            t4 += _t4;
            t5 += _t5;
            t6 += _t6;
        }
        fclose(fp);
        printf("computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf\n", t1 / all, t2 / all, t3 / all);
        printf("computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf\n", t4 / all, t5 / all, t6 / all);
    }
    return 0;
}

