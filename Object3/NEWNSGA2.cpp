#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<algorithm>
#include<vector>
#include<math.h>
#include<set>
#include<stack>
#include<time.h>
// in linux it's #include<sys/times.h>
using namespace std;

const int MAXN = 270;
const int MAXM = 270;
int m;
int n;
const int inf = 0x3f3f3f3f;
const int cycle=3;//������
const double Rdk=0.4;//��������������
const double Rsk=0.15;//������˯������
const double Rik=0.16;//��������������
const double Rb=0.2;//��������
const double cominitcost=2;
const double t0=200.0;//ʱ����ֵ
const double t_penalty=25;//ʱ��ͷ�
const double extra_E=1;//�ܺĳͷ�
int mycycle;
clock_t start, finish;
//in linux it's tms start finish
double start_time;
double real_time;
double exe_time;
//*******
//**pop - Population size
//**gen - Total number of generations
int pop, gen;
int isdep[cycle][MAXN];//��i�����ĸ���
int todep[cycle][MAXN];//i������������
int taskIndex[MAXN];// ��¼ÿ��������ƥ�䵽��λ��
bool taskUsed[MAXN];
set<int> doneSet;

/****************************************
Ŀ�꺯��   Minimize communication cost
Ŀ�꺯��   Minimize the balance of processor workload
Ŀ�꺯��   Minimize the Energy
t(m,n)     Execution time of a task on a processor
c(m,n)     Communication cost between two tasks
****************************************/


double t[MAXM][MAXN];
struct time_table
{
    double time;
    int id;
} time_in_machine[MAXN];

double c[MAXN][MAXN];
bool uesd[MAXN];

struct Individual
{
    vector<int> machine[MAXM]; //save permutation of the tasks
    double workload; //object 1
    double makespan; //object 2
    double energy;  //object 3
    double Ed;
    double Ei;
    double Es;
    double comtime;
    double idltime;
    double sleeptime;
    int front; //rank of domination
    vector< int > S; //the collections dominated by this Individual
    int n;// count of dominated solution
    double dfitness; //fitness
    double crowd_distance;
} Collection[MAXN], new_individual;

vector<Individual> Front[MAXN];
vector<Individual> ElistCollection;
//����
int cmp(const void *a, const void *b)
{
    return (*(Individual *)a).workload > (*(Individual *)b).workload ? 1:-1;
}
//����
int cmp1(const void *a, const void *b)
{
    return (*(Individual *)a).makespan > (*(Individual *)b).makespan ? 1:-1;
}
int cmp0(const void *a, const void *b)
{
    return (*(Individual *)a).energy > (*(Individual *)b).energy ? 1:-1;
}
//����
int cmp2(const void *a, const void *b)
{
    return (*(Individual *)a).crowd_distance > (*(Individual *)b).crowd_distance ? -1:1;
}
//����
int cmp3(const void *a, const void *b)
{
    return (*(time_table *)a).time > (*(time_table *)b).time ? -1 : 1;
}
//�����滻
void copy_individual(Individual *i, Individual *j)
{
    vector<int>:: iterator iter;
    for(int ii = 0 ; ii < m ; ii ++)
    {
        i->machine[ii].erase(i->machine[ii].begin(), i->machine[ii].end());
//    }
//    for(int ii = 0 ; ii < m ; ii ++)
//    {
        for(iter = j->machine[ii].begin(); iter != j->machine[ii].end(); ++ iter)
        {
            i->machine[ii].push_back((*iter));
        }
    }
}

double abs(double t)
{
    return t>0?t:-t;
}

void evaluate_objective(Individual *i)
{
    vector<int>:: iterator iter;
    vector<int>:: iterator jter;

    i->n = inf;
    i->makespan = 0;
    i->workload = 0;
    i->energy=0;
    i->Ed=0;
    i->Ei=0;
    i->Es=0;
    i->comtime=0;
    i->idltime=0;
    i->sleeptime=0;
    double depcopy[cycle][MAXN][MAXN];
    double Ed=0,Eis=0,Eb=0,Ei=0,Es=0;
    double comtime=0,idltime=0,sleeptime=0;
    int isdepcopy[cycle][MAXN],todepcopy[cycle][MAXN];
    for(int k=0; k<mycycle; k++)
    {
        for(int ii=0; ii<n; ii++)
        {
            isdepcopy[k][ii]=isdep[k][ii];
            todepcopy[k][ii]=todep[k][ii];
            for(int j=0; j<n; j++)
            {
                depcopy[k][ii][j]=c[ii][j];
            }
        }
    }
    double load[MAXM];
    memset(load, 0, sizeof(load));
    int jud_index[MAXM],flag_end[MAXM],flag_com[MAXM],position;
    memset(jud_index,0,sizeof(jud_index));
    memset(flag_end,0,sizeof(flag_end));
    double E[MAXM];
    memset(E,0,sizeof(E));
    double tao[cycle][MAXN];
    double taoend[cycle][MAXN];
    double Lstart,ltemp;
    double idletime=0;
    int it=1,task_in_machine[MAXN],flag;
    double avg = 0,workload=0;
    for(int j = 0 ; j < n ; j ++)
    {
        avg += t[0][j];
    }
    i->comtime=avg*mycycle;
    avg /= m;
    for(int j = 0 ; j < m ; j ++)
    {
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); ++ iter)
        {
            task_in_machine[*iter]=j;
            load[j]=load[j]+t[j][(*iter)];

        }
        workload+= abs(load[j]-avg);
        Ed+=load[j]*Rdk*mycycle;//���������ѵ�һ����
        flag_com[j]=0;
    }
    i->workload=workload;
    while(it<=n*mycycle)
    {
        for(int ii=0; ii<m; ii++)
        {
            flag_com[ii]=0;
            if(flag_end[ii]==mycycle||i->machine[ii].empty()) {
                continue;
            }
            position=i->machine[ii].at(jud_index[ii]);
            if(isdepcopy[flag_end[ii]][position]==0)
            {

                Lstart=E[ii];
                for(int kk=0; kk<n; kk++)
                {   //��Ҫͨ��
                    if((c[kk][position]>0)&&(task_in_machine[kk]!=task_in_machine[position]))
                    {   flag_com[ii]=1;
                        Eb+=c[kk][position]*Rb;
                        ltemp=taoend[flag_end[ii]][kk]+c[kk][position]+2;
                        if(ltemp>Lstart)
                        {
                            Lstart=ltemp;
                        }
                    }
                }
                idletime=Lstart-E[ii];
                //������˯���ܺ�
                if(idletime!=0){
                    if(idletime<t0){
                        Eis+=Rik*idletime;
                        Ei+=Rik*idletime;
                        idltime+=idletime;
                    }
                    else{
                        Eis+=Rsk*(idletime-t_penalty)+extra_E;
                        Es+=Rsk*(idletime-t_penalty)+extra_E;
                        //idltime+=t0;
						sleeptime+=idletime;//�л�ʱ��Ҳ����sleep��
                    }
                }
                tao[flag_end[ii]][position]=Lstart;
                E[ii]=tao[flag_end[ii]][position]+t[ii][position];
                taoend[flag_end[ii]][position]=E[ii];
                for(int kk=0;kk<n;kk++){
                    if(c[position][kk]>0 && (task_in_machine[kk] != task_in_machine[position])){
                        E[ii]+=2;
                        break;
                    }
                }
                it++;
                for(int jj=0; jj<n; jj++)
                {
                    if(depcopy[flag_end[ii]][position][jj]>0&&position!=jj)
                    {

                        depcopy[flag_end[ii]][position][jj]=0.0;
                        isdepcopy[flag_end[ii]][jj]--;
                        todepcopy[flag_end[ii]][position]--;
                    }
                }
                if(position==(*(i->machine[ii].rbegin()))) //
                {
                    flag_end[ii]++;
                    jud_index[ii]=0;
                }
                else
                {
                    jud_index[ii]++;
                }

            }
            if(1==flag_com[ii]){
                Ed+=cominitcost*Rdk;//�����ѭ���˾Ͳ���Ҫ��mycycle�ˡ����������ѵ���һ����
            }
        }
    }

    for(int jj=0; jj<m; jj++)
    {
        if(E[jj]>i->makespan)
        {
            i->makespan=E[jj];
        }
    }
    //double tempsleep=0;
    for(int jj=0; jj<m; jj++)
    {
        if(i->makespan-E[jj]<t0){
            idltime+=i->makespan-E[jj];
            Eis+=Rik*(i->makespan-E[jj]);
            Ei+=Rik*(i->makespan-E[jj]);
        }
        else{
            sleeptime+=i->makespan-E[jj];
            Eis+=Rsk*(i->makespan-E[jj]-t_penalty)+extra_E;
            Es+=Rsk*(i->makespan-E[jj]-t_penalty)+extra_E;
        }

       //tempsleep+=i->makespan-E[jj];
    }

    i->energy=Ed+Eis+Eb;
    i->Ed=Ed;
    i->Ei=Ei;
    i->Es=Es;
    //i->comtime=i->makespan-idltime-sleeptime;
    i->idltime=idltime;
    i->sleeptime=sleeptime;
    }

