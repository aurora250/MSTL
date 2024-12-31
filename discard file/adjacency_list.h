#ifndef MSTL_ADJACENCY_LIST_H__
#define MSTL_ADJACENCY_LIST_H__
#include "basiclib.h"

#include<iostream>
#include<stdlib.h>
#include<fstream>
#define MAX_VEXNUM 30//顶点最大个数 

typedef struct VNode {
	int adjvex;
	struct VNode* nextarc;
}VNode;//边 

typedef struct Adj {
	int data;
	VNode* firstarc;
}Adj, AdjList[MAX_VEXNUM];//表头 

typedef struct {
	int n, m;
	AdjList vertices;//邻接表 
}Graph;
bool visited[MAX_VEXNUM];//全局变量 

//队列基本操作：初始化->入队->判空->获取队头->出队->遍历测试 
typedef struct QNode {
	int data;
	struct QNode* next;
}QNode;
typedef struct Queue {
	QNode* front;
	QNode* rear;
}Queue;
//初始化 
void InitQueue(Queue& Q)
{
	Q.front = Q.rear = (QNode*)malloc(sizeof(QNode));
	Q.front->next = NULL;
}
//入队 
void EnQueue(Queue& Q, int e)
{
	QNode* p = (QNode*)malloc(sizeof(QNode));
	p->data = e;
	p->next = NULL;
	Q.rear->next = p;
	Q.rear = p;
}
//判空 
bool IsEmpty(Queue Q)
{
	if (Q.front == Q.rear)return true;
	else return false;
}
//出队 
void DeQueue(Queue& Q)
{
	if (IsEmpty(Q))return;
	//	QNode* p = (QNode*)malloc(sizeof(QNode));
	QNode* p = Q.front->next;
	Q.front->next = p->next;
	if (p->next == NULL)Q.rear = Q.front;//队列删成空，队尾回到队头 
	free(p);
	p = NULL;
}
//获取队顶 
QNode* GetTop(Queue Q)
{
	//	if(IsEmpty(Q))return NULL;
	return Q.front->next;
}
//？？？ 遍历 
void TraverseQueue(Queue Q)
{
	QNode* p = Q.front->next;
	while (p != NULL) {
		std::cout << p->data << " ";
		p = p->next;
	}std::cout << std::endl;
	//	while(!IsEmpty(Q)){
	//		cout<<GetTop(Q)->data<<" ";
	//		DeQueue(Q);
	//	}
}
//文件读取数据建立有向图/网  
void CreateGraph(Graph& G)
{
	std::fstream inFile("有向图.txt", std::ios::in);
	if (!inFile)std::cout << "Fail to open file!" << std::endl;
	inFile >> G.n >> G.m;
	int n = G.n, m = G.m; std::cout << n << " " << m << std::endl;

	for (int i = 0; i < n; i++) {//初始化 
		G.vertices[i].data = i;
		G.vertices[i].firstarc = NULL;
	}
	int u, v, w;
	VNode* p;
	//	Adj pfst;//链表头 注意指针使用，传值还是传址 
	for (int i = 0; i < m; i++) {
		inFile >> u >> v >> w;//cout<<u<<" "<<v<<" "<<w<<endl;
		u--; v--;//输入从1开始，而存储从0开始 
		p = (VNode*)malloc(sizeof(VNode));
		p->adjvex = v;
		p->nextarc = G.vertices[u].firstarc;
		G.vertices[u].firstarc = p;
	}
	inFile.close();
	//输出调试 
	for (int i = 0; i < n; i++) {
		//		pfst = G.vertices[i];
		p = G.vertices[i].firstarc;
		while (p != NULL) {
			std::cout << p->adjvex << " ";
			p = p->nextarc;
		}std::cout << std::endl;
	}
}
//返回u的第一个邻接点 
int FirstAdjvex(Graph G, int u)
{
	VNode* p = G.vertices[u].firstarc;
	while (p != NULL) {
		if (visited[p->adjvex])p = p->nextarc;//跳过访问过的 
		else return p->adjvex;
	}
	return -1;//表示未找到邻接点 
}
//返回相对于邻接点u后的邻接点 
int NextAdjvex(Graph G, int u, int v)
{
	VNode* p = G.vertices[u].firstarc;
	//先找到v 
	while (p != NULL) {
		if (p->adjvex == v)break;
		else p = p->nextarc;
	}
	p = p->nextarc;
	while (p != NULL) {//查找下一个邻接点 
		if (visited[p->adjvex])p = p->nextarc;
		else return p->adjvex;
	}
	return -1;//表示无下一邻接点 
}
//深度优先搜索，类似于树的先序遍历 
void DFS(Graph G, int u)
{
	visited[u] = true;
	std::cout << u + 1 << " ";

	for (int v = FirstAdjvex(G, u); v >= 0; v = NextAdjvex(G, u, v)) {//寻求下一个邻接点，为了回退时准备 
		if (!visited[v])DFS(G, v);
	}
}
// 
void DFSTraverse(Graph G)
{
	for (int i = 0; i < G.n; i++) {
		visited[i] = false;
	}
	//考虑到非连通图，才写循环
	for (int v = 0; v < G.n; v++) {//可计算连通分支数 
		if (!visited[v]) {
			DFS(G, v);
			std::cout << std::endl;
		}
	}
}
//广度优先：类似于树的层次遍历，队列实现 
//二者区别在于，图必须先访问再入队，否则会出现重复访问一个点；而树无所谓 
void BFS(Graph G, int u)
{
	Queue Q;
	InitQueue(Q);
	//先访问再入队 
	visited[u] = true;
	std::cout << u + 1 << " ";
	EnQueue(Q, u);

	while (!IsEmpty(Q)) {//队非空 
		u = GetTop(Q)->data;
		DeQueue(Q);
		for (int v = FirstAdjvex(G, u); v >= 0; v = NextAdjvex(G, u, v)) {//出队元素u 的所有相邻点入队 
			if (!visited[v]) {
				visited[v] = true;
				EnQueue(Q, v);
				cout << v + 1 << " ";
			}
		}
	}
}
//广度优先遍历 
void BFSTraverse(Graph G)
{
	//访问数组初始化 
	for (int i = 0; i < G.n; i++) {
		visited[i] = false;
	}
	//不一定是连通图 
	for (int i = 0; i < G.n; i++) {
		if (!visited[i]) {
			BFS(G, i);
			std::cout << std::endl;
		}
	}
}
//拓扑排序，同样用队列实现，并且和图一样，先访问，再入队，
//不过需要维护一个入度表，每次出队记得更新入度表
//void TopologicalSort(Graph G)
//{
//	int n = G.n, m = G.m;//方便用 
//
//	int indegree[G.n];//入度表 
//	for (int i = 0; i < n; i++) {//入度表初始化 
//		indegree[i] = 0;
//	}
//	//=========计算每个点的入度========= 
//	for (int i = 0; i < n; i++) {
//		VNode* pcur = G.vertices[i].firstarc;
//		while (pcur != NULL) {
//			indegree[pcur->adjvex]++;
//			pcur = pcur->nextarc;
//		}
//	}
//
//	Queue Q;
//	InitQueue(Q);
//	//初始化拓扑排序，寻找第一届入度为0的点 
//	for (int i = 0; i < n; i++) {
//		if (indegree[i] == 0) {
//			EnQueue(Q, i);
//			indegree[i] = -1;//表示已访问 
//			std::cout << i + 1 << " ";
//		}
//	}
//	while (!IsEmpty(Q)) {
//		int u = GetTop(Q)->data;
//		DeQueue(Q);
//		VNode* p = G.vertices[u].firstarc;
//		while (p != NULL) {//出队的元素所有边（仅有出边）都要删除，所以将与其相邻的点的入度-1 
//			if (indegree[p->adjvex] != -1)indegree[p->adjvex]--;
//			p = p->nextarc;
//		}
//		//扫描剩余点的入度，为0则标记且入队 
//		for (int i = 0; i < n; i++) {
//			if (indegree[i] != -1 && indegree[i] == 0) {
//				EnQueue(Q, i);
//				indegree[i] = -1;
//				std::cout << i + 1 << " ";
//			}
//		}
//	}
}


#endif // MSTL_ADJACENCY_LIST_H__
