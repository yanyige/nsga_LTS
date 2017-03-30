#include<stdio.h>
#include<vector>
#include<iterator>

using namespace std;

int seq[100];
int hashset[100];
int ans[100];
int n;
vector<int> ret;

int calcLR(int val1, int val2) {// 0代表左边 1代表右边
    if(hashset[val1] < hashset[val2]) {
        return 1;
    }else {
        return 0;
    }
}

int main() {
    freopen("code.txt", "w", stdout);
    scanf("%d" , &n);
        for(int i = 0 ; i < n ; i ++) {
            scanf("%d", &seq[i]);
            hashset[seq[i]] = i;
        }
        bool flag;
        ret.push_back(0);
        ans[0] = 0;
        for(int i = 1 ; i < n ; i ++) {
            int index = 0;
            flag = false;
            for(vector<int>::iterator iter = ret.begin(); iter != ret.end(); iter ++) {
                int pos = calcLR(i, (*iter));
                printf("pos = %d\n", pos);
                if(pos == 1) {
                    printf("index=%d\n", index);
                    ret.insert(ret.begin() + index, i);
                    ans[i] = index;
                    printf("ans[%d]=%d\n", i, ans[i]);
                    flag = true;
                    break;
                }
                index ++;

            }
            if(flag == false) {
                ret.insert(ret.begin() + index, i);
                ans[i] = index;
            }
        }
        for(int i = 0 ; i < n; i ++) {
            printf("%d ",ans[i]);
        }
        printf("\n");
        ret.erase(ret.begin(), ret.end());




    return 0;
}