// ��֧������
void non_domination_sort(Individual individuals[], int length, bool last)
{
    vector< Individual > frontCollection;
    vector< Individual > tempCollection;
    int rank = 1;
    for(int i = 0 ; i < length ; i ++)
    {
        //initial
        individuals[i].S.erase(individuals[i].S.begin(), individuals[i].S.end());
        individuals[i].n = 0;
        for(int j = 0 ; j < length ; j ++)
        {
            if(i!=j)
            {
                if(!last) {//last��Ϊ��֧ͬ���ϵ���ж����涨ִֻ��else����
                    // if individual[i] dominate individual[j]
                    if(individuals[i].makespan < individuals[j].makespan && individuals[i].workload < individuals[j].workload && individuals[i].energy < individuals[j].energy)
                    {
                        // let individual[j] added to the S of the individual[i]
                        individuals[i].S.push_back(j);
                    }
                    else if(individuals[j].makespan < individuals[i].makespan && individuals[j].workload < individuals[i].workload && individuals[j].energy < individuals[i].energy)
                    {
                        individuals[i].n = individuals[i].n + 1;
                    }
                }
                else {
                    // if individual[i] dominate individual[j]
                    if(individuals[i].makespan <= individuals[j].makespan && individuals[i].workload <= individuals[j].workload&&individuals[i].energy <= individuals[j].energy){
                        // let individual[j] added to the S of the individual[i]
                        if(individuals[i].makespan == individuals[j].makespan && individuals[i].workload == individuals[j].workload && individuals[i].energy == individuals[j].energy){
                            continue;
                        }
                        individuals[i].S.push_back(j);

                    }else if(individuals[j].makespan <= individuals[i].makespan && individuals[j].workload <= individuals[i].workload&& individuals[j].energy <= individuals[i].energy){
                        if(individuals[i].makespan == individuals[j].makespan && individuals[i].workload == individuals[j].workload&& individuals[i].energy == individuals[j].energy){
                            continue;
                        }
                        individuals[i].n = individuals[i].n + 1;
                    }
                }
            }
        }
    }
    for(int i = 0 ; i < MAXN ; i ++)
    {
        vector<Individual>().swap(Front[i]);
    }
    for(int i = 0 ; i < length ; i ++)
    {
        if(individuals[i].n == 0)
        {
            individuals[i].front = rank;
            frontCollection.push_back(individuals[i]);
            Front[rank].push_back(individuals[i]);
        }
    }

    while(!frontCollection.empty())
    {
        tempCollection.erase(tempCollection.begin(), tempCollection.end());
        for(vector<Individual>::iterator iter = frontCollection.begin() ; iter != frontCollection.end() ; ++iter)
        {
            for(vector<int>::iterator jter = (*iter).S.begin() ; jter != (*iter).S.end() ; ++jter)
            {
                individuals[(*jter)].n = individuals[(*jter)].n - 1;
                if(individuals[(*jter)].n == 0)
                {
                    individuals[(*jter)].front = rank + 1;
                    Front[rank+1].push_back(individuals[(*jter)]);
                    tempCollection.push_back(individuals[(*jter)]);
                }
            }
        }
        rank++;
        frontCollection.erase(frontCollection.begin(), frontCollection.end());
        frontCollection.assign(tempCollection.begin(), tempCollection.end());
    }
}

