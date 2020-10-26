#define _CRT_SECURE_NO_WARNINGS
#include "iostream"
#include "malloc.h"
#include "string.h"
#include <vector>
using namespace std;
#define VTXNUM  10 //node number+1
#define CPUNUM  2
#define MAX(a,b) ((a)>(b))?(a):(b)
int Layer[VTXNUM][VTXNUM];
bool TCflag=0;                          //�����Ʊ�־λ�������������ƣ�������Ϊ1��
//int cost[VTXNUM][VTXNUM];
int cost[VTXNUM][VTXNUM]={/*     T1      T2      T3     T4      T5     T6     T7     T8    T9 */
	                       0,    0,      0,      0,     0,      0,     0,     0,     0,     0,   
	           /*T1*/      0,    2,      4,      1,     1,      1,     0,     0,     0,     0, 
               /*T2*/      0,    0,      3,      0,     0,      0,     1,     12,    6,     0,
		       /*T3*/      0,	 0,      0,      3,     0,      0,     0,     12,    1,     0,          
		       /*T4*/      0,    0,      0,      0,     4,      0,     0,     0,     1,     0,
			   /*T5*/      0,    0,      0,      0,     0,      5,     0,     0,     10,    0,
		       /*T6*/      0,    0,      0,      0,     0,      0,     4,     0,     0,     10,
		       /*T7*/	   0,    0,      0,      0,     0,      0,     0,     4,     0,     10,
		       /*T8*/	   0,    0,      0,      0,     0,      0,     0,     0,     4,     10,
		       /*T9*/      0,    0,      0,      0,     0,      0,     0,     0,     0,      2
};//all the cost 

struct task
{
	int cpuid;
	int start;
	int end;
}Task[VTXNUM];

struct cpu
{
	int T[VTXNUM];
	int usedtime;
	int num;
}CPU[CPUNUM];



/*����ԭʼ����*/
void CreateOriginMatrix()
{
	int i=0,j=0;
	//1.input Task  execute time
	for(i=1;i<VTXNUM;i++)
	{
		printf("����������%d��ִ��ʱ�䣺",i);
		scanf("%d",&cost[i][i]);
	}

	//2.input comm between Taski and taskj
	while(1)
	{
		int c=0;
		printf("===============================��ѡ��=====================================\n");
		printf("=============1. ��������ͨ��ֵ=========2. ��������ͨ��ֵ==================\n");
		scanf("%d",&c);
		if(c==1)
		{
			printf("������i��j������i������j֮���ͨ��ʱ��\n");
			scanf("%d %d",&i,&j);
			scanf("%d",&cost[i][j]);
		}
		if(c==2)
		{
			break;	
		}
	}
	
}

/*��ӡ����x*/
void PrintMatrix(int (*x)[VTXNUM],char *log)
{
	printf("===============================%s===============================\n",log);
	for(int i=1;i<VTXNUM;i++)
	{
		for(int j=1;j<VTXNUM;j++)
		{
			cout<<x[i][j]<<'\t';
		}
		cout<<'\n';
	}
}

/*����һ����ʱ���󣬱����޸�ԭ����*/
void Copy(int (*src)[VTXNUM],int (*dest)[VTXNUM])
{
	for(int i=0;i<VTXNUM;i++)
	{
		for(int j=0;j<VTXNUM;j++)
		{
			dest[i][j]=src[i][j];
		}
	}
}

/*�ж�taski�����Ƿ����Ϊ0*/
bool IsIndgreeZero(int (*x)[VTXNUM],int taski)
{
	for(int j=1;j<taski;j++)
	{
		if(x[j][taski]!=0)
		{
			return false;
		}
	}
	return true;
}

/*���ѷ�������������е�ͨ��ֵ��Ϊ0�������Ը�����Ϊ��β����ص�����ڵ����Ⱦ���һ����ͬʱ��Ǹ�����*/
void SetZero(int (*x)[VTXNUM],vector<int> taski)
{
	int size=taski.size();
	int row=0;
	if(size!=0)
	{
		for(int i=0;i<taski.size();i++)
		{
			row=taski[i];
			for(int j=1;j<VTXNUM;j++)
			{
				x[row][j]=0;
			}
			x[row][row]=-1;
		}
	}
	
}
/*�ֲ�*/
void Layered()
{
	int tmp[VTXNUM][VTXNUM];
	int i,j;
	vector<int> zerotask;              //�洢�ѷ��������
	Copy(cost,tmp);
	PrintMatrix(tmp,"OriginMatrix");

	for(int k=1;k<VTXNUM;k++)
	{
		int z=1;
		for(i=1;i<VTXNUM;i++)
		{
			if(tmp[i][i]==-1)
			{
				continue;
			}
			if(IsIndgreeZero(tmp,i))
			{
				Layer[k][z++]=i;
				zerotask.push_back(i);
			}	
		}
		SetZero(tmp,zerotask);
		zerotask.clear();
	}
	PrintMatrix(Layer,"LayeredMatrix");
}

