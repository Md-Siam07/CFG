#include<iostream>
#include<vector>
#include<set>
#include<fstream>
#include <string>
#include<map>
#include<algorithm>
#include <regex>

using namespace std;

vector<string> lines;
int curIndex = 0;

class Node{
public:
    int lineNo;
    vector<Node*> parents;
    vector<Node*> children;
public:
    Node(int plineNo){
        lineNo = plineNo;
    }
    void addParent(Node* node);
    void addChild(Node* node);
};

void Node:: addParent(Node *node)
{
    parents.push_back(node);
}

void Node:: addChild(Node *node)
{
    children.push_back(node);
}

string trim(string s)
{
    //cout<< s << endl;
    //string ret = "";
    int i=0, j=0;
    while(s[i]==' '){
        i++;
        //cout << "i: " << s[i] << endl;
    }
    s = s.substr(i, s.size());
    //cout<< s<< endl;
    i = s.size();
    while(s[i]==' ' || s[i]=='\n'){
        //cout << s[i] << endl;
        i--;
    }
    s = s.substr(0, i);
    //cout<< s << endl;
    return s;
}

bool containsIf(string s)
{
    s = s.substr(0,2);
    return s == "if";
}

bool containsElseIf(string s)
{
    s = s.substr(0,7);
   // cout << s << endl;
    return s == "else if";
}

bool containsElse(string s)
{
    s = s.substr(0,4);
    return s == "else";
}

bool containsWhile(string s)
{
    s = s.substr(0,5);
    return s == "while";
}

bool containsFor(string s)
{
    return s.substr(0, 3) == "for";
}

bool containsLoop(string s)
{
    return containsFor(s) || containsWhile(s);
}

bool isEnd(string s)
{
    //c//out<< "sp: "<< s.size() << " " << s ;
   // printf("\n--%d--\n", s[0]);
    //cout<< "siam"<<endl;
    //cout<< "ss: "<< s.back()<<endl;
   //   cout<< s<< endl;
    //cout<< "string: " <<
   // cout<< s<< "s"<<endl;
    return s[s.size()-2]=='}';
}

void readFile(string fileName)
{
    ifstream myFile(fileName);
    string temp;
    while(getline(myFile, temp)){
        //cout<< trim(temp) << endl;
        lines.push_back(trim(temp));
    }

    // FILE* fp = fopen("stack_using_array.cpp", "r");
    // string temp;
    // while(!feof(fp)){
    //     char tc;
    //     fscanf(fp, "%c", &tc);
    //     temp+=tc;
    // }
    // cout<< temp<< endl;
    // std::stringstream ss(temp);
    // std::string token;
    // while (std::getline(ss, token, '\n')) {
    //     lines.push_back(token);
    // }
 
}

bool containsMain(string s)
{
    return s.substr(0,10) == "int main()";
}

Node* buildCFG(Node *root, bool isLoop)
{

    Node* parent = root;
    vector<Node*> branches;
   // cout<< "current-- " << root->lineNo << endl;
    while(curIndex < lines.size())
    {
        Node* curNode = new Node(curIndex);
        
        //cout<< "curNodew: "<< curNode->lineNo << " " << "parent: " << parent->lineNo<< "\n";
        if(containsLoop(lines[curIndex]) || containsIf(lines[curIndex])){
           // cout<< "loop or if -- ";
            if(branches.size()>0){
                for(int i=0;i<branches.size(); i++){
                    //cout<< "branches: " << i << endl;
                    branches[i]->children.push_back(curNode);
                }
                branches.clear(); //test
            }
            else{
                parent->children.push_back(curNode);
                // for(int i=0;i<parent->children.size(); i++){
                //     cout<< "parent: "<< parent->lineNo+1 << " child: " << parent->children[i]->lineNo+1 << endl;
                    
                // }
            }
            curIndex++;
            //branches.push_back(curNode);
            if(containsLoop(lines[curIndex-1]))
            {
                //cout << "loop paisi: " << endl;
                branches.push_back(curNode);
                curIndex++;
                buildCFG(curNode, true);
            }
            else
            {
                //cout<< "if paisi... curNode er line: "<< curNode->lineNo << endl;
                Node* temp = buildCFG(curNode, false);
                // cout<< "temp: "<< temp->lineNo<< endl;
                branches.push_back(temp);
            }
        }
        else if(containsElseIf(lines[curIndex]) || containsElse(lines[curIndex])){
            //cout<< "else if or else" << endl;
            //parent->addChild(curNode);
            parent->children.push_back(curNode);
            // for(int i=0;i<parent->children.size();i++){
            //     cout<< "else branch: "<< parent->children[i]->lineNo+1 << endl;
            // }
            curIndex++;
            branches.push_back(buildCFG(curNode, false));
        }
        else{
            //cout<< "normal" << endl;
            if(branches.size()>0){
                for(int i=0;i<branches.size();i++){
                    //cout<< "branches: " << i << endl;
                    branches[i]->children.push_back(curNode);
                }
                branches.clear();
                
            }
            else{
                parent->addChild(curNode);
            }
            curIndex++;
            //cout<<"test:"<< lines[curIndex]<< "*"<<endl;
            if(isEnd(lines[curIndex-1])){
                
               // cout<< "dhuksi" << endl;
                if(isLoop){
                    curNode->children.push_back(root);
                }
                return curNode;
            }
          //  cout<< "parentLine: " << parent->lineNo << endl;
            parent = curNode;
            //cout<< "parentLine: " << parent->lineNo << endl;
        }  
    }
   // cout<< "current Node:-" << root->lineNo << endl;
   // cout<< "branches: " ;
    // for(int i = 0; i< root->children.size(); i++){
    //     cout<< root->children[i]->lineNo << " ";
    // }
   // cout<< "\n";
    return nullptr;
}
bool visited[5000] = {false};
int graph[5000][5000] = {0};

void traverse(Node* current, int level)
{
    cout<< "node: " << current->lineNo+1 << " level: " << level << endl;
    visited[current->lineNo] = true;
    cout<< "child: ";
    for(int i=0;i< current->children.size(); i++){
        cout << current->children[i]->lineNo+1 << " ";
    }
    cout<< endl;
    for(int i=0;i< current->children.size(); i++){
        if(!visited[current->children[i]->lineNo]){
            //cout<< "child: " << current->children[i]->lineNo+1 << endl;
            traverse(current->children[i], level+1);
        }
    }
}

void init()
{
    
    while(!containsMain(lines[curIndex])){
        curIndex++;
    }
    curIndex++;
    Node *root = new Node(curIndex);
    //cout<<"siam: " << root->lineNo << " " <<  endl;
    curIndex++;
    buildCFG(root, false);
    traverse(root, 0);
}

int main()
{
    readFile("stack_using_array.cpp");
    init();

}