//һ�����������������壬һ����������һ������Ӣѡһ��
void gacrossover(int target1, int target2, Individual *individual) //��ѡ�����q��������н���
{
//    printf("crossover\n");
    int myMap[MAXN][2];//��¼һ���������������
    vector<int>:: iterator iter;
	Individual ind1,ind2;
//	for(int j=0; j<m;++j){
//                printf("~machine %d: ",j);
//			for(iter=ind1.machine[j].begin();iter!=ind2.machine[j].end();iter++){
//				printf("%d ",(*iter));
//			}
//			printf("\n");
//        }
    for(int j = 0 ; j < m ; ++ j)
    {
        vector<int>().swap(individual->machine[j]);
    }
    ind1=*individual;
    ind2=*individual;
    int temp = rand() % 10;
    //if(temp >= 0 && temp < 10 && ElistCollection.size() != 0) {
//    if(0) {
//        target2 = rand() % ElistCollection.size();
//        for(int j = 0 ; j < m ; ++ j)
//        {
//            for(iter = Collection[target1].machine[j].begin(); iter != Collection[target1].machine[j].end(); iter ++)
//            {
//                myMap[(*iter)][0] = j;
//            }
//            for(iter = ElistCollection[target2].machine[j].begin(); iter != ElistCollection[target2].machine[j].end(); iter ++)
//            {
//                myMap[(*iter)][1] = j;
//            }
//        }
//    }else {
        for(int j = 0 ; j < m ; ++ j)
        {
            for(iter = Collection[target1].machine[j].begin(); iter != Collection[target1].machine[j].end(); iter ++)
            {
                myMap[(*iter)][0] = j;
            }
            for(iter = Collection[target2].machine[j].begin(); iter != Collection[target2].machine[j].end(); iter ++)
            {
                myMap[(*iter)][1] = j;
            }
        }
//    }
//printf("crossover2\n");
		for(int j=0; j<m;++j){
			for(iter=Collection[target1].machine[j].begin();iter!=Collection[target1].machine[j].end();iter++){
				int temp=rand()%2;
				ind1.machine[myMap[(*iter)][temp]].push_back(*iter);
			}
			for(iter=Collection[target2].machine[j].begin();iter!=Collection[target2].machine[j].end();iter++){
				int temp=rand()%2;
				ind2.machine[myMap[(*iter)][temp]].push_back(*iter);
			}
		}
		temp=rand()%2;
		if(temp==0){
            //individual=&ind2;
            copy_individual(individual,&ind1);
		}
		else{
            copy_individual(individual,&ind2);
            //individual=&ind2;
		}
//        for(int j=0; j<m;++j){
//                printf("machine %d: ",j);
//			for(iter=individual->machine[j].begin();iter!=individual->machine[j].end();iter++){
//				printf("%d ",(*iter));
//			}
//			printf("\n");
//        }
//    for(int i = 0 ; i < n ; ++ i)
//    {
//        int temp = rand() % 2;
//        individual -> machine[myMap[i][temp]].push_back(i);//����һ���µĸ���,���浽individual��
//    }
}

void light_perturbation(int segment[], int size_of_segment, int interval[]){
    int temp, k, pos1, pos2, temp1;
    int interval1[MAXN];
    interval1[0] = interval[0];
    for(int i = 1 ; i < m ; i ++){
        interval1[i] = interval1[i-1] + interval[i];
    }
    //���������������
    temp = rand() % m;
    while(!interval[temp]){
        temp = rand() % m;
    }
    temp1 = rand() % m;
    while(temp == temp1){
        temp1 = rand() % m;
    }

    if(interval[temp1] != 0) {
        if(temp == 0){
            pos1 = rand() % interval1[temp];
        }else{
            pos1 = interval1[temp-1] + rand() % interval[temp];
        }
        if(temp1 == 0){
            pos2 = rand() % interval1[temp1];
        }else{
            pos2 = interval1[temp1-1] + rand() % interval[temp1];
        }
        if(pos1 > pos2) {
            k = pos1;
            pos1 = pos2;
            pos2 = k;
        }
        k = segment[pos1];
        //�������
        for(int i = pos1 ; i < pos2 ; i ++){
            segment[i] = segment[i + 1];
        }
        segment[pos2] = k;

        interval[temp] -= 1;
        interval[temp1] += 1;
    } else {
        interval[temp] -= 1;
        interval[temp1] += 1;
    }

}

void heavy_perturbation(int segment[], int size_of_segment, int interval[]){
    int temp, k, pos1, pos2, temp1;
    int interval1[MAXN];
    interval1[0] = interval[0];
    for(int i = 1 ; i < m ; i ++){
        interval1[i] = interval1[i-1] + interval[i];
    }

    temp = rand() % m;
    while(!interval[temp]){
        temp = rand() % m;
    }
    temp1 = rand() % m;
    while(temp == temp1){
        temp1 = rand() % m;
    }

    if(interval[temp1]) {
        if(temp == 0){
            pos1 = rand() % interval1[temp];
        }else{
            pos1 = interval1[temp-1] + rand() % interval[temp];
        }
        if(temp1 == 0){
            pos2 = rand() % interval1[temp1];
        }else{
            pos2 = interval1[temp1-1] + rand() % interval[temp1];
        }

        k = segment[pos1];
        segment[pos1] = segment[pos2];
        segment[pos2] = k;
    }else {

    }

}


void gamutation(Individual *individual)
{

    int segment[MAXN];
    int interval[MAXN];
    int size_of_segment = 0;
    vector<int>::iterator iter;

    for(int i = 0 ; i < m ; i ++)
    {
        interval[i] = individual -> machine[i].size();
        for(iter = individual -> machine[i].begin(); iter != individual -> machine[i].end(); ++ iter)
        {
            segment[size_of_segment ++] = (*iter);
        }
    }

    //int temp = rand() % 2;
//    if(temp == 0)
//    {

//    }
//    else
//    {
        //light_perturbation(segment, size_of_segment, interval);
        heavy_perturbation(segment, size_of_segment, interval);
        heavy_perturbation(segment, size_of_segment, interval);
        //light_perturbation(segment, size_of_segment, interval);
//    }

    for(int j = 0 ; j < m ; ++ j)
    {
        vector<int>().swap(individual->machine[j]);
    }

    int temp = 0;
    for(int i = 0 ; i < m ; i ++)
    {
//        printf("interval[%d] = %d\n", i , interval[i]);
        for(int j = 0 ; j < interval[i] ; j ++)
        {
            individual->machine[i].push_back(segment[temp]);
            temp ++;
        }
    }

}
double get_max_energy(int now_rank)
{
    vector<Individual>::iterator iter;
    double max_energy = -inf;
    double this_energy;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_energy = (*iter).energy;
        if(max_energy < this_energy)
        {
            max_energy = this_energy;
        }
    }
    return max_energy;
}
double get_min_energy(int now_rank)
{
    vector<Individual>::iterator iter;
    double min_energy = inf;
    double this_energy;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_energy = (*iter).energy;
        if(min_energy > this_energy)
        {
            min_energy = this_energy;
        }
    }
    return min_energy;
}
double get_max_communication(int now_rank)
{
    vector<Individual>::iterator iter;
    double max_communication = -inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_communication = (*iter).makespan;
        if(max_communication < this_communication)
        {
            max_communication = this_communication;
        }
    }
    return max_communication;
}

double get_min_communication(int now_rank)
{
    vector<Individual>::iterator iter;
    double min_communication = inf;
    double this_communication;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_communication = (*iter).makespan;
        if(min_communication > this_communication)
        {
            min_communication = this_communication;
        }
    }
    return min_communication;
}

