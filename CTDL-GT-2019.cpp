#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <mylib.h>
#include <map>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define DOWN -80
#define UP -72
#define ENTER 13
#define ESC 27
#define BACKSPACE 8
#define LEFT -75
#define RIGHT -77

#define BG_CLR 41
#define DARK_CLR 8
#define SELECTED_CLR 34
#define TXT_CLR 4

using namespace std;

const int MAX_PLANES = 300;

map<string, int> ListSoLuongGhe;
char MenuItem[3][30] = {" DANH SACH MAY BAY     ", " THONG TIN CHUYEN BAY  ", " DANH SACH HANH KHACH  "};
char ItemMayBay[3][20] = {"  THEM MAY BAY    ", "  XOA MAY BAY     ", "  HIEU CHINH MB   "};
char ItemChuyenBay[3][20] = {"  THEM CHUYEN BAY ", "  XOA CHUYEN BAY  ", "  HIEU CHINH CB   "};

int thang[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int NamNhuan(int nam){
	return (nam%4==0 && nam%100!=0 || nam%400==0)? 1: 0;
}
char Alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct Time{
	int gio, phut, ngay, thang, nam;
};

struct VeHanhKhach
{
	char SoVe[5];
	char SoCMND[10];
};

struct DanhSachVe
{
	int n;
	VeHanhKhach *dsVe;
};

struct ChuyenBay
{
	char MaCB[15];
	Time NgayGioKH;
	char NoiDen[40];
	char TrangThai[4];
	char SoHieuMB[15];
	DanhSachVe dsVe;	
};

struct MayBay
{
	char SoHieuMB[15];
	char LoaiMB[40];
	int SoDay;		
	int SoDong;		
};

struct ListMayBay{
	int n;
	MayBay *DSMayBay[MAX_PLANES];
};

struct NodeCB
{
	ChuyenBay *chuyenBay;
	NodeCB *pNext;
};

struct ListCB{
	NodeCB *head, *tail;
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

void setColor(int bgColor,int txtColor){
    SetBGColor(bgColor);
    SetColor(txtColor);
}

int nhapnam(int x=wherex(), int y=wherey()){
	int nam=0;
	int n, dem=0;
	gotoxy(x,y);
	while(dem!=4){
		n=getch(); 
		if(n==8){	//khi an phim delete
			gotoxy(x,y); cout << "    "; gotoxy(x,y);
			nam=0; dem=0;
			continue;
		}
		if(n==9) return -2;	//nhan tab tra ve 0, neu gia tri nhap =0 thi khong lay
		if(n==27) return -1;	////tra ve tru 1 thi khong nhap tiep nua va khong lay gia tri vua nhap
		n-=48;
		//if(n==-35) return 0;	//phim enter
		if((n<0 || n>9)||(n==0 && dem==0)) continue;
		dem++;
		cout << n;
		nam+=n*pow(10,4-dem);	
	}
	return nam;	
}
int nhapnt(int kieu=0, int x=wherex(), int y=wherey()){	//kieu: 0 ngay, 1 nhap thang, 2 nam,   3 nhap gio, 4 kieu phut
	int kthang=0, kgio=0, kphut=0;
	if(kieu==1) {
		kthang=1; x=x+3;
	}	
	if(kieu==2) {
		x=x+6;
		return nhapnam(x,y);
	}
	if(kieu==3) {
		kgio=1;
	}
	if(kieu==4) kphut=1;
	
	gotoxy(x,y);
	int ngay=0, dem=0, n;
	while(1){
		n=getch();
		if(n==8) {
			gotoxy(x,y); cout << "  "; gotoxy(x,y);
			ngay=0;dem=0;
			continue;
		}
		if(n==9) return -2;	//nhan tab tra ve -2, neu gia tri nhap =0 thi khong lay
		if(n==27) return -1;	//tra ve tru 1 thi khong nhap tiep nua va khong lay gia tri vua nhap	
		n-=48;	//enter 13   -48 = -35
	//	if(n==-35 && ngay==0) return 0;
	//	if(n==-35 && (kieu==3 || kieu==4) && dem==1) return ngay;//
		if(n==-35 && ngay>=1 && ngay<=3+(kthang*6)+(kgio*6)+(kphut*6)) {		//neu nhap 1-2-3 nhan enter thi dung lai
			gotoxy(x,y); cout << "0" << ngay; 
			return ngay;
		}
		if(n<0 || n>9) continue;
		if((n>=4-(kthang*2)-(kgio*1)+(kphut*2)) && dem==0){
			cout << "0" << n;
			return n;
		}
		if(dem==0) {
			dem++;
			ngay=n;
			cout << n;
			continue;
		}
		if(n==0 && (kieu==3||kieu==4)) return 0;
		if(n==0 && ngay==0) continue;
		
		if(ngay==3-(kthang*2)-(kgio*1)+(kphut*3)) 
			if(n>=2+(kthang*1)+(kgio*2)) continue;
		if(dem==1){
			cout << n;
			return ngay*10+n;
		}
	}
}
void NgayHT(Time time, int x=wherex(), int y=wherey()){
	gotoxy(x,y);
	time.ngay<10?cout << "0" << time.ngay:cout << time.ngay;
	time.thang<10?cout <<" 0" << time.thang:cout << " " << time.thang;
	time.nam<10?cout <<" " << time.nam:cout << " " << time.nam;
	setColor(0,15); gotoxy(x+2,y); cout << "/";	gotoxy(x+5,y); cout << "/";
}
bool NhapNgay(Time &time, int x=wherex(), int y=wherey()){
	int kieu=0, dd, mm, yy;
	setColor(0,8);
	NgayHT(time, x,y); 
	Time h;
	h.ngay=nhapnt(0,x,y); 	//Nhap ngay		
		if(h.ngay==-1) return false;	//bam esc thi thoat
		if(h.ngay==-2) {		//Neu bam phim tab thi tra ve ngay he thong
			h.ngay=time.ngay;
			gotoxy(x,y); time.ngay<10?cout << "0" << time.ngay:cout << time.ngay;
		}
	h.thang=nhapnt(1,x,y);	//Nhap thang
		if(h.thang==-1) return false;	//bam esc thi thoat
		if(h.thang==-2) {	//neu bam phim tab thi tra ve thang he thong
			h.thang=time.thang;
			gotoxy(x+3,y); time.thang<10?cout << "0" << time.thang:cout << time.thang;
		}
	while(h.ngay>thang[h.thang]+(h.thang==2)?1:0){	//kiem tra xem ngay vua nhap co hop he so voi thang khong
		gotoxy(x,y); cout << "  ";	gotoxy(x,y);
		h.ngay=nhapnt(0,x,y);	//Nhap cho den khi ngay dung voi thang	
			if(h.ngay==-1) return false;
			if(h.ngay==-2) {		//neu bam phim tab thi tra ve ngay he thong va quay lai kiem tra xem co hop le khong (neu ngay he thong =30 va thang minh nhap =2 thi nhap lai)
				h.ngay=time.ngay;
				gotoxy(x,y); time.ngay<10?cout << "0" << time.ngay:cout << time.ngay;
			}
	}
	h.nam=nhapnt(2,x,y); 	
		if(h.nam==-1) return false;
		if(h.nam==-2) {
			h.nam=time.nam; gotoxy(x+6, y); cout << time.nam;
		}
	while(h.thang==2 && h.ngay > thang[2]+NamNhuan(h.nam)) {	//neu nhap nam (nam nhuan) roi lam cho ngay cua thang 2 khong hop le thi nhap lai ngay
		gotoxy(x,y); cout << "  ";	gotoxy(x,y);
		h.ngay=nhapnt(0,x,y);		//nhap lai ngay
			if(h.ngay==-1) return false;	//nhan esc thi thoat luon
			if(h.ngay==-2) h.ngay=32;					//neu khong nhap ngay ma bam phim tab de bo qua thi cho dk bi sai quay lai nhap (khong duoc de trong)
	}
	//Luu gia tri cap nhat vao time
	if(h.ngay!=0) time.ngay=h.ngay;
	else if(h.ngay==0 || h.ngay==-1){
		NgayHT(time,x,y); 
		return false;
	}
	if(h.thang!=0) time.thang=h.thang;
	if(h.nam!=0) time.nam=h.nam;
	return true;
}

/* XU LI VE NODE HANH KHACH */
//tao BST hanh khach
ListHanhKhach *Tao_BST(HanhKhach hanhKhach) {
	ListHanhKhach *node = new ListHanhKhach();
	node->hanhKhach = hanhKhach;
	node->pLeft = node->pRight = NULL;
	return node;
}

//them moi hanh khach
void Insert_BST(ListHanhKhach *&rootHK, HanhKhach hanhKhach) {
	if(rootHK == NULL) {
		rootHK = Tao_BST(hanhKhach);
		return;
	}
	else {
		//strcmp(A, B) so sanh 2 chuoi: A > B => -1, A == B => 0, A < B => -1
		if(strcmp(rootHK->hanhKhach.SoCMND, hanhKhach.SoCMND) == 1) {
			Insert_BST(rootHK->pLeft, hanhKhach);
		}
		else {
			Insert_BST(rootHK->pRight, hanhKhach);
		}
	}			
}

void NLR_HanhKhach(ListHanhKhach *root) {
	if(root != NULL) {
		cout << "\nSo CMND: " << root->hanhKhach.SoCMND << "\n" << root->hanhKhach.Ho << "\n" << root->hanhKhach.Ten << "\n" << root->hanhKhach.Phai << endl;
		NLR_HanhKhach(root->pLeft);
		NLR_HanhKhach(root->pRight);
	}
	return;
}

/* XU LI VE NODE DANH SACH CHUYEN BAY */
NodeCB* CreateNodeCB(ChuyenBay *cb){
   NodeCB* node = new NodeCB;
   if (node) {
      node->chuyenBay = cb;
      node->pNext = NULL;
   }
   return node;
}

bool AddHeadCB(ListCB &list, NodeCB* node)
{
   if (!list.head) //x�t danh s�ch r?ng
      list.head = list.tail = node;
   else
   {
      node->pNext = list.head; //s?a lk node c?n th�m
      list.head = node; //ch?nh l?i con tr? c?a danh s�ch
   }
   return true;
}
 
bool AddTailCB(ListCB &list, NodeCB* node)
{
   if (!list.head)
      list.head = list.tail = node;
   else
   {
      list.tail->pNext = node;
      list.tail = node;
   }
    return true;
}
 
 
 //test
bool AddAfterCB(ListCB &list, NodeCB *node){
	NodeCB *pos = list.head;		//vi tri node truoc
	for(NodeCB *p = list.head; p != NULL; p = p ->pNext)
	{
		if(strcmp(node->chuyenBay->MaCB, p->chuyenBay->MaCB) < 0)
		{
			NodeCB *g = pos ->pNext;
			node ->pNext = g;
			pos ->pNext = node;
			return true;
		}
		pos = p;
	}
	return false;
}


void DocFileChuyenBay(ListCB &listCB) {
	string data;
	fstream f; 
	f.open("DSCHUYENBAY.txt", ios::in);
	if(f == NULL) return;
	int soluongve;
	while(!f.eof())
	{
		//tao bien chua thong tin chuyen bay
		ChuyenBay *chuyenBay = new ChuyenBay();
		getline(f,data);;
		if(data.size() == 0) break;
		strcpy(chuyenBay->MaCB, data.c_str());
		
		//doc ngay thang nam
		f >> chuyenBay->NgayGioKH.ngay;
		f >> chuyenBay->NgayGioKH.thang;
		f >> chuyenBay->NgayGioKH.nam;
		f >> chuyenBay->NgayGioKH.gio;
		f >> chuyenBay->NgayGioKH.phut;
		f.ignore();
		getline(f,data);	strcpy(chuyenBay->NoiDen, data.c_str());
		getline(f,data);	strcpy(chuyenBay->TrangThai, data.c_str());
		getline(f,data);	strcpy(chuyenBay->SoHieuMB, data.c_str());
		
		//xu li danh sach ve
		getline(f,data);
		soluongve = atoi(data.c_str());
		DanhSachVe DSVE;
		DSVE.n = soluongve;
		
		//tao vung nho chua ve
		DSVE.dsVe = new VeHanhKhach[ListSoLuongGhe[chuyenBay->SoHieuMB]];
				
		//doc file sanh sach ve
		for(int i = 0; i < soluongve; i++) {
			getline(f,data);	strcpy(DSVE.dsVe[i].SoCMND, data.c_str());
			getline(f,data);	strcpy(DSVE.dsVe[i].SoVe, data.c_str());
		}
		chuyenBay->dsVe = DSVE;
		NodeCB *node = CreateNodeCB(chuyenBay);
		AddTailCB(listCB, node);
	}
	f.close();
}

void DocFileMayBay(ListMayBay &rootMB) {
	string data;
	fstream f; 
	f.open("DSMAYBAY.txt", ios::in);
	if(f == NULL) return;	
	int  SL, i=0;		//vi tri hien tai trong mang
	f >> SL;	f.ignore();
//	cout <<"SL "<<SL;
	MayBay *mayBay;
	mayBay = new MayBay[SL];
	while(!f.eof()) {
		getline(f,data);
		if(data.size() == 0) break;
		string SHMB = string(data) ;
		strcpy(mayBay[i].SoHieuMB, data.c_str());
		getline(f,data);	strcpy(mayBay[i].LoaiMB, data.c_str());
		f >>mayBay[i].SoDay;
		f >> mayBay[i].SoDong;
		f.ignore();
		ListSoLuongGhe[SHMB] = mayBay[i].SoDay*mayBay[i].SoDong;
//		cout << "SHMB: " << SHMB << "\t" << ListSoLuongGhe[SHMB] <<endl;
		rootMB.DSMayBay[i] = &mayBay[i];
		rootMB.n++;
		i++;
	}
	f.close();
}

void GhiFileMayBay(ListMayBay &rootMB) {
	fstream f;
	f.open("DSMAYBAY.txt", ios::out);
	f << rootMB.n << endl;
	for(int i = 0; i < rootMB.n; i++) {
		f << rootMB.DSMayBay[i]->SoHieuMB << endl << rootMB.DSMayBay[i]->LoaiMB << endl << rootMB.DSMayBay[i]->SoDay << endl << rootMB.DSMayBay[i]->SoDong << endl;
	}
	f.close();
}

void GhiFileChuyenBay(ListCB &listCB) {
	fstream f;
	f.open("DSCHUYENBAY.txt", ios::out);
	while(listCB.head!=NULL){
		f << listCB.head->chuyenBay->MaCB << endl << listCB.head->chuyenBay->NgayGioKH.ngay << endl;
		f << listCB.head->chuyenBay->NgayGioKH.thang << endl << listCB.head->chuyenBay->NgayGioKH.nam << endl;
		f << listCB.head->chuyenBay->NgayGioKH.gio << endl << listCB.head->chuyenBay->NgayGioKH.phut << endl;
		f << listCB.head->chuyenBay->NoiDen << endl << listCB.head->chuyenBay->TrangThai << endl;
		f << listCB.head->chuyenBay->SoHieuMB << endl << listCB.head->chuyenBay->dsVe.n << endl;
		for(int i = 0; i < listCB.head->chuyenBay->dsVe.n; i++){
			f << listCB.head->chuyenBay->dsVe.dsVe[i].SoCMND << endl << listCB.head->chuyenBay->dsVe.dsVe[i].SoVe << endl;
		}
		listCB.head = listCB.head->pNext;
	}
	
	
	f.close();
}

void Ghi_NLR_HanhKhach(ListHanhKhach *root) {
	fstream f;
	f.open("DSMAYBAY.txt", ios::out);
	if(root != NULL) {
		f << root->hanhKhach.SoCMND << endl << root->hanhKhach.Ho << endl << root->hanhKhach.Ten << endl << root->hanhKhach.Phai << endl << endl;
		Ghi_NLR_HanhKhach(root->pLeft);
		Ghi_NLR_HanhKhach(root->pRight);
	}
	f.close();
}

void DocFileHanhKhach(ListHanhKhach &rootHK) {
	string data;
	fstream f; 
	f.open("DSHANHKHACH.txt", ios::in);
	if(f == NULL) return;
	while(!f.eof()) {
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
//		Insert_BST(&rootHK, hk);		//them moi vao BST
	}
	f.close();
}

//Bat ph�m
int GetKey()
{	
	char key;
	key = getch();
	if(key==-32 || key==0)
	return -getch();
	else return key;
}

/*XU LI DO HOA*/
void ClearScreen(int x, int y, int x1, int y1){
	SetBGColor(0);
	for(int i=x;i<=x1;i++)
		for(int j=y;j<=y1;j++){
			gotoxy(i,j); cout << " ";
		}
}


void ProjectName() {
	setColor(12, 14);
	gotoxy(5,1);	cout << "                       " << endl;
	gotoxy(5,2);	cout << "  QUAN LI CHUYEN BAY   " << endl;
	gotoxy(5,3);	cout << "        QUOC TE        " << endl;
	gotoxy(5,4);	cout << "                       " << endl;
	setColor(BG_CLR, TXT_CLR);
}

void DisplayText(char *s, int x, int y, int txtClr, int bgClr) {	
	setColor(bgClr, txtClr);
	gotoxy(x,y);	cout << "                       " << endl;
	gotoxy(x,y+1);	cout << s << endl;
	gotoxy(x,y+2);	cout << "                       " << endl;
	SetBGColor(0);	
}

void DisplayTextItem(char *s, int x, int y, int txtClr, int bgClr) {	
	setColor(bgClr, txtClr);
	gotoxy(x,y);	cout << "                  " << endl;
	gotoxy(x,y+1);	cout << s << endl;
	gotoxy(x,y+2);	cout << "                  " << endl;
	SetBGColor(0);	
}

bool ConfirmChange(int x, int y, char *s){
	bool confirm=false;
	setColor(4, 14);
	gotoxy(x, y);	cout<<"                      ";
	gotoxy(x,y+1);	cout<<s;
	gotoxy(x, y+2);	cout<<"                      ";
	gotoxy(x, y+3); cout<<"    YES          NO   ";
	gotoxy(x, y+4);	cout<<"                      ";
	SetBGColor(0);
	gotoxy(x+17,y+3);	cout<<"NO";
	char key;
	while(1){
		key=GetKey();
		switch(key){
			case LEFT: {
				confirm = true;
				gotoxy(x+4,y+3);
				cout << "YES";
				SetBGColor(4);
				gotoxy(x+17,y+3);
				cout<<"NO";
				SetBGColor(0);
				break;
			}
			case RIGHT: {
				confirm = false;
				SetBGColor(4);
				gotoxy(x+4,y+3);
				cout << "YES";
				SetBGColor(0);
				gotoxy(x+17,y+3);
				cout<<"NO";
				break;
			}
			case ENTER: {
				clrscr();
				return confirm;
			}
		}
	}
	
	
}

void VerticalLine(int x, int y, int height, char s){
	for(int i = y; i < height; i++){
		setColor(0,4);
		gotoxy(x, i);
		cout << s; 
	}		
}

void HorizontalLine(int x, int y, int width, char s){
	for(int i = x; i < width; i++){
		setColor(0,4);
		gotoxy(i, y);
		cout << s; 
	}
}

void FormMayBay(ListMayBay rootMB){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                          THONG TIN MAY BAY                                 ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(34,6);	cout << "              SO HIEU MB              LOAI MB                SO LUONG GHE   ";
	int y = 10;
	//ke dung
	VerticalLine(34,5, 21, 179); //VerticalLine(42,5, 21, 179);
	VerticalLine(65,5, 21, 179); 
	VerticalLine(90,5, 21, 179); VerticalLine(109,5, 21, 179);
	//ke ngang
	HorizontalLine(34,7,110,236); 
	HorizontalLine(34,5,110,236);
	HorizontalLine(34,20,110,236);
	//in du lieu
	for(int i = 0; i < rootMB.n; i++){
//		gotoxy(37,8+i);	cout << i+1;
		gotoxy(45,8+i);	cout << rootMB.DSMayBay[i]->SoHieuMB;
		gotoxy(68,8+i);	cout << rootMB.DSMayBay[i]->LoaiMB;
//		strcpy(rootMB.DSMayBay[i]->LoaiMB, rootMB.DSMayBay[i]->LoaiMB);
		gotoxy(97,8+i);	cout << (rootMB.DSMayBay[i]->SoDay)*(rootMB.DSMayBay[i]->SoDong);		
	}
}

void FormChuyenBay(ListCB list){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                        THONG TIN CHUYEN BAY                                ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(34,6);	cout << "    MA CB        THOI GIAN           NOI DEN       TRANG THAI   SO HIEU MB  ";
	int y = 10;
	//ke dung
	VerticalLine(34,5, 21, 179); VerticalLine(45,5, 21, 179);
	VerticalLine(65,5, 21, 179); 	VerticalLine(84,5, 21, 179);
	VerticalLine(95,5, 21, 179); 	VerticalLine(109,5, 21, 179);
	//ke ngang
	HorizontalLine(34,7,110,236); 
	HorizontalLine(34,5,110,236);
	HorizontalLine(34,20,110,236);
	//in du lieu
	int i=0;
	for(NodeCB *node = list.head; node != NULL; node = node->pNext){
		gotoxy(37,8+i);	cout << node->chuyenBay->MaCB;
		gotoxy(48,8+i);	cout << node->chuyenBay->NgayGioKH.ngay << "/" << node->chuyenBay->NgayGioKH.thang << "/" << node->chuyenBay->NgayGioKH.nam << " " << node->chuyenBay->NgayGioKH.gio << ":" << node->chuyenBay->NgayGioKH.phut;
		gotoxy(68,8+i);	cout << node->chuyenBay->NoiDen;
		gotoxy(89,8+i);	cout << node->chuyenBay->TrangThai;	
		gotoxy(98,8+i);	cout << node->chuyenBay->SoHieuMB;	
		i++;
	}
}

void ChildScreen(ListMayBay &rootMB, ListHanhKhach &rootHK, ListCB &ListCB, int x, int y, int selectedItem, int choose, char ItemMayBay[3][20]) {	
	ProjectName();
	DisplayText(MenuItem[choose], x, y + 4*0, TXT_CLR, BG_CLR);
	//item con
	for(int i = 0; i < 3; i++) {
		if(i == selectedItem)
			DisplayTextItem(ItemMayBay[i], x + 5, y + 4 + 4*i, TXT_CLR, BG_CLR);
		else 
			DisplayTextItem(ItemMayBay[i], x + 5 , y + 4 + 4*i, TXT_CLR, BG_CLR);		
	}
	//set menuitemcha
	if(choose == 0) {
		DisplayText(MenuItem[1], x, 22, TXT_CLR, DARK_CLR);
		DisplayText(MenuItem[2], x, 26, TXT_CLR, DARK_CLR);
	}
	else if(choose == 1) {
		DisplayText(MenuItem[0], x, 6, TXT_CLR, DARK_CLR);
		DisplayText(MenuItem[2], x, 26, TXT_CLR, DARK_CLR);
	}
	else if(choose == 2) {
		DisplayText(MenuItem[0], x, 6, TXT_CLR, DARK_CLR);
		DisplayText(MenuItem[1], x, 10, TXT_CLR, DARK_CLR);
	}
}

void MainScreen(int choose, int x, int y) {
	clrscr();
	ProjectName();
	for(int i = 0; i < 3; i++) {
		if(i == choose)
			DisplayText(MenuItem[i], x, y + 4*i, TXT_CLR, BG_CLR);
		else 
			DisplayText(MenuItem[i], x + 2, y + 4*i, TXT_CLR, BG_CLR);		
	}
	DisplayText(MenuItem[choose], 4, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
	SetBGColor(0);
}

bool InsertDSMB(ListMayBay &rootMB, MayBay *mayBay){
	if(rootMB.n == MAX_PLANES){
		return false;
	}
	else if(rootMB.n == 0 || strcmp(mayBay->SoHieuMB, rootMB.DSMayBay[rootMB.n-1]->SoHieuMB) > 0){
		rootMB.DSMayBay[rootMB.n] = mayBay;
		rootMB.n++;
		return true;
	}
	else{
		for(int i = 0; i < rootMB.n; i++){
			if(strcmp(mayBay->SoHieuMB, rootMB.DSMayBay[i]->SoHieuMB) < 0) {
				for(int j = rootMB.n; j > i; j--){
					rootMB.DSMayBay[j] = rootMB.DSMayBay[j-1];
				}
				rootMB.DSMayBay[i] = mayBay;
				rootMB.n++;
				return true;
			}
		}
	}
	return false;
}

//bool InsertDSVe(DanhSachVe &DSVE, VeHanhKhach *ve, int max){
//	if(DSVE.n == max){
//		return false;
//	}
//	else if(DSVE.n == 0 || strcmp(ve->SoCMND, DSVE.dsVe[rootMB.n-1]->SoHieuMB) > 0){
//		rootMB.DSMayBay[rootMB.n] = mayBay;
//		rootMB.n++;
//		return true;
//	}
//	else{
//		for(int i = 0; i < rootMB.n; i++){
//			if(strcmp(mayBay->SoHieuMB, rootMB.DSMayBay[i]->SoHieuMB) < 0) {
//				for(int j = rootMB.n; j > i; j--){
//					rootMB.DSMayBay[j] = rootMB.DSMayBay[j-1];
//				}
//				rootMB.DSMayBay[i] = mayBay;
//				rootMB.n++;
//				return true;
//			}
//		}
//	}
//	return false;
//}

void InsertDigit(char* A, char kt, int &vt, int &dem){
	int l=strlen(A);
	A[l+1]='\0';
	while(l!=vt){
		A[l]=A[l-1];
		l--;
	}
	A[vt]=kt;
	vt++;
	dem++;
}

void RemoveDigit(char* A, int &vt, int &dem){
	int l=strlen(A);
	for(int i=vt; i<l; i++){
		A[i]=A[i+1];
	}
	A[l-1]='\0';
	vt--;
	dem--;
}

char *GetText(int x, int y, int length, int type, bool &check)
{
	char *text = new char[length];	// hung ket qua
	text[0] = '\0';
	int dem=0, temp=0;		//temp vi tri hien tai trong mang char, dem la vi tri lon nhat cua chuoi nhap vao
	char key;
	gotoxy(x,y);
	while(check){
		 key = GetKey();
		if(key == ENTER && strlen(text) > 0){
			return text;
			check=false;
		}
		if(type == 0 && (key >= 65 && key <= 90) && (temp >= 0 && temp < length)) {
			InsertDigit(text, key, temp,dem);
			SetBGColor(0);
			for(int i=0; i <= length; i++){
				gotoxy(x+i,y); cout << " ";
			} 
			gotoxy(x,y);	cout << text;
		}
		if(type == 0 && (key >= 97 && key <= 122)  && (temp >= 0 && temp < length)) {
			key -=32;
			InsertDigit(text, key, temp,dem);
			SetBGColor(0);
			for(int i=0; i <= length; i++){
				gotoxy(x+i,y); cout << " ";
			} 
			gotoxy(x,y);	cout << text;
		}
		if((key >= 48 && key <= 57) && (temp >= 0 && temp < length)){
			InsertDigit(text, key, temp,dem);
			SetBGColor(0);
			for(int i=0; i <= length; i++){
				gotoxy(x+i,y); cout << " ";
			} 
			gotoxy(x,y);	cout << text;
		}
		if(key == LEFT && temp>0){
			temp--;
			gotoxy(x+temp,y);
		}
		if(key == RIGHT && temp<strlen(text)){
			temp++;
			gotoxy(x+temp,y);
		}
		if(key == BACKSPACE && temp>0){
			int pos = temp-1;
			RemoveDigit(text, pos ,dem);
			SetBGColor(0);
			for(int i=0; i <= length; i++){
				gotoxy(x+i,y); cout << " ";
			} 
			gotoxy(x,y);	cout << text;
			temp--;
		}
		if(key == ESC){
			check=false;
		}
	}	
}

void SearchMBScreen(char *s){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(45,22);	cout << s;
	gotoxy(45,23);	cout << "                                                      ";
	gotoxy(45,24);	cout << "      SO HIEU MAY BAY                                 ";
	gotoxy(45,25);	cout << "                                                      ";
	SetBGColor(0);
	gotoxy(70,24); cout << "                ";
	gotoxy(70,24);
}

void SearchCBScreen( char *s){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(45,22);	cout << s;
	gotoxy(45,23);	cout << "                                                      ";
	gotoxy(45,24);	cout << "       NHAP MA CHUYEN BAY                             ";
	gotoxy(45,25);	cout << "                                                      ";
	SetBGColor(0);
	gotoxy(71,24); cout << "                ";
	gotoxy(71,24);
}

void ThongTinMBScreen(char *title,MayBay *mb=NULL){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(40,22);	cout << title;
	gotoxy(40,23);	cout << "                                                                ";
	gotoxy(40,24);	cout << "    LOAI MAY BAY                                                ";
	gotoxy(40,25);	cout << "                                                                ";
	gotoxy(40,26);	cout << "  SO HIEU MB                   SO DAY        SO DONG            ";
	gotoxy(40,27);	cout << "                                                                ";
	SetBGColor(0);
	gotoxy(57,24); cout << "                                        ";
	gotoxy(53,26); cout << "               ";	
	gotoxy(78,26); cout << "    ";		
	gotoxy(93,26); cout << "    ";		
	if(mb){
		gotoxy(57,24); cout<<mb->LoaiMB;
		gotoxy(53,26); 	cout<<mb->SoHieuMB;
		gotoxy(78,26); 	cout<<mb->SoDay;
		gotoxy(93,26);	cout<<mb->SoDong;
	}
}

void ThongTinCBScreen(char *title){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(39,22);	cout << title;
	gotoxy(39,23);	cout << "                                                                  ";
	gotoxy(39,24);	cout << " MA CB                  SHMB                  TG                  ";
	gotoxy(39,25);	cout << "                                                                  ";
	gotoxy(39,26);	cout << "        NOI DEN                                                   ";
	gotoxy(39,27);	cout << "                                                                  ";
	
	SetBGColor(0);
	gotoxy(46,24); cout << "                ";
	gotoxy(68,24); cout << "                ";	
	gotoxy(55,26); cout << "                                        ";		
	gotoxy(88,24); cout << "                ";
	SetColor(8);
	gotoxy(88,24); cout << "26/07/2019 16:15";
	SetColor(TXT_CLR);	
}

int Search_MB(ListMayBay &rootMB, char *SHMB){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, SHMB) == 0) return i;
	}
	return -1;
}

