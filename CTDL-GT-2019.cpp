#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

int MAX_PLANES = 300;

struct MayBay
{
	char SoHieuMB[15];
	char LoaiMB[40];
	char SoDay[2];
	char SoDong[2];
};

struct ChuyenBay
{
	char MaCB[15];
	long NgayGioKH;
	char NoiDen[50];
	char TrangThai[2];
	char SoHieuMB[15];
	string danhSachVe[];
};

struct ListChuyenBay
{
	ChuyenBay chuyenBay;
	ListChuyenBay *pNext;
};

struct HanhKhach
{
	char SoCMND[10];
	char Ho[20];
	char Ten[20];
	char Phai[5];
};

struct ListHanhKhach
{
	HanhKhach hanhKhach;
	ListHanhKhach *pLeft, *pRight;
};

struct 

/* XU LI VE NODE HANH KHACH */
//tao BST hanh khach
ListHanhKhach *Tao_BST(HanhKhach hanhKhach)
{
	ListHanhKhach *node = new ListHanhKhach();
	node->hanhKhach = hanhKhach;
	node->pLeft = node->pRight = NULL;
	return node;
}

//them moi hanh khach
void insert_BST(ListHanhKhach *&rootHanhKhach, HanhKhach hanhKhach)
{
	if(rootHanhKhach == NULL)
	{
		rootHanhKhach = Tao_BST(hanhKhach);
		return;
	}
	else
	{
		//strcmp(A, B) so sanh 2 chuoi: A > B => -1, A == B => 0, A < B => -1
		if(strcmp(rootHanhKhach->hanhKhach.SoCMND, hanhKhach.SoCMND) == 1) 
		{
			insert_BST(rootHanhKhach->pLeft, hanhKhach);
		}
		else
		{
			insert_BST(rootHanhKhach->pRight, hanhKhach);
		}
	}			
}

void NLR_HanhKhach(ListHanhKhach *root)
{
	if(root != NULL)
	{
		cout << "\nSo CMND: " << root->hanhKhach.SoCMND << "\n" << root->hanhKhach.Ho << "\n" << root->hanhKhach.Ten << "\n" << root->hanhKhach.Phai << endl;
		NLR_HanhKhach(root->pLeft);
		NLR_HanhKhach(root->pRight);
	}
	return;
}

/* XU LI VE NODE DANH SACH CHUYEN BAY */
//tao moi 1 node cua danh sach lien ket don
ListChuyenBay *Tao_ChuyenBay(ChuyenBay chuyenBay)
{
	ListChuyenBay *list = new ListChuyenBay;
	list->chuyenBay = chuyenBay;
	list->pNext = NULL;
	return list;
	
}

void DocFileMayBay(MayBay *&mayBay)
{
	string data;
	fstream f; 
	f.open("DSMAYBAY.txt", ios::in);
	if(f == NULL) return;
	int currentPos = 0;		//vi tri hien tai trong mang
	while(!f.eof())
	{
		MayBay mb;
		f >> data;
		if(data.size() == 0) break;
		strcpy(mb.SoHieuMB, data.c_str());
		f >> data;
		strcpy(mb.LoaiMB, data.c_str());
		f >> data;
		strcpy(mb.SoDay, data.c_str());
		f >> data;
		strcpy(mb.SoDong, data.c_str());
		mayBay[currentPos] = mb;
		currentPos++;
	}
	f.close();
}

void GhiFileMayBay(MayBay *list, int n)
{
	fstream f;
	f.open("DSMAYBAY.txt", ios::out);
	for(int i = 0; i < n; i++) 
	{
		f << list[i].SoHieuMB << endl << list[i].SoHieuMB << endl << list[i].SoDay << endl << list[i].SoDong << endl << endl;
			
	}
	f.close();
}

void Ghi_NLR_HanhKhach(ListHanhKhach *root)
{
	fstream f;
	f.open("DSMAYBAY.txt", ios::out);
	if(root != NULL)
	{
		f << root->hanhKhach.SoCMND << endl << root->hanhKhach.Ho << endl << root->hanhKhach.Ten << endl << root->hanhKhach.Phai << endl << endl;
		Ghi_NLR_HanhKhach(root->pLeft);
		Ghi_NLR_HanhKhach(root->pRight);
	}
	f.close();
}

void DocFileHanhKhach(ListHanhKhach *&rootHanhKhach)
{
	string data;
	fstream f; 
	f.open("DSHANHKHACH.txt", ios::in);
	if(f == NULL) return;
	while(!f.eof())
	{
		HanhKhach hk;
		f >> data;
		if(data.size() == 0) break;
		strcpy(hk.SoCMND, data.c_str());
		f >> data;
		strcpy(hk.Ho, data.c_str());
		f >> data;
		strcpy(hk.Ten, data.c_str());
		f >> data;
		strcpy(hk.Phai, data.c_str());
		insert_BST(rootHanhKhach, hk);		//them moi vao BST
	}
	f.close();
}

void DocFileChuyenBay(ListChuyenBay *&listChuyenBay)
{
	string data;
	fstream f; 
	f.open("DSCHUYENBAY.txt", ios::in);
	if(f == NULL) return;
	while(!f.eof())
	{
		ChuyenBay chuyenBay;
		f >> data;
		if(data.size() == 0) break;
		strcpy(chuyenBay.MaCB, data.c_str());
		f >> data;
		strcpy(chuyenBay.NoiDen, data.c_str());
		f >> data;
		strcpy(chuyenBay.NoiDen, data.c_str());
		f >> data;
		strcpy(chuyenBay.TrangThai, data.c_str());
		f >> data;
		strcpy(chuyenBay.SoHieuMB, data.c_str());
		//doc danh sach ve
		string *arrCMND;
		while(!f.eof())
		{
			char ve[10];
			f >> data;
			strcpy(ve, data.c_str());
			if(strcmp("*", ve) == 0) break; 
			cout << "data: " << data << "\tarrCMND: " << arrCMND << endl;			
		}
		cout << "End loop " << endl;
		cout << "arrCMND: " << arrCMND << endl;
	}
	f.close();
}

void themDSMayBay(MayBay *&mayBay, int n)
{
	cout << "\nNhap so hieu MB: ";
	cin >> mayBay[n].SoHieuMB;
	cout << "Nhap loai MB: ";
	cin >> mayBay[n].LoaiMB;
	cout << "Nhap so day: ";
	cin >> mayBay[n].SoDay;
	cout<< "Nhap so dong: ";
	cin >> mayBay[n].SoDong;
}

int main()
{
	MayBay *rootMB = new MayBay[MAX_PLANES];
	ListHanhKhach *rootHanhKhach = NULL;
	ListChuyenBay *listChuyenBay = NULL;
	DocFileMayBay(rootMB);
	DocFileHanhKhach(rootHanhKhach);
	DocFileChuyenBay(listChuyenBay);
	
//	NLR_HanhKhach(rootHanhKhach);
//	Ghi_NLR_HanhKhach(rootHanhKhach);
			
	return 0;
}

