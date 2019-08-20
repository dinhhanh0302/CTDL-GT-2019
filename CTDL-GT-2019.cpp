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
#define TAB 9
#define SPACE 32
#define DELETE -83

#define BG_CLR 41
#define DARK_CLR 8
#define SELECTED_CLR 34
#define TXT_CLR 4

using namespace std;

const int MAX_PLANES = 300;
const int T_SLEEP = 1000;

map<string, int> ListSoLuongGhe;
char MenuItem[3][30] = {" DANH SACH MAY BAY     ", " THONG TIN CHUYEN BAY  ", " QUAN LI VE - THONG KE "};
char ItemMayBay[3][20] = {"  THEM MAY BAY    ", "  XOA MAY BAY     ", "  HIEU CHINH MB   "};
char ItemChuyenBay[3][20] = {"  THEM CHUYEN BAY ", "  XOA CHUYEN BAY  ", "  HIEU CHINH CB   "};
char ItemQuanLi[5][30] = {"    DAT VE MAY BAY     ", " DANH SACH HANH KHACH  ", " DANH SACH CHUYEN BAY  ", "  DANH SACH VE TRONG   ", "  SO LUOT THUC HIEN CB "};
//                                                                                   char MenuItem[3][30] = {" DANH SACH MAY BAY     ", " THONG TIN CHUYEN BAY  ", " QUAN LI VE - THONG KE "};	
int DayOfMonth[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int NamNhuan(int nam){
	return (nam%4==0 && nam%100!=0 || nam%400==0) ? 1 : 0;
}
char Alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct DateTime{
	int gio, phut, ngay, thang, nam;
};

struct VeHanhKhach {
	char SoVe[5]; 
	char SoCMND[10];
};

struct DanhSachVe {
	int n;
	VeHanhKhach *dsVe;
};

struct ChuyenBay {
	char MaCB[15];
	DateTime NgayGioKH;
	char NoiDen[40];
	char TrangThai[4];
	char SoHieuMB[15];
	DanhSachVe dsVe;	
};

struct MayBay {
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
	char Ten[10];
	int Phai;
};

struct ListHanhKhach
{
	HanhKhach hanhKhach;
	ListHanhKhach *pLeft, *pRight;
};

struct ThongKe{
	char *SHMB;
	int soluot=0;
};

typedef struct ListHanhKhach* ListHK;

void setColor(int bgColor,int txtColor){
    SetBGColor(bgColor);
    SetColor(txtColor);
}

//Xu li time
void TimeNow(DateTime &t){
	//tu 1/1/1970
	time_t now = time(0);
	tm *ltm = localtime(&now);
	t.ngay = ltm->tm_mday;
	t.thang = ltm->tm_mon+1;
	t.nam = ltm->tm_year+1900;
	t.gio = ltm->tm_hour;
	t.phut = ltm->tm_min;
}

void DisplayDateTime(DateTime time, int x, int y){
	gotoxy(x,y);
	(time.ngay<10) ? cout << "0" << time.ngay : cout << time.ngay;
	(time.thang<10) ? cout <<" 0" << time.thang : cout << " " << time.thang;
	cout << " " << time.nam;
	gotoxy(x+2,y); cout << "/";	gotoxy(x+5,y); cout << "/";
	gotoxy(x+11,y);	
	(time.gio<10) ? cout << "0" << time.gio : cout << time.gio;
	(time.phut<10) ? cout <<":0" << time.phut : cout << ":" << time.phut;
	cout << " ";
	setColor(BG_CLR, TXT_CLR);
}

int TongSoNgay(int dd, int mm, int yyyy){
	int T=0;
	for(int t=1; t<mm; t++){
		T+=DayOfMonth[t];
		if(t==2) T+=NamNhuan(yyyy);
	}
	T+=dd;
	return T;
}

int CompareDateTime(DateTime time1, DateTime time2){
	if(time1.nam < time2.nam) return 2;
	if(time1.nam > time2.nam) return 1;
	int TongTime2 = TongSoNgay(time2.ngay, time2.thang, time2.nam);
	int TongTime1 = TongSoNgay(time1.ngay, time1.thang, time1.nam);
	if(TongTime1 < TongTime2) return 2;
	else if(TongTime1 > TongTime2) return 1;
	else {		//ngay truyen vao bang ngay he thong
		if((60*time1.gio+time1.phut) < (60*time2.gio+time2.phut)) return 2;
		else if((60*time1.gio+time1.phut) > (60*time2.gio+time2.phut)) return 1;
		else return 0;
	}
}

/* XU LI VE NODE HANH KHACH */
//tao BST hanh khach
ListHK Tao_BST(HanhKhach hanhKhach) {
	ListHK node;
	node->hanhKhach = hanhKhach;
	node->pLeft = node->pRight = NULL;
	return node;
}

//them moi hanh khach
void InsertHK(ListHK &t, HanhKhach hanhkhach){
//	cout << "\nCMND: " << hanhkhach.SoCMND << "\t" << hanhkhach.Ho << "\t" << hanhkhach.Ten << "\t" <<hanhkhach.Phai;
	if(t==NULL){
		ListHanhKhach* p = new ListHanhKhach;
		p->hanhKhach = hanhkhach;
		p->pLeft = p->pRight = NULL;
		t = p;
	}
 else {
		if(strcmp(t->hanhKhach.SoCMND, hanhkhach.SoCMND) > 0) InsertHK(t->pLeft, hanhkhach);
		else InsertHK(t->pRight, hanhkhach);
	}
}

void Preorder (ListHK tree){ 
	if(tree != NULL) { 
	   	cout <<"\ndanh sach CMND: " << tree->hanhKhach.SoCMND << "\t" << tree->hanhKhach.Ho << "\t" << tree->hanhKhach.Ten << "\t" << tree->hanhKhach.Phai;	
	    Preorder(tree->pLeft);
	    Preorder (tree->pRight);
  	}
}

ListHK SearchHK(ListHK tree, char* CM){
	if(tree==NULL) return NULL;
	if(strcmp(tree->hanhKhach.SoCMND,CM)>0) SearchHK(tree->pLeft,CM);
	else if(strcmp(tree->hanhKhach.SoCMND,CM)<0)SearchHK(tree->pRight,CM);
	else return tree;
}

void InsertVe(DanhSachVe &dsVe, VeHanhKhach *ve){
	if(dsVe.n == 0){
		strcpy(dsVe.dsVe[0].SoCMND, ve->SoCMND);
		strcpy(dsVe.dsVe[0].SoVe, ve->SoVe);
		dsVe.n++;
	}
	else{
		strcpy(dsVe.dsVe[dsVe.n].SoCMND, ve->SoCMND);
		strcpy(dsVe.dsVe[dsVe.n].SoVe, ve->SoVe);
		dsVe.n++;
	}
	return ;
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

bool AddHeadCB(ListCB &list, NodeCB* node) {
   if (!list.head) //xet danh sach 
      list.head = list.tail = node;
   else {
      node->pNext = list.head; //sua lai node con
      list.head = node; //chinh con tro cua danh sach
   }
   return true;
}
 
bool AddTailCB(ListCB &list, NodeCB *node) {
   if (!list.head)
      list.head = list.tail = node;
   else {
      list.tail->pNext = node;
      list.tail = node;
   }
    return true;
}
 
 //test
bool AddAfterCB(ListCB &list, NodeCB *node){
	NodeCB *pos = list.head;		//vi tri node truoc
	for(NodeCB *p = list.head; p != NULL; p = p ->pNext) {
		if(strcmp(node->chuyenBay->MaCB, p->chuyenBay->MaCB) < 0) {
			NodeCB *g = pos ->pNext;
			node ->pNext = g;
			pos ->pNext = node;
		}
		pos = p;
	}
	return true;
}

void DocFileChuyenBay(ListCB &listCB) {
	string data;
	fstream f; 
	f.open("DSCHUYENBAY.txt", ios::in);
	if(f == NULL) return;
	int soluongve;
	while(!f.eof()) {
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
		if(soluongve != 0){
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
		}
		else {
			chuyenBay->dsVe.n = 0;
			chuyenBay->dsVe.dsVe = NULL;
		}
	
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
	while(!f.eof()) {
		MayBay *mb = new MayBay();
		getline(f,data);
		if(data.size() == 0) break;
		string SHMB = string(data) ;	//luu gia tri vao map
		strcpy(mb->SoHieuMB, data.c_str());
		getline(f,data);	strcpy(mb->LoaiMB, data.c_str());
		f >> mb->SoDay;
		f >> mb->SoDong;
		f.ignore();
		ListSoLuongGhe[SHMB] = mb->SoDay*mb->SoDong;
//		cout << "SHMB: " << SHMB << "\t" << ListSoLuongGhe[SHMB] <<endl;
		rootMB.DSMayBay[i] = mb;
		rootMB.n++;
		i++;
	}
	f.close();
}

void GhiFileMayBay(ListMayBay rootMB) {
	fstream f;
	f.open("DSMAYBAY.txt", ios::out);
	f << rootMB.n << endl;
	for(int i = 0; i < rootMB.n; i++) {
		f << rootMB.DSMayBay[i]->SoHieuMB << endl << rootMB.DSMayBay[i]->LoaiMB << endl << rootMB.DSMayBay[i]->SoDay << endl << rootMB.DSMayBay[i]->SoDong << endl;
	}
	f.close();
}

void GhiFileChuyenBay(ListCB listCB) {
	fstream f;
	f.open("DSCHUYENBAY.txt", ios::out);
	while(listCB.head!=NULL){
		f << listCB.head->chuyenBay->MaCB << endl << listCB.head->chuyenBay->NgayGioKH.ngay << endl;
		f << listCB.head->chuyenBay->NgayGioKH.thang << endl << listCB.head->chuyenBay->NgayGioKH.nam << endl;
		f << listCB.head->chuyenBay->NgayGioKH.gio << endl << listCB.head->chuyenBay->NgayGioKH.phut << endl;
		f << listCB.head->chuyenBay->NoiDen << endl;
		DateTime time;
		TimeNow(time);
		
		//xu li trang thai
		if(CompareDateTime(time, listCB.head->chuyenBay->NgayGioKH) == 1){
			f << "3" << endl;	//trang thai hoan tat
		}
		else if(listCB.head->chuyenBay->dsVe.n == ListSoLuongGhe[listCB.head->chuyenBay->SoHieuMB]){
			f << "2" << endl; //het ve
		}
		else {
			f << listCB.head->chuyenBay->TrangThai << endl;		//trang thai hien tai
		}
		f << listCB.head->chuyenBay->SoHieuMB << endl << listCB.head->chuyenBay->dsVe.n << endl;
		for(int i = 0; i < listCB.head->chuyenBay->dsVe.n; i++){
			f << listCB.head->chuyenBay->dsVe.dsVe[i].SoCMND << endl << listCB.head->chuyenBay->dsVe.dsVe[i].SoVe << endl;
		}
		listCB.head = listCB.head->pNext;
	}
	f.close();
}

//phai co &f de tranh loi ios_base is private
void GhiHanhKhach(ListHanhKhach *root, ofstream &f){
	if(root != NULL) {
		f << root->hanhKhach.SoCMND << endl << root->hanhKhach.Ho << endl << root->hanhKhach.Ten << endl << root->hanhKhach.Phai << endl;
		GhiHanhKhach(root->pLeft, f);
		GhiHanhKhach(root->pRight, f);
	}
}

void Ghi_NLR_HanhKhach(ListHanhKhach *root) {
	ofstream f;
	f.open("DSHANHKHACH.txt", ios_base::out);
	GhiHanhKhach(root, f);
	f.close();
}

void DocFileHanhKhach(ListHK &rootHK) {
	string data;
	fstream f; 
	f.open("DSHANHKHACH.txt", ios::in);
	if(f == NULL) return;
	while(!f.eof()) {
		HanhKhach hk;
		getline(f,data);
		if(data.size() == 0) break;
		strcpy(hk.SoCMND, data.c_str());
		getline(f,data);
		strcpy(hk.Ho, data.c_str());
		getline(f,data);
		strcpy(hk.Ten, data.c_str());
		f >> hk.Phai;
		f.ignore();
		InsertHK(rootHK, hk);		//them moi vao BST
	}
	f.close();
}

//Bat phím
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
		for(int j=y;j<=y1;j++)
			gotoxy(i,j); cout << " ";
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

void FormChuyenBay(ListCB list, char *NoiDen = NULL){
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
	
	if(NoiDen == NULL){
		for(NodeCB *node = list.head; node != NULL; node = node->pNext){
			gotoxy(37,8+i);	cout << node->chuyenBay->MaCB;
			gotoxy(48,8+i);	cout << node->chuyenBay->NgayGioKH.ngay << "/" << node->chuyenBay->NgayGioKH.thang << "/" << node->chuyenBay->NgayGioKH.nam << " " << node->chuyenBay->NgayGioKH.gio << ":" << node->chuyenBay->NgayGioKH.phut;
			gotoxy(68,8+i);	cout << node->chuyenBay->NoiDen;
			gotoxy(89,8+i);	cout << node->chuyenBay->TrangThai;	
			gotoxy(98,8+i);	cout << node->chuyenBay->SoHieuMB;
			i++;
		}
	}
	else {
		for(NodeCB *node = list.head; node != NULL; node = node->pNext){
			if(strcmp(node->chuyenBay->NoiDen, NoiDen) == 0){
				gotoxy(37,8+i);	cout << node->chuyenBay->MaCB;
				gotoxy(48,8+i);	cout << node->chuyenBay->NgayGioKH.ngay << "/" << node->chuyenBay->NgayGioKH.thang << "/" << node->chuyenBay->NgayGioKH.nam << " " << node->chuyenBay->NgayGioKH.gio << ":" << node->chuyenBay->NgayGioKH.phut;
				gotoxy(68,8+i);	cout << node->chuyenBay->NoiDen;
				gotoxy(89,8+i);	cout << node->chuyenBay->TrangThai;	
				gotoxy(98,8+i);	cout << node->chuyenBay->SoHieuMB;
				i++;
			}
		}
	}
}

void ChildScreen(int x, int y, int selectedItem, int choose, char ItemMayBay[3][20]) {	
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

void QuanLiScreen(int choose, int x, int y) {
	clrscr();
	ProjectName();
	for(int i = 0; i < 5; i++) {
		if(i == choose)
			DisplayText(ItemQuanLi[i], x, y + 4*i, TXT_CLR, BG_CLR);
		else 
			DisplayText(ItemQuanLi[i], x + 2, y + 4*i, TXT_CLR, BG_CLR);		
	}
	DisplayText(ItemQuanLi[choose], 4, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
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

char *GetText(int x, int y, int length, int type, bool &check, bool isValue=false, bool isSpace=false)
{
	//isValue kiem tra co gia tri cua text khong
	//type=0 => chu thuong khong co space => nhap ma
	//type=1 => nhap so
	char *text = new char[length];	// hung ket qua
	text[0] = '\0';
	int dem=0, temp=0;		//temp vi tri hien tai trong mang char, dem la vi tri lon nhat cua chuoi nhap vao
	char key;
	gotoxy(x,y);
	while(check){
		 key = GetKey();
		if(key == ENTER && strlen(text) > 0){
//			check=false;
			return text;
			
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
		if(isSpace==true && key == SPACE && (temp >= 0 && temp < length)){
			InsertDigit(text, ' ', temp,dem);
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
		if(key == TAB && isValue==true){
			return NULL;
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

void SearchCMNDScreen(){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(45,23);	cout << "                                                      ";
	gotoxy(45,24);	cout << "         NHAP SO CMND                                 ";
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

void ThongTinMBScreen(char *title){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(40,22);	cout << title;
	gotoxy(40,23);	cout << "                                                                ";
	gotoxy(40,24);	cout << "    LOAI MAY BAY                                                ";
	gotoxy(40,25);	cout << "                                                                ";
	gotoxy(40,26);	cout << "  SO HIEU MB                   SO DAY        SO DONG            ";
	gotoxy(40,27);	cout << "                                                                ";
	SetBGColor(0);
	gotoxy(57,24); cout << "                                         ";
	gotoxy(53,26); cout << "                ";	
	gotoxy(78,26); cout << "     ";		
	gotoxy(93,26); cout << "     ";		
}

void ThongTinCBScreen(char *title){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(39,22);	cout << title;
	gotoxy(39,23);	cout << "                                                                   ";
	gotoxy(39,24);	cout << " MA CB                  SHMB                  TG                   ";
	gotoxy(39,25);	cout << "                                                                   ";
	gotoxy(39,26);	cout << "        NOI DEN                                                    ";
	gotoxy(39,27);	cout << "                                                                   ";
	
	SetBGColor(0);
	gotoxy(46,24); cout << "                ";
	gotoxy(68,24); cout << "                ";	
	gotoxy(55,26); cout << "                                        ";		
	gotoxy(88,24); cout << "                ";
	SetColor(8);
//	gotoxy(88,24); cout << "26/07/2019 16:15";
	DateTime time;
	TimeNow(time);
	DisplayDateTime(time, 88, 24);
	SetColor(TXT_CLR);	
}

//tra ve vi tri
int Search_MB(ListMayBay &rootMB, char *SHMB){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, SHMB) == 0) return i;
	}
	return -1;
}

char *Search_LoaiMB(ListMayBay rootMB, char *SHMB){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, SHMB) == 0) return rootMB.DSMayBay[i]->LoaiMB;
	}
	return NULL;
}

//tra ve gia tri du lieu
MayBay *SearchMB(ListMayBay &rootMB, char *SHMB){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, SHMB) == 0) return rootMB.DSMayBay[i];
	}
	return NULL;
}

//tra ve vi tri
bool Update_MB(ListMayBay &rootMB, MayBay *mb){
	for(int i = 0; i < rootMB.n; i++){
		if(strcmp(rootMB.DSMayBay[i]->SoHieuMB, mb->SoHieuMB) == 0) {
			rootMB.DSMayBay[i]->SoDong = mb->SoDong;
			rootMB.DSMayBay[i]->SoDay = mb->SoDay;
			strcpy(rootMB.DSMayBay[i]->LoaiMB, mb->LoaiMB);
			return true;
		}
	}
	return false;
}

NodeCB *SearchCBBySHMB(ListCB listCB, char *SHMB){
	while(listCB.head!=NULL){
		if(strcmp(listCB.head->chuyenBay->SoHieuMB, SHMB) == 0){
			return listCB.head;
		}
		listCB.head = listCB.head->pNext;
	}
	return NULL;
}

bool Delete_MB(ListMayBay &rootMB, int i){
	int j;
	if(i < 0 || i > rootMB.n || rootMB.n == 0) return false;
	for(int j=i+1; j<rootMB.n; j++)
		rootMB.DSMayBay[j-1] = rootMB.DSMayBay[j];
	rootMB.n--;	
	return true;
}

void Notification(int x, int y, char *s){
	setColor(6, 4);
	gotoxy(x, y);	cout << s;
	Sleep(T_SLEEP);
	setColor(0, TXT_CLR);
	gotoxy(x, y);	cout << "                                                                     ";
}

MayBay *NhapThongTinMB(ListMayBay &rootMB, bool &check){
	MayBay *mb;
	char *SoDong, *SoDay, *SHMB, *LoaiMB;
	LoaiMB = new char[40];
	SHMB = new char[15];
	SoDong = new char[5];
	SoDay = new char[5];
	LoaiMB = GetText(57, 24, 40, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
SOHIEUMB:
	SHMB = GetText(53, 26, 15, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
		if(SearchMB(rootMB, SHMB)){
			SHMB = NULL;
			Notification(45,21, "    SO HIEU MAY BAY BI TRUNG! VUI LONG NHAP LAI      ");
			gotoxy(53,26);	cout<<"               ";
			goto SOHIEUMB;
		}
	SoDay = GetText(78, 26, 2, 1, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	SoDong = GetText(93, 26, 2, 1, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}				
	mb = new MayBay();
	strcpy(mb->LoaiMB, LoaiMB);
	strcpy(mb->SoHieuMB, SHMB);
	mb->SoDay = atoi(SoDay);
	mb->SoDong = atoi(SoDong);
	mb->LoaiMB[strlen(mb->LoaiMB)] = '\0';
	return mb;
}

MayBay *HieuChinhMB(MayBay *mb, bool &check){
	setColor(0, DARK_CLR);
	gotoxy(53,26); 	cout<<mb->SoHieuMB;
	setColor(0, TXT_CLR);
	gotoxy(57,24); cout<<mb->LoaiMB;
	gotoxy(78,26); 	cout<<mb->SoDay;
	gotoxy(93,26);	cout<<mb->SoDong;
	
	char *SoDong, *SoDay, *LoaiMB;
	LoaiMB = new char[40];
	SoDong = new char[5];
	SoDay = new char[5];
	
	LoaiMB = GetText(57, 24, 40, 0, check, true, true);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	
	if(strcmp(mb->LoaiMB, LoaiMB) != 0) 
		strcpy(mb->LoaiMB, LoaiMB);
		
	SoDay = GetText(78, 26, 2, 1, check, true);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	if(mb->SoDay != atoi(SoDay)) 
		mb->SoDay=atoi(SoDay);
		
	SoDong = GetText(93, 26, 2, 1, check, true);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}		
	if(mb->SoDong != atoi(SoDong))
		mb->SoDong=atoi(SoDong);
	return mb;
}

//ham con
void MayBayController(ListMayBay &rootMB, ListCB &listCB) {
	bool change = false;
START:
	bool check=true;
	//initial x, y
	int x=5, y=6;		
	int SelectedItem = 0;
	char key;
	ClearScreen(0,0,120,28);
	FormMayBay(rootMB);
	MayBay *mb;
	char *SHMB;
BEGIN:
	ChildScreen( x, y, SelectedItem, 0, ItemMayBay);
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
					mb = NhapThongTinMB(rootMB, check);
					if(!mb) goto START;
					change = InsertDSMB(rootMB, mb);
					ClearScreen(35,21,115,28);
					goto START;			
				}
				else if(SelectedItem == 1){
					SearchMBScreen("          NHAP SO HIEU MAY BAY CAN XOA                ");
					SHMB = new char[15];
					bool check = true;
				XOAMB:
					SHMB = GetText(70, 24, 15, 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					int delete_pos = Search_MB(rootMB, SHMB);
					if(delete_pos != -1){
						if(SearchCBBySHMB(listCB, SHMB) != NULL){
							SHMB = NULL;
							Notification(45,21, "   BAN KHONG THE XOA VI MAY BAY DA DUOC TAO CHUYEN   ");
							gotoxy(70,24);	cout<<"               ";
							goto XOAMB;
						} else if(ConfirmChange(50,20," BAN CO CHAC MUON XOA?")){
							change = Delete_MB(rootMB, delete_pos );
						}
					}
					else{
						SHMB = NULL;
						Notification(45,21, "    SO HIEU MAY BAY KHONG TON TAI TRONG HE THONG     ");
						gotoxy(70,24);	cout<<"               ";
						goto XOAMB;
					}
					goto START;
				}	
				else if(SelectedItem == 2){
					SearchMBScreen("       NHAP SO HIEU MAY BAY CAN HIEU CHINH            ");
					SHMB = new char[15];
				HIEUCHINHMB:
					bool check = true;
					SHMB = GetText(70, 24, 15, 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					mb = new MayBay();
					mb = SearchMB(rootMB, SHMB);
					if(mb != NULL){
						ClearScreen(35,21,115,28);
						ThongTinMBScreen("               THONG TIN MAY BAY HIEU CHINH                     ");
						mb = HieuChinhMB(mb, check);
						change = Update_MB(rootMB, mb);
						if(change){
							ListSoLuongGhe[mb->SoHieuMB] = mb->SoDong*mb->SoDay;
							Notification(45,21, "           HIEU CHINH MAY BAY THANH CONG             ");
						}
					}
					else{
						SHMB = NULL;
						Notification(45,21, "    SO HIEU MAY BAY KHONG TON TAI TRONG HE THONG     ");
						gotoxy(70,24);	cout<<"               ";
						goto HIEUCHINHMB;
					}
					goto START;
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

ChuyenBay *SearchMaCB(ListCB listCB, char *MaCB){
	while(listCB.head!=NULL){
		if(strcmp(listCB.head->chuyenBay->MaCB, MaCB) == 0){
			return listCB.head->chuyenBay;
		}
		listCB.head = listCB.head->pNext;
	}
	return NULL;
}

char *GetNumberOfTime(int x, int y, int length, int type, int max, bool &check, bool isValue=false){
	char *text = new char[length];	// hung ket qua
	text[0] = '\0';
	int dem=0, temp=0;		//temp vi tri hien tai trong mang char, dem la vi tri lon nhat cua chuoi nhap vao
	char key;
	gotoxy(x,y);
	while(check){
		key = GetKey();
		if(key == ENTER && strlen(text) > 0){
			return text;
		}
		if((key >= 48 && key <= 57) && (temp >= 0 && temp < length)){
			InsertDigit(text, key, temp,dem);
			SetBGColor(0);
			for(int i=0; i <= length; i++){
				gotoxy(x+i,y); cout << " ";
			} 
			gotoxy(x,y);	cout << text;
			if(strlen(text) == max){
				return text;
			}
		}
		if(key == TAB && isValue==true){
			return NULL;
		}
	}	
}

void GetDate(DateTime &time, int x, int y){
	bool check=true;
	char *ngay = new char[5];
	char *thang = new char[5];
	char *nam = new char[5];
DAY:
	setColor(0, TXT_CLR);
	setColor(0,15); gotoxy(x+2,y); cout << "/";	gotoxy(x+5,y); cout << "/";
	ngay = GetNumberOfTime(x, y, 2, 1, 2, check);
	int dd = atoi(ngay);
	if(dd < 1 || dd > 31){
		ngay = NULL;
		gotoxy(x,y);	cout << "  ";
		goto DAY;
	}
MONTH:
	setColor(0,15); gotoxy(x+2,y); cout << "/";	gotoxy(x+5,y); cout << "/";
	thang = GetNumberOfTime(x+3, y, 2, 1, 2, check);
	int mm = atoi(thang);
	if(mm < 1 || mm > 12){
		thang = NULL;
		gotoxy(x+3,y);	cout << "  ";
		goto MONTH;
	}
YEAR:
	setColor(0,15); gotoxy(x+2,y); cout << "/";	gotoxy(x+5,y); cout << "/";
	nam = GetNumberOfTime(x+6, y, 4, 1, 4, check);
	int yy = atoi(nam);
	if(yy < 2000 || yy > 2100){
		nam = NULL;
		gotoxy(x+6,y);	cout << "    ";
		goto YEAR;
	}
	if(NamNhuan(yy) == 1 && mm == 2 && dd > 29){ //&& mm == 2 && dd > 29
		Notification(45,21, " NAM NHUAN - THANG 2 TOI DA DUOC 29 NGAY! VUI LONG NHAP LAI ");
		gotoxy(x,y);	cout << "  ";
		goto DAY;
	}
	else if(NamNhuan(yy) == 0 && mm == 2 && dd > 28){
		Notification(45,21, "       THANG 2 TOI DA DUOC 28 NGAY! VUI LONG NHAP LAI       ");
		gotoxy(x,y);	cout << "  ";
		goto DAY;
	}
	else if(mm!= 2 && dd > DayOfMonth[mm] ){
		Notification(45,21, "     THOI GIAN BAN NHAP KHONG HOP LE! VUI LONG NHAP LAI     ");
		gotoxy(x,y);	cout << "  ";
		goto DAY;
	}
	time.ngay=atoi(ngay);
	time.thang=atoi(thang);
	time.nam=atoi(nam);
}

void GetTime(DateTime &time, int x, int y){
	bool check=true;
	char *gio = new char[5];
	char *phut = new char[5];
HOUR:
	gotoxy(x+2, y); cout << ":";
	gio = GetNumberOfTime(x, y, 2, 1, 2, check);
	int h = atoi(gio);
	if(h < 0 || h > 23){
		gio = NULL;
		gotoxy(x,y);	cout << "  ";
		goto HOUR;
	}
MIN:
	gotoxy(x+2, y); cout << ":";
	phut = GetNumberOfTime(x+3, y, 2, 1, 2, check);
	int min = atoi(phut);
	if(min < 0 || min > 59){
		phut = NULL;
		gotoxy(x+3,y);	cout << "  ";
		goto MIN;
	}
	time.gio=atoi(gio);
	time.phut=atoi(phut);
}

ChuyenBay *NhapThongTinCB(ListMayBay &rootMB, ListCB listCB, bool &check){
	ChuyenBay *cb = new ChuyenBay();
	char *MaCB, *NoiDen, *TrangThai, *SoHieuMB;
	MaCB = new char[15];
	SoHieuMB = new char[15];
	NoiDen = new char[40];
	TrangThai = new char[4];
	DateTime time;
MACB:
	MaCB = GetText(46, 24, 15, 0, check);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	if(SearchMaCB(listCB, MaCB) != NULL){
		SoHieuMB = NULL;
		Notification(45,21, "      MA CHUYEN BAY DA TON TAI TRONG HE THONG        ");
		gotoxy(46,24);	cout<<"               ";
		goto MACB;
	}
SOHIEUMB:
	SoHieuMB = GetText(68, 24, 15, 0, check);;
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
	if(SearchMB(rootMB, SoHieuMB) == NULL){
		SoHieuMB = NULL;
		Notification(45,21, "  SO HIEU MAY BAY KHONG TON TAI! VUI LONG NHAP LAI   ");
		gotoxy(68,24);	cout<<"               ";
		goto SOHIEUMB;
	}
	
	//hien thi thoi gian
TIME:
	TimeNow(time);
	gotoxy(88,24); cout<<"                 ";
	DisplayDateTime(time, 88,24);
	GetDate(time, 88, 24);
	GetTime(time, 99, 24);
	DateTime dt;
	TimeNow(dt);
	if(CompareDateTime(time, dt) == 2){
		gotoxy(88,24); cout<<"                 ";
		Notification(45,21, "   THOI GIAN NHAP KHONG HOP LE! VUI LONG NHAP LAI    ");
		goto TIME;
	}
	setColor(0, TXT_CLR);
	NoiDen = GetText(55, 26, 39, 0, check, false, true);
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

void FormHieuChinhCB(ChuyenBay *cb){
	setColor(25, 4);
	gotoxy(39,22);	cout << "                HIEU CHINH THONG TIN CHUYEN BAY                    ";
	gotoxy(39,23);	cout << "                                                                   ";
	gotoxy(39,24);	cout << " MA CB                  SHMB                  TG                   ";
	gotoxy(39,25);	cout << "                                                                   ";
	gotoxy(39,26);	cout << " NOI DEN                                           TRANG THAI      ";
	gotoxy(39,27);	cout << "                                                                   ";
	setColor(0, DARK_CLR);
	gotoxy(46,24); cout << "                ";	gotoxy(46,24); cout<<cb->MaCB;
	gotoxy(68,24); cout << "                ";	gotoxy(68,24); cout<<cb->SoHieuMB;
	setColor(0, TXT_CLR);
	gotoxy(48,26); cout << "                                        ";	gotoxy(48,26); cout<<cb->NoiDen;	
	gotoxy(101,26); cout << "    "; gotoxy(102,26); cout<<cb->TrangThai;
	gotoxy(88,24); cout << "                "; DisplayDateTime(cb->NgayGioKH, 88, 24);
}

ChuyenBay *HieuChinhCB(ChuyenBay *cb, bool &check){
	ClearScreen(35,21,115,28);
	FormHieuChinhCB(cb);
	char *NoiDen, *TrangThai;
	DateTime time;
	NoiDen = new char[40];
	TrangThai = new char[5];
	GetDate(time, 88, 24);
	GetTime(time, 99, 24);
	setColor(0, TXT_CLR);
	NoiDen = GetText(48, 26, 39, 0, check, true, true);
		if(!check) {ClearScreen(48,21,115,28); return NULL;}
TRANGTHAI:
	TrangThai = GetText(102, 26, 2, 1, check, true);
		if(!check) {ClearScreen(35,21,115,28); return NULL;}
		int tt = atoi(TrangThai);
		if(tt < 0 || tt > 3){
			TrangThai = NULL;
			gotoxy(101,26); cout<<"    ";
			goto TRANGTHAI;
		}
	strcpy(cb->NoiDen, NoiDen);
	strcpy(cb->TrangThai, TrangThai);
	cb->NgayGioKH.ngay=time.ngay;
	cb->NgayGioKH.thang=time.thang;
	cb->NgayGioKH.nam=time.nam;
	cb->NgayGioKH.gio=time.gio;
	cb->NgayGioKH.phut=time.phut;
	return cb;
}

bool UpdateCB(ListCB listCB, ChuyenBay *cb){
	while(listCB.head!=NULL){
		if (strcmp(listCB.head->chuyenBay->MaCB, cb->MaCB) == 0){
			listCB.head->chuyenBay = cb;
			return true;
		}
		listCB.head = listCB.head->pNext;
	}
	return false;
}

void ChuyenBayController(ListMayBay &rootMB, ListCB &listCB) {
	bool change=false;
START:
	bool check=true;
	//initial x, y
	int x=5, y=10;		
	int SelectedItem = 0;
	char key;
	ClearScreen(0,0,120,28);
	FormChuyenBay(listCB);
	ChuyenBay *cb;
BEGIN:
	ChildScreen(x, y, SelectedItem, 1, ItemChuyenBay);
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
					ThongTinCBScreen("                    THEM CHUYEN BAY MOI                            ");
					cb = NhapThongTinCB(rootMB, listCB, check);
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
				MACB:
					MaCB = GetText(71, 24, 15 , 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					if(SearchMaCB(listCB, MaCB) == NULL){
						MaCB = NULL;
						Notification(45,21, "     MA CHUYEN BAY KHONG TON TAI TRONG HE THONG      ");
						gotoxy(71,24);	cout<<"               ";
						goto MACB;
					}
					else {
						if(ConfirmChange(50,20," BAN CO CHAC MUON XOA?")){
							change = DeleteCB(listCB, MaCB);
						}
					}
					goto START;
				}	
				else if(SelectedItem == 2){
					SearchCBScreen("               HIEU CHINH CHUYEN BAY                  ");
					char *MaCB = new char[15];
				MACBHC:
					MaCB = GetText(71, 24, 15 , 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					ChuyenBay *cb = new ChuyenBay();
					cb = SearchMaCB(listCB, MaCB);
					if(cb == NULL){
						MaCB = NULL;
						Notification(45,21, "     MA CHUYEN BAY KHONG TON TAI TRONG HE THONG      ");
						gotoxy(71,24);	cout<<"               ";
						goto MACBHC;
					}
					else{
						ClearScreen(35,21,115,28);
						cb = HieuChinhCB(cb, check);		
						change = UpdateCB(listCB, cb);
					}
					goto START;	
				}		
			}
			case ESC: {
				if(change){
					if(ConfirmChange(50,20," BAN CO MUON THAY DOI ")){
						GhiFileChuyenBay(listCB);
						
					}
				}
				check=false;
			}
		}
	}
}

NodeCB *SearchCB(ListCB listCB, char *NoiDen){
	while(listCB.head!=NULL){
		if( (strcmp(listCB.head->chuyenBay->TrangThai, "1") == 0) &&  (strcmp(listCB.head->chuyenBay->NoiDen, NoiDen) == 0)){
			return listCB.head;
		}
		listCB.head = listCB.head->pNext;
	}
	return NULL;
}

NodeCB *SearchCB(ListCB listCB, char *NoiDen, char *MaCB){
	while(listCB.head!=NULL){
		if((strcmp(listCB.head->chuyenBay->TrangThai, "1") == 0) &&  (strcmp(listCB.head->chuyenBay->NoiDen, NoiDen) == 0) && (strcmp(listCB.head->chuyenBay->MaCB, MaCB) == 0)){
			return listCB.head;
		}
		listCB.head = listCB.head->pNext;
	}
	return NULL;
}

char *SearchNoiDen(bool &check){
	setColor(25, 4);
	gotoxy(45,23);	cout << "                                                         ";
	gotoxy(45,24);	cout << " NHAP NOI DEN                                            ";
	gotoxy(45,25);	cout << "                                                         ";
	SetBGColor(0);
	gotoxy(59,24); cout << "                                          ";
	char *NoiDen = new char[40];
	NoiDen = GetText(59, 24, 40 , 0, check,false, true);
	return NoiDen;
}

void FormNhapVe(){
	ClearScreen(35,21,115,28);
	setColor(25, 4);
	gotoxy(40,22);  cout << "                                                                ";
	gotoxy(40,23);  cout << "         BAN MUON DAT VE VOI MA CHUYEN BAY                      ";
	gotoxy(40,24);	cout << "                                                                ";
	gotoxy(40,25);	cout << " SO CMND             HO                        TEN              ";
	gotoxy(40,26);	cout << "                                                                ";
	gotoxy(40,27);	cout << " PHAI             ND                                            ";
	gotoxy(40,28);  cout << "                                                                ";
	SetBGColor(0);
	gotoxy(49,25); cout << "          ";
	gotoxy(64,25); cout << "                     ";	
	gotoxy(83,23); cout << "                ";	
	gotoxy(91,25); cout << "           ";		
	gotoxy(46,27); cout << "     ";
	gotoxy(61,27); cout << "                                         ";
}

void GetTicket(ListHK &rootHK, ListCB &listCB, char *NoiDen, bool &check1, HanhKhach &hk, char *&MaCB, bool &checkExist){
MACB:
	gotoxy(61, 27);	cout << NoiDen;
	MaCB = GetText(83, 23, 15 , 0, check1);
		if(!check1) {ClearScreen(35,21,115,28); return;}
	if(SearchCB(listCB, NoiDen, MaCB) == NULL){
		MaCB = NULL;
		Notification(45,21, "   MA CHUYEN BAY KHONG HOP LE! VUI LONG NHAP LAI     ");
		gotoxy(83,23);	cout << "                ";
		goto MACB;
	}
SOCMND:
	char *SoCMND = new char[10];
	char *Ho, *Ten, *Phai;
	SoCMND = GetText(49, 25, 9 , 1, check1);
		if(!check1) {ClearScreen(35,21,115,28); return;}
	ListHK item = NULL;
	item = SearchHK(rootHK, SoCMND);
	strcpy(hk.SoCMND, SoCMND);
	Ho = new char[20];
	Ten = new char[10];
	Phai = new char[4];
	if(item){
		gotoxy(49,25); cout << item->hanhKhach.SoCMND;
		gotoxy(64,25); cout << item->hanhKhach.Ho;	
		gotoxy(91,25); cout << item->hanhKhach.Ten;		
		gotoxy(46,27);	(item->hanhKhach.Phai == 0) ? cout<<"Nam" : cout<<"Nu";
	}
	else checkExist=false;
	Ho = GetText(64, 25, 20 , 0, check1, checkExist);
		if(!check1) {ClearScreen(35,21,115,28); return;}
		(Ho == NULL) ? strcpy(hk.Ho, item->hanhKhach.Ho) : strcpy(hk.Ho, Ho);
	Ten = GetText(91, 25, 10 , 0, check1, checkExist);
		if(!check1) {ClearScreen(35,21,115,28); return;}
		(Ten != NULL) ? strcpy(hk.Ten, Ten) : strcpy(hk.Ten, item->hanhKhach.Ten);
PHAI:
	Phai = GetText(46, 27, 2 , 1, check1, checkExist);
		if(!check1) {ClearScreen(35,21,115,28); return;}
		int gender = atoi(Phai);
		if(gender != 0 && gender != 1){
			Phai = NULL;
			Notification(45,21, " GIOI TINH PHAI CO GIA TRI LA 0 = (NAM) HOAC 1 - (NU) ");
			gotoxy(46,27);	cout << "    ";
			goto PHAI;
		}
		(Phai != NULL) ? hk.Phai=gender : (hk.Phai=item->hanhKhach.Phai);
	return;
}

char *ChuyenSoVe(int soDay, int soDong){
	char *SoVe = new char[5];	//hung so day
	SoVe[0] = Alphabet[soDay];
	SoVe[1] = '\0';
	char *b = new char[3];	//hung so dong
	if(soDong < 10){
		strcat(SoVe, "0");
		strcat(SoVe, itoa(soDong, b, 10));
	}
	else{
		strcat(SoVe, itoa(soDong, b, 10));
	}
	return SoVe;
}

bool CheckTicket(NodeCB *nodeCB, char *SoVe){
	for(int i = 0; i < nodeCB->chuyenBay->dsVe.n; i++ ){
		if(strcmp(nodeCB->chuyenBay->dsVe.dsVe[i].SoVe, SoVe) == 0) return true;
	}
	return false;
}

bool ExistTicket(MayBay *mb, char *SoVe){
	for(int i=0; i < mb->SoDay; i++){
		for(int j=0; j < mb->SoDong; j++){
			if(strcmp(ChuyenSoVe(i, j), SoVe) == 0) return true;
		}
	}
	return false;
}

NodeCB *CheckCMND(ListCB &listCB, char *SoCMND){
	NodeCB *node = listCB.head;
	while(node != NULL){
		for(int i=0; i < node->chuyenBay->dsVe.n; i++){
			if(strcmp(node->chuyenBay->dsVe.dsVe[i].SoCMND, SoCMND) == 0) {
				return node;
			}	
		}
		node = node->pNext;
	}
	return NULL;
}

bool DeleteVe(ListCB &listCB, char *SoCMND){
	NodeCB *node = listCB.head;
	while(node != NULL){
		for(int i=0; i < node->chuyenBay->dsVe.n; i++){
			if(strcmp(node->chuyenBay->dsVe.dsVe[i].SoCMND, SoCMND) == 0) {
				for(int j = i+1; j < node->chuyenBay->dsVe.n; j++){
					node->chuyenBay->dsVe.dsVe[j-1] = node->chuyenBay->dsVe.dsVe[j];
				}
				node->chuyenBay->dsVe.n--;
				return true;
			}
		}
		node = node->pNext;
	}
	return false;
}

void FormDatVe(ListMayBay &rootMB, ListCB &listCB, ListHK &rootHK, NodeCB *nodeCB, bool &check, char *SoCMND){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                        THONG TIN CHUYEN BAY                                ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(40,5);
	int SLGhe = ListSoLuongGhe[nodeCB->chuyenBay->SoHieuMB];
	cout << "Tong so luong ve: " << SLGhe << endl ;

	//Tim may bay de liet ke so ghe theo dong va day
	MayBay *mb = new MayBay();
	mb = SearchMB(rootMB, nodeCB->chuyenBay->SoHieuMB);

	for(int i=0; i < mb->SoDay; i++){
		gotoxy(40,6+i);
		for(int j=0; j < mb->SoDong; j++){
			if(CheckTicket(nodeCB, ChuyenSoVe(i, j))){
				setColor(0, DARK_CLR);
				cout << ChuyenSoVe(i, j) << "\t";
				setColor(0, TXT_CLR);
			}
			else cout << ChuyenSoVe(i, j) << "\t";
		}
		cout << endl<<endl;
	}
	setColor(25, 4);
	gotoxy(40,22);  cout << "                                                              ";
	gotoxy(40,23);  cout << "      MOI BAN NHAP VI TRI GHE BAN MUON DAT                    ";
	gotoxy(40,24);	cout << "                                                              ";
	SetBGColor(0);
	gotoxy(83, 23); cout<< "            ";
	gotoxy(83, 23);
	char *SoVe = new char[5];
	VeHanhKhach *ve = new VeHanhKhach();
NHAPGHE:
	SoVe = GetText(83, 23, 5 , 0, check,false, true);
		if(!check) {ClearScreen(35,21,115,28); return;}
	//kiem tra ton tai so ve trong may bay
	if(ExistTicket(mb, SoVe)){
		if(!CheckTicket(nodeCB, SoVe)){
			DanhSachVe DSVE;
			strcpy(ve->SoCMND, SoCMND);
			strcpy(ve->SoVe, SoVe);
			NodeCB *node = listCB.head;
			
			while(node != NULL){
				if(strcmp(node->chuyenBay->MaCB, nodeCB->chuyenBay->MaCB) == 0) {
					
					if(node->chuyenBay->dsVe.dsVe == NULL){
						node->chuyenBay->dsVe.dsVe = new VeHanhKhach[ListSoLuongGhe[node->chuyenBay->SoHieuMB]];
					}
					InsertVe(node->chuyenBay->dsVe, ve);
					break;
				}				
				node = node->pNext;
			}
			//ghi file
			GhiFileChuyenBay(listCB);	
			Ghi_NLR_HanhKhach(rootHK);
			Notification(45,21, "                DAT VE THANH CONG                   ");
			ClearScreen(30,5,120,28);
			return;
		} else {
			SoVe = NULL;
			Notification(45,21, "    SO VE BAN NHAP DA DUOC DAT! VUI LONG NHAP LAI    ");
			gotoxy(83,23); cout<< "            ";
			goto NHAPGHE;
		}	
	} else {
		SoVe = NULL;
		Notification(45,21, "       SO VE KHONG PHU HOP! VUI LONG NHAP LAI        ");
		gotoxy(83,23); cout<< "            ";
		goto NHAPGHE;
	}
}

void DatVe(ListMayBay &listMB,ListHK &rootHK, ListCB &listCB, bool &check){
START:
	char key;
	FormChuyenBay(listCB);
	bool check1=true;
	while(check1){
		key = GetKey();
		if(key == ESC){
			check1 =false;
		}
		if(key == ENTER){
			char *NoiDen = new char[40];
			ClearScreen(35,21,115,28);
NOIDEN:
			HanhKhach hk;
			char *MaCB = new char[15];
			NoiDen = SearchNoiDen(check1);
				if(!check1) {ClearScreen(35,21,115,28); goto START;}
			if(SearchCB(listCB, NoiDen) == NULL){
				NoiDen = NULL;
				Notification(45,21, "    HIEN CHUA CO CHUYEN BAY DEN NOI BAN VUA NHAP     ");
				goto NOIDEN;
			} else{
				ClearScreen(34,0,120,28);
				FormChuyenBay(listCB, NoiDen);
				FormNhapVe();
				bool checkExist = true;
				GetTicket(rootHK, listCB, NoiDen, check1, hk, MaCB, checkExist);
				if(!checkExist)
					InsertHK(rootHK, hk);
			}
			NodeCB *nodeCB = SearchCB(listCB, NoiDen, MaCB);
			//Co ton tai ma chuyen bay tuong ung
			if(nodeCB != NULL) {
				ClearScreen(30,5,120,28);
				FormDatVe( listMB, listCB, rootHK,nodeCB, check1, hk.SoCMND);
				goto START;	
			}
		}	
		if(key == DELETE){
			char *SoCMND = new char[10];
			SearchCMNDScreen();
NHAPCMND:
			SoCMND = GetText(70, 24, 15, 1, check1);
				if(!check1) {ClearScreen(35,21,115,28); goto START;}
			if(CheckCMND(listCB, SoCMND) == NULL){
				SoCMND = NULL;
				Notification(45,21, " SO CMND BAN NHAP CHUA DUOC DAT VE! VUI LONG NHAP LAI ");
				gotoxy(70,24); cout<< "            ";
				goto NHAPCMND;
			}
			else {
				if(DeleteVe(listCB, SoCMND)){
					GhiFileChuyenBay(listCB);
					Notification(45,21, "                 HUY VE THANH CONG                    ");
					return;
				}
				else {
					SoCMND = NULL;
					Notification(45,21, "        CO LOI XAY RA! VUI LONG THAO TAC LAI          ");
					gotoxy(70,24); cout<< "            ";
					goto NHAPCMND;
				}
			}
		}
	}
}

//liet ke tung hang theo de in danh sach
HanhKhach *ItemHanhKhach(ListHK tree, char *SoCMND){
	if(tree == NULL) return NULL;
	else {
		if (strcmp(tree->hanhKhach.SoCMND, SoCMND) > 0){
			ItemHanhKhach(tree->pLeft, SoCMND);
		}
		else if (strcmp(tree->hanhKhach.SoCMND, SoCMND) < 0){
			ItemHanhKhach(tree->pRight, SoCMND);
		}
		else{
			return &tree->hanhKhach;
		}
	}
	
}


void InDSHK(ListCB listCB, ListHK rootHK, ChuyenBay *cb){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                     DANH SACH HANH KHACH THUOC CHUYEN BAY                  ";
	gotoxy(34,4);	cout << "  Ngay gio khoi hanh:                      Noi den:                         ";
	setColor(0, 4);
	gotoxy(34,6);	cout << "   STT       SO VE           SO CMND             HO TEN             PHAI    ";
	setColor(SELECTED_CLR, 4);
	gotoxy(93, 3); cout << cb->MaCB;
	DisplayDateTime(cb->NgayGioKH, 56, 4);
	setColor(SELECTED_CLR, 4);
	gotoxy(86, 4); cout << cb->NoiDen;
	int y = 10;
	//ke dung
	VerticalLine(34,5, 21, 179); VerticalLine(42,5, 21, 179);
	VerticalLine(57,5, 21, 179); 	VerticalLine(74,5, 21, 179);
	VerticalLine(98,5, 21, 179); 	VerticalLine(109,5, 21, 179);
	//ke ngang
	HorizontalLine(34,7,110,236); 
	HorizontalLine(34,5,110,236);
	HorizontalLine(34,20,110,236);
	setColor(0, TXT_CLR);
	NodeCB *node;
	while(listCB.head != NULL){
		if(strcmp(listCB.head->chuyenBay->MaCB, cb->MaCB) == 0) {
			node = listCB.head;
			break;
		}
		listCB.head = listCB.head->pNext;
	}
	for(int i=0; i < node->chuyenBay->dsVe.n; i++){
		HanhKhach *hk = new HanhKhach();
		hk = ItemHanhKhach(rootHK, node->chuyenBay->dsVe.dsVe[i].SoCMND);
		gotoxy(37, 8+i); cout<<i+1;
		gotoxy(47, 8+i); cout << node->chuyenBay->dsVe.dsVe[i].SoVe;
		gotoxy(61, 8+i); cout << node->chuyenBay->dsVe.dsVe[i].SoCMND;
		gotoxy(80, 8+i);  cout << hk->Ho << " " << hk->Ten;	
		gotoxy(102, 8+i); (hk->Phai == 0) ? cout<<"Nam" : cout<<"Nu";;		
	}
	getch();
}

//nhap thoi gian khoi hanh va noi den de lam in danh sach
void FormNhapNoiDen(){
	setColor(25, 4);
	gotoxy(45,22);	cout << "            NHAP THONG TIN CHUYEN BAY CAN IN               ";
	gotoxy(45,23);	cout << "                                                           ";
	gotoxy(45,24);	cout << " NHAP NOI DEN                                              ";
	gotoxy(45,25);	cout << "                                                           ";
	gotoxy(45,26);	cout << "       NHAP NGAY GIO KHOI HANH                             ";
	gotoxy(45,27);	cout << "                                                           ";
	SetBGColor(0);
	gotoxy(59,24); cout << "                                          ";
	gotoxy(77,26); cout << "                 ";
}

bool CheckNoiDenTime(ListCB list, char *NoiDen, DateTime time){
	for(NodeCB *node = list.head; node != NULL; node = node->pNext){
		if(strcmp(node->chuyenBay->NoiDen, NoiDen) == 0 && (CompareDateTime(time, node->chuyenBay->NgayGioKH) == 0)){
			return true;
		}
	}
	return false;
}

void InDSCB(ListCB list, char *NoiDen, DateTime time){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                        THONG TIN CHUYEN BAY                                ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(34,6);	cout << "    MA CB        THOI GIAN           NOI DEN     SO GHE TRONG   SO HIEU MB  ";
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
		if(strcmp(node->chuyenBay->NoiDen, NoiDen) == 0 && (CompareDateTime(time, node->chuyenBay->NgayGioKH) == 0)){
			gotoxy(37,8+i);	cout << node->chuyenBay->MaCB;
			gotoxy(48,8+i);	cout << node->chuyenBay->NgayGioKH.ngay << "/" << node->chuyenBay->NgayGioKH.thang << "/" << node->chuyenBay->NgayGioKH.nam << " " << node->chuyenBay->NgayGioKH.gio << ":" << node->chuyenBay->NgayGioKH.phut;
			gotoxy(68,8+i);	cout << node->chuyenBay->NoiDen;
			gotoxy(89,8+i);	cout << (ListSoLuongGhe[node->chuyenBay->SoHieuMB] - node->chuyenBay->dsVe.n);	
			gotoxy(98,8+i);	cout << node->chuyenBay->SoHieuMB;
			i++;
		}
	}
	getch();
}

NodeCB *GetNodeCB(ListCB listCB, char *MaCB){
	while(listCB.head!=NULL){
		if(strcmp(listCB.head->chuyenBay->MaCB, MaCB) == 0){
			return listCB.head;
		}
		listCB.head = listCB.head->pNext;
	}
	return NULL;
}

void InDSGhe(ListMayBay rootMB, ListCB listCB, char *MaCB ){
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                        THONG TIN CHUYEN BAY                                ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(40,5);
	
	NodeCB *nodeCB = GetNodeCB(listCB, MaCB);
	int SLGhe = ListSoLuongGhe[nodeCB->chuyenBay->SoHieuMB];
	cout << "Tong so luong ve: " << SLGhe << endl ;

	//Tim may bay de liet ke so ghe theo dong va day
	MayBay *mb = new MayBay();
	mb = SearchMB(rootMB, nodeCB->chuyenBay->SoHieuMB);

	for(int i=0; i < mb->SoDay; i++){
		gotoxy(40,6+i);
		for(int j=0; j < mb->SoDong; j++){
			if(CheckTicket(nodeCB, ChuyenSoVe(i, j))){
				setColor(0, DARK_CLR);
				cout << ChuyenSoVe(i, j) << "\t";
				setColor(0, TXT_CLR);
			}
			else cout << ChuyenSoVe(i, j) << "\t";
		}
		cout << endl<<endl;
	}
	setColor(25, 4);
}

void ThongKeSoLuotCB(ListMayBay rootMB, ListCB listCB){
	int n = rootMB.n;
	if(n == 0) {
		Notification(45,21,"   DANH SACH MAY BAY TRONG !");
		return;
	}
	ThongKe *A = new ThongKe[n+1];		
	for(int i=0; i<n; i++) A[i].SHMB=rootMB.DSMayBay[i]->SoHieuMB;
	DateTime time;
	TimeNow(time);
	for(NodeCB* p = listCB.head; p!=NULL; p = p->pNext) {
		if(strcmp(p->chuyenBay->TrangThai, "3") == 0) {
			for(int i=0; i<n; i++){
				if(strcmp(p->chuyenBay->SoHieuMB, A[i].SHMB)==0) {
					A[i].soluot++;
					break;
				}
			}
		}
	}
	//sap xep bubble sort
	ThongKe tam;
	for(int i=0; i<n-1; i++){
		for(int j=n; j>=i; j--){
			if(A[j].soluot>A[i].soluot){
				tam = A[j];
				A[j] = A[i];
				A[i] =tam;
			}
		}
	}
	setColor(SELECTED_CLR, 4);
	gotoxy(34,2);	cout << "                                                                            ";
	gotoxy(34,3);	cout << "                THONG KE SO LAN THUC HIEN CHUYEN BAY                        ";
	gotoxy(34,4);	cout << "                                                                            ";
	setColor(0, 4);
	gotoxy(34,6);	cout << "    SO HIEU MAY BAY               LOAI MAY BAY                  SO LUOT     ";
	int y = 10;
	//ke dung
	VerticalLine(34,5, 21, 179); 
	VerticalLine(65,5, 21, 179); 	
	VerticalLine(95,5, 21, 179); 	VerticalLine(109,5, 21, 179);
	//ke ngang
	HorizontalLine(34,7,110,236); 
	HorizontalLine(34,5,110,236);
	HorizontalLine(34,20,110,236);
	//in du lieu
	for(int i = 0; i < n; i++){	
		gotoxy(37,8+i); cout << A[i].SHMB;
		gotoxy(70,8+i); cout << Search_LoaiMB(rootMB, A[i].SHMB);
		gotoxy(98,8+i); cout << A[i].soluot;
	}
	getch();
}

void QuanLiController(ListMayBay &rootMB, ListHK &rootHK, ListCB &listCB){
	int choose=0;
START:
	bool check=true;
BEGIN:
	QuanLiScreen(choose, 4,6);
	char key;
	while(check) {
		key = GetKey();
		switch(key) {
			case UP: {
				choose--;
				//ve lai menu
				if(choose < 0) choose=0;
				DisplayText(ItemQuanLi[choose], 5, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case DOWN: {
				choose++;
				if(choose > 4) choose=4;
				DisplayText(ItemQuanLi[choose], 5, 6 + 4*choose, TXT_CLR, SELECTED_CLR);
				goto BEGIN;
			}
			case ENTER: {
				bool check = true;		//check item con
//				ClearScreen(0,0,120,28);
				if(choose == 0){
					DatVe(rootMB, rootHK, listCB, check);
				}
				if(choose == 1){
					FormChuyenBay(listCB);
					SearchCBScreen("        IN DANH SACH HANH KHACH THEO CHUYEN BAY       ");
					char *MaCB = new char[15];
				MACB:
					MaCB = GetText(71, 24, 15 , 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					ChuyenBay *cb = new ChuyenBay();
					cb = SearchMaCB(listCB, MaCB);
					if(cb == NULL){
						MaCB = NULL;
						Notification(45,21, "     MA CHUYEN BAY KHONG TON TAI TRONG HE THONG      ");
						gotoxy(71,24);	cout<<"               ";
						goto MACB;
					}
					else{
						ClearScreen(34,0,115,28);
						InDSHK(listCB, rootHK, cb);
					}
				}
				if(choose == 2){
					FormChuyenBay(listCB);
				NOIDEN:
					HanhKhach hk;
					FormNhapNoiDen();
					char *NoiDen = new char[40];
					NoiDen = GetText(59, 24, 40 , 0, check,false, true);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					if(SearchCB(listCB, NoiDen) == NULL){
						NoiDen = NULL;
						Notification(45,21, "    HIEN CHUA CO CHUYEN BAY DEN NOI BAN VUA NHAP     ");
						goto NOIDEN;
					} else{
						ClearScreen(34,0,120,28);
				NHAPTG:
						FormChuyenBay(listCB, NoiDen);
						FormNhapNoiDen();
						gotoxy(59,24); cout << NoiDen;
						DateTime time;
						TimeNow(time);
						GetDate(time, 77, 26);
						GetTime(time, 88, 26);
//						gotoxy(77,26); cout<<"                 ";
						if(CheckNoiDenTime(listCB, NoiDen, time)){
							InDSCB(listCB, NoiDen, time);
						}
						else{
							Notification(45,21, "       THOI GIAN BAN NHAP KHONG CO CHUYEN BAY        ");
							ClearScreen(34,20,120,28);
							goto NHAPTG;
						}
					}
					getch();
				}
				if(choose == 3){
					FormChuyenBay(listCB);
					SearchCBScreen("      IN DANH SACH VE CON TRONG THEO MA CHUYEN BAY    ");
					char *MaCB = new char[15];
				MACBTK:
					MaCB = GetText(71, 24, 15 , 0, check);
						if(!check) {ClearScreen(35,21,115,28); goto START;}
					ChuyenBay *cb = new ChuyenBay();
					cb = SearchMaCB(listCB, MaCB);
					if(cb == NULL){
						MaCB = NULL;
						Notification(45,21, "     MA CHUYEN BAY KHONG TON TAI TRONG HE THONG      ");
						gotoxy(71,24);	cout<<"               ";
						goto MACBTK;
					}
					else{
						ClearScreen(34,0,115,28);
						InDSGhe(rootMB, listCB, MaCB);
						getch();
					}
				}	
				if(choose == 4){
					ThongKeSoLuotCB(rootMB, listCB);
				}		
				ClearScreen(0,0,120,28);
				goto START;
			}
			case ESC: {
				return;
			}
		}
	}
} 

void MenuController(ListMayBay &rootMB, ListHK &rootHK, ListCB &listCB)
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
				if(choose == 0) MayBayController(rootMB, listCB);
				if(choose == 1) ChuyenBayController(rootMB, listCB);
				if(choose == 2) QuanLiController(rootMB, rootHK, listCB);
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

int main()
{
	ListMayBay rootMB;
	rootMB.n = 0;
	ListHK rootHK = NULL;
	ListCB listCB;
	listCB.head = listCB.tail = NULL;
	DocFileMayBay(rootMB);
	DocFileHanhKhach(rootHK);
	DocFileChuyenBay(listCB);
	MenuController(rootMB, rootHK, listCB);	
	return 0; 	
}
