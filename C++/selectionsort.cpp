// nh?p m?ng n ph?n t?, g�n ng?u nhi�n t? 1-n
// 1. selection sort
// 2. interchange sort
#include <iostream>
using namespace std;
void hoanVi(int &a, int &b);
void xuatMang(int a[], int n);
void selectionSort(int a[], int n);
void interchangeSort(int a[], int n);
void bubbleSort(int a[], int n);
void insertionSort(int a[], int n);
void quickSort(int a[], int left, int right);
int main(int argc, char const *argv[])
{
	int a[] = {2, 3, 1, 5, 4, 10, 11, 20};
	int n = 8 ;
	// selectionSort(a,n);
	// interchangeSort(a,n);
	// bubbleSort(a,n);
	// insertionSort(a, n);
	quickSort(a,0,n-1);
	xuatMang(a, n);
	return 0;
}
void xuatMang(int a[], int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << "\na[" << i << "]= " << a[i];
	}
}
void selectionSort(int a[], int n)
{
	for (int i = 0; i < n; i++)
	{
		int min = i;
		for (int j = i + 1; j < n; j++)
		{
			if (a[j] < a[min])
				min = j;
			hoanVi(a[i], a[min]);
		}
	}
}
void hoanVi(int &a, int &b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}
void interchangeSort(int a[], int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			if (a[i] > a[j])
				hoanVi(a[i], a[j]);
		}
	}
}
void bubbleSort(int a[], int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = n - 1; j > i; j--)
		{
			if (a[j] < a[j - 1])
			{

				hoanVi(a[j], a[j - 1]);
			}
		}
	}
}
void insertionSort(int a[], int n)
{
	int i, j, tam;
	for (i = 0; i < n; i++)
	{
		tam = a[i];
		// j = i - 1;
		for (j = i - 1; j >= 0; j--)
		{
			if (a[j] > tam)
			{
				a[j + 1] = a[j];
			}
			else
			{
				break;
			}
		}

		a[j + 1] = tam;
	}
}
void quickSort(int a[], int left, int right){
	int mid = (left + right)/2;
	int i = left; //duyet tu trai
	int j = right; //duyet tu phai
	while(i < j)
	{
		while (a[i]< a[mid])
		{
			i++;
		}
		while (a[j] > a[mid])
		{
			j--;
		}
		if (i<=j)
		{
			hoanVi(a[i], a[j]);
			i++;
			j--;
		}
		
		
	}
		if (i<right)// sx mang ben phai = quicksort
		{
		quickSort(a,i,right);
		}
		if (i>left)
		{
		quickSort(a,left,j);
		}
		
	}
	
