#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
//���������Ϣ
string up_type[18] = { " ","��Ϸ","����","�赸","����","����","Ӱ��","����","����","����","ʱ��","�Ƽ�","����","��¼Ƭ","���","����","��Ӱ","���Ӿ�" };
//ͼ�ڵ�
struct  Node
{
	int uid;//Ψһ��ʶ������up����uid
	int indice=-1;//������Ϊ��ʹ��d3.js���ӻ����õ�
	string name;//up������
	double closeness;//�������Ķ�
	int type;//��ʾ�������ͣ���up_type[type]�Ϳ���֪��
	bool used_in_A = false;//�Ƿ����㷨��ʹ��
	
	vector<int> follow;//��ע������
	vector<float>w;//�ڱ�Ȩ��
	vector<Node*> neighbor;//�ڽӣ���ʾ�໥��ע��
	bool status = false;//�Ƿ񱻷���
	bool picked = false;//�Ƿ��Ѿ���ѡ��
	Node* pred = NULL;//ǰ��,Ѱ·�㷨��prim�㷨�ж�����ʹ��
	int m_w;//����Ȩ�أ���prim�㷨�п���ʹ��
	Node(int id)
	{
		uid = id;
	}
};
struct Graph
{
	//ͼ�ṹ�����ڽ��ṹ���Ѿ������˱���Ϣ������ṹ��ֻ��Ҫ���������С�
	vector<Node*>v;
	void show_tree();//�ڿ���̨��ʾ���ṹ��Ҫ���Ƚ���prim�㷨
	void out_tree();//�����ı�������ļ���Ҫ���Ƚ���prim�㷨
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

Node* up_list[20000];//��up����Ϣ������
vector<int> id_list;//���õ�id
vector<Node*>up_node;//�����������н��
vector<Graph*>branch;//������ͨ��֧
vector<Node*>big100;//����ٴ�up��
int myhash(int num)
{
	//��ϣ�������ѽ�����uidӳ�䵽�����ﴢ��
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
	//���ݹ�ϣ������Ѱuid��Ӧ������
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
	//���ļ��л�ȡ��Ϣ������Ϣ��ʽ��Ҫ������ĵ�
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
	//�����Ƿ����ע�ж��ڽӣ����ɾ��û���ڽ��κνڵ�Ľڵ�
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
	//����Ȩ��
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
		//�Ƿ����н�㱻���ʣ����������ʷ��ؿ�ָ�룬����δ�����ʵľͷ������
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
	//��ȡ��ͨ��֧����ͨ��֧������branch��
	for (auto node : up_node)
	{
		node->status = false;//���Ƿ��������Ϊ��
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
		
	}cout << "��ͨ��֧������"<<branch.size() << endl;
}

void prim(Graph* G)
{
	//����ͼG����С������
	//�Ȱѽ��״̬���
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
		int min_w = 3, min_node;//��ΪȨ�ز��ᳬ��2��
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
	//����·������
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
	//�ҵ����·���󷵻ؾ��룬������û����ͨ�Ľڵ�
		//�Ȱ��Ƿ����ȫ����Ϊfalse;
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
	//����������Ķ�
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
	//������Ķ���Ϣ
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
	//�Ȱ��Ƿ����ȫ����Ϊfalse;
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
	//���js�ļ�����ʽΪ
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
	//�������Ϣ
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
	//���ͼ��Ϣ
	out_node(G,nodefile);
	out_edges(G,edgefile);
}
void query_way()
{
	//��ѯ·��
	cout << "��������up��uid������·��:";
	int uid1, uid2;
	cin >> uid1 >> uid2;
	Node* result = find_way(uid1, uid2);
	if (result)
	{
		show_way(result);
	}
	else
	{
		cout << "����ͨ" << endl;
	}
}

int main(void)
{


	
	
	int n = 0;
	cout << "��ȡ���ݣ�"<<endl;
	get_info("up_info.txt");	
	get_neighbor();
	cout << "Done";
	cout << "����Ȩ�أ�"<<endl;
	calculate_w();
	cout << "Done\n�ܽڵ�����";
	cout << up_node.size();
	cout << "\n������֧ͨ��";
	get_branch();
	
	cout << "��ͨ��֧����" << branch.size() << endl;
	cout << "չʾ����֧ͨ�������";
	
	for(auto g:branch)
	{
		cout << g->v.size() << endl;
	}
	cout << "��ʼ������С������";
	for(auto g:branch)
	{
		prim(g);
		cout << "�������" << endl;
	}
	//��ѡ��չʾ��С������
	//branch[0]->show_tree();
	Graph* big100 = new Graph;
	//�������ӻ��ٴ�
	cout << "����ٴ�up����Ϣ��" << endl;
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