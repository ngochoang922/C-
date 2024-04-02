// struct PhanSo
// {
//     int tuSo;
//     int mauSo;
// };

//1. hàm nhập xuất mảng PhanSO n phần tử nhập từ bàn phím(3đ)
//2. (4đ) (DEMO) so sánh 2 phân số bất kỳ trong mảng
//3. (3đ)
#include <bits/stdc++.h>
#include <functional>
#include <vector>
using namespace std;

struct phanso{
    int tu;
    int mau;
    };

void nhapPhanSo (phanso a[], int n);
void xuat (phanso a[], int n);
void solve(){
    int n; cin >> n;
    phanso a[n]; 
    nhapPhanSo(a, n); 
    xuat (a, n);
}

int main(){
solve();
    
return 0;
}
void nhapPhanSo (phanso a[], int n) {
for(int i = 0; i < n; i++) { cin >> a[i].tu >> a[i].mau;
}
}
void xuat (phanso a[], int n){

    for(int i = 0; i < n; i++){
        cout <<("mau: << a[i].mau << tu: <<< a[i].tu << endl");
    }
}
