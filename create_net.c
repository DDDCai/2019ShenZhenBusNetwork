#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>

// #define LINE (0x1<<(atoi(entry->d_name)-1));
#define MAX_SIZE 600000

typedef struct node {
    int id;
    int *line;
    int edge_num;  //  degree.
    // struct edge *e;
} node;

typedef struct edge {
    node *node_id;
    int weight;
    struct edge *next;
} edge;

struct {
    int sum;
    char **name;
    edge **e;
    node **subway_station;
} bus_net;

void get_more_memory(int *n)
{
    node **tmp1 = (node**)malloc(sizeof(node*)*((*n)+50));
    char **tmp2 = (char**)malloc((*n+50)*sizeof(char*));
    edge **tmp3 = (edge**)malloc((*n+50)*sizeof(edge*));
    memcpy(tmp1,bus_net.subway_station,(*n)*sizeof(node*));
    memcpy(tmp2,bus_net.name,(*n)*sizeof(char*));
    memcpy(tmp3,bus_net.e,(*n)*sizeof(edge*));
    free(bus_net.subway_station);
    free(bus_net.name);
    free(bus_net.e);
    bus_net.subway_station = tmp1;
    bus_net.name = tmp2;
    bus_net.e = tmp3;
    (*n) += 50;
}

int add_a_node(char *buf,int *sum,int *n,struct dirent *entry)
{
    int i;
    //  Get a node.
    for(i=0;i<*sum;i++)
    {
        if(!strcmp(buf,bus_net.name[i]))
            break;
    }
    //  Add a node.
    if(i==*sum)
    {
        if(*sum>=*n)
            get_more_memory(n);
        bus_net.name[*sum] = (char*)malloc(32);
        // bus_net.name[*sum] = (char*)malloc(64);
        bus_net.subway_station[*sum] = (node*)malloc(sizeof(node));
        bus_net.subway_station[*sum]->line = (int*)malloc(sizeof(int)*2);
        bus_net.subway_station[*sum]->line[0] = 1;
        bus_net.subway_station[*sum]->line[1] = atoi(entry->d_name);
        strcpy(bus_net.name[*sum],buf);
        bus_net.subway_station[*sum]->id = *sum;
        bus_net.subway_station[*sum]->edge_num = 0;
        bus_net.e[*sum] = NULL;
        (*sum) ++;
    }
    else 
    {
        int *line_ = bus_net.subway_station[i]->line;
        int num = line_[0];
        int *tmp = (int*)malloc(sizeof(int)*(num+1));
        memcpy(tmp,line_,num*sizeof(int));
        tmp[num] = atoi(entry->d_name);
        tmp[0] ++;
        free(line_);
        bus_net.subway_station[i]->line = tmp;
    }
    return i;
}

void add_an_edge(char *buf,node* from,node* to)
{
    struct edge *tmp = bus_net.e[from->id];
    while(tmp)
    {
        if(tmp->node_id->id!=to->id)
            tmp = tmp->next;
        else 
            break;
    }
    if(tmp==NULL)
    {
        struct edge *p = (struct edge*)malloc(sizeof(struct edge));
        p->node_id = to;
        p->weight = 1;
        p->next = bus_net.e[from->id];
        bus_net.e[from->id] = p;
        from->edge_num ++;
    }
    else 
    {
        tmp->weight ++;
    }
}