//��CPU���г�ʼ��
void InitiCPU()
{
	for(int i=0;i<CPUNUM;i++)
	{
		for(int j=0;j<VTXNUM;j++)
			CPU[i].T[j]=0;	
		CPU[i].usedtime=0;
		CPU[i].num=0;
	}
}

//������CPU��ʼ��--ÿ��CPU�ĵ�һ�����������Ϊtask1
void InitiCPU_TaskCopy()
{
	for(int i=0;i<CPUNUM;i++)
	{
		for(int j=1;j<VTXNUM;j++)
		{
			if(j==1)
			{
				CPU[i].T[j]=1;
			}
			CPU[i].T[j]=0;	
		}
		CPU[i].usedtime=2;
		CPU[i].num=1;
	}
	TCflag=true;
}

void InitiTask()
{
	for(int i=1;i<VTXNUM;i++)
	{
		//if(TCflag==true&&i==1)
		//{
		//	Task[i].cpuid=0;                           //?????????????????????????????
		//	Task[i].start=0;
		//	Task[i].end=2;
		//	continue;
		//}
		Task[i].cpuid=0;
		Task[i].start=0;
		Task[i].end=0;
	}
}


//�ҳ�����task��ǰ���ڵ����У�������
vector<int> FindPrevTask(int task)
{
	vector<int> prev_task;
	//��DAG�����У�task�����ǰ������Ϊ��1-task-1�У�task����ֵ�������ֵ���ϡ�
	for(int k=1;k<task;k++)
	{
		if(cost[k][task]!=0)
		{
			prev_task.push_back(k);
		}
	}
	return prev_task;
}


//��ӡ����task��ǰ������
void PrintPrevTask(int task,vector<int> prev_task)
{
	if(prev_task.size()==0)
	{
		printf("����%dΪ��ʼ����\n",task);
		return;
	}
	printf("����%d��ǰ������Ϊ��",task);
	for(int k=0;k<prev_task.size();k++)
	{
		printf("%d��",prev_task[k]);
	}
	printf("\n");
}

//��ӡ���������ǰ�����񼯡�
void TestPrevTasks()
{
	printf("===============================PrevTasks===============================\n");
	for(int i=1;i<VTXNUM;i++)
	{
		PrintPrevTask(i,FindPrevTask(i));
	}
}

//��������i��cpuid��cpu�Ŀ�ʼʱ��
int CalStartTime(int cpuid,int distribute_task,vector<int> prev_task)
{
	int max=CPU[cpuid].usedtime;            //cpu��ʹ��ʱ��
	int tmpstarttime=0;            //��ʱ��ʼʱ��
	int size=prev_task.size();
	if(distribute_task==1)
	{
		return 0;
	}
	for(int i=0;i<size;i++)
	{
		//�ж������ǰ���������У��Ƿ���ô�����������һ��CPU�ϣ��������һ��CPU�ϣ�������ʼʱ��Ϊ��CPU usedtime������Ϊ��max{ǰ��end+ͨ��ֵ}
		if(Task[prev_task[i]].cpuid!=cpuid)
		{
			tmpstarttime=Task[prev_task[i]].end+cost[prev_task[i]][distribute_task];
		}
		max=MAX(max,tmpstarttime);
	}
	return max;
}

//Ϊ����distribute_task�����ʱ��С��CPU
void FindMinCostCPU(int distribute_task)
{
	int cpuid=0;
	vector<int> prev_task;                    //ǰ������
	int minstarttime=0x00ffffff;              //��С��ʼʱ��
	int tmp=0;
	int mincpuid=0;                           //��С��CPU���
	//1.���α�������CPU������С��ʱ��һ����������cpu�ṹ��
	for(cpuid;cpuid<CPUNUM;cpuid++)
	{
		//2.��ǰ�����񣬲���¼����
		prev_task=FindPrevTask(distribute_task);
		//3.���㽫��������䵽��CPU�Ϻ󣬸�����Ŀ�ʼʱ�䡣max{��cpu usedtime��max{ǰ��1��end+ͨ��ֵ��ǰ��2��end+ͨ��ֵ������}}
		tmp=CalStartTime(cpuid,distribute_task,prev_task);
		if(tmp<minstarttime)
		{
			minstarttime=tmp;
			mincpuid=cpuid;
		}
	}
	Task[distribute_task].cpuid=mincpuid;
	Task[distribute_task].start=minstarttime;
	Task[distribute_task].end=Task[distribute_task].start+cost[distribute_task][distribute_task];
	CPU[mincpuid].T[++CPU[mincpuid].num]=distribute_task;
	CPU[mincpuid].usedtime=Task[distribute_task].end;
}

