#include "bitmap.h"
#include "list.h"
#include "hash.h"
#include "debug.h"
#include "round.h"
#include "limits.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct datalist{
    int data;
    struct list_elem elem;
};  // int 형 데이터를 포함하는 리스트 (header 의 주석에 나온 foo와 같은구조)
struct datahash{
    int hdata;
    struct hash_elem elem;
}; // int 형 데이터를 포함하는 해쉬 element 
struct list lists[10];
struct hash hashes[10];
struct bitmap *bm[10];
struct list_elem *te,*te2,*te3; // list에서 함수의 리턴형을 받아올 임시변수
struct hash_iterator iter;
struct hash_elem *he1;
static bool list_less(const struct list_elem *e1,const struct list_elem *e2,void *aux)
{
    struct datalist *d1=list_entry(e1,struct datalist,elem);
    struct datalist *d2=list_entry(e2,struct datalist,elem);
    if(d1->data<d2->data)
        return true;
    else
        return false;
}   // list_less 함수를 이용해서 이 것을 파라미터로 넘겼을 때  e1의 원소가 크거나 같다면 false, 작다면 true를 리턴한다.
unsigned hash_hash(const struct hash_elem *e,void *aux)
{
    return hash_int(hash_entry(e,struct datahash,elem)->hdata);
} // hash table의 데이터를 hash_int를 한 hash 값을 리턴한다.
bool hash_less(const struct hash_elem *a,const struct hash_elem *b,void *aux)
{
    struct datahash *a1;
    struct datahash *b1;
    a1 = hash_entry(a,struct datahash,elem);
    b1 = hash_entry(b,struct datahash,elem);
    if(a1->hdata<b1->hdata)
        return true;
    else
        return false;
} // list_less와 마찬가지로 hash에 있는  a의 데이터가 hash에 있는 b의 데이터보다 작다면 true, 반대면 false를 반환한다.
void hash_action_destroy(struct hash_elem *e,void *aux)
{
    free(hash_entry(e,struct datahash,elem));
}  //hash_destroy에서 destructor로 쓰일 함수로 data를 포함한 datahash를 free해준다
void hash_action_square(struct hash_elem *e,void *aux)
{
    int res;
    struct datahash *a1;
    a1 = hash_entry(e,struct datahash,elem);
    res = a1->hdata;
    res*= res;
    a1->hdata = res;
} // hash_apply에서 제곱된 값을 계산할 때 쓰이는 함수
void hash_action_triple(struct hash_elem *e,void *aux)
{
    int res;
    struct datahash *a1;
    a1 = hash_entry(e,struct datahash,elem);
    res = a1->hdata;
    res = res*res*res;
    a1->hdata = res;
}
// hash_apply에서 세제곱된 값을 계산할 때 쓰이는 함수
void listcreate(char *newname)
{
    int tmpnum;
    struct list* temp;
    tmpnum = newname[4] - '0';  // testcase의 list 구조는 listx 의 구조이므로 그냥 바로 x를 받아서 int로 바꾼다음 배열의 인덱스로 사용한다.
    list_init((&lists[tmpnum]));
}
// list_init을 이용해서 새로운 리스트를 만들어주는 함수
void dumplist(int num)
{
    struct list_elem *tmp;
    struct datalist *tmp2;
    tmp = list_begin(&lists[num]);
    for(tmp = list_begin(&lists[num]); tmp != list_end(&lists[num]); tmp = list_next(tmp))
    {
        tmp2 = list_entry(tmp,struct datalist, elem);
        printf("%d ",tmp2->data);
    }
    // list_begin으로 리스트의 head를 받아오고 list_end일때까지 list_next로 옆으로 이동한다.
    printf("\n");
}
// list에 현재 있는 데이터를 보여주는 함수
void deletelist(int num)
{
    while(!list_empty(&lists[num]))
    {
        list_remove(list_front(&lists[num])); 
    }
    // 리스트의 맨 앞을 지운다. ( 리스트의 맨앞을 지우면서 반복하면 결국 리스트가 비워진다는 것은 자명하다.)
}
void deletebm(int num)
{
    bitmap_destroy(bm[num]);
}
//bitmap_destroy를 이용하여 bitmap을 delete한다
void list_swap(struct list_elem *a,struct list_elem *b)
{
    struct list_elem *tmpnext;
    struct list_elem *tmpprev;
    a->next->prev = b;
    a->prev->next = b;
    b->prev->next = a;
    b->next->prev = a;
    tmpnext = a->next;
    a->next = b->next;
    b->next = tmpnext;
    tmpprev = a->prev;
    a->prev = b->prev;
    b->prev = tmpprev;

}
// 두개의 list를 swap해주는 함수. int형을 swap할때와같은 방법으로 swap한다.
void list_shuffle(struct list *list)
{
    int size = list_size(list);
    int random;
    int i,j;
    int tmp;
    //printf("size = %d\n",size);
    //bool tmp=false;
    //bool *tag;
    if(size>1){
    //tag = (bool*)malloc(sizeof(bool)*size);
    for(i=0;i<size;i++)
    {
        te = list_begin(list);
        for(j=0;j<i;j++)
            te = list_next(te);
        // 현재의 인덱스를 가리킨다.
        //tag[i] = true;
        //tag[random] = true;
        while(random==i){
            random = rand()%(size);
        }
        // random이 현재의 인덱스 값과 같지 않을때까지 반복한다.
        te2= list_begin(list);
        for(j=0;j<random;j++)
            te2 = list_next(te2);
        //list_swap(te,te2);
        tmp = list_entry(te,struct datalist,elem)->data;
        list_entry(te,struct datalist,elem)->data = list_entry(te2,struct datalist,elem)->data;
        list_entry(te2,struct datalist,elem)->data = tmp;
        printf("Swap %d and %d\n",i,random);
        // swap할 두개의 리스트의 데이터 값만 swap한다.
        // list swap시 보다 안정적이기 때문. 또한 데이터만 바꾸고 노드 간의 연결관계는 수정할 필요가 없으므로  시간도 더 적게 걸린다.

        
    }

    }
}
struct bitmap *bitmap_expand(struct bitmap *bitmap,int size)
{
    /*int rsize = bitmap_size(bitmap);
    int addsize = (size+rsize);
    bitmap->bit_cnt +=size;
    bitmap->bits = (unsigned long*)realloc(bitmap,byte_cnt(size+rsize));
    bitmap_set_multiple(bitmap,rsize,rsize+size-1,false);*/
    bool set;
    int i;
    size_t rsize = bitmap_size(bitmap);  // 현재 비트맵의 사이즈
    size_t afsize;
    unsigned long* tmp;  // 현재 비트맵에 들어가있는 비트들을 복사할 배열
    struct bitmap *b;
    tmp = (unsigned long*)malloc(sizeof(unsigned long)*rsize);
    for(i = 0;i<rsize;i++)
    {
        set = bitmap_test(bitmap,i);
        if(set==true)
            tmp[i] = 1;
        else
            tmp[i] = 0;
    }
    // 현재 비트맵에 들어있는 비트들을 복사한다.
    afsize = size+ rsize;
            
