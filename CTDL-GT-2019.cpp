#include<iostream>
#include <string>

using namespace std;

int MAX_PLANES = 300;

struct MayBay
{
	char SoHieuMB[15];
	char LoaiMB[40];
	int SoDay;
	int SoDong;
};

struct ChuyenBay
{
	char MaCB[15];
	long NgayGioKH;
	char NoiDen[50];
	int TrangThai;
	char SoHieuMB[15];
//	string Ve[];
};

struct ListChuyenBay
{
	ChuyenBay chuyenBay;
	ListChuyenBay *pNext;
};

struct HanhKhach
{
	int SoCMND;
	char Ho[20];
	char Ten[20];
	bool Phai;
};

struct ListHanhKhach
{
	HanhKhach hanhKhach;
	ListHanhKhach *pLeft;
	ListHanhKhach *pRight;
};

int main()
{
	 MayBay *ListMayBay[MAX_PLANES];
	 
	cout<<"gere";
}
