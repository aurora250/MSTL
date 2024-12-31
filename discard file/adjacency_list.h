#ifndef MSTL_ADJACENCY_LIST_H__
#define MSTL_ADJACENCY_LIST_H__
#include "basiclib.h"

#include<iostream>
#include<stdlib.h>
#include<fstream>
#define MAX_VEXNUM 30//���������� 

typedef struct VNode {
	int adjvex;
	struct VNode* nextarc;
}VNode;//�� 

typedef struct Adj {
	int data;
	VNode* firstarc;
}Adj, AdjList[MAX_VEXNUM];//��ͷ 

typedef struct {
	int n, m;
	AdjList vertices;//�ڽӱ� 
}Graph;
bool visited[MAX_VEXNUM];//ȫ�ֱ��� 

//���л�����������ʼ��->���->�п�->��ȡ��ͷ->����->�������� 
typedef struct QNode {
	int data;
	struct QNode* next;
}QNode;
typedef struct Queue {
	QNode* front;
	QNode* rear;
}Queue;
//��ʼ�� 
void InitQueue(Queue& Q)
{
	Q.front = Q.rear = (QNode*)malloc(sizeof(QNode));
	Q.front->next = NULL;
}
//��� 
void EnQueue(Queue& Q, int e)
{
	QNode* p = (QNode*)malloc(sizeof(QNode));
	p->data = e;
	p->next = NULL;
	Q.rear->next = p;
	Q.rear = p;
}
//�п� 
bool IsEmpty(Queue Q)
{
	if (Q.front == Q.rear)return true;
	else return false;
}
//���� 
void DeQueue(Queue& Q)
{
	if (IsEmpty(Q))return;
	//	QNode* p = (QNode*)malloc(sizeof(QNode));
	QNode* p = Q.front->next;
	Q.front->next = p->next;
	if (p->next == NULL)Q.rear = Q.front;//����ɾ�ɿգ���β�ص���ͷ 
	free(p);
	p = NULL;
}
//��ȡ�Ӷ� 
QNode* GetTop(Queue Q)
{
	//	if(IsEmpty(Q))return NULL;
	return Q.front->next;
}
//������ ���� 
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
//�ļ���ȡ���ݽ�������ͼ/��  
void CreateGraph(Graph& G)
{
	std::fstream inFile("����ͼ.txt", std::ios::in);
	if (!inFile)std::cout << "Fail to open file!" << std::endl;
	inFile >> G.n >> G.m;
	int n = G.n, m = G.m; std::cout << n << " " << m << std::endl;

	for (int i = 0; i < n; i++) {//��ʼ�� 
		G.vertices[i].data = i;
		G.vertices[i].firstarc = NULL;
	}
	int u, v, w;
	VNode* p;
	//	Adj pfst;//����ͷ ע��ָ��ʹ�ã���ֵ���Ǵ�ַ 
	for (int i = 0; i < m; i++) {
		inFile >> u >> v >> w;//cout<<u<<" "<<v<<" "<<w<<endl;
		u--; v--;//�����1��ʼ�����洢��0��ʼ 
		p = (VNode*)malloc(sizeof(VNode));
		p->adjvex = v;
		p->nextarc = G.vertices[u].firstarc;
		G.vertices[u].firstarc = p;
	}
	inFile.close();
	//������� 
	for (int i = 0; i < n; i++) {
		//		pfst = G.vertices[i];
		p = G.vertices[i].firstarc;
		while (p != NULL) {
			std::cout << p->adjvex << " ";
			p = p->nextarc;
		}std::cout << std::endl;
	}
}
//����u�ĵ�һ���ڽӵ� 
int FirstAdjvex(Graph G, int u)
{
	VNode* p = G.vertices[u].firstarc;
	while (p != NULL) {
		if (visited[p->adjvex])p = p->nextarc;//�������ʹ��� 
		else return p->adjvex;
	}
	return -1;//��ʾδ�ҵ��ڽӵ� 
}
//����������ڽӵ�u����ڽӵ� 
int NextAdjvex(Graph G, int u, int v)
{
	VNode* p = G.vertices[u].firstarc;
	//���ҵ�v 
	while (p != NULL) {
		if (p->adjvex == v)break;
		else p = p->nextarc;
	}
	p = p->nextarc;
	while (p != NULL) {//������һ���ڽӵ� 
		if (visited[p->adjvex])p = p->nextarc;
		else return p->adjvex;
	}
	return -1;//��ʾ����һ�ڽӵ� 
}
//������������������������������ 
void DFS(Graph G, int u)
{
	visited[u] = true;
	std::cout << u + 1 << " ";

	for (int v = FirstAdjvex(G, u); v >= 0; v = NextAdjvex(G, u, v)) {//Ѱ����һ���ڽӵ㣬Ϊ�˻���ʱ׼�� 
		if (!visited[v])DFS(G, v);
	}
}
// 
void DFSTraverse(Graph G)
{
	for (int i = 0; i < G.n; i++) {
		visited[i] = false;
	}
	//���ǵ�����ͨͼ����дѭ��
	for (int v = 0; v < G.n; v++) {//�ɼ�����ͨ��֧�� 
		if (!visited[v]) {
			DFS(G, v);
			std::cout << std::endl;
		}
	}
}
//������ȣ����������Ĳ�α���������ʵ�� 
//�����������ڣ�ͼ�����ȷ�������ӣ����������ظ�����һ���㣻��������ν 
void BFS(Graph G, int u)
{
	Queue Q;
	InitQueue(Q);
	//�ȷ�������� 
	visited[u] = true;
	std::cout << u + 1 << " ";
	EnQueue(Q, u);

	while (!IsEmpty(Q)) {//�ӷǿ� 
		u = GetTop(Q)->data;
		DeQueue(Q);
		for (int v = FirstAdjvex(G, u); v >= 0; v = NextAdjvex(G, u, v)) {//����Ԫ��u ���������ڵ���� 
			if (!visited[v]) {
				visited[v] = true;
				EnQueue(Q, v);
				cout << v + 1 << " ";
			}
		}
	}
}
//������ȱ��� 
void BFSTraverse(Graph G)
{
	//���������ʼ�� 
	for (int i = 0; i < G.n; i++) {
		visited[i] = false;
	}
	//��һ������ͨͼ 
	for (int i = 0; i < G.n; i++) {
		if (!visited[i]) {
			BFS(G, i);
			std::cout << std::endl;
		}
	}
}
//��������ͬ���ö���ʵ�֣����Һ�ͼһ�����ȷ��ʣ�����ӣ�
//������Ҫά��һ����ȱ�ÿ�γ��Ӽǵø�����ȱ�
//void TopologicalSort(Graph G)
//{
//	int n = G.n, m = G.m;//������ 
//
//	int indegree[G.n];//��ȱ� 
//	for (int i = 0; i < n; i++) {//��ȱ��ʼ�� 
//		indegree[i] = 0;
//	}
//	//=========����ÿ��������========= 
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
//	//��ʼ����������Ѱ�ҵ�һ�����Ϊ0�ĵ� 
//	for (int i = 0; i < n; i++) {
//		if (indegree[i] == 0) {
//			EnQueue(Q, i);
//			indegree[i] = -1;//��ʾ�ѷ��� 
//			std::cout << i + 1 << " ";
//		}
//	}
//	while (!IsEmpty(Q)) {
//		int u = GetTop(Q)->data;
//		DeQueue(Q);
//		VNode* p = G.vertices[u].firstarc;
//		while (p != NULL) {//���ӵ�Ԫ�����бߣ����г��ߣ���Ҫɾ�������Խ��������ڵĵ�����-1 
//			if (indegree[p->adjvex] != -1)indegree[p->adjvex]--;
//			p = p->nextarc;
//		}
//		//ɨ��ʣ������ȣ�Ϊ0��������� 
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
