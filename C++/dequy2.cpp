#include<iostream>
#include<cmath>
using namespace std;
long xN(int x,int n);               //Tính p(n) = xn, n nguyên và n >= 1:
long facN(int n);                   //Tính n! (n nguyên và n >= 0)
long maxUoc(int a, int b);          //Uoc chung lon nhat
float tongN(int n);                 //Tinh S(n) = 1/2 + 2/3 + 3/4 + .... + n/(n+1)
float tongXN(int x, int n);         //Tinh S(x,n) = x + x2 + x3 + ... + xn
int TongCacPhanTu(int a[], int n);   //Tinh tong n phan tu trong mang so nguyen
//Giai bai toan thap Ha Noi 
int main(int argc, char const *argv[])
{
    int x=4;
    int n=3;
    int a = 20, b =45;
    int z[]={1,2,3};
    // cout << xN(x,n) << endl;
    // cout << facN(n) << endl;
    // cout << maxUoc(a, b) << endl;
    // cout << tongN(n) << endl;
    // cout << tongXN(x,n) << endl;
    cout << TongCacPhanTu(z,n);
    system("pause");
    return 0;
}
//1.
long xN(int x, int n){
    if (n==1)
        return x;
    else 
        return xN(x,n-1)*x;
}
//2.
long facN(int n){
    if (n==1 || n==0)
    {
        return 1;
    }
    else 
        return facN(n-1)*n;
}
//3.
long maxUoc(int a, int b){
    if(a==b)
        return a;
    else 
        if(a>b)
            return maxUoc(a-b,b);
        else    
            return maxUoc(b-a,a);       
}
//4.
float tongN(int n){
    if(n==1)
        return (float)1/2;
    else 
        return tongN(n-1) +(float)n/(n+1);
}
//5.
float tongXN(int x, int n)
{
    if(n==1)
        return x;
    else 
        return tongXN(x,n-1) + pow(x, n);
}
//6.
int TongCacPhanTu(int a[], int n) {
   if(n==0){
    return 0;
   }
   return a[n - 1 ]+ TongCacPhanTu(a,n-1);
}
