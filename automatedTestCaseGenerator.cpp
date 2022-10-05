#include<iostream>
#include<vector>
#include<set>
#include<fstream>
#include <string>
#include<map>
#include<algorithm>

using namespace std;

vector<string> lines;
int curIndex = 0, cyclomaticComplexity=2;

class Node{
public:
    int lineNo;
    int x, y, positionInLevel;
    vector<Node*> children;
    vector<string> conditions;
    vector<char> operators;
    bool hasBackedge;
    bool loopInitiated = false, isLoop = false;
    int loopStart, loopEnd, loopIncrement;
public:
    Node(int plineNo){
        lineNo = plineNo;
        hasBackedge = false;
    }
    void addChild(Node* node);
};


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
    return s[s.size()-1]=='}';
}

void readFile(string fileName)
{
    ifstream myFile(fileName.c_str());
    string temp;
    while(getline(myFile, temp)){
        lines.push_back(trim(temp));
    }

}

bool containsMain(string s)
{
    return s.substr(0,10) == "int main()";
}

string getItemsInsideIf(string s)
{
    int index = 3;
    string ret = "";
    while(s[index]!=')'){
        ret += s[index];
        index++;
    }
    return ret;
}

string getItemsInsideElseIf(string s)
{
    int index = 8;
    string ret = "";
    while(s[index]!=')'){
        ret += s[index];
        index++;
    }
    return ret;
}

vector<string> detachIfMultipleCondition(string s)
{
    int index = 0;
    vector<string> toRet;
    string tem = "";
    while(index < s.size())
    {
        tem += s[index];
        if(index+1 < s.size() && ((s[index] == '&' && s[index+1] == '&') || (s[index] == '|' && s[index+1] == '|'))){
            tem[tem.size()-1]='\0';
            toRet.push_back(trim(tem));
            index+=2;
            tem = "";
            continue;
        }

        index++;
    }
    toRet.push_back(trim(tem));
    return toRet;
}

vector<char> getOperators(string s)
{
    int index = 0;
    vector<char> toRet;
    while(index < s.size())
    {
        if(index+1 < s.size() && ((s[index] == '&' && s[index+1] == '&') || (s[index] == '|' && s[index+1] == '|'))){
            if(s[index] == '&' && s[index+1] == '&')
                toRet.push_back('&');
            else
                toRet.push_back('|');
            index+=2;
            continue;
        }
        index++;
    }
    return toRet;
}

string getItemsInsideLoop(string s)
{
    string toRet = "";
    if(containsWhile(s)){
        int index = 6;
        while(index< s.size() && s[index]!=')'){
            toRet += s[index];
            index++;
        }
    }
    else{

    }
    return toRet;
}

Node* buildCFG(Node *root, bool isLoop)
{

    Node* parent = root;
    vector<Node*> branches;
    while(curIndex < lines.size())
    {
        Node* curNode = new Node(curIndex);

        if(containsLoop(lines[curIndex]) || containsIf(lines[curIndex])){
            cyclomaticComplexity++;
            if(branches.size()>0){
                for(int i=0;i<branches.size(); i++){
                    branches[i]->children.push_back(curNode);
                }
                branches.clear(); //test
            }
            else{
                parent->children.push_back(curNode);
            }
            curIndex++;
            if(containsLoop(lines[curIndex-1]))
            {
                branches.push_back(curNode);
                buildCFG(curNode, true);
            }
            else
            {
                string tem = getItemsInsideIf(lines[curIndex-1]);
               // cout << "tem: " << tem << endl;
                vector<string> conditions = detachIfMultipleCondition(tem);
                vector<char> operators = getOperators(tem);
                curNode->operators = operators;
                curNode->conditions = conditions;
                Node* temp = buildCFG(curNode, false);
                branches.push_back(temp);
            }
        }
        else if(containsElseIf(lines[curIndex]) || containsElse(lines[curIndex])){
            if(containsElseIf(lines[curIndex])){
                string tem = getItemsInsideElseIf(lines[curIndex]);
                vector<string> conditions = detachIfMultipleCondition(tem);
                vector<char> operators = getOperators(tem);
                curNode->operators = operators;
                curNode->conditions = conditions;
                cyclomaticComplexity++;
            }
            parent->children.push_back(curNode);
            curIndex++;
            branches.push_back(buildCFG(curNode, false));
        }
        else{
            if(branches.size()>0){
                for(int i=0;i<branches.size();i++){
                    branches[i]->children.push_back(curNode);
                }
                branches.clear();

            }
            else{
                parent->addChild(curNode);
            }
            curIndex++;
            if(isEnd(lines[curIndex-1])){

                if(isLoop){
                    root->hasBackedge = true;

                    curNode->children.push_back(root);
                }
                return curNode;
            }
            parent = curNode;
        }
    }
    return NULL;
}
bool visited[5000] = {false};
int graph[5000][5000] = {0};
int levelNodeCount[5000];

void traverse(Node* current, int level)
{
    levelNodeCount[level]++;
    visited[current->lineNo] = true;
    /*for(auto a: current->conditions){
        cout<< a << " ";
    }
    cout<< endl;
    for(auto a: current->operators){
        cout<< a << " ";
    }
    cout<< endl;*/
    cout<< current->lineNo+1 << endl;
    for( auto a: current->children){
        cout<< a->lineNo+1 << " ";
    }
    cout<< endl;
    for(int i=0;i< current->children.size(); i++){
        if(!visited[current->children[i]->lineNo]){
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
    curIndex++;
    buildCFG(root, false);
    char s[60] = "Cyclomatic Complexity: ";
    s[23] = '0' + cyclomaticComplexity ;
    traverse(root, 0);
    cout<< "cyclomatic complexity: " << cyclomaticComplexity << endl;
}

int main()
{
    readFile("stack_using_array.cpp");
    init();
    /*string in =  getItemsInsideElseIf("else if(a>b && b>c || c<a)");
    vector<string> detached = detachIfMultipleCondition(in);
    for(auto a:detached){
        cout<< a<< endl;
    }
    vector<char> operators = getOperators(in);
    for(auto a:operators){
        cout<< a<< endl;
    }*/

    cout << getItemsInsideLoop("while(siam<5)");
}

