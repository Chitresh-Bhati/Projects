code uses only 4 files 
1. Node.cpp
2. dict.cpp 
3. qna_tool.cpp 
4. tester.cpp 

for part 1 one should comment out the part 2 code in tester which include
lastline i.e    qna_tool.query(question, "api_call.py");

now set the question along with total para need 
you will receive topk para list along .txt file of paragraphs 


for part 2 one should comment out whole part 1 code in tester and set 
the question and diretly run these files you will find para along with chatgpt
answer on console 


for runnig in both part write g++ Node.cpp dict.cpp qna_tool.cpp tester.cpp -o f
and then ./f


