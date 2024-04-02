// *Xây dựng danh sách liên kết với các hàm
// 1. Thêm phần tử(Đầu, cuối, giữa)
// 2. Duyệt danh sách
// 3. Xoá phân tử (Đầu, cuối, xoá phần tử x)
// 4. Truy xuất phần tử
// 5. Xoá danh sách( xoá toàn bộ danh sách)
// 6. Đếm số lượng node/phần tử trong danh sách
// -----~~~~----~~~----~~~----~~~-----
#include<iostream>
using namespace std;
struct Node
{
    /* data */
    int key;
    Node *next;
};
Node *newNode(int data);
void getNode(Node *pHead);

void adHead(Node *&pHead, int data);
void adMiddle(Node *&pHead, int data);
void adLast(Node *&pHead, int data);

void removeHead(Node *&pHead);
int main(int argc, char const *argv[])
{
// 8 2 1 32 42
    
    Node *pHead = NULL;
    //them phan tu dau
    adHead(pHead,1);
     adHead(pHead,2);
      adHead(pHead,3);
    getNode(pHead);// in danh sách 
    cout<<endl;

    // thêm phần tử giữa
    adMiddle(pHead, 15);
    adMiddle(pHead, 17);
    getNode(pHead);// in danh sách
    // thêm phần tử cuối 
    adLast(pHead, 4);
    adLast(pHead, 5);
    getNode(pHead);//in danh sách
    
    //  cout<<"\n";

    //  removeHead(pHead);
    //  removeHead(pHead);
    // cout<<"sau khi xoá phần tử đầu";
    //  getNode(pHead);
     getchar();
    return 0;
}

Node *newNode(int data)
{
    Node *temp = new Node;
    temp -> key = data;
    temp -> next = NULL;
    return temp;
}
void getNode(Node *pHead){
    while ( pHead != NULL )
    {
        /* code */
        cout<<"\n"<<pHead -> key;
        pHead = pHead -> next;
    }
    
}
void adHead(Node *&pHead, int data){
    Node *temp = newNode(data);
    if (pHead == NULL)
    {
        pHead = temp;
    }else
    {
        temp -> next = pHead;
        pHead = temp;

    }
}
void adMiddle(Node *&pHead, int data){
    Node *current = pHead; 
     Node *temp = newNode(data);
    int count = 0;
     while (current != NULL)  // tìm số phần tử của danh sách
     {
        current = current -> next;
        count++;
     }
    int middle = count / 2;
    int countmiddle = 0;
    current = pHead;
    while (countmiddle < middle)
     {
        current = current -> next;
        countmiddle++;
     }
    Node *node  = newNode(data);
    Node *Temp = current -> next;
    current -> next = node;
    node -> next = temp;
     
}


void adLast(Node *&pHead, int data){
     Node *temp = newNode(data);
        while (pHead->next != NULL)
        {
            pHead = pHead->next;
        }
        pHead->next = temp;
}

void removeHead(Node *&pHead)
{
   
    // Node *temp = pHead;
    // pHead = pHead->next;
    // delete temp;
}