double get_max_maxspan(int now_rank)
{
    vector<Individual>::iterator iter;
    double max_maxspan = -inf;
    double this_max_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_max_span = (*iter).workload;
        if(max_maxspan < this_max_span)
        {
            max_maxspan = this_max_span;
        }
    }
    return max_maxspan;
}

double get_min_maxspan(int now_rank)
{
    vector<Individual>::iterator iter;
    double min_maxspan = inf;
    double this_min_span;
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++iter)
    {
        this_min_span = (*iter).workload;
        if(min_maxspan > this_min_span)
        {
            min_maxspan = this_min_span;
        }
    }
    return min_maxspan;
}

//����ӵ������
void crowdDistance(int now_rank)
{
    vector<Individual>::iterator iter;
    Individual front_individuals[MAXN];
    int length = 0;
    double max_communication = get_max_communication(now_rank);
    double min_communication = get_min_communication(now_rank);
    double max_maxspan = get_max_maxspan(now_rank);
    double min_maxspan = get_min_maxspan(now_rank);
    double max_energy = get_max_energy(now_rank);
    double min_energy = get_min_energy(now_rank);
    for(iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
    {
        front_individuals[length] = (*iter);
        length ++;
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++)
    {
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].workload - front_individuals[i-1].workload) / (max_communication - min_communication);
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp1);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++)
    {
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].makespan - front_individuals[i-1].makespan) / (max_maxspan - min_maxspan);
    }
    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp0);
    front_individuals[0].crowd_distance = inf;
    front_individuals[length - 1].crowd_distance = inf;
    for(int i = 1 ; i < length - 1 ; i ++)
    {
        front_individuals[i].crowd_distance = front_individuals[i].crowd_distance + (front_individuals[i+1].energy - front_individuals[i-1].energy) / (max_energy - min_energy);
    }

    qsort(front_individuals, length, sizeof(front_individuals[0]), cmp2);
//    printf("*************\n");
//    for(int i = 0 ; i < n ; i ++){
//        printf("distance = %.2lf\n", front_individuals[i].crowd_distance);
//    }
//    printf("*************\n");
    Front[now_rank].erase(Front[now_rank].begin(), Front[now_rank].end());
    for(int i = 0 ; i < length ; i ++)
    {
        Front[now_rank].push_back(front_individuals[i]);
    }
}

void insertMachine(Individual *i, int a, int b, int c) { // a = taskIndex[nowPoint], b = nowPoint, c = temp
    vector<int>:: iterator iter;
    int j = 0;
    int k = 0;
    // delete temp
    for(j = 0 ; j < m ; j ++) {
        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); iter ++) {
            if((*iter) == c) {
                i->machine[j].erase(iter);
                iter = i->machine[b].begin();
                while(k < a) {
                    k ++;
                    iter ++;
                }
                i->machine[b].insert(iter, c);
                return ;
            }
        }
    }
    return ;
}

//void swapMachine(Individual *i, int a, int b, int c) { // a = taskIndex[nowPoint], b = nowPoint, c = temp
//    vector<int>:: iterator iter;
//    int j = 0;
//    int k = 0;
//    //printf("value = %d c = %d\n", i->machine[b][a], c);
//    // delete temp
//    for(j = 0 ; j < m ; j ++) {
//        for(iter = i->machine[j].begin(); iter != i->machine[j].end(); iter ++) {
//            if((*iter) == c) {
//                //printf("b = %d a= %d\n", b, a);
//                int temp = c;
//                (*iter) = i->machine[b][a];
//                i->machine[b][a] = temp;
//                return ;
//            }
//        }
//    }
//    return ;
//}

int test(int task) {
    int i;
    int tot = 0;
    int arr[MAXN];
    memset(arr, 0, sizeof(arr));
    for(i = 0 ; i < n ; i ++) {
        if(c[i][task] > 0) {
            if(doneSet.find(i) == doneSet.end() && taskUsed[i] == false) { //���û���ҵ�
                arr[tot ++] = i;
            }
        }
    }
    if(tot == 0)return -1;//��������Ա�ִ��
    else {
        int random = rand() % tot;
        return arr[random];//������Ҫ������һ������
    }
}

//Repair segment
/********************************************
Function: repair_segment
Description: repair input segment of machines to fit in with requirement
Input: An individual
Output: void;
Others: Get accepted input of an individual
********************************************/
void repair_segment(Individual *i) {
    vector<int>:: iterator iter;
    /***************initialize******************/
//    printf("repairǰ�Ľ��\n");
//    for(int j = 0 ; j < m ; j ++){
//        printf("��%d������: ", j);
//        for(iter = i->machine[j].begin(); iter != i->machine[j].end() ; iter ++){
//            printf("%d ", (*iter));
//        }
//        printf("\n");
//    }
    memset(taskIndex, 0, sizeof(taskIndex));
    int acTask = 0;
    int nowPoint = 0;
    int depentTask[MAXM];
    bool dependent = false;
    memset(taskUsed, 0, sizeof(taskUsed));
    doneSet.clear();
    /*******************done********************/
    while(acTask < n) {
//        printf("actask = %d\n", acTask);
        dependent = false;
        for(nowPoint = 0 ; nowPoint < m ; nowPoint ++) { // �����е�ָ�����ѭ��
//            printf("size = %d\n", i->machine[nowPoint].size());
//            printf("taskindex = %d\n", taskIndex[nowPoint]);
            if(i->machine[nowPoint].size() == taskIndex[nowPoint]) {
//                    dependent = true;
                    depentTask[nowPoint] = -1; // ���ָ��ָ�����һ������ĺ��棬˵����������Ѿ���ɣ�����������-1.
                    continue; // ���ָ��ָ�����һ��Ԫ�أ�����
            }
            int nowTask = i->machine[nowPoint].at(taskIndex[nowPoint]);
//            printf("nowPoint = %d\n", nowPoint);
//            printf("nowTask = %d\n", nowTask);
            depentTask[nowPoint] = test(nowTask);
//            printf("depentTask[nowPoint] = %d\n", depentTask[nowPoint]);
            if(depentTask[nowPoint] == -1) { // ����ܷ���������ָ����Ʋ��ҿ�����Ļ�����+1.
                taskIndex[nowPoint] ++;
                acTask ++;
                taskUsed[nowTask] = true;
                doneSet.insert(nowTask);
                dependent = true;
                break;
            }
        }

        if(dependent == false) {
//            printf("����������\n");
//            int machine = rand() % m;

//            int temp = rand() % m;
//            printf("ѡ��Ļ�����%d\n", temp);
//            while(depentTask[temp] == -1) temp = rand() % m;
//            int number = rand() % m;
//            while(depentTask[number] == -1) number = rand() % m;
//            insertMachine(i, taskIndex[machine], machine, depentTask[temp]);
//            printf("�����Ľ��\n");
//            for(int j = 0 ; j < m ; j ++){
//                printf("��%d������: ", j);
//                for(iter = i->machine[j].begin(); iter != i->machine[j].end() ; iter ++){
//                    printf("%d ", (*iter));
//                }
//                printf("\n");
//            }
            for(int machine=0;machine<m;machine++){
                if(depentTask[machine]==-1)
                    continue;
                else{
                    insertMachine(i, taskIndex[machine], machine, depentTask[machine]);
                }
            }
        }
    }
}

