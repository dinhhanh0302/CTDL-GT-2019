#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <mylib.h>
using namespace std;
struct node
{  int info ;
   struct node *next ;
};
typedef struct node *PTR;

const int so_item = 10;
const int dong =2;
const int cot = 20 ;
char thucdon [so_item][50] = {"1. Nhap day so            ",
			                  "2. Nap danh sach tu file ",
			                  "3. Tao day ngau nhien",
			                  "4. Liet ke danh sach    ",
			                  "5. Them phan tu vao danh sach",
							  "6. Xoa 1 phan tu theo gia tri",
							  "7. Xoa tat ca phan tu theo gia tri",
							  "8. Tim kiem so trong danh sach co thu tu",
							  "9. Tao day ngau nhien khong trung",
							  "10. Ket thuc chuong trinh     "};


int Menu (char td [so_item][50]){

  system("cls");   int chon =0;
  int i;
  for ( i=0; i< so_item ; i++)
  { gotoxy(cot, dong +i);
    cout << td[i];
  }
Nhaplai:
  gotoxy (cot, dong + so_item);
  printf("Ban chon 1 so (1.. %d  ) :    ", so_item );
  gotoxy (wherex()-4, wherey());
  cin >> chon;
  if (chon <1 || chon >so_item) goto Nhaplai;
  return chon;
}
void Insert_First(PTR &First, int x)
{
   PTR p;
   p = new node;
   p->info = x;
   p->next = First;
   First = p;
}
void Insert_Last(PTR &, int );
void Create_List( PTR &First)
{ int so;
  while (1)
  {
     printf("Nhap so nguyen (-9999 de dung) =");
     scanf("%d",&so);
     if (so==-9999) break;
     Insert_Last(First, so);
  }
  
}

void Insert_Last(PTR &First, int x)
{
   PTR p;
   p = new node;
   p->info = x;
   p->next = NULL;
   if (First==NULL)   First = p;
   else
   { PTR Last= First;
     for ( ; Last->next !=NULL ; Last=Last->next) ;
     Last->next=p;
   }
}
void ClearList(PTR &First)
{  
   PTR p;  
  while(First != NULL)
   {
      p = First;
      First = First->next;
      delete p;
   }
}

int LoadFile (char *tenfile , PTR &First) {
FILE *f; int so, n;
f=fopen(tenfile,"rt");
if (f==NULL)  return 0;
ClearList(First);
fscanf(f,"%d", &n);
for ( int i=0 ; i<n ; i++) {
   fscanf(f,"%d ", &so);
   Insert_Last(First, so);
   }
fclose (f);
return 1;
}
void LietKe(PTR First)
{ PTR p;
   int stt = 0;
   
   for (p = First ; p  != NULL; p=p->next)
   {   printf( "%6d", p->info);
       stt++;
       if (stt %10 ==0) printf ("\n");
   }
   getch();
}
PTR SearchOrder(PTR First, int x){
 PTR p;
 for (p=First ; p!=NULL && p->info < x; p=p->next);
 if (p!=NULL && p->info ==x) return p;
 return NULL ;
}
int Delete_All_Info(PTR &First,int x)
{  PTR p=First; int dem=0;  
  while  (First!=NULL && First->info ==x ) {
     p=First;
	 First = p->next ;
	 delete p ; dem++;
  }
  if (First==NULL) return dem;
 PTR q;   
for ( p=First; p->next!=NULL  ; ) 
   if (p->next->info == x ) {
		q=p->next; 
		p->next=q->next;
		delete q; dem++;
	}
	else p=p->next;
return dem;
}

int Delete_Info(PTR &First,int x)
{  PTR p=First;
  if (First==NULL ) return 0;
  if (First->info ==x ) {
     p=First;
	 First = p->next ;
	 delete p ; return 1;
  }
  PTR q;  
for ( p=First; p->next!=NULL && p->next->info != x; p=p->next) ;
if (p->next!= NULL ) {
		q=p->next; 
		p->next=q->next;
		delete q; return 1;
	}
     return 0;
}


int main () {
	PTR First=NULL; char tenfile[] = "d:/dayso.txt";
	int chon,i,x;  
    while  (1) {
        chon = Menu (thucdon);
        switch (chon) {
        
        case 1: 
		        Create_List( First);
				break;
		case 2: 
		        if (LoadFile (tenfile , First) ==0)
		           cout << "Khong doc file duoc" << endl;
		          
				break;
		case 3: 
		        //TaoDayNgauNhien(ds);
		    
				break;				
        case 4:
		        LietKe( First); 
		         break;
	    case 6: cout << "Nhap gia tri muon xoa :";
				cin >> x;
				if (Delete_Info ( First,  x)==0)
	  				cout << "Gia tri " << x << "khong co."<< endl;
		        else cout << "Da xoa thanh cong" << endl;
		        getch();
				break;			         
		case 7: cout << "Nhap gia tri muon xoa :";
				cin >> x;
				if (Delete_All_Info ( First,  x)==0)
	  				cout << "Gia tri " << x << "khong co."<< endl;
		        else cout << "Da xoa thanh cong" << endl;
		        getch();
				break;
		case 8: cout << "Nhap gia tri muon tim :";
				cin >> x;
				if (SearchOrder ( First,  x)==NULL)
	  				cout << "Gia tri " << x << "khong co."<< endl;
		        else cout << "Co " << x << "trong danh sach."<< endl;
		        getch();
				break;					
    }	 // case
  } // while 
    return 0;
}

