#ifndef MSTL_AVL_TREE_HPP__
#define MSTL_AVL_TREE_HPP__
#include <iostream>
using namespace std;

template <class T>
struct TreeNode
{
	T data;
	TreeNode* pLeft;
	TreeNode* pRight;
	int height;
	TreeNode(const T& data)
	{
		this->data = data;
		pLeft = pRight = nullptr;
		height = 0;
	}
};

template <class T>
class ACL_Tree
{
public:
	ACL_Tree() { pRoot = nullptr; }
	~ACL_Tree() {}
	//插入节点
	void Insert_Node(const T& newdata)
	{
		_insert(pRoot, newdata);
	}
	//获取某个节点的高度
	int _getHeight(TreeNode<T>* root);

	//情况一 右旋
	TreeNode<T>* RR(TreeNode<T>* root);
	//情况二 左旋
	TreeNode<T>* LL(TreeNode<T>* root);
	//情况三 左右旋
	TreeNode<T>* LR(TreeNode<T>* root);
	//情况四 右左旋
	TreeNode<T>* RL(TreeNode<T>* root);
private:
	void _insert(TreeNode<T>*& pRoot, const T& data);
private:
	TreeNode<T>* pRoot;
};


template <class T>
void ACL_Tree<T>::_insert(TreeNode<T>*& pRoot, const T& data)
{
	if (pRoot == nullptr)
	{
		//如果当前节点为空，则直接插入
		pRoot = new TreeNode<T>(data);
	}
	else if (data < pRoot->data)
	{
		//左插
		_insert(pRoot->pLeft, data);
		//判断是否需要旋转
		if (_getHeight(pRoot->pLeft) - _getHeight(pRoot->pRight) > 1)
		{
			if (data < pRoot->pLeft->data)
			{
				//RR  左旋
				pRoot = RR(pRoot);
			}
			else
			{
				//LR  左右旋
				pRoot = LR(pRoot);
			}
		}
	}
	else
	{
		//右插
		_insert(pRoot->pRight, data);
		if (_getHeight(pRoot->pRight) - _getHeight(pRoot->pLeft) > 1)
		{
			if (data >= pRoot->pRight->data)
			{
				//LL 右旋
				pRoot = LL(pRoot);
			}
			else
			{
				//RL  右左旋
				pRoot = RL(pRoot);
			}
		}
	}
	//3 设置高度
	int leftHeight = _getHeight(pRoot->pLeft);
	int rightHeight = _getHeight(pRoot->pRight);

	pRoot->height = 1 + (
		(leftHeight > rightHeight) ? leftHeight : rightHeight
		);
}

template<class T>
//获取某个节点的高度
inline int ACL_Tree<T>::_getHeight(TreeNode<T>* root)
{
	if (root)
	{
		return root->height;
	}
	return 0;
}
//右旋
template<class T>
inline TreeNode<T>* ACL_Tree<T>::RR(TreeNode<T>* pRoot)
{
	//1. pTemp临时存储pRoot的pLeft
	TreeNode<T>* pTemp = pRoot->pLeft;
	//2. pTemp的右孩子成为pRoot的左孩子
	pRoot->pLeft = pTemp->pRight;
	//3. pRoot成为pTemp的右孩子
	pTemp->pRight = pRoot;
	//4. 高度设置
	pRoot->height = 1 + ((_getHeight(pRoot->pLeft) > _getHeight(pRoot->pRight)) ?
		_getHeight(pRoot->pLeft) : _getHeight(pRoot->pRight));
	pTemp->height = 1 + ((_getHeight(pTemp->pLeft) > _getHeight(pTemp->pRight)) ?
		_getHeight(pTemp->pLeft) : _getHeight(pTemp->pRight));

	return pTemp;
}
//左旋
template<class T>
inline TreeNode<T>* ACL_Tree<T>::LL(TreeNode<T>* pRoot)
{
	//1. pTemp临时存储pRoot的pRight
	TreeNode<T>* pTemp = pRoot->pRight;
	//2. pTemp的左孩子成为pRoot的右孩子
	pRoot->pRight = pTemp->pLeft;
	//3. pRoot成为pTemp的左孩子
	pTemp->pLeft = pRoot;
	//4. 高度设置
	pRoot->height = 1 + ((_getHeight(pRoot->pLeft) > _getHeight(pRoot->pRight)) ?
		_getHeight(pRoot->pLeft) : _getHeight(pRoot->pRight));
	pTemp->height = 1 + ((_getHeight(pTemp->pLeft) > _getHeight(pTemp->pRight)) ?
		_getHeight(pTemp->pLeft) : _getHeight(pTemp->pRight));

	return pTemp;
}

template<class T>
inline TreeNode<T>* ACL_Tree<T>::LR(TreeNode<T>* pRoot)
{
	//以pRoot的pLeft为轴左旋
	pRoot->pLeft = LL(pRoot->pLeft);
	//再右旋
	return RR(pRoot);
}

template<class T>
inline TreeNode<T>* ACL_Tree<T>::RL(TreeNode<T>* pRoot)
{
	//以pRoot的pRight为轴右旋
	pRoot->pRight = RR(pRoot->pRight);
	//再左旋
	return LL(pRoot);
}

#endif // MSTL_AVL_TREE_HPP__
