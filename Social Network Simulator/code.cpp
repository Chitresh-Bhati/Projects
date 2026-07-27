#include <iostream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

struct PostNode {
    int time;
    string data;
    PostNode* left;
    PostNode* right;
    int height;
    PostNode(int k,string s="") : time(k), data(s), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    PostNode* root=nullptr;

    int height(PostNode* N) {
        if (N == nullptr)
            return 0;
        return N->height;
    }

    PostNode* rightRotate(PostNode* y) {
        PostNode* x = y->left;
        PostNode* T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        // Update heights
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        // Return new root
        return x;
    }

    PostNode* leftRotate(PostNode* x) {
        PostNode* r=x->right;
        x->right=r->left;
        r->left=x;
        
        r->height=max(height(r->left),height(r->right)) + 1;
        x->height=max(height(x->left),height(x->right)) + 1;
        
        return r;
        
    }

    int getBalance(PostNode* N) {
        return height(N->left)-height(N->right);
    }

    PostNode* insertHelper(PostNode* node, int time, string data) {
        // 1. Standard BST insertion
        if (node == nullptr)
            return new PostNode(time,data);

        if (time < node->time)
            node->left = insertHelper(node->left, time,data);
        else if (time > node->time)
            node->right = insertHelper(node->right, time,data);
        else // Duplicate keys are not allowed
            return node;

        // 2. Update height of this ancestor node
        node->height = 1 + max(height(node->left), height(node->right));

        // 3. Get the balance factor to check for imbalance
        int balance = height(node->left)-height(node->right);

        // 4. If unbalanced, perform rotations
        // Left Left Case
        if (balance > 1 && time < node->left->time)
            return rightRotate(node);

        // Right Right Case
        else if(balance<-1 && time>node->right->time){
            return leftRotate(node);
        }
     
        

        // Left Right Case
        else if (balance > 1 && time > node->left->time){
            node->left=leftRotate(node->left);
            return rightRotate(node);
        }
     


        // Right Left Case
        else if(balance<-1 && time<node->right->time){
            node->right=rightRotate(node->right);
            return leftRotate(node);
        }
      

        return node;
    }
    
    PostNode* minValueNode(PostNode* node) {
        PostNode* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    

    void rev_inorder_helper(PostNode* node, int& k){
        if(node==nullptr)return;
        rev_inorder_helper(node->right,k);
        if(k==0)return;
        cout<<node->data<<endl;
        k--;
        rev_inorder_helper(node->left,k);
    }

public:
    AVLTree() {
        root = nullptr;
    }

    

    void insert(int time, string data) {
        root = insertHelper(root, time, data);
    }

    int getRootKey() {
        if(root==nullptr)return -1;
        return root->time;
    }

    void print_rev_inorder(int k){
        rev_inorder_helper(root,k);
    }
};

class UserNode{
public:
    string username;
    set<string> friends;
    AVLTree posts;

