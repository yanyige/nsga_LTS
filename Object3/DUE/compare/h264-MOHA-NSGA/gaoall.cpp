#include<stdio.h>
#include<string.h>

const int inf = 0x3f3f3f3f;
char str[100000];
char fileName[100000];
double ans[10000][10];
double ans1[10000][10];
double t1, t2, t3;
double t4, t5, t6;
double _t1, _t2, _t3;
double _t4, _t5, _t6;
int all = 0;
bool flag;
int main(){
    freopen("GAO-NSGA2-MACHINE16.txt", "w", stdout);

    int k = 0;
    t1 = t2 = t3 = t4 = t5 = t6 = 0;
    all = 0;
    int myrand = 1;
    int machine = 16;
    char _machine[300];
    char _rand[300];
    for(int i = 1 ; i <= 10 ; i ++) {
        strcpy(fileName, "");
        strcat(fileName, "fine-grainedH264-");
        sprintf(_rand, "%d", i);
        strcat(fileName, _rand);
        strcat(fileName, "-30-400-NSGA2-3object-MACHINE");
        sprintf(_machine, "%d", machine);
        strcat(fileName, _machine);
        strcat(fileName, "-3600s-RAND");

        strcat(fileName, _rand);
        strcat(fileName, ".txt");
        printf("%s\n", fileName);

        FILE *fp;
        if( (fp=fopen(fileName,"rt"))!=NULL) {
            while( !feof(fp)) {
    //            fscanf(fp,"%s",str);
                fgets(str,400,fp);
                flag = true;
                _t1 = _t2 = _t3 = _t4 = _t5 = _t6 = 0;
//                printf("%s\n", str);
    //            printf("%c\n", str[12]);
                if(str[0] == 'A' && str[21] == 'e'){
                    sscanf(str,"AVERAGE: computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf", &_t1, &_t2, &_t3);
//                    printf("%lf\n", _t1);
                    all ++;
                }
                if(str[0] == 'A' && str[21] == 't') {
                    sscanf(str,"AVERAGE: computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf", &_t4, &_t5, &_t6);
                }
                t1 += _t1;
                t2 += _t2;
                t3 += _t3;
                t4 += _t4;
                t5 += _t5;
                t6 += _t6;
//                printf("t1=%lf\n", t1);
            }
            fclose(fp);

        }

    }
    printf("all=%d\n", all);
    printf("computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf\n", t1 / all, t2 / all, t3 / all);
    printf("computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf\n", t4 / all, t5 / all, t6 / all);




    return 0;
}

