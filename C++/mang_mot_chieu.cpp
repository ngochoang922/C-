#include<iostream>

using namespace std;
// int main(){
	
// // IN MANG
// //	int numbers[4] = {1, 2, 3, 4};
// //	for(int i = 0; i < 4; i++){
// //		cout<<" "<<numbers[i];
// //		}

// // NHAP XUAT MANG
// //	int n;
// //	int a[n];
// //	cout<<"n = ";
// //	cin>> n;
// //
// //	 //nhap vao c�ch nhau dau backspace
// //	for(int i = 0; i<n; i++){
// //	 	cin>>a[i];
// //	}
// //	
// //	//xuat ra cac phan tu tren cung 1 dong
// //	for(int i = 0; i<n; i++){
// //	 	cout<<a[i]<< " ";
// //
// //	 	}

// // bai tap 1: viet chuong trinh nhap vao hai so nguyen duong n v� m (0 < n <= 10), (0 < m <= 10)
// // tao mang 2 chieu co chua cac so nguyen hang n v� m cot. t�nh v� in ra m�n h�nh tong c�c phan tu co trong mang
// //
// //	int n, m, arr[10][10];
// //	cout<<"n = ";
// //	cin>> n;
// //	cout<<"m = ";
// //	cin>> m;
// //	
// //	cout<<"nhap mang: ";
// //	
// //	for(int i = 0; i < n; i++){
// //		
// //		for(int j = 0; j < m; j++){
// //		 cin>> arr[i][j];
// //		}
// //	}
// //	
// //	int sum = 0;
// //	for(int i = 0; i < n; i++){
// //		
// //		for(int j = 0; j < m; j++){
// //		sum += arr[i][j];
// //		}
// //	}
// //	cout<<"tong cac phan tu trong mang: "<< sum;
// //			

//  int numbers[2][3] = {
//         { 4, 3, 2 },
//         { 9, 7, 5 }
//     };

//     // Vi?t code ? d�y
//     for (int i = 0; i < 2; i++) {
//         for (int j = 0; j <3; j++) {
//             std::cout << numbers[i][j] << " ";
//         }
//         std::cout << std::endl;
//     }
// 	return 0;
// }

void nhapMang(int a[],int n);
void xuatMang(int a[],int n);
void themPhanTu(int *&p, int &n);
void xoaPhanTu(int *&p, int &n);
int main(int argc, char const *argv[])
{
    int *p;
    int n;
    cout<<"\nnhap so phan tu n = ";
    cin>>n;
    p = new int[n];
    nhapMang(p,n);
    xuatMang(p,n);
    themPhanTu(p,n);
    cout<<"\nsau khi them";
    xuatMang(p,n);
    xoaPhanTu(p,n);
    cout<<"\nsau khi xoa";
    xuatMang(p,n);
    return 0;
}
void nhapMang(int a[],int n){
    for (int i = 0; i < n; i++)
    {
        cout<<"\na["<<i<<"] =";
        cin>>a[i];
    }
    
}
void xuatMang(int a[],int n){
    for (int i = 0; i < n; i++)
    {
        cout<<"\na["<<i<<"] ="<<a[i];
    }
    
}
void themPhanTu(int *&p, int &n){
    int soPhanTuCanThem;
    cout<<"\nnhap them so phan tu can them = ";
    cin>>soPhanTuCanThem;
    int kichThuocMoi = soPhanTuCanThem + n;
    int *temp = new int[kichThuocMoi];
    for (int i = 0; i < n; i++)
    {
        *(temp + i) = *(p + i);
    }
    for (int i = n; i < kichThuocMoi; i++)
    {
        cout<<"\na["<<i<<"] =";
        cin>>*(temp + i);
    }
    delete(p);
    p = temp;
    n = kichThuocMoi;
    
}
void xoaPhanTu(int *&p, int &n)
{
    int kichThuocMoi = n/2;
    int *temp = new int[kichThuocMoi];
    for (int i = 0; i < kichThuocMoi; i++)
    {
        *(temp + i) = *(p + i);
       
    }
    delete(p);
    p = temp;
    n = kichThuocMoi;
}
       