void swap_machine(Individual *individual, int nowM, int nowPos, int toM, int toPos) {
    vector<int>::iterator iter;
    int temp = individual->machine[nowM][nowPos];
    individual->machine[nowM][nowPos] = individual->machine[toM][toPos];
    individual->machine[toM][toPos] = temp;
}

void insert_machine(Individual *individual, int nowM, int nowPos, int toM, int toPos) {
    vector<int>::iterator iter;
    int temp = individual->machine[nowM][nowPos];
    individual->machine[toM].insert(individual->machine[toM].begin() + toPos, temp);

    if(nowM == toM && nowPos > toPos) individual->machine[nowM].erase(individual->machine[nowM].begin() + nowPos + 1);//����temp������Ҫ��1
    else individual->machine[nowM].erase(individual->machine[nowM].begin() + nowPos);//����ɾ������
}

bool check_machine(Individual *individual) {
    vector<int>:: iterator iter;
    /***************initialize******************/
    memset(taskIndex, 0, sizeof(taskIndex));
    int acTask = 0;
    int nowPoint = 0;
    int depentTask[MAXM];
    bool dependent = false;
    memset(taskUsed, 0, sizeof(taskUsed));
    doneSet.clear();
    /*******************done********************/
    while(acTask < n) {
        dependent = false;
        for(nowPoint = 0 ; nowPoint < m ; nowPoint ++) { // �����е�ָ�����ѭ��
            if(individual->machine[nowPoint].size() == taskIndex[nowPoint]) {
                    depentTask[nowPoint] = -1; // ���ָ��ָ�����һ������ĺ��棬˵����������Ѿ���ɣ�����������-1.
                    continue; // ���ָ��ָ�����һ��Ԫ�أ�����
            }
            int nowTask = individual->machine[nowPoint].at(taskIndex[nowPoint]);
            depentTask[nowPoint] = test(nowTask);
            if(depentTask[nowPoint] == -1) { // ����ܷ���������ָ����Ʋ��ҿ�����Ļ�����+1.
                taskIndex[nowPoint] ++;
                acTask ++;
                if(acTask == n) return true;
                taskUsed[nowTask] = true;
                doneSet.insert(nowTask);
                dependent = true;
                break;
            }
        }
        if(dependent == false) {
            return false;
        }
    }
}
//�����֧���ϵ
int compareIndividual(Individual *a, Individual *b) {
    if(a->makespan == b->makespan && a->workload == b->workload && a->energy==b->energy) return 0;//a=b
//    if((a->makespan < b->makespan && a->workload <= b->workload) || (a->workload < b->workload && a->makespan <= b->makespan)) return 1;//a dominates b
//    if((a->makespan >= b->makespan && a->workload >= b->workload) || (a->workload > b->workload && a->makespan >= b->makespan)) return 2;//b dominates a
    if(a->makespan <= b->makespan && a->workload <= b->workload && a->energy<=b->energy) return 1;//a dominates b
	if(a->makespan >= b->makespan && a->workload >= b->workload && a->energy>=b->energy) return 2;//a dominates b

	return 3;
}

void relaxElistCollection(Individual *i) {
    vector<Individual>::iterator iter;
    for(iter = ElistCollection.begin(); iter != ElistCollection.end(); iter ++) {
        if(i->makespan == (*iter).makespan && (i->workload == (*iter).workload)&& (i->energy == (*iter).energy)) {
            continue;
        }else {
            int ret = compareIndividual(i, &(*iter));
            if(ret == 1) {
                ElistCollection.erase(iter);
                iter --;
            }
        }
    }
}

void updateElistCollection(Individual *i) {
    bool flag = false;
    bool changed = false;
    vector<Individual>::iterator iter;
    if(ElistCollection.empty()) {
        ElistCollection.push_back(*i);
        finish = clock();
        real_time=(double)(finish - start) / CLOCKS_PER_SEC;
        printf("find a better solution in: %.2f s	\n", real_time);
        changed = true;
    }else {
        for(iter = ElistCollection.begin(); iter != ElistCollection.end(); iter ++) {
            int ret = compareIndividual(i, &(*iter));
            if(ret == 0) {
                flag = true;
                break;
            }
            else if(ret == 1) {
                (*iter) = *i;
                flag = true;
                relaxElistCollection(i);
                finish = clock();
                real_time=(double)(finish - start) / CLOCKS_PER_SEC;
                printf("find a better solution in: %.2f s	\n", real_time);
                changed = true;
                break;
            }else if(ret == 2) {
                flag = true;
                break;
            }
        }
        if(!flag) {
            ElistCollection.push_back(*i);
            changed = true;
            finish = clock();
            real_time=(double)(finish - start) / CLOCKS_PER_SEC;
            printf("find a better solution in : %.2f s	\n", real_time);
        }
    }
}

