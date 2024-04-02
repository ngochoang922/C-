//nhập mảng n phần tử, gán ngẫu nhiên từ 1-n 
//1. selection sort
//2. interchange sort
 #include<iostream>
using namespace std;
void hoanVi(int &a, int &b );
void xuatMang(int a[], int n);
void selectionSort(int a[], int n);
int main(int argc, char const *argv[])
{
    int a[] = {2, 3, 1, 5 ,4 };
    int n = 5;
    selectionSort(a,n);
    xuatMang(a,n);
    return 0;
}
void xuatMang(int a[],int n){
    for(int i = 0; i < n; i++){
        cout<<"\na["<< i <<"]= "<< a[i];
    }
}
void selectionSort(int a[], int n){
    for(int i = 0; i < n; i++){
        int min = i;
    for(int j = i + 1 ; j < n; j++ ){
        if(a[i]< a[min])
        min = j;
    hoanVi(a[i], a[min]);
    }
    }
}
void hoanVi(int &a, int &b){
     int temp;
     temp = a;
     a = b;
     b = temp;
     
}

