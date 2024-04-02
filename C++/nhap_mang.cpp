#include <iostream>
#include <cstdlib>
using namespace std;
void vetcan(int a[], int arraySize);
void linhcanh(int a[], int arraySize);
int main()
{       
    int arraySize = 100;
    int a[arraySize];
    
    for (int i = 0; i < arraySize; i++)
    {
        a[i]=rand()%100;
        cout<< a[i] << "\n";
    }
//    vetcan(a,arraySize);
	
    linhcanh(a,arraySize);
    return 0;
}
	//vetcan
void vetcan(int a[], int arraySize){
    bool check = false; //bien kiem tra
    int x;
    cout<< "\nnhap gia tri x = "; 
	cin>> x;
     for (int i = 0; i < arraySize; i++)
    {  
    if(a[i]==x){
    	cout<<"\ntim thay x tai vi tri: "<< i + 1;
    	check = true;
    	break;
	}
}
	if(check == false){
		cout<<"\nkhong tim thay x";
	}
}
void linhcanh(int a[], int arraySize){
    int x;
    cout<<"\n nhap gia tri x = ";
	cin>> x ;
	a[arraySize] = x ;
    for(int i = 0; ; i++){
        if(a[i] == x ){
//           if(i == 100){
//                 cout<< "Khong Tim Thay X";
//                return;
//            }
            cout << "\nTim Thay Gia Tri X = " << x << " Tai Vi Tri " << i;
            return;
        
		}
		
		
	}
	


}