void swap_localsearch(Individual *individual) {
    /***************initialize******************/
    int flag[MAXN];
    int k = 1;
    int kMax = n;
    int nowMachine;
    int nowPos;
    int toMachine;
    int toPos;
    int segmentSize[MAXM];
    int randomSequence[MAXN];
    int machineNumber;
    int temp;
    Individual temp_individual = *individual;
    Individual best_individual = *individual;
    Individual origin_individual = *individual;
    for(int l = 0 ; l < m ; l ++) {
        segmentSize[l] = temp_individual.machine[l].size();
    }

    //generate a sequence by randomly swap
    for(int i = 0 ; i < n ; i ++) {
        randomSequence[i] = i;
    }
    for(int i = 0 ; i < n ; i ++) {
        temp = rand() % n;
        swap(randomSequence[i], randomSequence[temp]);
    }

    int mark = 0;
    machineNumber = 0;
    /******************done*********************/
    while(k <= kMax) {

//        origin_individual = temp_individual;

        evaluate_objective(&best_individual);
        double makespan = best_individual.makespan;
        double workload = best_individual.workload;
        double energy=best_individual.energy;
        nowMachine = 0;
        temp = randomSequence[machineNumber];
//        printf("temp = %d\n", temp);
        for(int l = 0 ; l < m ; l ++) {
            segmentSize[l] = temp_individual.machine[l].size();
        }
        while(temp - segmentSize[nowMachine] >= 0) {
            temp -= segmentSize[nowMachine];
            nowMachine ++;
        }
//        printf("segmentSize[%d] = %d\n", nowMachine, segmentSize[nowMachine]);
        if(temp < 0) nowPos = segmentSize[nowMachine] + temp;
        else nowPos = temp;
        machineNumber ++;
//        printf("nowMachine = %d nowPos = %d\n", nowMachine, nowPos);
        // random generate a task
//        nowMachine = rand() % m;
//        if(segmentSize[nowMachine] == 0) continue;
//        nowPos = rand() % segmentSize[nowMachine];
        // task done
        // find best neighbor
        /*-------------------------------------------*/
        for(toMachine = 0 ; toMachine < m ; toMachine ++) {
            if(segmentSize[toMachine]) for(toPos = 0 ; toPos < segmentSize[toMachine] ; toPos ++) {
                if(nowMachine != toMachine || nowPos != toPos) {
//                    printf("nowMachine = %d nowPos = %d toMachine = %d toPos = %d\n", nowMachine, nowPos, toMachine, toPos);
//                    printf("insert ǰ\n");
//                    for(int j = 0 ; j < m ; j ++){
//                        printf("��%d������: ", j);
//                        for(vector<int>::iterator iter = temp_individual.machine[j].begin(); iter != temp_individual.machine[j].end() ; iter ++){
//                            printf("%d ", (*iter));
//                        }
//                        printf("\n");
//                    }

                    swap_machine(&temp_individual, nowMachine, nowPos, toMachine, toPos);
//                    printf("insert ��\n");
//                    for(int j = 0 ; j < m ; j ++){
//                        printf("��%d������: ", j);
//                        for(vector<int>::iterator iter = temp_individual.machine[j].begin(); iter != temp_individual.machine[j].end() ; iter ++){
//                            printf("%d ", (*iter));
//                        }
//                        printf("\n");
//                    }

                    if(check_machine(&temp_individual)) { //�����������
//                        printf("fuhe\n");
                        evaluate_objective(&temp_individual);
//                        printf("evaluate ��\n");
//                        for(int j = 0 ; j < m ; j ++){
//                            printf("��%d������: ", j);
//                            for(vector<int>::iterator iter = temp_individual.machine[j].begin(); iter != temp_individual.machine[j].end() ; iter ++){
//                                printf("%d ", (*iter));
//                            }
//                            printf("\n");
//                        }
//                        printf("individual\n");
//                        for(int j = 0 ; j < m ; j ++){
//                            printf("��%d������: ", j);
//                            for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                                printf("%d ", (*iter));
//                            }
//                            printf("\n");
//                        }
//                        for(int j = 0 ; j < m ; j ++){
//                            printf("��%d������: ", j);
//                            for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                                printf("%d ", (*iter));
//                            }
//                            printf("\n");
//                        }
                        //if((temp_individual.makespan <= makespan && temp_individual.workload < workload) || (temp_individual.makespan < makespan && temp_individual.workload <= workload)) {
						if(temp_individual.makespan <= makespan && temp_individual.workload <= workload && temp_individual.energy <= energy){
							if(temp_individual.makespan==makespan && temp_individual.workload==workload && temp_individual.energy==energy){

							}
							else{

                            makespan = temp_individual.makespan;
                            workload = temp_individual.workload;
                            energy=temp_individual.energy;
//                            printf("find best before before\n");
//                            for(int j = 0 ; j < m ; j ++){
//                                printf("��%d������: ", j);
//                                for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                                    printf("%d ", (*iter));
//                                }
//                                printf("\n");
//                            }
                            best_individual = temp_individual;
//                            printf("find best before\n");
//                            for(int j = 0 ; j < m ; j ++){
//                                printf("��%d������: ", j);
//                                for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                                    printf("%d ", (*iter));
//                                }
//                                printf("\n");
//                            }
                            makespan = best_individual.makespan;
                            workload = best_individual.workload;
                            energy=best_individual.energy;
                            //�ҵ����Ž⣬����
                            updateElistCollection(&temp_individual);
//                            printf("find best\n");
//                            for(int j = 0 ; j < m ; j ++){
//                                printf("��%d������: ", j);
//                                for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                                    printf("%d ", (*iter));
//                                }
//                                printf("\n");
//                            }
                        }
						}
						//��֧�� �����
                        if(temp_individual.makespan < makespan || temp_individual.workload < workload||temp_individual.energy<energy) {
                            updateElistCollection(&temp_individual);

                            //in linux it's times(&)
//                            real_time = double(finish.tms_utime - start.tms_utime + finish.tms_stime - start.tms_stime)/sysconf(_SC_CLK_TCK);
//                            real_time = round(real_time * 100)/100.0;

                        }
                    }
//                    printf("individual\n");
//                    for(int j = 0 ; j < m ; j ++){
//                        printf("��%d������: ", j);
//                        for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                            printf("%d ", (*iter));
//                        }
//                        printf("\n");
//                    }
                    temp_individual = *individual;

//                    printf("��ԭtemp\n");
//                    for(int j = 0 ; j < m ; j ++){
//                        printf("��%d������: ", j);
//                        for(vector<int>::iterator iter = temp_individual.machine[j].begin(); iter != temp_individual.machine[j].end() ; iter ++){
//                            printf("%d ", (*iter));
//                        }
//                        printf("\n");
//                    }
//                    printf("individual\n");
//                    for(int j = 0 ; j < m ; j ++){
//                        printf("��%d������: ", j);
//                        for(vector<int>::iterator iter = individual->machine[j].begin(); iter != individual->machine[j].end() ; iter ++){
//                            printf("%d ", (*iter));
//                        }
//                        printf("\n");
//                    }
                }
            }
        }
        k ++;
//        printf("best_individual.makespan = %.2lf individual->makespan = %.2lf\n", best_individual.makespan, individual->makespan);
       // if((best_individual.makespan <= individual->makespan && best_individual.workload < individual->workload) || (best_individual.makespan < individual->makespan && best_individual.workload <= individual->workload)) {
          if(best_individual.makespan <= individual->makespan && best_individual.workload <= individual->workload && best_individual.energy<=individual->energy){
		  	if(best_individual.makespan == individual->makespan && best_individual.workload == individual->workload && best_individual.energy==individual->energy)
			{
				}
			else{

		    *individual = best_individual;
            temp_individual = best_individual;
            k = 1;
            machineNumber = 0;
            for(int i = 0 ; i < n ; i ++) {
                randomSequence[i] = i;
            }
            for(int i = 0 ; i < n ; i ++) {
                temp = rand() % n;
                swap(randomSequence[i], randomSequence[temp]);
            }
            for(int l = 0 ; l < m ; l ++) {
                segmentSize[l] = temp_individual.machine[l].size();
            }
//            printf("k init\n");
			}
        }
    }
}

