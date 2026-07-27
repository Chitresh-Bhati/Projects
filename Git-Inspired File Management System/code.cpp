#include "hashmap.h"
#include "heap.h"
#include "mystack.h"
#include <ctime>

using namespace std;


class TreeNode{//one node represents a version of a file(snapshoted/unsnapshoted).
public:
    // Version (TreeNode) Structure
    int version_id;
    string content;
    string message; // Empty if not a snapshot
    time_t created_timestamp;
    time_t snapshot_timestamp; // 0 if not a snapshot
    TreeNode* parent;
    vector<TreeNode*> children;
    
    TreeNode(string content="",string message="",TreeNode* parent=nullptr){//create a new node
        this->content=content;
        this->message=message;
        created_timestamp=time(nullptr);
        snapshot_timestamp=0;
        this->parent=parent;
        
    }

};
class FileStructure{
public:
    TreeNode* root; // Your implementation of the tree
    TreeNode* active_version;
    hashmap<int, TreeNode*> version_map; // stores ID , filenode
    int total_versions;
    time_t lastupdatetime=0;
    

    FileStructure(){//initialize new (empty) file structure.
        root= new TreeNode();
        active_version=root;
        total_versions=1;
        root->version_id=total_versions-1;
        version_map[total_versions-1]=root;//IDs start from 0;

    }
    string READ(){//displays content of current active_version of file.
        return active_version->content;
    }
    void INSERT(string cntnt){//adds content at the end of current content.
        if(active_version->snapshot_timestamp==0){//not snapshotted.
            active_version->content+=cntnt;
        }
        else{//snapshotted.
            TreeNode* new_version = new TreeNode(active_version->content+cntnt,"",active_version);
            active_version->children.push_back(new_version);
            active_version=new_version;
            total_versions++;
            new_version->version_id=total_versions-1;
            version_map[total_versions-1]=active_version;
        }
        lastupdatetime=time(nullptr);
    }
    void UPDATE(string cntnt){//replaces the content.
        if(active_version->snapshot_timestamp==0){//not snapshotted.
            active_version->content=cntnt;
        }
        else{//snapshotted.
            TreeNode* new_version = new TreeNode(cntnt,"",active_version);
            active_version->children.push_back(new_version);
            active_version=new_version;
            total_versions++;
            new_version->version_id=total_versions-1;
            version_map[total_versions-1]=active_version;
        }
        lastupdatetime=time(nullptr);
    }
    void SNAPSHOT(string msg){
        if (active_version->snapshot_timestamp!=0) {
            cout<<"Already Snapshotted."<<endl;
            return;
        }
        active_version->message=msg;
        active_version->snapshot_timestamp=time(nullptr);
    }
    void ROLLBACK(int versionID){
        if(versionID==-1){
            if(active_version->parent==nullptr){
                cout<<"No parent exists."<<endl;
                return;
            }
            else {
                active_version=active_version->parent;
                cout<<"Rollback to parent successful."<<endl;
            }
        }
        else if (version_map[versionID]==nullptr) {
            cout<<"Invalid version ID."<<endl;
        }
        else{

            active_version=version_map[versionID];
            cout<<"Rollback to version ID-"<<versionID<<" successfull."<<endl;
        }
    }
    void HISTORY(){//each row contain {ID,snap_shottimestamp,message} of all snapshotted versions that are ancestors of current file.
        TreeNode* temp=active_version;
        mystack<TreeNode*> s;
        if(temp->snapshot_timestamp!=0){
            s.push(temp);
        }
        while(temp->parent!=nullptr){
            temp=temp->parent;
            s.push(temp);
        }
        while(!s.empty()){
            cout<<"Version ID: "<<to_string(s.top()->version_id)+", Snapshot Timestamp: "+to_string(s.top()->snapshot_timestamp)+", Snapshot Message: "+s.top()->message<<endl;
            s.pop();
        }
    }



};


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
bool isInteger(string& s) {
    if(s.empty()) return false;
    int start=0;
    if (s[0]=='+'||s[0]=='-')start=1;
    if (start==s.size())return false;
    for (int i=start; i<s.size(); i++) {
        if (!isdigit(s[i]))return false;
    }
    return true;
}

