#include<iostream>
#include<vector>
#include<set>
#include<fstream>
#include<map>
#include<algorithm>

using namespace std;

struct Node {
    char name;
    vector<Node*> children;
    set<int> lines;

    Node(char pName, int lineNumber)
    {
        name = pName;
        lines.insert(lineNumber);
    }
       // name(pName), lines.push_back(lineNumber){}
    add(int line){
        lines.insert(line);
    }

};

vector<string> readLines;
int lineNo = 0;
char nodeName = '0';

readFile(string fileName)
{
    ifstream myFile(fileName);
    string temp;
    while(getline(myFile, temp)){
        readLines.push_back(temp);
    }
}

bool containsStatement(string s)
{
    for(int i=0;i<s.size();i++){
        if(s[i]>=0 && s[i]<127)
            return true;
    }
    return false;
}

vector<char> getChildren(Node* parent)
{
    vector<char> children;
    for(int i = 0; i< parent->children.size(); i++){
        children.push_back(parent->children[i]->name);
    }
    return children;
}


map<char, vector<char>> traverse(Node* root, vector<Node*>vis, map<char, vector<char>> graph)
{
    vis.push_back(root);
    vector<Node*> tempChildren = root->children;
    Node* cur = tempChildren[0];
    graph[root->name] = getChildren(root);
    for(int i=0;i<tempChildren.size();i++){
        cur = tempChildren[i];
        if(find(vis.begin(), vis.end(), cur) == vis.end())
            graph = traverse(cur, vis, graph);
    }
    return graph;
}



void addParent(Node* child, vector<Node*> parent)
{
    for(int i=0;i<parent.size();i++){
        parent[i]->children.push_back(child);
    }
}

bool isBlock(Node* node)
{
    int sz = readLines[lineNo].size();
    if(readLines[lineNo][sz-1] == '{') return true;
    lineNo++;
    while(true){
        sz = readLines[lineNo].size();
        if(readLines[lineNo][sz-1] == '{') return true;
        else if(readLines[lineNo][sz-1] == ';'){
            lineNo--;
            return false;
        }
        else node->lines.insert(lineNo);
        lineNo++;
    }
}

Node* dfsChildSearch(Node* parent, bool haveMultipleLine)
{
    Node* normalStatement = NULL;
    vector<Node*> branchTerminatorNodes;
    lineNo++;
    while(1)
    {
        if(readLines.size()<=lineNo) return NULL;

        if(readLines[lineNo].find("for")!= string::npos ||
           (readLines[lineNo].find("while")!= string::npos && readLines[lineNo].find("}while")!= string::npos)){
            Node* newNode = (Node*) malloc(sizeof(Node*));
            newNode->name = nodeName++;
            newNode->lines.insert(lineNo);
            if(!branchTerminatorNodes.empty()){
                addParent(newNode, branchTerminatorNodes);
                branchTerminatorNodes.clear();
            }
            else{
                parent->children.push_back(newNode);
            }
            parent = newNode;
            branchTerminatorNodes.push_back(parent);
            newNode = dfsChildSearch(newNode, isBlock(newNode));
            if(newNode==NULL) return NULL;
            newNode->children.push_back(parent);
            parent = newNode;
            normalStatement = NULL;
        }

        else if(readLines[lineNo].find("do")!= string::npos){
            Node* newNode = (Node*) malloc(sizeof(Node*));
            newNode->name = nodeName++;
            newNode->lines.insert(lineNo);
            if(!branchTerminatorNodes.empty()){
                addParent(newNode, branchTerminatorNodes);
                branchTerminatorNodes.clear();
            }
            else{
                parent->children.push_back(newNode);
            }
            parent = newNode;
            newNode = dfsChildSearch(newNode, isBlock(newNode));
            if(newNode==NULL) return NULL;
            newNode->children.push_back(parent);
            parent = newNode;
            normalStatement = NULL;
        }

        else if(readLines[lineNo].find("if")!= string::npos){
            Node* newNode = (Node*) malloc(sizeof(Node*));
            newNode->name = nodeName++;
            newNode->lines.insert(lineNo);
            if(!branchTerminatorNodes.empty()){
                addParent(newNode, branchTerminatorNodes);
                branchTerminatorNodes.clear();
            }
            else{
                parent->children.push_back(newNode);
            }
            parent = newNode;
            newNode = dfsChildSearch(newNode,  isBlock(newNode));
            if(newNode == NULL) return NULL;
            branchTerminatorNodes.push_back(newNode);
            normalStatement = NULL;
        }

         else if(readLines[lineNo].find("else")!= string::npos){
            Node* newNode = (Node*) malloc(sizeof(Node*));
            newNode->name = nodeName++;
            newNode->lines.insert(lineNo);
            parent->children.push_back(newNode);
            newNode = dfsChildSearch(newNode, isBlock(newNode));
            if(newNode == NULL) return NULL;
            branchTerminatorNodes.push_back(newNode);
         }

         else{
            if(!containsStatement(readLines[lineNo++]))
                continue;
            if(normalStatement == NULL){
                normalStatement = (Node*) malloc(sizeof(Node*));
                normalStatement->name = nodeName++;
                normalStatement->lines.insert(lineNo);
                if(!branchTerminatorNodes.empty()){
                    addParent(normalStatement, branchTerminatorNodes);
                    branchTerminatorNodes.clear();
                }
                else parent->children.push_back(normalStatement);
                parent = normalStatement;

            }
            else{
                normalStatement->lines.insert(lineNo);
            }

            if(readLines[lineNo].find("}")!= string::npos){
                return normalStatement;
            }
         }
         lineNo++;
    }
}

buildControlFlowGraph()
{
    Node* root = (Node*) malloc(sizeof(Node*));
    root->name = nodeName++;
    root->lines.insert(lineNo);
    //dfsChildSearch(root, true);

   // vector<Node*> vis;
   // map<char, vector<char>> graph;
   // traverse(root, vis, graph);

}

int main()
{
    readFile("stack_using_array.cpp");
    buildControlFlowGraph();
}