void produce_net_from_files()
{
    FILE *fp;
    char buf[32],buf2[16];
    // char buf2[16];
    // int n = 100;
    int n = 1500;
    int sum = 0;
    int i;
    int last;
    bus_net.subway_station = (node**)malloc(sizeof(node*)*n);
    bus_net.name = (char**)malloc(n*sizeof(char*));
    bus_net.e = (edge**)malloc(n*sizeof(edge*));

    DIR *dir = opendir("./bus_data");
    struct dirent *entry;
    while((entry=readdir(dir))!=0)
    {
        if(!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        char path[32] = "./bus_data/";
        strcat(path,entry->d_name);
        fp = fopen(path,"r");
        // char buf[32];
        fgets(buf,32,fp);
        //  Get a node.
        last = add_a_node(buf,&sum,&n,entry);
        //  Get a weight.
        fgets(buf2,16,fp);
        if(!strcmp(buf2,"0"))
            continue ;       
        while(fgets(buf,32,fp))
        {
            //  Get a node.
            i = add_a_node(buf,&sum,&n,entry);
            //  Add edges.
            add_an_edge(buf2,bus_net.subway_station[last],bus_net.subway_station[i]);
            add_an_edge(buf2,bus_net.subway_station[i],bus_net.subway_station[last]);
            //  Get a weight.
            fgets(buf2,16,fp);
            if(!strcmp(buf2,"0"))
                break;
            
            last = i;
        }
        fclose(fp);
    }
    bus_net.sum = sum;
}

int transfor_name_to_id(char *name)
{
    int i;
    for(i=0;i<bus_net.sum;i++)
    {
        if(!strcmp(name,bus_net.name[i]))
            return i;
    }
    printf("Can't find this station %s in ShenZhen!\n",name);
    return -1;
}

char *transfor_id_to_name(int id)
{
    if(id < 0 || id >= bus_net.sum)
    {
        printf("Wrong ID!\n");
        return NULL;
    }
    return bus_net.name[id];
}

void free_memory()
{
    int i;
    struct edge *p,*q;
    for(i=0;i<bus_net.sum;i++)
    {
        free(bus_net.name[i]);
        p = bus_net.e[i];
        while(p)
        {
            q = p;
            p = p->next;
            free(q);
        }
        free(bus_net.subway_station[i]->line);
        free(bus_net.subway_station[i]);
    }
    free(bus_net.name);
    free(bus_net.subway_station);
    free(bus_net.e);
}

float average_degree()
{
    // produce_net_from_files();
    int i,degree=0;
    for(i=0;i<bus_net.sum;i++)
    {
        degree += bus_net.subway_station[i]->edge_num;
    }
    return ((float)degree)/bus_net.sum;
}

//  The number of i degree nodes is tmp[i].
int* degree_distribution()
{
    // produce_net_from_files();
    int degree[256];
    memset(degree,0,256*sizeof(int));
    int i;
    for(i=0;i<bus_net.sum;i++)
    {
        degree[bus_net.subway_station[i]->edge_num] ++;
    }
    for(i=0;i<256;i++)
    {
        if(degree[i]!=0)
            degree[0] = i;
    }
    int *tmp = (int*)malloc(sizeof(int)*(degree[0]+1));
    for(i=0;i<=degree[0];i++)
        tmp[i] = degree[i];
    return tmp;
}

float clustering_coefficient(int id)
{
    int n_num = bus_net.subway_station[id]->edge_num;
    if(n_num == 1 || n_num == 0)
        return 0;
    float t = n_num*(n_num - 1)/2.0;
    int *neighbors = (int*)malloc(n_num*sizeof(int));
    int i,j;
    int E = 0;
    struct edge *p = bus_net.e[id];
    for(i=0;i<n_num;i++)
    {
        neighbors[i] = p->node_id->id;
        p = p->next;
    }
    for(i=0;i<n_num;i++)
    {
        p = bus_net.e[neighbors[i]];
        while(p)
        {
            int tmp = p->node_id->id;
            for(j=i;j<n_num;j++)
            {
                if(tmp==neighbors[j])
                {
                    E ++;
                    break;
                }
            }
            p = p->next;
        }
    }
    free(neighbors);
    return E/t;
}

float average_c()
{
    int i=0;
    float sum_c = 0;
    for(i=0;i<bus_net.sum;i++){
        sum_c += clustering_coefficient(i);
        // printf("%d\n",i);
        }
    return sum_c/bus_net.sum;
}

int** transfor_table_to_matrix()
{
    int **matrix = (int**)malloc(sizeof(int*)*bus_net.sum);
    int i;
    for(i=0;i<bus_net.sum;i++)
    {
        matrix[i] = (int*)malloc(bus_net.sum*sizeof(int));
        for(int j=0;j<bus_net.sum;j++)
            matrix[i][j] = MAX_SIZE;
        matrix[i][i] = 0;
    }
    struct edge *p;
    for(i=0;i<bus_net.sum;i++)
    {
        p = bus_net.e[i];
        while(p)
        {
            matrix[i][p->node_id->id] = 1;
            p = p->next;
        }
    }
    return matrix;
}

int** transfor_table_to_matrix_with_weight()
{
    int **matrix = (int**)malloc(sizeof(int*)*bus_net.sum);
    int i;
    for(i=0;i<bus_net.sum;i++)
    {
        matrix[i] = (int*)malloc(bus_net.sum*sizeof(int));
        for(int j=0;j<bus_net.sum;j++)
            matrix[i][j] = MAX_SIZE;
        matrix[i][i] = 0;
    }
    struct edge *p;
    for(i=0;i<bus_net.sum;i++)
    {
        p = bus_net.e[i];
        while(p)
        {
            matrix[i][p->node_id->id] = p->weight;
            p = p->next;
        }
    }
    return matrix;
}

int deep_search(int start,int *done)
{
    int node_num = 1;
    done[start] = 1;
    edge *p = bus_net.e[start];
    while(p)
    {
        if(!done[p->node_id->id])
            node_num += deep_search(p->node_id->id,done);
        p = p->next;
    }
    return node_num;
}

int *num_of_sub_graph()
{
    int num = 0;
    int *done = (int*)malloc(sizeof(int)*bus_net.sum);
    int *node_num = (int*)malloc(sizeof(int));
    node_num[0] = 0;
    memset(done,0,sizeof(int)*bus_net.sum);
    int i;
    while(1)
    {
        for(i=0;i<bus_net.sum;i++)
        {
            if(done[i]==0)
            {
                num ++;
                int *tmp = (int*)malloc(sizeof(int)*(num+1));
                memcpy(tmp,node_num,sizeof(int)*num);
                free(node_num);
                node_num = tmp;
                break;
            }
        }
        if(i==bus_net.sum)
            break;
        // node_num = 0;
        node_num[num] = deep_search(i,done);
    }
    node_num[0] = num;
    return node_num;
}

float average_shortest_path()
{
    int **matrix = transfor_table_to_matrix();
    int i,j,k;
    int sum = bus_net.sum;
    for(i=0;i<sum;i++)
    {
        for(j=0;j<sum;j++)
        {
            for(k=0;k<sum;k++)
            {
                if(matrix[j][k] > matrix[j][i] + matrix[i][k])
                    matrix[j][k] = matrix[j][i] + matrix[i][k];
            }
        }
        // printf("shortest:%d\n",i);
    }
    int sum_of_path = 0;
    for(i=0;i<sum;i++)
    {
        for(j=0;j<sum;j++)
        {
            if(matrix[i][j]>=MAX_SIZE)
                continue;
            sum_of_path += matrix[i][j];
        }
    }

    ///////////////////////////////////////////
    for(i=0;i<sum;i++)
        free(matrix[i]);
    free(matrix);
    ///////////////////////////////////////////

    int *sub = num_of_sub_graph();
    int sub_num = sub[0];
    int path_num = 0;
    for(i=1;i<=sub_num;i++)
    {
        path_num += ((sub[i]-1)*sub[i]);
    }
    if(!path_num)
        return 0;
    else 
        return ((float)sum_of_path)/path_num;

    // return ((float)sum_of_path)/((sum-1)*sum);
}

void delete_all_k_degree_nodes(int k,int *dead,int id)
{
    struct edge *p,*q;
    if(bus_net.subway_station[id]->edge_num <= k)
    {
        bus_net.subway_station[id]->edge_num = -1;
        dead[id] = 1;
        p = bus_net.e[id];
        while(p!=NULL)
        {
            int nid = p->node_id->id;

            if(dead[nid]!=1)
            {
                p->node_id->edge_num --;
                q = bus_net.e[nid];
                struct edge *pre;
                if(q->node_id->id==id)
                {
                    bus_net.e[nid] = q->next;
                }
                else 
                {
                    do{
                        pre = q;
                        q = q->next;
                    } while(q->node_id->id!=id);
                    pre->next = q->next;
                }
                free(q);
                delete_all_k_degree_nodes(k,dead,nid);
                // printf("%d\n",nid);
            }

            q = p;
            p = p->next;
            free(q);
        }
        bus_net.e[id] = NULL;
    }
}

int coreness_of_a_node(int id)
{
    int *dead = (int*)malloc(sizeof(int)*bus_net.sum);
    memset(dead,0,bus_net.sum*sizeof(int));
    int k = 1;
    int i;
    
    while(1)
    {
        for(i=0;i<bus_net.sum;i++)
        {
            if((bus_net.subway_station[i]->edge_num <= k) \
                && (bus_net.subway_station[i]->edge_num != -1))
                break;
        }
        if(i<bus_net.sum)
            delete_all_k_degree_nodes(k,dead,i);
        else 
        {
            if(dead[id])
            {
                free(dead);
                free_memory();
                produce_net_from_files();
                return k;
            }
            else 
                k ++;
        }
    }
}

int coreness_of_a_graph()
{
    int *dead = (int*)malloc(sizeof(int)*bus_net.sum);
    memset(dead,0,sizeof(int)*bus_net.sum);
    int k = 1;
    int i;

    while(1)
    {
        for(i=0;i<bus_net.sum;i++)
        {
            if((bus_net.subway_station[i]->edge_num <= k) && \
                bus_net.subway_station[i]->edge_num != -1)
                break;
        }
        if(i<bus_net.sum)
            delete_all_k_degree_nodes(k,dead,i);
        else 
        {
            i = 0;
            while((i<bus_net.sum)&&(dead[i]))
                i ++;
            if(i==bus_net.sum)
            {
                free(dead);
                free_memory();
                produce_net_from_files();
                    //  Can be deleted but be careful about the free_memory() in main function.
                return k;
            }
            else 
                k ++;
        }
    }
    return 0;
}

int coreness_of_a_station(char *name)
{
    int id = transfor_name_to_id(name);
    if(id==-1)
        return -1;
    return coreness_of_a_node(id);
}

int degree_of_a_station(char *name)
{
    int id = transfor_name_to_id(name);
    if(id==-1)
        return -1;
    return bus_net.subway_station[id]->edge_num;
}

float c_of_a_station(char *name)
{
    int id = transfor_name_to_id(name);
    if(id==-1)
        return -1;
    return clustering_coefficient(id);
}

void c_free(void *pointer)
{
    free(pointer);
}

int get_node_num()
{
    return bus_net.sum;
}

void attack(int id)
{
    edge *p = bus_net.e[id];
    int nid;
    while(p)
    {
        nid = p->node_id->id;
        bus_net.subway_station[nid]->edge_num --;
        edge *q = bus_net.e[nid];
        edge *pre;
        if(q->node_id->id==id)
        {
            bus_net.e[nid] = q->next;
        }
        else 
        {
            do{
                pre = q;
                q = q->next;
            } while(q->node_id->id!=id);
            pre->next = q->next;
        }
        free(q);
        p = p->next;
    }
    p = bus_net.e[id];
    while(p)
    {
        edge *q = p;
        p = p->next;
        free(q);
    }
    free(bus_net.name[id]);
    free(bus_net.subway_station[id]);
    int i;
    for(i=id;i<bus_net.sum-1;i++)
    {
        bus_net.name[i] = bus_net.name[i+1];
        bus_net.subway_station[i] = bus_net.subway_station[i+1];
        bus_net.e[i] = bus_net.e[i+1];
        bus_net.subway_station[i]->id --;
    }
    bus_net.sum --;
}

void random_attack(int percent)
{
    int times = bus_net.sum*percent/100;
    int i = 0,j;
    int id;
    while(i<times)
    {
        srand((unsigned)time(NULL));
        id = rand() % bus_net.sum;
        attack(id);
        i++;
    }
}

void intention_attack(int percent)
{
    int times = bus_net.sum*percent/100;
    int *will_do = (int*)malloc(sizeof(int)*times);
    int i=0,j=0,k;
    int *degree = degree_distribution();
    i = degree[0];
    while(j<times)
    {
        j += degree[i];
        k = i;
        i --;
    }
    int need_i = degree[k] - j + times;
    for(j=0,k=0;j<bus_net.sum && k<times;j++)
    {
        if((bus_net.subway_station[j]->edge_num > i) && (bus_net.subway_station[j]->edge_num <= degree[0]))
        {
            if(bus_net.subway_station[j]->edge_num==i+1)
            {
                if(need_i)
                {
                    will_do[k++] = j;
                    need_i --;
                }
                continue;
            }
            will_do[k++] = j;
        }
    }
    // node **tmp = (node**)malloc(sizeof(node*)*k);
    // for(j=0;j<k;j++)
    //     tmp[j] = bus_net.subway_station[will_do[j]];
    for(j=0;j<k;j++)
        attack(will_do[j]-j);   
        // attack(tmp[j]->id);
    free(degree);
    free(will_do);
    // free(tmp);
}

int* dijkstra(int **C,int start,int end,int n)
{
    start= start+1;
    for(int i=0;i<bus_net.sum;i++)
    {
        for(int j=0;j<bus_net.sum;j++)
            if(i==j) C[i][j]=MAX_SIZE;
    }
    int D[n];
    int P[n],S[n];
    int i,j,k,v1,pre;
    int min,max=600000,inf=800000;
    v1=start-1;
    for(i=0;i<n;i++)
    {
        D[i]=C[v1][i];
        if(D[i]!=max) 
            P[i]=start;
        else
            P[i]=0;
    }
    for(i=0;i<n;i++)
        S[i]=0; 
    S[v1]=1;
    D[v1]=0;
    for(i=0;i<n-1;i++)
    {
        min=inf;
        for(j=0;j<n;j++)
        {
            if((!S[j])&&(D[j]<min))
            {
                min=D[j];
                k=j;
            }
        }
        S[k]=1;
        for(j=0;j<n;j++)
        {
            if((!S[j])&&(D[j]>D[k]+C[k][j]))
            {
                D[j]=D[k]+C[k][j]; 
                P[j]=k+1; 
            }
        }
    }

    int *re= (int*)malloc(sizeof(int)*n*2);
    int *be= (int*)malloc(sizeof(int)*n*2);
    for(i=0;i<n;i++)
    {
        if(end!= i)
            continue;
        int j=1;
        pre=P[i];
        re[j++]=i;
        while(pre!=0)
        {
            re[j++]=pre-1;
            pre=P[pre-1];
        }
        re[0]=j-1;
    }
    be[0]=re[0];
    int number =re[0];
    for(int i=1;i<=number;i++)
        be[i]=re[number-i+1];
    
    free(re);
    return be;
}

int go_deep_line(int *path,int *line,int which,int id)
{
    node *p = bus_net.subway_station[path[id]];
    int i;
    for(i=1;i<=p->line[0];i++)
    {
        if(p->line[i]==which)
        {
            if(id==path[0])
            {
                line[id] = which;
                return 1;
            }
            if(go_deep_line(path,line,which,id+1))
            {
                line[id] = which;
                return 1;
            }
            else 
            {
                for(i=1;i<=p->line[0];i++)
                {
                    if(p->line[i]==which)
                        continue;
                    if(go_deep_line(path,line,p->line[i],id+1))
                    {
                        line[id] = p->line[i];
                        return 1;
                    }
                }
                return 0;
            }
        }
    }
    return 0;
}

int *change_line(int *path)
{
    int length = path[0];
    int *line_path = (int*)malloc(sizeof(int)*(length+1));
    line_path[0] = length;
    if(go_deep_line(path,line_path,bus_net.subway_station[path[1]]->line[1],1))
        return line_path;
    else 
        return NULL;
}

int *find_a_path(char *from_n,char *to_n)
{
    int from = transfor_name_to_id(from_n);
    int to = transfor_name_to_id(to_n);
    int **matrix = transfor_table_to_matrix();
    int n = bus_net.sum;
    int *tmp = dijkstra(matrix,from,to,n);
    int *line = change_line(tmp);
    for(int i=0;i<n;i++)
        free(matrix[i]);
    free(matrix);
    return line;
}


// void main()
// {
//     produce_net_from_files();

//     // printf("%d----\n",bus_net.sum);
//     int *degree = degree_distribution();
//     int i;
//     // for(i=1;i<=degree[0];i++)
//     //     printf("%d-----%d\n",i,degree[i]);
//     // free(degree);
//     // printf("\n");
//     // printf("ave_degree:%f\n",average_degree());
//     // printf("coefficient:%f\n",average_c());
//     printf("ave_sh_pa:%f\n",average_shortest_path());
//     // printf("coreness:%d\n",coreness_of_a_graph());
//     // printf("sub:%d\n",num_of_sub_graph());

//     // random_attack(30);
//     intention_attack(30);
    
//     // printf("%d----\n",bus_net.sum);
//     // degree = degree_distribution();
//     // for(i=1;i<=degree[0];i++)
//     //     printf("%d-----%d\n",i,degree[i]);
//     // free(degree);
//     // printf("\n");
//     // printf("ave_degree:%f\n",average_degree());
//     // printf("coefficient:%f\n",average_c());
//     printf("ave_sh_pa:%f\n",average_shortest_path());
//     int *sub = num_of_sub_graph();
//     printf("sub:%d\n",sub[0]);
//     free(sub);

//     // printf("coreness:%d\n",coreness_of_a_graph());

//     free_memory();
// }

// void main()
// {
//     produce_net_from_files();
//     char sta[] = "哈工大深圳\r\n";
//     int all = bus_net.sum,i;
//     for(i=0;i<all;i++)
//     {
//         if(!strcmp(sta,bus_net.name[i]))
//             break;
//     }
//     if(i!=all)
//         printf("%d\n",bus_net.subway_station[i]->edge_num);
    
//     random_attack(60);
//     all = bus_net.sum,i;
//     for(i=0;i<all;i++)
//     {
//         if(!strcmp(sta,bus_net.name[i]))
//             break;
//     }
//     if(i!=all)
//         printf("%d\n",bus_net.subway_station[i]->edge_num);
//     else 
//         printf("error\n");
// }

// void main()
// {
//     produce_net_from_files();

//     char a[] = "哈工大园区\r\n";
//     char b[] = "新屋村\r\n";


//     int aid = transfor_name_to_id(a);
//     int bid = transfor_name_to_id(b);
//     printf("%d,%d\n",aid,bid);
//     int path[3] = {2,aid,bid};
//     int *re = change_line(path);
//     for(int i=0;i<=re[0];i++)
//         printf("%d\n",re[i]);
// }