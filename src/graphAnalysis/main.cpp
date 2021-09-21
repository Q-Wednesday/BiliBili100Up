#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
//分区编号信息
string up_type[18] = { " ","游戏","动画","舞蹈","音乐","鬼畜","影视","数码","生活","国创","时尚","科技","娱乐","纪录片","广告","番剧","电影","电视剧" };
//图节点
struct  Node
{
	int uid;//唯一标识，代表up主的uid
	int indice=-1;//索引，为了使用d3.js可视化设置的
	string name;//up主名字
	double closeness;//紧密中心度
	int type;//表示分区类型，用up_type[type]就可以知道
	bool used_in_A = false;//是否在算法中使用
	
	vector<int> follow;//关注者名单
	vector<float>w;//邻边权重
	vector<Node*> neighbor;//邻接，表示相互关注者
	bool status = false;//是否被访问
	bool picked = false;//是否已经被选择
	Node* pred = NULL;//前驱,寻路算法和prim算法中都可以使用
	int m_w;//自身权重，在prim算法中可以使用
	Node(int id)
	{
		uid = id;
	}
};
struct Graph
{
	//图结构，由于结点结构体已经包含了边信息，这个结构体只需要包含结点就行。
	vector<Node*>v;
	void show_tree();//在控制台显示树结构，要求先进行prim算法
	void out_tree();//把树的边输出到文件，要求先进行prim算法
};
void Graph::show_tree()
{
	for(auto node:v)
	{
		if(node->pred)
		cout << node->name<< "<-" << node->pred->name << ' ';
	}
	cout << endl;
}
void Graph::out_tree()
{
	ofstream fp("tree_edges.js");
	fp << "var tree_edges=[" << endl;
	for (auto node :v)
	{
		if (node->pred)
		{
			fp << "{\"source\":" << node->indice << ",\"target\":" << node->pred->indice << ",\"weight\":1}," << endl;

		}
	}
	fp << "]";
	fp.close();
}