int main(){
    hashmap<string, FileStructure*> files;//stores filename, pointer to a file.

    
    
    while(true){
        cout<<"Write your command here: ";
        string line;
        getline(cin,line);
        vector<string> cmd=split(line);//given whole command.
        if (cmd.size()==0) {
            continue;
        }

        string command = cmd[0];
        if(command=="CREATE"){//creates empty snapshotted initial file.
            if(cmd.size()>2){
                cout<<"ERROR: Filename should not contain spaces."<<endl;
            }
            else if(cmd.size()!=2){
                cout<<"ERROR: Invalid filename."<<endl;
            }
            else{
                string filename= cmd[1];
                if(files[filename]!=nullptr){
                    cout<<"This file already exists."<<endl;
                    
                }
                else{
                
                    string initial_message= "initial version of "+filename;
                    FileStructure* file= new FileStructure();
                    file->SNAPSHOT(initial_message);
                    files[filename]=file;
                    cout<<filename<<" successfully created!"<<endl;

                    
                }
            }
            
            

        }
        else if(command=="READ"){
            string filename=cmd[1];
            
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"ERROR: No such file exists"<<endl;
            }
            else{
                cout<<file->READ()<<endl;
            }
        }
        else if(command=="INSERT"){
            string filename=cmd[1],content= (cmd.size()==3) ? cmd[2] : "";
            
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"No such file exists"<<endl;
            }
            else{
                file->INSERT(content);
                cout<<"Insertion successful."<<endl;
                
            }
        }
        else if(command=="UPDATE"){
            string filename=cmd[1],content= (cmd.size()==3) ? cmd[2] : "";
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"No such file exists"<<endl;
            }
            else{
                file->UPDATE(content);
                cout<<"Updation successful!"<<endl;
                
            }
        }
        else if(command=="SNAPSHOT"){
            string filename=cmd[1],message= (cmd.size()==3) ? cmd[2] : "";
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"No such file exists"<<endl;
            }
            else if (file->active_version->snapshot_timestamp!=0) {
                cout<<"ERROR: "<<filename<<" is already snapshotted."<<endl;
            }
            else{
                file->SNAPSHOT(message);
                cout<<filename<<" successfully snapshotted."<<endl;
            }
        }
        else if(command=="ROLLBACK"){
            string filename=cmd[1],id= (cmd.size()==3) ? cmd[2] : "-1";//if id not given, rollbacks to parent.
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"ERROR: No such file exists"<<endl;
            }
            else{
                file->ROLLBACK(stoi(id));

            }
        }
        else if(command=="HISTORY"){
            string filename=cmd[1];
            FileStructure* file=files[filename];
            if(file==nullptr){
                cout<<"ERROR: No such file exists"<<endl;
            }
            else{
                file->HISTORY();
            }
        }
        else if(command=="RECENTFILES"){
            if (cmd.size()==1) {
                cout<<"ERROR: Incomplete Command."<<endl;
            }
            else{

                if (!isInteger(cmd[1])) {
                    cout<<"ERROR: Invalid Input."<<endl;
                    continue;
                }
                
                int k=stoi(cmd[1]);
                vector<pair<string, FileStructure*>> allentries=files.getAll();
                min_heap h;
                for(pair<string, FileStructure*> entry:allentries){
                    h.push({entry.second->lastupdatetime,entry.first});
                    if(h.size()>k)h.pop();
                }
                vector<string> names;
                while(!h.empty()){
                    names.push_back(h.top());
                    h.pop();
                }
                for(int i=names.size()-1;i>=0;i--){
                    cout<<names[i]<<endl;
                }

            }

        }
        else if(command=="BIGGESTTREES"){
            if (cmd.size()==1) {
                cout<<"ERROR: Incomplete Command."<<endl;
            }
            else {

                if (!isInteger(cmd[1])) {
                    cout<<"ERROR: Invalid Input."<<endl;
                    continue;
                }
                int k=stoi(cmd[1]);
                if(k<0){
                    cout<<"ERROR: Invalid number entry."<<endl;
                    continue;

                }
                vector<pair<string, FileStructure*>> allentries=files.getAll();
                min_heap h;
                for(pair<string, FileStructure*> entry:allentries){
                    h.push({entry.second->total_versions,entry.first});
                    if(h.size()>k)h.pop();
                }
                vector<string> names;
                while(!h.empty()){
                    names.push_back(h.top());
                    h.pop();
                }
                for(int i=names.size()-1;i>=0;i--){
                    cout<<names[i]<<endl;
                }
            }

        }
        else if(command=="EXIT") {
            cout<<"Exited from the program."<<endl;
            break;
        }

        else{
            cout<<"Invalid Command"<<endl;
        }


        
    }
    
}