//��������Ϊ�����������CPU
void DistributeCPU()
{
	//1.����Ϊ�������CPU����������Layer
	for(int i=1;i<VTXNUM;i++)

	{
		for(int j=1;j<VTXNUM;j++)
		{
			if(Layer[i][j]==0)	
			{
				break;
			}
			if(TCflag==true&&Layer[i][j]==1)
			{
				break;
			}
			//2.Ϊ��i���j���ڵ������С���ѵ�CPU�����ֱ�����Layer[i][j]
			FindMinCostCPU(Layer[i][j]);
		}
	}
}

//��ӡ��������CPU�ϵ����񼯡�
void PrintCPUCost()
{
	cout<<"===============================CpuCost==============================="<<endl;
	for(int i=0;i<CPUNUM;i++)
	{
		printf("cpu%d������Ϊ��",i);
		for(int j=1;j<=CPU[i].num;j++)
		{
			/*if(TCflag&&(CPU[i].T[j]==1))
			{
				printf("����1:��ʼʱ��0������ʱ��2");
				continue;
			}*/
			printf("����%d:��ʼʱ��%d������ʱ��%d\t",CPU[i].T[j],Task[CPU[i].T[j]].start,Task[CPU[i].T[j]].end);
		}
		cout<<endl;
	}
}

void Test1()
{
	//PrintMatrix(cost);
	/*CreateOriginMatrix();
	PrintMatrix();*/
	Layered();
	
	InitiCPU();
	InitiTask();
	TestPrevTasks();

	DistributeCPU();
	PrintCPUCost();
}

void Test2()
{
	//PrintMatrix(cost);
	/*CreateOriginMatrix();
	PrintMatrix();*/
	Layered();
	
	InitiCPU_TaskCopy();
	InitiTask();
	TestPrevTasks();

	DistributeCPU();
	PrintCPUCost();
}

int main(int argc, char* argv[])
{  
	
	//Test1();
	Test2();


	system("pause");
	return 0;
}













///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*DataStruction :ADj*/
//typedef char VertextType;
//typedef int EdgeType;
//
//
//
//typedef struct EdgeNode{
//	int adjvex;
//	EdgeType weight;
//	EdgeNode *next;
//}EdgeNode;
//
//typedef struct VertexNode{
//	VertextType data;
//	EdgeNode *firstedge;
//}VertexNode,AdjList[MAXVEX];
//
//typedef struct{
//	AdjList adjlist;
//	int numVertexes,numEdges;
//}GraphAdjList;
//
//void CreateALGraph(GraphAdjList *G)
//{
//	int i,j,k;
//	EdgeNode *e;
//	printf("���붥�����ͱ�����\n");
//	scanf("%d,%d",&G->numVertexes,&G->numEdges);
//
//	/*��������*/
//	for(i=0;i<G->numVertexes;i++)             
//	{
//		scanf("%d",&G->adjlist[i].data);
//		G->adjlist[i].firstedge=NULL;
//	}
//	/*�����߱�*/
//	for(k=0;k<G->numEdges;k++)
//	{
//		printf("�����(vi,vj)�ϵĶ�����ţ�\n");
//		scanf("%d,%d",&i,&j);
//		e=(EdgeNode *)malloc(sizeof(EdgeNode));
//		e->adjvex=j;
//		e->next=G->adjlist[i].firstedge;
//		printf("�����(v%d,v%d)�ϵ�Ȩ�أ�\n",i,j);
//		scanf("%d",&e->weight);
//		G->adjlist[i].firstedge=e;
//		
//		/*e=(EdgeNode *)malloc(sizeof(EdgeNode));
//		e->adjvex=i;
//		e->next=G->adjlist[j].firstedge;
//		G->adjlist[j].firstedge=e;*/
//	}
//}
//
//GraphAdjList* CreateRALGraph(GraphAdjList *G)
//{
//	GraphAdjList* tmp=(GraphAdjList *)malloc(sizeof(GraphAdjList));
//	int i,j,k;
//	for(i=0;i<tmp->numVertexes;i++)
//	{
//		tmp->adjlist[i].data=G->adjlist[i].data;
//		tmp->adjlist[i].firstedge=NULL;
//	}
//	for(k=0;k<tmp->numEdges;k++)
//	{
//		;
//	}
//	return tmp;
//}