Node* up_list[20000];//放up主信息的数组
vector<int> id_list;//启用的id
vector<Node*>up_node;//储存启用所有结点
vector<Graph*>branch;//储存连通分支
vector<Node*>big100;//储存百大up主
int myhash(int num)
{
	//哈希函数，把结点根据uid映射到数组里储存
	int key = num %19999;
	if(up_list[key]==NULL)
	{
		up_list[key] = new Node(num);
		return  key;
	}
	else
	{
		while (up_list[key]!=NULL)
		{
			key = (key + 1) % 19999;
		}
		up_list[key] = new Node(num);
		return key;
	}
	
}
int find_key(int num)
{
	//根据哈希函数找寻uid对应的索引
	int key = num % 19999;
	if(up_list[key]->uid==num)
	{
		return key;
	}
	else
	{
		while (up_list[key]->uid!=num)
		{
			key = (key + 1) % 19999;
		}
		return key;
	}
}
void get_info(const char filename[])
{
	//从文件中获取信息，对信息格式由要求，详见文档
	fstream fp;
	fp.open(filename);
	int n;
	fp >> n;
	for(int i=0;i<n;i++)
	{
		int uid;
		fp >> uid;
		
		int key = myhash(uid);
		
		fp >>up_list[key]->name;
		
		fp >>up_list[key]->type;
		int follow_id;
		fp>> follow_id;
		
		while (follow_id)
		{
			up_list[key]->follow.push_back(follow_id);
			fp >> follow_id;
		}

		up_node.push_back(up_list[key]);
		id_list.push_back(uid);
		
	}
}
void get_neighbor()
{
	//根据是否互相关注判断邻接，最后删除没有邻接任何节点的节点
	sort(id_list.begin(),id_list.end());
	for(int uid:id_list)
	{
		int key1 = find_key(uid);
		for(int id:up_list[key1]->follow)
		{
			if(find(id_list.begin(),id_list.end(),id)==id_list.end())continue;
			else
			{
				int key2 = find_key(id);
				for(int fid:up_list[key2]->follow)
				{
					if(fid==uid)
					{
						up_list[key1]->neighbor.push_back(up_list[key2]);
						
					}
				}
			}
		}
	}
	vector<Node*>::iterator itr = up_node.begin();
	while(itr!=up_node.end())
	{
		if((*itr)->neighbor.size()==0)
		{
			itr = up_node.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}
void calculate_w()
{
	//计算权重
	for(int uid:id_list)
	{
		int key1 = find_key(uid);
		Node* node1 = up_list[key1];
		for(auto node2:node1->neighbor )
		{
			int count = 0;
			for(auto follow1:node1->follow)
			{
				for(auto follow2:node2->follow)
				{
					if (follow1 == follow2)count++;
				}
			}
			node1->w.push_back((count == 0) ? 2 : (1.0f / count));
		}
	}
}

Node* visit_all()
	{
		//是否所有结点被访问，若都被访问返回空指针，若有未被访问的就返回这个
	for (auto node : up_node)
	{
		if(node->status==false)
		{
			return node;
		}
	}
	return  nullptr;
	}
void get_branch()
{
	//获取连通分支，连通分支储存在branch中
	for (auto node : up_node)
	{
		node->status = false;//把是否访问重置为否
	}
	Node* node;
	
	while (node=visit_all())
	{
		Graph* G = new Graph;
		branch.push_back(G);
		queue<Node*> Q;
		Q.push(node); G->v.push_back(node);
		node->status = true;
		while(!Q.empty())
		{
			Node* a = Q.front(); Q.pop();
			for(auto neighbor:a->neighbor)
			{
				if(neighbor->status==false)
				{
					neighbor->status = true;
					Q.push(neighbor); G->v.push_back(neighbor);
				}
			}
		}
		
	}cout << "连通分支个数："<<branch.size() << endl;
}

void prim(Graph* G)
{
	//求子图G的最小生成树
	//先把结点状态清空
	for (auto node : G->v)
	{
		node->status = false;
		node->pred = nullptr;
		node->used_in_A = true;
		node->picked = false;
	}
	vector<Node*>Q;
	Q.push_back(G->v[0]);
	G->v[0]->status = true;
	G->v[0]->m_w = 0;
	while (!Q.empty())
	{
		int min_w = 3, min_node;//因为权重不会超过2的
		for (int i = 0; i < Q.size(); i++)
		{
			if (Q[i]->m_w < min_w)
			{
				min_node = i;
				min_w = Q[i]->m_w;
			}
		}
		Node* new_node = Q[min_node];
		Q.erase(Q.begin() + min_node);
		new_node->picked = true;
		for (int i = 0; i < new_node->neighbor.size(); i++)
		{
			if (new_node->neighbor[i]->picked)continue;
			
			if (new_node->neighbor[i]->status)
			{

				if (new_node->neighbor[i]->m_w > new_node->w[i])
				{
					new_node->neighbor[i]->m_w = new_node->w[i];
					new_node->neighbor[i]->pred = new_node;
				}
			}
			else
			{
				new_node->neighbor[i]->status = true;
				new_node->neighbor[i]->m_w = new_node->w[i];
				new_node->neighbor[i]->pred = new_node;
				Q.push_back(new_node->neighbor[i]);
			}
		}
	}
}
int cal_way_len(Node* node)
{
	//计算路径长度
	int len = 0;
	while (node)
	{
		len++;
		node = node->pred;
	}
	return len;
}
int find_way(Node* up1,Node*up2)
{
	//找到最短路径后返回距离，不考虑没有连通的节点
		//先把是否访问全部置为false;
	for (int i = 0; i < 10000; i++)
	{
		if (up_list[i])
		{
			up_list[i]->status = false;
			up_list[i]->pred = NULL;
			
		}
	}
	queue<Node*>Q;
	up1->status = true;
	up1->pred = nullptr;
	Q.push(up1);
	while (!Q.empty())
	{
		Node* node = Q.front();
		Q.pop();
		for (auto neighbor : node->neighbor)
		{
			if (neighbor == up2)
			{
				neighbor->pred = node;
				return  cal_way_len(neighbor);
			}
			if (neighbor->status)continue;
			
			neighbor->pred = node;
			neighbor->status = true;
			Q.push(neighbor);
		}
	}
	return  0;
}
void cal_closeness(Graph*G)
{
	//计算紧密中心度
	for(auto node:G->v)
	{
		node->closeness = 0;
		
	}
	for(int i=0;i<G->v.size()-1;i++)
	{
		for(int j=i+1;j<G->v.size();j++)
		{
			if(j==93)
			{
				int nomean;
				int length = find_way(G->v[i], G->v[j]);
			}
			int length = find_way(G->v[i], G->v[j]);
			G->v[i]->closeness += length;
			G->v[j]->closeness += length;
		
		}
	}
	for(auto node:G->v)
	{
		if(node->closeness)
		{
			node->closeness = 1 / node->closeness;
		}
	}
}
void out_closeness(Graph*G)
{
	//输出中心度信息
	ofstream fp("closeness.js");
	fp << "var closeness =[" << endl;
	for(auto node:G->v)
	{
		fp << "{\"name\":\"" << node->name << "\",\"value\":" << node->closeness * 100000 << ",\"type\":" << node->type << "}," << endl;
	}
	fp<< "]";
	fp.close();
}
Node* find_way(int up1,int up2)
{
	//先把是否访问全部置为false;
	for(int i=0;i<10000;i++)
	{
		if(up_list[i])
		{
			up_list[i]->status = false;
			up_list[i]->pred = NULL;
		}
	}
	queue<Node*>Q;
	int key1 = find_key(up1);
	up_list[key1]->status = true;
	Q.push(up_list[key1]);
	while (!Q.empty())
	{
		Node* node = Q.front();
		Q.pop();
		for(auto neighbor:node->neighbor)
		{
			if(neighbor->uid==up2)
			{
				neighbor->pred = node; return neighbor;
			}
			if(neighbor->status)continue;
			neighbor->pred = node;
			neighbor->status = true;
			Q.push(neighbor);
		}
	}
	return nullptr;
}
void show_way(Node* node)
{
	if(node)
	{
		show_way(node->pred);
		cout << ' ' << node->name;
	}
	else
	{
		return;
	}
}

void out_node(Graph* G,string filename="nodes.js")
{
	//输出js文件，格式为
	ofstream fp;
	fp.open(filename);
	fp << "var nodes=[" << endl;
	int indice = 0;
	for(auto node:G->v)
	{
		node->indice = indice;
		indice++;
		fp << "{\"name\":\"" << node->name << "\",\"type\":" << node->type
			<< ",\"uid\":" << node->uid <<",\"closeness\":"<<node->closeness<< "}";
		if(indice==G->v.size())
		{
			fp << endl;
		}
		else
		{
			fp << "," << endl;
		}
		

	}
	fp << "];";
	fp.close();
	
}
void out_edges(Graph*G,string filename="edges.js")
{
	//输出边信息
	ofstream fp;
	fp.open(filename);
	fp << "var links=[" << endl;
	for(auto node:G->v)
	{
		for(int i=0;i<node->neighbor.size();i++)
		{
			if(node->neighbor[i]->indice>=0)
			fp << "{\"source\":"<<node->indice<< ",\"target\":" << node->neighbor[i]->indice<< ",\"weight\":" << node->w[i] <<"},"<<endl;
		}
	}
	fp << "]";
	fp.close();
}
void out_graph_info(Graph*G,string nodefile= "nodes.js",string edgefile="edges.js" )
{
	//输出图信息
	out_node(G,nodefile);
	out_edges(G,edgefile);
}
void query_way()
{
	//查询路径
	cout << "输入两个up的uid，查找路径:";
	int uid1, uid2;
	cin >> uid1 >> uid2;
	Node* result = find_way(uid1, uid2);
	if (result)
	{
		show_way(result);
	}
	else
	{
		cout << "不连通" << endl;
	}
}

int main(void)
{


	
	
	int n = 0;
	cout << "获取数据："<<endl;
	get_info("up_info.txt");	
	get_neighbor();
	cout << "Done";
	cout << "计算权重："<<endl;
	calculate_w();
	cout << "Done\n总节点数：";
	cout << up_node.size();
	cout << "\n计算连通支：";
	get_branch();
	
	cout << "连通分支数：" << branch.size() << endl;
	cout << "展示各连通支结点数：";
	
	for(auto g:branch)
	{
		cout << g->v.size() << endl;
	}
	cout << "开始计算最小生成树";
	for(auto g:branch)
	{
		prim(g);
		cout << "计算完成" << endl;
	}
	//可选：展示最小生成树
	//branch[0]->show_tree();
	Graph* big100 = new Graph;
	//单独可视化百大
	cout << "输出百大up主信息：" << endl;
	for(int i=0;i<=93;i++)
	{
		big100->v.push_back(up_node[i]);

	}
	cal_closeness(big100);
	out_closeness(big100);
	out_graph_info(big100);
	prim(big100);
	big100->out_tree();
	out_graph_info(branch[0], "big_graph_node.js", "big_graph_edge.js");
	cout << "Done"<<endl;
	query_way();
	return 0;
}