    bitmap_destroy(bitmap);
    b = bitmap_create(afsize);
    // 증가된 사이즈를 가진 새로운 비트맵을 만들고 원래의 비트맵은 destory를 통해 free해준다
    for(i=0;i<rsize;i++)
    {
        if(tmp[i] == 1)
            bitmap_mark(b,i);
    }
    // 새로 create한 비트맵에 복사했던 비트들을 넣는다.
    bitmap_set_multiple(b,rsize,size,false); // 새로 추가된 공간은 0으로 채운다
    if(bitmap_size(b) != afsize)
        return NULL;
    else
        return b;
    // size가 증가하면 성공한것으로 간주하고 새로운 비트맵을 리턴 아니라면 아무것도 리턴하지않는다.
}
unsigned hash_int_2(int i)
{
    size_t u;
    int tmp;
    if(i<0)
        tmp = -i;
    else
        tmp  = i;
    u =  i%137;
    return u;
}
// 새로운 hash함수. 원래 hash_int는 hash_bytes를 호출하여 매우 큰 소수와 basis를 이용하여 해시를 구성하였지만, 우리의 testcase는 그렇게 큰 값을 필요로 할정도로 큰 경우가 없기 때문에 100보다 큰 임의의 소수를 이용하여 새로운 해시함수를 구성하였다.
int main(void)
{
    bool set;       // 대부분의 비트맵 함수에서 bool 리턴값을 받을 변수
    bool param;  // 미리 설정해서 contains에서 쓰일 변수
    int i;
    int tmpnum=0;
    int htname;
    int data=0;
    int stp,edp;
    int datat=0;
    size_t btc=0;
    size_t bmsize=0;
    size_t btc2=0;
    size_t stidx=0;             // 비트맵에서 쓰일 size_t변수들
    char input[100];            // command를 받을 문자열
    char cre[20];
    char cre2[20];
    char junk[100];
    char newname[100];          // input을 parsing할때 쓰일 변수들
    //struct bitmap *bmp;
    struct datahash *temphash; // hash 함수들에서 사용될 
    struct datalist *tempnode; // push ,insert에 사용될 datalist 임시변수
    struct datalist *ordered; // insert_ordered에 쓰일 임시변수
    struct datalist *point; // list_entry에 쓰일 datalist 임시변수
    while(1)
    {
        memset(input,0,100);
        fgets(input,100,stdin);         //명령 입력
        if(!strncmp(input,"list",4))
        {
            //printf("list\n");
            if(!strncmp(input,"list_push_front",15))
            {
                //printf("front\n");
                sscanf(input,"%s %s %d",junk,newname,&data);
                tmpnum = newname[4]-'0';
                tempnode = (struct datalist*)malloc(sizeof(struct datalist));
                tempnode->data = data;
                list_push_front((&lists[tmpnum]),&(tempnode->elem));
            }
            else if(!strncmp(input,"list_push_back",14))
            {
                //printf("back\n");
                sscanf(input,"%s %s %d",junk,newname,&data);
                //printf("data : %d\n",data);
                tmpnum = newname[4] - '0';
                tempnode = (struct datalist*)malloc(sizeof(struct datalist));
                tempnode->data = data;
                list_push_back(&lists[tmpnum],&(tempnode->elem));
            }
            else if(!strncmp(input,"list_pop_front",14))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_pop_front(&lists[tmpnum]);
                if(te == NULL)
                    printf("Nothing to pop_front\n");

            }
            else if(!strncmp(input,"list_pop_back",13))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_pop_back(&lists[tmpnum]);
                if(te == NULL)
                    printf("Nothing to pop_back\n");
            }
            else if(!strncmp(input,"list_insert",11))
            {
                if(!strncmp(input,"list_insert_ordered",19))
                {
                    sscanf(input,"%s %s %d",junk,newname,&data);
                    ordered = (struct datalist*)malloc(sizeof(struct datalist));
                    ordered->data = data;
                    tmpnum = newname[4] - '0';
                    list_insert_ordered(&lists[tmpnum],&(ordered->elem),list_less,NULL);
                }
                else{
                //printf("insert\n");
                sscanf(input,"%s %s %d %d",junk,newname,&data,&datat);
                //printf("data: %d\n",data);
                tempnode = (struct datalist*)malloc(sizeof(struct datalist));
                tempnode->data = datat;
                tmpnum = newname[4] - '0';
                te = list_begin(&lists[tmpnum]);
                for(i=0;i<data;i++)
                    te = list_next(te);
                list_insert(te,&tempnode->elem);
                }
            }
            /*else if(!strncmp(input,"list_insert_ordered",19))
            {
                printf("!");
                data = 0;
                sscanf(input,"%s %s %d",junk,newname,&data);
              
            }  printf("%d\n",data);
                ordered = (struct datalist*)malloc(sizeof(struct datalist));
                ordered->data = data;
                tmpnum = newname[4] -'0';
                //te = list_begin(&lists[tmpnum]);
                list_insert_ordered(&lists[tmpnum],&(ordered->elem),listless,NULL);
            }*/
            else if(!strncmp(input,"list_remove",11))
            {
                //iprintf("remove\n");
                sscanf(input,"%s %s %d",junk,newname,&data);
                //printf("data: %d\n",data);
                tmpnum = newname[4] - '0';
                te = list_begin(&lists[tmpnum]);
                for(i=0;i<data;i++)
                    te = list_next(te);
                list_remove(te);
            }
            else if(!strncmp(input,"list_front",10))
            {
             
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_front(&lists[tmpnum]);
                if(te==NULL)
                    printf("Nothing on the list\n");
                else
                {
                    point = list_entry(te,struct datalist,elem);
                    printf("%d\n",point->data);
                }

            }
            else if(!strncmp(input,"list_size",9))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                printf("%u\n",list_size(&lists[tmpnum]));
            }
            else if(!strncmp(input,"list_back",9))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_back(&lists[tmpnum]);
                if(te==NULL)
                    printf("Nothing on the back of list\n");
                else
                {
                    point = list_entry(te,struct datalist,elem);
                    printf("%d\n",point->data);
                }
            }
            else if(!strncmp(input,"list_empty",10))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                if(list_empty(&lists[tmpnum])==true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"list_reverse",12))
            {
            
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                list_reverse(&lists[tmpnum]);
            }
            else if(!strncmp(input,"list_max",8))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_max(&lists[tmpnum],list_less,NULL);
                point = list_entry(te,struct datalist,elem);
                printf("%d\n",point->data);
            }
            else if(!strncmp(input,"list_min",8))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                te = list_min(&lists[tmpnum],list_less,NULL);
                point = list_entry(te,struct datalist,elem);
                printf("%d\n",point->data);
            }
            else if(!strncmp(input,"list_unique",11))
            {
                datat = 0;
                for(i=0;i<strlen(input);i++)
                {
                    if(input[i]==' ')
                        datat++;
                }
                //sscanf(input,"%s %[^\n]",junk,newname);
                if(datat == 2){
                    sscanf(input,"%s %s %s",junk,cre,cre2);
                    tmpnum = cre[4] - '0';
                    data = cre2[4] - '0';
                    list_unique(&lists[tmpnum],&lists[data],list_less,NULL);
                }
                else
                {
                    sscanf(input,"%s %s",junk,newname);
                    tmpnum = newname[4] - '0';
                    list_unique(&lists[tmpnum],NULL,list_less,NULL);
                }

            }
            else if(!strncmp(input,"list_swap",9))
            {
                //printf("!");
                sscanf(input,"%s %s %d %d",junk,newname,&data,&datat);
                tmpnum = newname[4] - '0';
                //printf("%d %d %d\n",tmpnum,data,datat);
                te = list_begin(&lists[tmpnum]);
                for(i=0;i<data;i++)
                    te = list_next(te);
                te2 = list_begin(&lists[tmpnum]);
                for(i=0;i<datat;i++)
                    te2 = list_next(te2);
                list_swap(te,te2);
            }
            else if(!strncmp(input,"list_sort",9))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                list_sort(&lists[tmpnum],list_less,NULL);
            }
            else if(!strncmp(input,"list_splice",11))
            {
                sscanf(input,"%s %s %d %s %d %d",junk,newname,&data,cre,&stp,&edp);
                tmpnum = newname[4] - '0';
                datat = cre[4] - '0';
                te = list_begin(&lists[tmpnum]);
                te2 = list_begin(&lists[datat]);
                te3 = list_begin(&lists[datat]);
                for(i=0;i<data;i++)
                    te = list_next(te);
                for(i=0;i<stp;i++)
                    te2 = list_next(te2);
                for(i=0;i<edp;i++)
                    te3 = list_next(te3);
                list_splice(te,te2,te3);
            }
            else if(!strncmp(input,"list_shuffle",12))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[4] - '0';
                list_shuffle(&lists[tmpnum]);

            }
           

        }
        // 리스트 관련 command 들을 sscanf를 통해 parsing하고 인덱스를 설정해준 다음 data를 받을 것들이 있다면 받아서 주어진 command를 수행한다.
        else if(!strncmp(input,"hash",4))
        {
            if(!strncmp(input,"hash_insert",11))
            {
                sscanf(input,"%s %s %d",junk,newname,&data);
                htname = newname[4] - '0';
                temphash = (struct datahash*)malloc(sizeof(struct datahash));
                temphash->hdata = data;
                hash_insert(&hashes[htname],&(temphash->elem));
            }
            else if(!strncmp(input,"hash_find",9))
            {
                sscanf(input,"%s %s %d",junk,newname,&data);
                htname = newname[4] - '0';
                temphash = (struct datahash *)malloc(sizeof(struct datahash));
                temphash->hdata = data;
                he1 = hash_find(&hashes[htname],&(temphash->elem));
                    //printf("Nothing in hash\n");
                if(he1!=NULL)
                {
                    data = (hash_entry(he1,struct datahash,elem))->hdata;
                    printf("%d\n",data);
                }

            }
            else if(!strncmp(input,"hash_apply",10))
            {
                sscanf(input,"%s %s %s",junk,newname,cre);
                htname = newname[4] - '0';
                if(!strcmp(cre,"square"))
                {
                    hash_apply(&hashes[htname],hash_action_square);
                }
                else if(!strcmp(cre,"triple"))
                {
                    hash_apply(&hashes[htname],hash_action_triple);
                }
            }
            else if(!strncmp(input,"hash_replace",12))
            {
                sscanf(input,"%s %s %d",junk,newname,&data);
                htname = newname[4] - '0';
                temphash = (struct datahash*)malloc(sizeof(struct datahash));
                temphash->hdata = data;
                hash_replace(&hashes[htname],&(temphash->elem));
            }
            else if(!strncmp(input,"hash_delete",11))
            {
                sscanf(input,"%s %s %d",junk,newname,&data);
                htname = newname[4] - '0';
                temphash = (struct datahash*)malloc(sizeof(struct datahash));
                temphash->hdata = data;
                hash_delete(&hashes[htname],&(temphash->elem));
            }
            else if(!strncmp(input,"hash_empty",10))
            {
                if(hash_empty(&hashes[htname]) == true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"hash_size",9))
            {
                sscanf(input,"%s %s",junk,newname);
                htname = newname[4] - '0';
                printf("%u\n",hash_size(&hashes[htname]));
            }
            else if(!strncmp(input,"hash_clear",10))
            {
                sscanf(input,"%s %s",junk,newname);
                htname = newname[4] - '0';
                hash_clear(&hashes[htname],hash_action_destroy);
            }
        }
        //hash 관련 함수들을 수행한다. input을 sscanf를 통해 parsing 하고 필요한 데이터들을 sscanf를 통해서 받아서 함수들을 수행한다.
        // htname = newname[4] - '0';의 의미는 testcase에서 hashx으로 모든 해시가 설정되어 있으므로 x에 해당하는 정수를 받아서 인덱스로 활용하는 것이다.
        else if(!strncmp(input,"bitmap",6))
        {
            if(!strncmp(input,"bitmap_size",11))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[2] - '0';
                bmsize = bitmap_size(bm[tmpnum]);
                printf("%u\n",bmsize);
            }
            else if(!strncmp(input,"bitmap_mark",11))
            {
                sscanf(input,"%s %s %u",junk,newname,&btc);
                tmpnum = newname[2] - '0';
                bitmap_mark(bm[tmpnum],btc);
            }
            else if(!strncmp(input,"bitmap_dump",11))
            {
                sscanf(input,"%s %s",junk,newname);
                tmpnum = newname[2] - '0';
                bitmap_dump(bm[tmpnum]);
            }
            else if(!strncmp(input,"bitmap_set",10))
            {
                if(!strncmp(input,"bitmap_set_multiple",19))
                {
                    sscanf(input,"%s %s %u %u %s",junk,newname,&btc,&btc2,cre);
                    tmpnum = newname[2] - '0';
                    if(!strcmp(cre,"true"))
                        param = true;
                    else
                        param = false;
                    bitmap_set_multiple(bm[tmpnum],btc,btc2,param);
                }
                else if(!strncmp(input,"bitmap_set_all",14))
                {
                    sscanf(input,"%s %s %s",junk,newname,cre);
                    tmpnum = newname[2] - '0';
                    if(!strcmp(cre,"true"))
                        param = true;
                    else
                        param = false;
                    bitmap_set_all(bm[tmpnum],param);
                }
                else{
                    sscanf(input,"%s %s %u %s",junk,newname,&btc,cre);
                    tmpnum = newname[2] - '0';
                    if(!strcmp(cre,"true"))
                        param = true;
                    else
                        param = false;
                    bitmap_set(bm[tmpnum],btc,param);
                }
            }
            else if(!strncmp(input,"bitmap_scan",11))
            {
                if(!strncmp(input,"bitmap_scan_and_flip",20))
                {
                    sscanf(input,"%s %s %u %u %s",junk,newname,&stidx,&btc,cre);
                    tmpnum = newname[2] - '0';
                    if(!strcmp(cre,"true"))
                        param = true;
                    else
                        param = false;
                    btc2= bitmap_scan_and_flip(bm[tmpnum],stidx,btc,param);
                    printf("%u\n",btc2);
                }
                else{
                    sscanf(input,"%s %s %u %u %s",junk,newname,&stidx,&btc,cre);
                    tmpnum = newname[2] - '0';
                    if(!strcmp(cre,"true"))
                        param = true;
                    else
                        param = false;
                    btc2 = bitmap_scan(bm[tmpnum],stidx,btc,param);
                    printf("%u\n",btc2);
                }
            }
            else if(!strncmp(input,"bitmap_test",11))
            {
                sscanf(input,"%s %s %u",junk,newname,&stidx);
                tmpnum = newname[2] - '0';
                if(bitmap_test(bm[tmpnum],stidx)==true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"bitmap_flip",11))
            {
                sscanf(input,"%s %s %u",junk,newname,&stidx);
                tmpnum = newname[2] - '0';
                bitmap_flip(bm[tmpnum],stidx);
            }
            else if(!strncmp(input,"bitmap_none",11))
            {
                sscanf(input,"%s %s %u %u",junk,newname,&stidx,&btc);
                tmpnum = newname[2] - '0';
                set = bitmap_none(bm[tmpnum],stidx,btc);
                if(set==true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"bitmap_all",10))
            {
                sscanf(input,"%s %s %u %u",junk,newname,&stidx,&btc);
                tmpnum = newname[2] - '0';
                set = bitmap_all(bm[tmpnum],stidx,btc);
                if(set==true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"bitmap_any",10))
            {
                sscanf(input,"%s %s %u %u",junk,newname,&stidx,&btc);
                tmpnum = newname[2] - '0';
                set = bitmap_any(bm[tmpnum],stidx,btc);
                if(set == true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"bitmap_contains",15))
            {
                sscanf(input,"%s %s %u %u %s",junk,newname,&stidx,&btc,cre);
                tmpnum = newname[2] - '0';
                if(!strcmp(cre,"true"))
                    param = true;
                else
                    param = false;
                set = bitmap_contains(bm[tmpnum],stidx,btc,param);
                if(set==true)
                    printf("true\n");
                else
                    printf("false\n");
            }
            else if(!strncmp(input,"bitmap_count",12))
            {
                sscanf(input,"%s %s %u %u %s",junk,newname,&stidx,&btc,cre);
                tmpnum = newname[2] - '0';
                if(!strcmp(cre,"true"))
                    param = true;
                else
                    param = false;
                btc2 = bitmap_count(bm[tmpnum],stidx,btc,param);
                printf("%u\n",btc2);
            }
            else if(!strncmp(input,"bitmap_reset",12))
            {
                sscanf(input,"%s %s %u",junk,newname,&stidx);
                tmpnum = newname[2] - '0';
                bitmap_reset(bm[tmpnum],stidx);
            }
            else if(!strncmp(input,"bitmap_expand",13))
            {
                sscanf(input,"%s %s %u",junk,newname,&btc);
                tmpnum = newname[2] - '0';
                bm[tmpnum] =bitmap_expand(bm[tmpnum],btc);
               // bm[tmpnum] = bmp;
                /*btc2 = bitmap_size(bm[tmpnum]);
                tmpbm = bitmap_create(btc+btc2);
                //btc += bitmap_size(bm[tmpnum]);
                tmpbm = bitmap_expand(bm[tmpnum],btc);
                bitmap_destroy(bm[tmpnum]);
                bm[tmpnum] = tmpbm;*/

            }
            memset(junk,0,sizeof(junk));
            memset(newname,0,sizeof(newname));
            memset(cre,0,sizeof(cre));

        }
        // bitmap관련 함수들을 sscanf를 통한 parsing으로 해결하여 수행한다. sscanf를 통해 input에서 필요한 데이터들과 command를 분리하여 사용하면 되는 구조이다.
        // bitmap에서는 list나 hash와 다르게 size_t가 사용되므로 parameter나 return 값으로 size_t를 이용할 경우 항상 %u나 size_t변수값을 이용하여야 한다.
        // 각각의 기능은 document에 설명되어있다.
        else if(!strncmp(input,"dumpdata",8))
        {
            sscanf(input,"%s %s",junk,newname);
           // printf("%s\n",newname);
            if(!strncmp(newname,"list",4))
            {
                tmpnum = newname[4] - '0';
                dumplist(tmpnum);
            }
            else if(!strncmp(newname,"bm",2))
            {
                //printf("1\n");
                tmpnum = newname[2] - '0';
                bmsize = bitmap_size(bm[tmpnum]);
                for(i=0;i<bmsize;i++)
                {
                    if(bitmap_test(bm[tmpnum],i) == true)
                        printf("1");
                    else
                        printf("0");

                }
                printf("\n");
                //bitmap_dump(bm[tmpnum]);
            }
            else if(!strncmp(newname,"hash",4))
            {
                //printf("2\n");
                htname = newname[4] - '0';
                hash_first(&iter,&hashes[htname]);
                i=0;
                while(hash_next(&iter))
                {
                    struct datahash *dh1 = hash_entry(hash_cur(&iter),struct datahash,elem);
                    printf("%d ",dh1->hdata);
                    i++;
                }
                if(i!=0)
                    printf("\n");
            }
        }
        // list,hash,bitmap에 현재 들어있는 데이터를 출력해준다.
        else if(!strncmp(input,"create",6))
        {
            if(!strncmp(input,"create list",11))
            {
                sscanf(input,"%s %s %s",cre,junk,newname);
                //printf("list name is %s\n",newname);
                //listcreate(newname);
                listcreate(newname);
            }
            else if(!strncmp(input,"create hashtable",16))
            {
                sscanf(input,"%s %s %s",cre,junk,newname);
                //printf("hastable name is %s\n",newname);
                tmpnum = newname[4] - '0';
                hash_init(&hashes[tmpnum],hash_hash,hash_less,NULL);
            }
            
            else if(!strncmp(input,"create bitmap",13))
            {
            
                sscanf(input,"%s %s %s %d",cre,junk,newname,&btc);
                tmpnum = newname[2] - '0';
                bm[tmpnum] = bitmap_create(btc);

            }
        }
        // 각각의 list,hashtable,bitmap 을 생성해준다. 
        else if(!strncmp(input,"delete",6))
        {
            sscanf(input,"%s %s",junk,newname);
            if(!strncmp(newname,"list",4))
            {
                tmpnum = newname[4]-'0';
                deletelist(tmpnum);
            }
            else if(!strncmp(newname,"bm",2))
            {
                tmpnum = newname[2] - '0';
                deletebm(tmpnum);
            }
            else if(!strncmp(newname,"hash",4))
            {
                tmpnum = newname[4] - '0';
                hash_destroy(&hashes[tmpnum],hash_action_destroy);
            }
        }
        //각각의 list,hashtable,bitmap을 delete해준다.
        else
        {
            if(!strncmp(input,"quit",4))
                break;
            // quit이 입력되면 바로 끝.
            else
                printf("???\n");
        }
    }
    return 0;
}