MayBay *Return_MB(ListMayBay &rootMB, char *SHMB){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, SHMB) == 0) return rootMB.DSMayBay[i];
	}
	return NULL;
}

bool Delete_MB(ListMayBay &rootMB, int i){
	int j;
	if(i < 0 || i > rootMB.n || rootMB.n == 0) return false;
	for(int j=i+1; j<rootMB.n; j++)
		rootMB.DSMayBay[j-1] = rootMB.DSMayBay[j];
	rootMB.n--;
	
	for(int i = 0; i < rootMB.n; i++){
		cout << "\nMay bay thu: " << i;
		cout << "\nSo hieu MB: " << rootMB.DSMayBay[i]->SoHieuMB << "\t" << rootMB.DSMayBay[i]->LoaiMB << "\t" << rootMB.DSMayBay[i]->SoDay << "\t" << rootMB.DSMayBay[i]->SoDong << "\t" << endl;
	}
	
	getch();
	
	return true;
}

MayBay *NhapThongTinMB(bool &check){
	MayBay *mb;
	char *SoDong, *SoDay, *SHMB, *LoaiMB;
	LoaiMB = new char[40];
	SHMB = new char[15];
	SoDong = new char[5];
	SoDay = new char[5];
	LoaiMB = GetText(57, 24, 40, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	SHMB = GetText(53, 26, 15, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	SoDay = GetText(78, 26, 4, 1, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	SoDong = GetText(93, 26, 4, 1, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}	
						
	mb = new MayBay();
	strcpy(mb->LoaiMB, LoaiMB);
	strcpy(mb->SoHieuMB, SHMB);
	mb->SoDay = atoi(SoDay);
	mb->SoDong = atoi(SoDong);
	mb->LoaiMB[strlen(mb->LoaiMB)] = '\0';
	return mb;
}

//ham con
void MayBayController(ListMayBay &rootMB, ListHanhKhach &rootHK, ListCB &listCB) {
	bool change = false;
START:
	bool check=true;
	//initial x, y
	int x=5, y=6;		
	int SelectedItem = 0;
	char key;
	FormMayBay(rootMB);
	MayBay *mb;
	char *SHMB;
BEGIN:
	ChildScreen(rootMB, rootHK, listCB, x, y, SelectedItem, 0, ItemMayBay);
	DisplayTextItem(ItemMayBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
	while(check) {
		key = GetKey();
		switch(key) {
			case UP: {
				SelectedItem--;
				//ve lai menu
				if(SelectedItem < 0) SelectedItem=0;
				DisplayTextItem(ItemMayBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case DOWN: {
				SelectedItem++;
				if(SelectedItem > 2) SelectedItem=2;
				DisplayTextItem(ItemMayBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case ENTER: {
				if(SelectedItem == 0){
					ThongTinMBScreen("                 THEM MAY BAY VAO DANH SACH                     ");
					mb = NhapThongTinMB(check);
					if(!mb) goto START;
					change = InsertDSMB(rootMB, mb);
					clrscr();
					for(int i = 0; i < rootMB.n; i++){
						cout << "\nMay bay thu: " << i;
						cout << "\nSo hieu MB: " << rootMB.DSMayBay[i]->SoHieuMB << "\t" << rootMB.DSMayBay[i]->LoaiMB << "\t" << rootMB.DSMayBay[i]->SoDay << "\t" << rootMB.DSMayBay[i]->SoDong << "\t" << endl;
					}
					getch();	
					ClearScreen(35,21,115,28);
					goto START;
							
				}
				else if(SelectedItem == 1){
					SearchMBScreen("          NHAP SO HIEU MAY BAY CAN XOA                ");
					SHMB = new char[15];
					bool check = true;
					SHMB = GetText(70, 24, 15, 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					int delete_pos = Search_MB(rootMB, SHMB);
					if(delete_pos != -1){
						if(ConfirmChange(50,20," BAN CO CHAC MUON XOA?")){
							if(Delete_MB(rootMB, delete_pos )){
								change=true;
							}
							else{
								clrscr();
							}
						}
					}
					else{
						clrscr();
						cout << "not ok";
					}					
					getch();
					goto START;
				}	
				else if(SelectedItem == 2){
					SearchMBScreen("       NHAP SO HIEU MAY BAY CAN HIEU CHINH            ");
					SHMB = new char[15];
					bool check = true;
					SHMB = GetText(70, 24, 15, 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					mb = new MayBay();
					mb = Return_MB(rootMB, SHMB);
					if(mb != NULL){
						ClearScreen(35,21,115,28);
						ThongTinMBScreen("               THONG TIN MAY BAY HIEU CHINH                     ", mb);
						
					}
					else{
						clrscr();
						cout << "not ok";
					}
					getch();
						
				}		
			}
			case ESC: {
				if(change){
					if(ConfirmChange(50,20," BAN CO MUON THAY DOI ")){
						GhiFileMayBay(rootMB);	
					}
				}
				rootMB.n=0;
				DocFileMayBay(rootMB);
				check=false;
			}
		}
	}
}

bool NhapGio(Time &time, int x=wherex(), int y=wherey()){
	int kieu=0;
	setColor(0,8);
	gotoxy(x,y); time.gio<10?cout << "0" << time.gio : cout << time.gio;
	time.phut<10?cout <<"0" << time.phut:cout << "" << time.phut;
	setColor(0,15); gotoxy(x+2,y); cout << ":";	gotoxy(x+5,y);
	
	Time h;
	h.gio=nhapnt(3,x,y);
		if(h.gio==-1) return false;	//tuc la ham nhap nhap vao esc
		if(h.gio==-2) {
			h.gio=time.gio; //??????????????????????????
			gotoxy(x,y); time.gio<10?cout << "0" << time.gio:cout << time.gio;
		}
		if(h.gio==0) {
			gotoxy(x,y); cout << "0" << h.gio;
		}
	h.phut=nhapnt(4,x+3,y);	
		if(h.phut==-1) return false;
		if(h.phut==-2) {
			h.phut = time.phut;	//???????????????????????
			gotoxy(x+3,y); time.phut<10?cout << "0" << time.phut:cout << time.phut;
		}
		if(h.phut==0) {
			gotoxy(x+3,y); cout << "0" << h.phut;
		}
	//Luu gia tri cap nhat vao time
	if(h.phut!=-2) time.gio=h.gio;
	if(h.gio!=-2) time.phut=h.phut;
	return true;
}

ChuyenBay *NhapThongTinCB(bool &check){
	ChuyenBay *cb = new ChuyenBay();
	char *MaCB, *NoiDen, *TrangThai, *SoHieuMB;
	MaCB = new char[15];
	SoHieuMB = new char[15];
	NoiDen = new char[40];
	TrangThai = new char[4];
	Time time;


	
	MaCB = GetText(46, 24, 15, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	SoHieuMB = GetText(68, 24, 15, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	NhapNgay(time, 88, 24);
	NhapGio(time, 99,24);	
	NoiDen = GetText(55, 26, 39, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
			
	strcpy(cb->MaCB, MaCB) ;
	strcpy(cb->NoiDen, NoiDen) ;
	strcpy(cb->SoHieuMB, SoHieuMB) ;
	cb->NgayGioKH.ngay = time.ngay;
	cb->NgayGioKH.thang = time.thang;
	cb->NgayGioKH.nam = time.nam;
	cb->NgayGioKH.gio = time.gio;
	cb->NgayGioKH.phut = time.phut;
	strcpy(cb->TrangThai, "1");		//tao may bay nen trang thai phai la con ve
	cb->dsVe.n = 0;
	cb->dsVe.dsVe = NULL;		//set danh sach ve rong
	return cb;
}

bool DeleteCB(ListCB &listCB, char *MaCB){
	if(listCB.head == NULL) return false;
	if( strcmp(listCB.head->chuyenBay->MaCB, MaCB) == 0 ){
		NodeCB *p = listCB.head;
		listCB.head = listCB.head ->pNext;
		delete p;
		return true;
	}
	NodeCB *pos = listCB.head;		//vi tri node truoc
	for(NodeCB *p = listCB.head; p != NULL; p = p->pNext){
		cout << "\nMa Chuyen bay : " << p->chuyenBay->MaCB;
		if( strcmp(MaCB, p->chuyenBay->MaCB) == 0 ){
			NodeCB *k = pos ->pNext;
			pos ->pNext = k ->pNext;
			delete k;

			return true;
		}
		pos = p;
	}
	return false;
}

void ChuyenBayController(ListMayBay &rootMB, ListHanhKhach &rootHK, ListCB &listCB) {
	bool change=false;
START:
	bool check=true;
	//initial x, y
	int x=5, y=10;		
	int SelectedItem = 0;
	char key;
	FormChuyenBay(listCB);
	ChuyenBay *cb;
BEGIN:
	
	ChildScreen(rootMB, rootHK, listCB, x, y, SelectedItem, 1, ItemChuyenBay);
	DisplayTextItem(ItemChuyenBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
	while(check) {
		key = GetKey();
		switch(key) {
			case UP: {
				SelectedItem--;
				//ve lai menu
				if(SelectedItem < 0) SelectedItem=0;
				DisplayTextItem(ItemChuyenBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case DOWN: {
				SelectedItem++;
				if(SelectedItem > 2) SelectedItem=2;
				DisplayTextItem(ItemChuyenBay[SelectedItem], x + 5, y + 4 + 4*SelectedItem, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case ENTER: {
				if(SelectedItem == 0){
					ThongTinCBScreen("                    THEM CHUYEN BAY MOI                           ");
					cb = NhapThongTinCB(check);
					if(!cb) goto START;
					NodeCB *node = CreateNodeCB(cb);
					if(listCB.head == NULL || (strcmp(listCB.head->chuyenBay->MaCB, cb->MaCB) > 0)){
						change=AddHeadCB(listCB, node);
					}
					else if((strcmp(listCB.tail->chuyenBay->MaCB, cb->MaCB) < 0)){
						change=AddTailCB(listCB, node);
					}
					else{
						change=AddAfterCB(listCB, node);
					}
					goto START;
							
				}
				else if(SelectedItem == 1){
					SearchCBScreen("                    HUY CHUYEN BAY                    ");
					char *MaCB = new char[15];
					MaCB = GetText(71, 24, 15 , 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					change = DeleteCB(listCB, MaCB);
					if(change){
						clrscr();
						cout << "OK";
					}else{
						clrscr();
						cout << " Not OK";
					}
					goto START;
				}	
				else if(SelectedItem == 2){
					
						
				}		
			}
			case ESC: {
				if(change){
					if(ConfirmChange(50,20," BAN CO MUON THAY DOI ")){
						GhiFileChuyenBay(listCB);
					}
				}
//				rootMB.n=0;
//				DocFileMayBay(rootMB);
				DocFileChuyenBay(listCB);
				check=false;
			}
		}
	}
}

void MenuController(ListMayBay &rootMB, ListHanhKhach &rootHK, ListCB &listCB)
{
	int choose=0;
BEGIN:
	MainScreen(choose, 4, 6);	
	char key;
	while(1) {
		key = GetKey();
		switch(key) {
			case UP: {
				choose--;
				//ve lai menu
				if(choose < 0) choose=0;
				DisplayText(MenuItem[choose], 5, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case DOWN: {
				choose++;
				if(choose > 2) choose=2;
				DisplayText(MenuItem[choose], 5, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case ENTER: {
				ClearScreen(0,0,120,28);
				if(choose == 0) MayBayController(rootMB, rootHK, listCB);
				if(choose == 1) ChuyenBayController(rootMB, rootHK, listCB);
				clrscr();
				goto BEGIN;
			}
			case ESC: {
				clrscr();
				exit(0);
			}
		}
	}
	
} 

void InsertVe(DanhSachVe &dsVe, VeHanhKhach ve){
	if(dsVe.n == 0){
		strcpy(dsVe.dsVe[0].SoCMND, ve.SoCMND);
		strcpy(dsVe.dsVe[0].SoVe, ve.SoVe);
		dsVe.n++;
	}
	else{
		strcpy(dsVe.dsVe[dsVe.n].SoCMND, ve.SoCMND);
		strcpy(dsVe.dsVe[dsVe.n].SoVe, ve.SoVe);
		dsVe.n++;
	}
}

int main()
{
	ListMayBay rootMB;
	rootMB.n = 0;
	ListHanhKhach rootHK;
	ListCB listCB;
	listCB.head = listCB.tail = NULL;
	DocFileMayBay(rootMB);
	DocFileHanhKhach(rootHK);
	DocFileChuyenBay(listCB);
		
	MenuController(rootMB, rootHK, listCB);	


//	ChuyenBay *cb = new ChuyenBay();
//
//	strcpy(cb->MaCB, "CB01") ;
//	strcpy(cb->NoiDen, "DA LAT") ;
//	strcpy(cb->SoHieuMB, "MB02") ;
//	cb->NgayGioKH.ngay = 27;
//	cb->NgayGioKH.thang = 7;
//	cb->NgayGioKH.nam = 2019;
//	cb->NgayGioKH.gio = 20;
//	cb->NgayGioKH.phut = 9;
//	cb->dsVe.n = 0;
//	cb->dsVe.dsVe = NULL;		//set danh sach ve rong
//	
//	
//	
//	//xu li danh sacsh ve
//	DanhSachVe DSVE;
//	DSVE.n = 0;
//	DSVE.dsVe = NULL;
//	DSVE.dsVe = new VeHanhKhach[3];
//	
//	VeHanhKhach ve1;
//	strcpy(ve1.SoCMND, "10000");
//	strcpy(ve1.SoVe, "A01");
//	InsertVe(DSVE, ve1);
//	
//	VeHanhKhach ve2;
//	strcpy(ve2.SoCMND, "20000");
//	strcpy(ve2.SoVe, "A02");
//	InsertVe(DSVE, ve2);
//	
//	VeHanhKhach ve3;
//	strcpy(ve3.SoCMND, "30000");
//	strcpy(ve3.SoVe, "A03");
//	InsertVe(DSVE, ve3);
//	
//	cb->dsVe = DSVE;
//	NodeCB *node = CreateNodeCB(cb);
//	AddHeadCB(listCB, node);
//	
//	for(int i = 0; i < DSVE.n; i++){
//		cout << "\nSTT: " << i;
//		cout << "\nSo CMND: " << DSVE.dsVe[i].SoCMND;
//		cout << "\nSo Ve: " << DSVE.dsVe[i].SoVe << endl;
//	}
//	
//
//	while(listCB.head!=NULL){
//		cout << "\n\n\n\nMa CB: " << listCB.head->chuyenBay->MaCB;
//		cout << "\nNoi den: " << listCB.head->chuyenBay->NoiDen;
//		cout << "\nSHMB: " << listCB.head->chuyenBay->SoHieuMB;
//		cout << "\nTrang thai: " << listCB.head->chuyenBay->TrangThai;
//		cout << "\nNgay gio khoi hanh: " << listCB.head->chuyenBay->NgayGioKH.ngay << "\t" << listCB.head->chuyenBay->NgayGioKH.thang << "\t" << listCB.head->chuyenBay->NgayGioKH.nam << "\t" << listCB.head->chuyenBay->NgayGioKH.gio << "\t" << listCB.head->chuyenBay->NgayGioKH.phut;
//		
//		cout << "\nSo luong ve: " << listCB.head->chuyenBay->dsVe.n;
////		cout << "\nDS ve: " << listCB.head->chuyenBay->dsVe.dsVe->SoCMND;
//		
//		if(listCB.head->chuyenBay->dsVe.dsVe != NULL){
//			cout << "\nCo ve";
//		}
//		else cout <<"\nKhong co ve";
//		
//		for(int i = 0; i < listCB.head->chuyenBay->dsVe.n; i++){
//			cout << "\n\tVe "<< i << ": " << listCB.head->chuyenBay->dsVe.dsVe[i].SoCMND << "\t" << listCB.head->chuyenBay->dsVe.dsVe[i].SoVe;
//		}
//		
//		listCB.head = listCB.head->pNext;
//	}

	return 0; 	
}