    UserNode(string name) : username(name) {}
};

int addtime=0;
unordered_map<string,UserNode*> users;

void add_user(string username){
    if (users.find(username)!=users.end()) {
        cout<<"ERROR: This user already exists (case-insensitive)"<<endl;
        return;
    }
    users[username] = new UserNode(username);
    cout<<username<<" is added!"<<endl;
}
void add_friends(string user1, string user2){
    if(users.find(user1)!=users.end() && users.find(user2)!=users.end()){
        users[user1]->friends.insert(user2);
        users[user2]->friends.insert(user1);
        cout<<user1<<" and "<<user2<<" are added as friends!"<<endl;
    }
    else {
        cout<<"ERROR: Invalid username(s)."<<endl;
    }

}
void list_friends(string username){
    if (users.find(username)==users.end()) {
        cout<<"User not found!"<<endl;
        return;
    }
    if (users[username]->friends.size()==0) {
        cout<<username<<" doesn't have any friends."<<endl;
        return;
    }

    for(const string &friend_name:users[username]->friends){
        cout<<friend_name<<endl;
    }

}
void add_post(string username, string post_content){
    if (users.find(username)==users.end()) {
        cout<<"User not found!"<<endl;
        return;
    }

    users[username]->posts.insert(addtime++,post_content);
    cout<<"Post added for "<<username<<endl;
}
void output_posts(string username, int n){
    if (users.find(username)==users.end()) {
        cout<<"User not found!"<<endl;
        return;
    }
    users[username]->posts.print_rev_inorder(n);
}
int degrees_of_separation(string user1,string user2){
    if (users.find(user1) == users.end() || users.find(user2) == users.end())return -1;
    if (user1==user2)return 0;
    UserNode *start=users[user1], *target=users[user2];
    queue<pair<UserNode*,int>> nodes;//nodes with dist from start.
    unordered_map<UserNode*,int> done_nodes;
    nodes.push({start,0});
    done_nodes[start]++;
    while(!nodes.empty()){
        for(const string &friend_name:nodes.front().first->friends){
            if(done_nodes[users[friend_name]]!=1){
                if(users[friend_name]==target)return nodes.front().second+1;
                nodes.push({users[friend_name],nodes.front().second+1});
                done_nodes[users[friend_name]]=1;//markvisited
            }
        }
        nodes.pop();
    }
    return -1;
}
typedef pair<int,UserNode*> pp;
bool compare(const pair<int, string>& a, const pair<int, string>& b){
    if(a.first!=b.first)return a.first<b.first;
    return a.second<b.second;
}
void suggest_friends(string username,int k){
    unordered_map<UserNode*,int> suggestions; //friend-of-friend which r not friend, no. of mutual friends.
    for(const string &friend_name:users[username]->friends){
        for(const string&friend_of_friend:users[friend_name]->friends){
            if(users[username]->friends.find(friend_of_friend)==users[username]->friends.end() && friend_of_friend!=username){//not already a friend AND not the user itself.
                suggestions[users[friend_of_friend]]++;
            }
        }
    }

    ////METHOD1 BY MINHEAP 
    // priority_queue<pp,vector<pp>,greater<pp>> minheap;
    // for(auto it:suggestions){
    //     minheap.push({it.second,it.first});
    //     if(minheap.size()>k)minheap.pop();
    // }
    // stack<string> ordered_suggestions;//stores top k suggestions in reverse order
    // while(!minheap.empty()){
    //     ordered_suggestions.push(minheap.top().second->username);
    //     minheap.pop();
    // }
    // while(!ordered_suggestions.empty()){
    //     cout<<ordered_suggestions.top()<<endl;
    //     ordered_suggestions.pop();
    // }

    // //METHOD2 BY SET
    // set<pair<int,string>> res;
    // for(auto it:suggestions){
    //     res.insert({-it.second,it.first->username});
    // }
    // for(auto p:res){
    //     cout<<p.second<<endl;
    //     k--;
    //     if (k==0)break;
    // }

    //METHOD3 BY SORT
    vector<pair<int,string>> res;
    for(auto it:suggestions){
        res.push_back({-it.second,it.first->username});
    }
    sort(res.begin(), res.end(), compare);
    for(auto p:res){
        cout<<p.second<<endl;
        k--;
        if (k==0)break;
    }
}

vector<string> split(const string& input) {//splits into three parts(using first two spaces)
    vector<string> ans;
    string temp="";
    int i=0;
    while(i<input.size() && input[i]!=' '){
        temp.push_back(input[i]);
        i++;
    }
    if (temp!="")ans.push_back(temp);
    i++;
    temp="";

    while(i<input.size() && input[i]!=' '){
        temp.push_back(input[i]);
        i++;
    }
    if (temp!="")ans.push_back(temp);
    i++;
    temp="";

    while(i<input.size()){
        temp.push_back(input[i]);
        i++;
    }
    if (temp!="")ans.push_back(temp);
    return ans;

}
void convert_to_lowercase(string &s){
    for(char &c:s){
        c=tolower(c);
    }
    return;
}

int main() {
    unordered_map<string, UserNode*> users;
    vector<string> cmd;
    string input;

    while (true) {
        cout << "Write your command here: ";

        getline(cin, input);
        if (input.empty()) continue;

        cmd = split(input);
        if (cmd.size()==0) continue;
        convert_to_lowercase(cmd[0]);
        string command=cmd[0];
        if (cmd.size() > 1)convert_to_lowercase(cmd[1]);
        if (cmd.size() > 2)convert_to_lowercase(cmd[2]);


        if (command == "add_user") {
            if (cmd.size() < 2) {
                cout<<"Error: Missing username"<<endl;
                continue;
            }
            add_user(cmd[1]);
        }

        else if (command == "add_friend") {
            if (cmd.size() < 3) {
                cout <<"Error: Missing usernames"<<endl;
                continue;
            }
            add_friends(cmd[1],cmd[2]);
        }

        else if (command == "add_post") {
            if (cmd.size() < 3) {
                cout <<"Error: Missing username or post text"<< endl;
                continue;
            }
            add_post(cmd[1], cmd[2]);
        }

        else if (command == "output_posts") {
            if (cmd.size() < 3) {
                cout<<"Error: Missing username or count"<<endl;
                continue;
            }
            int k =stoi(cmd[2]);
            output_posts(cmd[1], k);
        }

        else if(command=="list_friends"){
            if (cmd.size() < 2) {
                cout<<"Error: Missing username"<<endl;
                continue;
            }
            list_friends(cmd[1]);
        }
        else if(command=="suggest_friends"){
            if (cmd.size() < 3) {
                cout <<"Error: Missing username or number"<<endl;
                continue;
            }

            suggest_friends(cmd[1],stoi(cmd[2]));
        }
        else if(command=="degrees_of_separation"){
            if (cmd.size() < 3) {
                cout <<"Error: Missing username(s)"<<endl;
                continue;
            }
            int degree=degrees_of_separation(cmd[1],cmd[2]);
            cout<<"Degree of separation between "<<cmd[1]<<" and "<<cmd[2]<<" is "<<degree<<endl;

        }

        else if(command=="exit") {
            cout<<"Goodbye!"<<endl;
            break;
        }

        else {
            cout<<"Error: Unknown command"<<endl;
        }
    }
}
