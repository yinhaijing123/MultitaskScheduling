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
bool TCflag=0;                          //任务复制标志位（若进行任务复制，则将其置为1）
//int cost[VTXNUM][VTXNUM];
int cost[VTXNUM][VTXNUM]={         /*    T1      T2      T3     T4      T5     T6     T7     T8    T9     */
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



/*建立原始矩阵*/
void CreateOriginMatrix()
{
	int i=0,j=0;
	//1.input Task  execute time
	for(i=1;i<VTXNUM;i++)
	{
		printf("请输入任务%d的执行时间：",i);
		scanf("%d",&cost[i][i]);
	}

	//2.input comm between Taski and taskj
	while(1)
	{
		int c=0;
		printf("===============================请选择=====================================\n");
		printf("=============1. 继续输入通信值=========2. 结束输入通信值==================\n");
		scanf("%d",&c);
		if(c==1)
		{
			printf("请输入i，j及任务i和任务j之间的通信时间\n");
			scanf("%d %d",&i,&j);
			scanf("%d",&cost[i][j]);
		}
		if(c==2)
		{
			break;	
		}
	}
	
}

/*打印矩阵x*/
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

/*拷贝一个临时矩阵，避免修改原矩阵*/
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

/*判断taski任务是否入度为0*/
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

/*将已分配的任务所在行的通信值置为0（即：以该任务为弧尾的相关的任务节点的入度均减一）；同时标记该任务*/
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
/*分层*/
void Layered()
{
	int tmp[VTXNUM][VTXNUM];
	int i,j;
	vector<int> zerotask;              //存储已分配的任务
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

//对CPU进行初始化
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

//任务复制CPU初始化--每个CPU的第一个任务均复制为task1
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


//找出任务task的前驱节点序列，并返回
vector<int> FindPrevTask(int task)
{
	vector<int> prev_task;
	//在DAG矩阵中，task任务的前驱任务为第1-task-1行，task列中值非零的行值集合。
	for(int k=1;k<task;k++)
	{
		if(cost[k][task]!=0)
		{
			prev_task.push_back(k);
		}
	}
	return prev_task;
}


//打印任务task的前驱任务集
void PrintPrevTask(int task,vector<int> prev_task)
{
	if(prev_task.size()==0)
	{
		printf("任务%d为起始任务\n",task);
		return;
	}
	printf("任务%d的前驱任务集为：",task);
	for(int k=0;k<prev_task.size();k++)
	{
		printf("%d，",prev_task[k]);
	}
	printf("\n");
}

//打印所有任务的前驱任务集。
void TestPrevTasks()
{
	printf("===============================PrevTasks===============================\n");
	for(int i=1;i<VTXNUM;i++)
	{
		PrintPrevTask(i,FindPrevTask(i));
	}
}

//计算任务i在cpuid号cpu的开始时间
int CalStartTime(int cpuid,int distribute_task,vector<int> prev_task)
{
	int max=CPU[cpuid].usedtime;            //cpu已使用时间
	int tmpstarttime=0;            //临时开始时间
	int size=prev_task.size();
	if(distribute_task==1)
	{
		return 0;
	}
	for(int i=0;i<size;i++)
	{
		//判断任务的前继任务序列，是否与该待分配任务在一个CPU上，如果均在一个CPU上，该任务开始时间为本CPU usedtime；否则：为其max{前驱end+通信值}
		if(Task[prev_task[i]].cpuid!=cpuid)
		{
			tmpstarttime=Task[prev_task[i]].end+cost[prev_task[i]][distribute_task];
		}
		max=MAX(max,tmpstarttime);
	}
	return max;
}

//为任务distribute_task分配耗时最小的CPU
void FindMinCostCPU(int distribute_task)
{
	int cpuid=0;
	vector<int> prev_task;                    //前驱任务集
	int minstarttime=0x00ffffff;              //最小开始时间
	int tmp=0;
	int mincpuid=0;                           //最小的CPU编号
	//1.依次遍历三个CPU，求最小耗时的一个，并更新cpu结构体
	for(cpuid;cpuid<CPUNUM;cpuid++)
	{
		//2.找前继任务，并记录下来
		prev_task=FindPrevTask(distribute_task);
		//3.计算将该任务分配到该CPU上后，该任务的开始时间。max{本cpu usedtime，max{前驱1的end+通信值，前驱2的end+通信值，……}}
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

//按层依次为所有任务分配CPU
void DistributeCPU()
{
	//1.按层为任务分配CPU。即：遍历Layer
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
			//2.为第i层第j个节点查找最小花费的CPU。即分别将任务Layer[i][j]
			FindMinCostCPU(Layer[i][j]);
		}
	}
}

//打印任务分配后，CPU上的任务集。
void PrintCPUCost()
{
	cout<<"===============================CpuCost==============================="<<endl;
	for(int i=0;i<CPUNUM;i++)
	{
		printf("cpu%d的任务集为：",i);
		for(int j=1;j<=CPU[i].num;j++)
		{
			/*if(TCflag&&(CPU[i].T[j]==1))
			{
				printf("任务1:开始时间0—结束时间2");
				continue;
			}*/
			printf("任务%d:开始时间%d—结束时间%d\t",CPU[i].T[j],Task[CPU[i].T[j]].start,Task[CPU[i].T[j]].end);
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