void make_new_pop(Individual individuals[], int length){
    vector<int>::iterator iter;
    int flag_individual[MAXN]; //�����������Ƿ�ѡ���
    //tournament_selection
    memset(flag_individual, 0, sizeof(flag_individual)); //��ʼȫ��δ��ѡ��
    for(int i = 0 ; i < length ; i ++)
    {
        int target1 = rand() % ((length<<1));
        while(flag_individual[target1] != 0)
        {
            target1 = rand() % ((length<<1));
        }
        //   printf("1�����ɻ���%d\n",i);
        flag_individual[target1] = 1;
        int target2 = rand() % ((length<<1));
        while(flag_individual[target2] != 0)
        {
            target2 = rand() % ((length<<1));
        } //���ѡ������Ŀ��
        //     printf("2�����ɻ���%d\n",i);
        flag_individual[target2] = 1;
//        printf("target1 = %d, target2 = %d\n", target1, target2);
        int temp = rand() % 10;
        gacrossover(target1, target2, &new_individual);
        //printf("3�����ɻ���%d\n",i);
        if(temp >= 0 && temp < 9) {}
        else
        {
            //        printf("4�����ɻ���%d\n",i);
            gamutation(&new_individual);
            //        printf("5�����ɻ���%d\n",i);
        }
//        printf("temp = %d\n", temp);
//        printf("�����ɻ���%d\n",i);
//        for(int j = 0 ; j < m ; j ++){
//            printf("��%d������: ", j);
//            for(iter = new_individual.machine[j].begin(); iter != new_individual.machine[j].end() ; iter ++){
//                printf("%d ", (*iter));
//            }
//            printf("\n");
//        }
          //Swap_localsearch(&new_individual);

        repair_segment(&new_individual);
        evaluate_objective(&new_individual);
//        swap_localsearch(&new_individual);
        copy_individual(&individuals[length + i], &new_individual);

//        gacrossover(target1, target2, &new_individual);
//        gamutation(&new_individual);


    }
}
//ֻ����һ����
void greedy_with_topo() {
    int in[MAXN];
    int number_in_machine[MAXM];
    int ac_task = 0;
    int now_machine = 0;
    int maxN = n / m;
    int vis[MAXN];
    for(int i = 0 ; i < n ; i ++) {
        in[i] = isdep[0][i];
    }
    memset(number_in_machine, 0, sizeof(number_in_machine));
    memset(vis, 0, sizeof(vis));

    while(ac_task < n) {
        for(int i = 0 ; i < n ; i ++) {
            if(!in[i] && !vis[i]) {
                ac_task ++;
                Collection[0].machine[now_machine].push_back(i);
                vis[i] = 1;
                for(int j = 0 ; j < n ;j ++) {
                    if(c[i][j] > 0) {
                        in[j] --;
                    }
                }
                if(Collection[0].machine[now_machine].size() >= maxN && now_machine < m - 1) {
                    now_machine ++;
                }
            }
        }
    }
}

void init(){
    set<int> flag_machine;
    stack<int> segment;
    set<int> interval;
    int kk, kkk;
    vector<int>::iterator iter;
    set<int>::iterator iiter;
    int tempkk;
    for(int i = 0 ; i < pop*2 ; i ++)
    {
        for(int j = 0 ; j < m ; j ++)
        {
            Collection[i].machine[j].erase(Collection[i].machine[j].begin(), Collection[i].machine[j].end());
        }
    }
    printf("greedy begin\n");
    greedy_with_topo();
    printf("greedy done\n");

    for(int i = 1 ; i < pop*2 ; i ++)
    {
        flag_machine.clear();
        while(!segment.empty())
        {
            segment.pop();
        }

        for(int j = 0 ; j < n * 2 ; j ++)
        {
            int temp = rand() % n;
            if(flag_machine.insert(temp).second)
            {
                segment.push(temp);
            }
        }
        for(int k = 0 ; k < n ; k ++)
        {
            if(flag_machine.insert(k).second)
            {
                segment.push(k);
            }
        }

        interval.clear();
        kk = m - 1;
        while(kk)
        {
            int temp = rand() % (n);
            if(temp == 0) temp = 1;
            while(!interval.insert(temp).second)
            {
                temp = rand() % (n);
                if(temp == 0) temp = 1;
            }
            kk --;
        }
        kkk = 0;
        for(iiter = interval.begin(); iiter != interval.end(); iiter ++)
        {
            if(iiter == interval.begin())
            {
                kk = (*iiter);
            }
            else
            {
                kk = (*iiter) - kk;
            }
            tempkk = kk;
            while(tempkk --)
            {
                Collection[i].machine[kkk].push_back(segment.top());
                segment.pop();
            }
            kkk ++;
            kk = (*iiter);
        }

        kk = n - kk;
        tempkk = kk;
        while(tempkk --)
        {
            Collection[i].machine[kkk].push_back(segment.top());
            segment.pop();
        }
        repair_segment(&Collection[i]);
    }
}





//Main Process
void solve(){

//    scanf("%d%d", &pop, &gen);
//���ļ�
    scanf("%d", &mycycle);
    scanf("%d%d", &m , &n);
    printf("m = %d n = %d\n", m , n);
    printf("pop = %d gen = %d\n", pop, gen);
    //���������ڲ�ͬ������ִ��ʱ����ͬ
    for(int i=0; i<n; i++)
    {
        scanf("%lf",&t[0][i]);
    }
    for(int j=1; j<m; j++)
    {
        for(int i=0; i<n; i++)
        {
            t[j][i]=t[0][i];
        }
    }
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<cycle; j++)
        {
            isdep[j][i]=0;//��������i�ĸ���
            todep[j][i]=0;//i������������
        }

    }
    for(int i = 0 ; i < n ; i ++)
    {
        for(int j = 0 ; j < n ; j ++)
        {
            scanf("%lf", &c[i][j]);
             if(c[i][j]>0)
            {
                todep[0][i]++;
                isdep[0][j]++;
            }
        }
    }


