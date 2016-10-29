#include<iostream>
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<algorithm>
#include<vector>
#include<math.h>
#include<set>
#include<stack>
using namespace std;

/****************************************
Ŀ�꺯��              Minimize workload
Ŀ�꺯��              Minimize makespan
t(m,n)                Execution time of a task on a processor
c(m,n)                Communication cost between two tasks
m                     number of machines
n                     number of tasks
****************************************/
const int MAXN = 2005;
const int inf = 0x3f3f3f3f;
double c[MAXN][MAXN];
double t[MAXN][MAXN];

int m; //number of machines
int n; //number of tasks
int pop; //population
int gen; //generation
int number_of_machines;
int isdep[10][MAXN];//��i�����ĸ���
int todep[10][MAXN];//i������������
int taskIndex[MAXN];// ��¼ÿ��������ƥ�䵽��λ��

//define individual
class Individual {
public:
	Individual(void);
	~Individual(void);
public:
	vector<int> machine[MAXN];
	double workload; //object 1
	double makespan; //object 2
	int front; //rank of domination
	vector<int> S; //the collections dominated by this Individual
	int n;// count of dominated solution
	double dfitness; //fitness
	double crowd_distance; //ӵ����
public:
	void init();
};

//constructed function
Individual::Individual(void) {
}
//destructed function
Individual::~Individual(void) {
}

//algorithm frame
class MOHA {
public:
	MOHA(void);
	~MOHA(void);
public:
	Individual Population[MAXN];
	Individual new_individual;
public:
	void init(Individual* population);

};

//constructed function
MOHA::MOHA(void) {
}
//destructed function
MOHA::~MOHA(void) {
}

//greedy_with_topo
/********************************************
Function: greedy_with_topo,
Description: initialize population greedy with topo,
Input: void,
Output: void,
Others: Get a segment by average workload based topo
********************************************/
void greedy_with_topo(Individual Population[]) {
	int in[MAXN]; //ÿ���ڵ�����
	int ac_task = 0; //task completed
	int now_machine = 0; //����ִ�м�������Ļ������
	int maxN = n / m; //ÿ̨���������������
	int vis[MAXN]; //��������Ƿ��Ѿ��������
	for (int i = 0; i < n; i++) {
		in[i] = isdep[0][i];
	}
	memset(vis, 0, sizeof(vis)); //initialize vis vector
	while (ac_task < n) {
		for (int i = 0; i < n; i++) {
			if (!in[i] && !vis[i]) {
				ac_task++;
				Population[0].machine[now_machine].push_back(i);
				vis[i] = 1;
				for (int j = 0; j < n; j++) {
					if (c[i][j] > 0) {
						in[j] --;
					}
				}
				if (Population[0].machine[now_machine].size() >= MAXN && now_machine < m - 1) {
					now_machine++;
				}

			}
		}
	}
}

//init population
void MOHA::init(Individual* population) {
	greedy_with_topo(population);
}

//init individual
void Individual::init() {

}



int main() {
	return 0;
}