//    for(int i = 0 ; i < n ; i ++)
//    {
//        for(int j = 0 ; j < n ; j ++)
//        {
//            if(c[i][j]>0)
//            {
//                todep[0][i]++;
//                isdep[0][j]++;
//            }
//        }
//    }
    for(int i=1; i<cycle; i++)
    {
        for(int j=0; j<n; j++)
        {
            todep[i][j]=todep[0][j];
            isdep[i][j]=isdep[0][j];
        }
    }

    int t = 0;
    printf("init...begin\n");
    init();
    //greedy_with_topo();
    printf("init...done\n");

    while(exe_time < 3600)
    {
        printf("*****************************t=%d*****************************\n", t);
        int P_size = 0;
        int now_rank = 1;
        make_new_pop(Collection, pop);
        for(int i = 0 ; i < pop*2 ; i ++)
        {
            evaluate_objective(&Collection[i]);
        }
        non_domination_sort(Collection, pop * 2, true);//�������ķ�֧���ϵ
        for(vector<Individual>::iterator iter = Front[1].begin(); iter != Front[1].end(); ++ iter) {
            updateElistCollection(&(*iter)); // ���¾�Ӣ��Ⱥ
        }

        while(1)
        {
            if(P_size + Front[now_rank].size() > pop)
            {
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
            {
                Collection[P_size] = (*iter);
                P_size ++;
            }
            now_rank ++;
        }
        crowdDistance(now_rank);
        while(1)
        {
            if(P_size > pop)
            {
                break;
            }
            for(vector<Individual>::iterator iter = Front[now_rank].begin(); iter != Front[now_rank].end(); ++ iter)
            {
                Collection[P_size] = (*iter);
                P_size ++;
            }
        }
        t ++;
    finish = clock();
    exe_time=(double)(finish - start) / CLOCKS_PER_SEC;

	printf("%d generation time: %.2f s\n", t,exe_time);
//	for(int i = 0 ; i < ElistCollection.size() ; i ++)
//        {
//    //        if(Collection[i].front == 1)
//    //        {
//                printf("[");
//                printf("%.2lf,", ElistCollection[i].makespan);
//                printf("%.2lf", ElistCollection[i].workload);
//                printf("],");
//                printf("\n");
//    //        }
//        }
    }

    non_domination_sort(Collection, pop * 2, true);
    vector<int>:: iterator iter;
    int tot = 0;
    double tempCE=0,tempIE=0,tempSE=0;
    double tempCT=0,tempIT=0,tempST=0;
    for(int i = 0 ; i < ElistCollection.size() ; i ++)
    {
//        if(Collection[i].front == 1)
//        {
            printf("[");
            printf("%.2lf,", ElistCollection[i].makespan);
            printf("%.2lf,", ElistCollection[i].workload);
            printf("%.2lf", ElistCollection[i].energy);
            printf("],");
            printf("\n");
            tempCE+=ElistCollection[i].Ed/ElistCollection[i].energy;
            tempIE+=ElistCollection[i].Ei/ElistCollection[i].energy;
            tempSE+=ElistCollection[i].Es/ElistCollection[i].energy;
            tempCT+=ElistCollection[i].comtime/(ElistCollection[i].makespan*m);
            tempIT+=ElistCollection[i].idltime/(ElistCollection[i].makespan*m);
            tempST+=ElistCollection[i].sleeptime/(ElistCollection[i].makespan*m);
            printf("computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf\n",ElistCollection[i].Ed/ElistCollection[i].energy,ElistCollection[i].Ei/ElistCollection[i].energy,ElistCollection[i].Es/ElistCollection[i].energy);
            printf("computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf\n",ElistCollection[i].comtime/(ElistCollection[i].makespan*m),ElistCollection[i].idltime/(ElistCollection[i].makespan*m),ElistCollection[i].sleeptime/(ElistCollection[i].makespan*m));

            tot ++;
//        }
    }
    tempCE=tempCE/ElistCollection.size();
    tempIE=tempIE/ElistCollection.size();
    tempSE=tempSE/ElistCollection.size();
    tempCT=tempCT/ElistCollection.size();
    tempIT=tempIT/ElistCollection.size();
    tempST=tempST/ElistCollection.size();
    printf("AVERAGE: computation energy ratio: %lf, idle energy ratio: %lf, sleep energy ratio: %lf\n",tempCE,tempIE,tempSE);
    printf("AVERAGE: computation time ratio: %lf, idle time ratio: %lf, sleep time ratio: %lf\n",tempCT,tempIT,tempST);

 printf("x=[");
for(int i = 0 ; i < ElistCollection.size() ; i ++)
    {
//        if(Collection[i].front == 1)
//        {

            printf("%.2lf ", ElistCollection[i].makespan);

            tot ++;
//        }
    }
     printf("];");
            printf("\n");
             printf("y=[");
    for(int i = 0 ; i < ElistCollection.size() ; i ++)
    {
//        if(Collection[i].front == 1)
//        {

            printf("%.2lf ", ElistCollection[i].workload);


//        }
    }
      printf("];");
            printf("\n");
            printf("z=[");
    for(int i = 0 ; i < ElistCollection.size() ; i ++)
    {
//        if(Collection[i].front == 1)
//        {

            printf("%.2lf ", ElistCollection[i].energy);


//        }
    }
     printf("];");
            printf("\n");
    printf("tot = %d\n", tot);
}

int main(int argc, char **argv){
//int main(){
    char outPutFile[100];
    char testcase[100];
    char *fileName;
    int myRand;
    strcpy(testcase, argv[1]);
    strtok(strtok(argv[1], "."), "/");
    strcpy(outPutFile, fileName = strtok(NULL, "/"));
    strcat(outPutFile, "/");
    strcat(outPutFile, fileName);
    strcat(outPutFile, "-");
    strcat(outPutFile, argv[2]);
    myRand = atoi(argv[2]);
    strcat(outPutFile, "-");
    strcat(outPutFile, argv[3]);
    pop = atoi(argv[3]);
    strcat(outPutFile, "-");
    strcat(outPutFile, argv[4]);
    strcat(outPutFile, "-MACHINE4-NEWNSGA2-3object-3600");

    gen = atoi(argv[4]);
    srand(myRand);
    strcat(outPutFile, ".txt");
    freopen(testcase, "r", stdin);
    freopen(outPutFile, "w", stdout);
//    printf("%s\n", outPutFile);
    start = clock();
//    start_time = start.tms_utime + start.tms_stime;
    solve();

//	double finish_time = double(finish.tms_utime - start.tms_utime + finish.tms_stime - start.tms_stime)/sysconf(_SC_CLK_TCK);
//	finish_time = round(finish_time * 100)/100.0;
    finish = clock();
    exe_time=(double)(finish - start) / CLOCKS_PER_SEC;

	printf("total time: %.2f s\n", exe_time);
//    srand(3);
////    freopen("testcase/TMNR.dat", "r", stdin);
//    freopen("testcase/h264_14.dat", "r", stdin);
//    solve();

    return 0;
}
