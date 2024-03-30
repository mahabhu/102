# include "iGraphics.h"
#include "glut.h"
#include<bits/stdc++.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>


double absol(double g){
    if(g>=0) return g;
    return -g;
}

int mini(int a, int b){
    return (a<b)? a : b;
}

typedef struct{
    int mode;
    int cr;
    int cg;
    int cb;
    char name[20];
} page;

typedef struct{
    double a;
    double b;
    double c;
}stline;

typedef struct{
    double x;
    double y;
    double z;
} joint;

typedef struct{
    int n;
    int cr;
    int cg;
    int cb;
    int a[25];
} plane;

typedef struct{
    double x;
    double y;
}point;

typedef struct{
    int x;
    int y;
}intpoint;

typedef struct{
    point pos;
    point speed;
    int t;
    int on;
} dynamic;

stline constructline(double a, double b, point pq){
    stline re;
    re.a=a;
    re.b=b;
    re.c=-(a*pq.x+b*pq.y);
    if(re.a<0){
        re.a*=-1;
        re.b*=-1;
        re.c*=-1;
    }
    else if(re.a==0 && re.b<0){
        re.a*=-1;
        re.b*=-1;
        re.c*=-1;
    }
    return re;
}

point bricklen={60,30}, screen={1320,630};

joint p[32],q[32];
joint v[32]={ 0,           10,         0,
              10*cos(18*acos(0)/90),  10*sin(18*acos(0)/90), 0,
              10*cos(54*acos(0)/90), -10*sin(54*acos(0)/90), 0,
             -10*cos(54*acos(0)/90), -10*sin(54*acos(0)/90), 0,
             -10*cos(18*acos(0)/90),  10*sin(18*acos(0)/90), 0};;

double x, y, tomovex=0, tomovey=0;
int mouse=-1, n=32, np=12, mi, rty;
page allmode[12]={0,255, 62, 48,"Exit",
                 1, 23,107,239,"Help",
                 2, 23,156, 82,"Play",
                 3,231,126, 34,"About",
                 4,247,181, 41,"Settings",
                 5,143, 68,173,"Bests",
                 6, 0, 0, 0, "GamePlay",
                 7, 0, 0, 0, "Pause",
                 8, 0, 0, 0, "Congrats",
                 9, 0, 0, 0, "Gameover",
                10, 0, 0, 0, "Name",
                11, 0, 0, 0, "Intro"};
int currentmode, colorset;
joint brickcolor[6][2]={255, 62, 48,207,103,105,//red
                        57,123,265, 7,183,233,//blue
                        204,233,41,202,271,41,//yellow
                        61,233,137,17,289,105,//green
                        207,123,73,207,162,9,//orange
                        182,25,265,207,7,233};//purple

//variables
double r=10, paddlelen=180, paddlex=600, paddleref=600, paddleheight=20, paddlegap=0;
double timepassed=0, killpause=0, marks=0, scorefactor=1000, endgame=0, rightwall;
dynamic ball={100,100, 0, 8, 0}, perks[34]={0}, decoball[60];
int hardmode[2][30]={3,5,12,10,3,5,12,10,3,5,12,10,3,5,12,10,0,1,2,4,6,7,8,9,11,13,14,0,1,13,
                     0,1,2,3,4,5,6,14,7,8,9,10,11,12,13,14,1,2,5,7,8,9,10,11,12,13,14,5,13,14};
int hard;
int cx=screen.x/bricklen.x, cy=screen.y/bricklen.y, scored=0, heart=3, shooting=0, grab=0, previous, remaining;
point zero={0,0};

int block[(1320/60)+2][(630/30)+2]={0};
char** perkname; char** perkname2;

plane polygons[12]; bool polydrawn[12];
char newname[150]; int namesize, modenow;
char highscorer[11][150];  int highest[11];

void initdice();
void current(int);

void arrange(){
    char stock[150];
    for(int i=10; i>0; i--){
        if(highest[i-1]<highest[i]){
            for(int k=0; k<150; k++) stock[k]=highscorer[i][k];
            for(int k=0; k<150; k++) highscorer[i][k]=highscorer[i-1][k];
            for(int k=0; k<150; k++) highscorer[i-1][k]=stock[k];
            int replas=highest[i];
            highest[i]=highest[i-1];
            highest[i-1]=replas;
        }
    }
}


void interpretscore(char* filename){
    FILE* fp;
    fp=fopen(filename,"r");
    char s[200];
    for(int k=0; k<10; k++){
        fgets(s, 200, fp);
        fscanf(fp, "\n");
        highest[k]=0;
        for(int i=0; i<7; i++){
            int ten=1000000;
            for(int j=0; j<i; j++) ten/=10;
            char convh=s[i]; int ripl=(convh-'0'); ripl*=ten;
            highest[k]+=ripl;
        }
        int ripl=strlen(s);
        for(int i=0; i<=ripl-6; i++) highscorer[k][i]=s[i+7];
    }
    fclose(fp);
}


void savescore(char* filename){
    FILE* fp;
    fp=fopen(filename,"w");
    char s[200];
    for(int k=0; k<10; k++){
        for(int i=0; i<7; i++){
            int ten=1000000;
            for(int j=0; j<i; j++) ten/=10;
            int ripl=(highest[k]/ten); ripl%=10; char convh=('0'+ripl);
            s[i]=convh;
        }
        int ripl=strlen(highscorer[k]);
        for(int i=0; i<=ripl; i++) s[i+7]=highscorer[k][i];
        fputs(s,fp);
        fprintf(fp,"\n");
    }
    fclose(fp);
}

void interpret(char* filename){
    FILE* fp;
    fp=fopen(filename,"r");
    char s[1000]; remaining=0;
    fscanf(fp,"%s", s);
    int e=0;
    if(s[0]=='1') previous=1; else previous=0; e++;
    for(int i=2; i<=cx-1; i++){
        for(int j=8; j<=cy-4; j++){
            char cv=s[e];  e++;  int asd=(cv-'0'); block[i][j]=asd;
            remaining+=asd;
        }
    }
    double zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.pos.x=zxc;

    zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.pos.y=zxc;

    e++; zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.speed.x=zxc; ball.speed.x/=100; if(s[e-5]=='1') ball.speed.x*=-1;

    e++; zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.speed.y=zxc; ball.speed.y/=100; if(s[e-5]=='1') ball.speed.y*=-1;

    zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.t=zxc;

    zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } ball.on=zxc;

    zxc=0;  for(int i=0; i<2; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } r=zxc;

    zxc=0;  for(int i=0; i<6; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } timepassed=zxc/1000;

    zxc=0;  for(int i=0; i<7; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } marks=zxc;

    zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } shooting=zxc;

    zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } grab=zxc;

    zxc=0;  for(int i=0; i<6; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } scorefactor=zxc;

    zxc=0;  for(int i=0; i<3; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } paddlelen=zxc;

    zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } heart=zxc;

    for(int j=0; j<34; j++){
        zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].pos.x=zxc;

        zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].pos.y=zxc;

        e++; zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].speed.x=zxc; perks[j].speed.x/=100; if(s[e-5]=='1') perks[j].speed.x*=-1;

        e++; zxc=0;  for(int i=0; i<4; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].speed.y=zxc; perks[j].speed.y/=100; if(s[e-5]=='1') perks[j].speed.y*=-1;

        zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].t=zxc;

        zxc=0;  for(int i=0; i<1; i++){ char cv=s[e];  e++;  int asd=(cv-'0');  zxc*=10;  zxc+=asd; } perks[j].on=zxc;
    }
    fclose(fp);
    return;
}


void saveprogress(char* filename){
    char s[1000];
    int e=0, ten[8];
    ten[1]=1; for(int i=2; i<8; i++) ten[i]=10*ten[i-1];
    double tokn;
    if(previous==1) s[0]='1'; else s[0]='0'; e++;
    for(int i=2; i<=cx-1; i++){
        for(int j=8; j<=cy-4; j++){
            char cv=('0'+block[i][j]); s[e]=cv;  e++;
        }
    }
    int zxc=ball.pos.x;  for(int i=4; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=ball.pos.y;  for(int i=4; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    if(ball.speed.x<0) s[e]='1'; else s[e]='0';
    e++;
    tokn=ball.speed.x*100; tokn=absol(tokn); zxc=tokn;   for(int i=4; i>0; i--){
        int ten=1;
        for(int j=1; j<i; j++) ten*=10;
        ten=(zxc/ten);  ten%=10;  char cv=('0'+ten); s[e]=cv;  e++;}

    if(ball.speed.y<0) s[e]='1'; else s[e]='0';
    e++;
    tokn=ball.speed.y*100; tokn=absol(tokn); zxc=tokn;  for(int i=4; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=ball.t;  for(int i=1; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=ball.on;  for(int i=1; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=r;  for(int i=2; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    tokn=timepassed*1000; tokn=absol(tokn); zxc=tokn;   for(int i=6; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=marks;  for(int i=7; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=shooting;  for(int i=1; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=grab;  for(int i=1; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=scorefactor;  for(int i=6; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=paddlelen;  for(int i=3; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    zxc=heart;  for(int i=1; i>0; i--){
        int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

    for(int j=0; j<34; j++){
        zxc=perks[j].pos.x;  for(int i=4; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

        zxc=perks[j].pos.y;  for(int i=4; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

        if(perks[j].speed.x<0) s[e]='1'; else s[e]='0';
        e++;
        tokn=absol(perks[j].speed.x*100); zxc=tokn; for(int i=4; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

        if(perks[j].speed.y<0) s[e]='1'; else s[e]='0';
        e++;
        tokn=absol(perks[j].speed.y*100); zxc=tokn;  for(int i=4; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

        zxc=perks[j].t;  for(int i=1; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}

        zxc=perks[j].on;  for(int i=1; i>0; i--){
            int tn=(zxc/ten[i]);  tn%=10;  char cv=('0'+tn); s[e]=cv;  e++;}
    }
    s[e]='\0';
    FILE* fp;
    fp=fopen(filename,"w");
    fprintf(fp, "%s", s);
    fclose(fp);
    return;
}

double perpendicular(stline stl, point pr){
    double dis=absol((stl.a*pr.x + stl.b*pr.y + stl.c)/sqrt(stl.a*stl.a + stl.b*stl.b));
    return dis;
}

double pointgap(point pa, point pb){
    return sqrt((pa.x-pb.x)*(pa.x-pb.x)+(pa.y-pb.y)*(pa.y-pb.y));;
}

point intersection(stline sa, stline sb){
    point np={(sb.c*sa.b-sa.c*sb.b)/(sb.b*sa.a-sa.b*sb.a),(sb.c*sa.a-sa.c*sb.a)/(sa.b*sb.a-sb.b*sa.a)};
    return np;
}

void movesphere(double mx, double my){
    double a=mx, b=my;;
    double r=sqrt(a*a+b*b), k=72;
    for(int i=0; i<n; i++){
        double c=q[i].x, d=q[i].y, e=q[i].z;
        double x1=b*(b*c-a*d)/(r*r), y1=a*(a*d-b*c)/(r*r);
        double r1=sqrt((x1-c)*(x1-c)+(y1-d)*(y1-d));
        double r2=sqrt(r1*r1+e*e);
        double theta=acos(absol(r1)/absol(r2));
        double det=((c-x1)*a)+((d-y1)*b);;
              if(det< 0 && e>=0) theta=2*acos(0)-theta;
         else if(det< 0 && e< 0) theta=2*acos(0)+theta;
         else if(det>=0 && e< 0) theta=-theta;
         theta-=r/k;
         double x2=r2*cos(theta), y2=r2*sin(theta);
         double x3=x2*a/r, y3=x2*b/r;
         p[i].x=x1+x3;
         p[i].y=y1+y3;
         p[i].z=y2;
    }
}

char* int2char(double dbe){
    int i=1, db=absol(dbe), sig=0;
    if(db==0) return "0";
    char* ct; ct=(char*)malloc(12*sizeof(char));  ct[0]='\0';
    if(dbe<=-1){
        sig=1;
        ct[1]=ct[0];
        ct[0]='-';
        i++;
    }
    while(db>=1){
        for(int j=i; j>=(sig+1); j--) ct[j]=ct[j-1];
        int fg=db%10; db/=10;
        char tc=('0'+fg);
        ct[sig]=tc; i++;
    }
    if(ct[0]=='\0'){
        int crm=currentmode;
        initdice();
        if(crm<6) current(crm);
    }
    return ct;
}

void texts(char sto[], double x1, double y1){
    char rt[12]={'i','m','a','g','e','\\','0','.','b','m','p','\0'};
    int sze=strlen(sto);
    for(int i=0; i<sze; i++){
        rt[6]=sto[i];
        if(sto[i]!=' ') iShowBMP2(x1,y1,rt,0x00ffffff);
        x1+=30;
    }
    return;
}

void line(int n1, int n2){
    iLine(p[n1].x*6+200,p[n1].y*6+430,p[n2].x*6+200,p[n2].y*6+430);
    return;
}

void drawfunc(int t){
    double px[polygons[t].n], py[polygons[t].n];
    for(int i=0; i<polygons[t].n; i++){
        px[i]=6*p[polygons[t].a[i]].x+200;
        py[i]=6*p[polygons[t].a[i]].y+430;
    }
    iSetColor(polygons[t].cr,polygons[t].cg,polygons[t].cb);
    iFilledPolygon(px,py,polygons[t].n);
    return;
}

void drawpolygons(){
    for(int i=0; i<np; i++) polydrawn[i]=0;
    double minz=1000000;
    for(int i=0; i<np; i++){
        minz=1000000; int mini=0;
        for(int j=0; j<np; j++){
            if(!polydrawn[j]){
                double mnz=1000000;
                for(int k=0; k<polygons[j].n; k++) if(mnz>p[polygons[j].a[k]].z) mnz=p[polygons[j].a[k]].z;
                if(minz>mnz){
                    minz=mnz; mini=j;
                }
            }
        }
        drawfunc(mini);
        polydrawn[mini]=1;
    }
    return;
}

void background(){
    iSetColor(0,30,60);
    iFilledRectangle(0,0,screen.x, screen.y);
    if(currentmode==6)iSetColor(0,60,120);
    iFilledRectangle(0,screen.y-(bricklen.y*2),screen.x,2*bricklen.y);
    return;
}

void current(int m){
    for(int i=20; i<32; i++){
        if(p[i].z>=0 && i%6==m){
            x=200+p[i].x;
            y=430+p[i].y;
            mi=i; i=100;
            break;
        }
    }
    currentmode=m;
    mouse=3;
    rightwall=400;
    return;
}

void brickcollide(int x, int y, int t){
    if(block[x][y]>=1){
        block[x][y]--;
        saveprogress("txt\\test2.txt");//saveprogress-03
        double increment=(scorefactor*pow(2.7182818, -0.0153505*timepassed));
        if(hard==0) increment/=2;
        marks+=increment;
        if(t==3 || t==4){
            brickcollide(x+1,y,1);
            brickcollide(x-1,y,1);
            brickcollide(x,y+1,1);
            brickcollide(x,y-1,1);
        }
        scored++;
        remaining=0;
        for(int i=2; i<=cx-1; i++){
            for(int j=8; j<=cy-4; j++){
                remaining+=block[i][j];
            }
        }
        if(remaining==0){
            previous=0;
            currentmode=8;
            interpret("txt\\test1.txt");
            rightwall=0;
        }
        if(block[x][y]==0){
            double rf=pointgap(ball.speed,zero);
            double re=(rf*160+(r-0.5))/161;
            ball.speed.x=ball.speed.x*re/rf;
            ball.speed.y=ball.speed.y*re/rf;
        }
        if(scored%7==0 && scored>0 && scored<200 && block[x][y]==0){
            perks[scored/7].on=1;
            perks[scored/7].pos.x=(x-1)*bricklen.x+(0.5)*bricklen.x;
            perks[scored/7].pos.y=(y-1)*bricklen.y+(0.5)*bricklen.y;
            perks[scored/7].speed.x=ball.speed.x/2;
            perks[scored/7].speed.y=ball.speed.y/2;
            int tint=timepassed*6072;
            perks[scored/7].t=hardmode[hard][tint%30];

        }
    }
    return;
}


void iDraw()
{
    iClear();

    if(currentmode>=0 && currentmode<=5){
        iSetColor(0,30,60);
        iFilledRectangle(0, 0, 1320, 630);
        iSetColor(0,45,80);
        iFilledRectangle(0,230,400,400);
        for(int i=0; i<60; i++) iFilledCircle(decoball[i].pos.x,decoball[i].pos.y,8);

        double dr=10000000;
        for(int i=20; i<32; i++){
            if(p[i].z>0){
                if(sqrt(p[i].x*p[i].x+p[i].y*p[i].y)<=dr){
                    dr=sqrt(p[i].x*p[i].x+p[i].y*p[i].y); mi=i;
                }
            }
        }
        currentmode=mi%6;
        iSetColor(allmode[mi%6].cr,allmode[mi%6].cg,allmode[mi%6].cb);
        iFilledRectangle(0,0,400,230);
        iSetColor(0,45,80);
        double len1=strlen(allmode[mi%6].name);  len1/=2;
        iText(200-9*len1,115-4.5-20, allmode[mi%6].name,GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(0,0,0);;
        iFilledRectangle(0,200,400,30);
        iSetColor(150,150,150);
        iText(55,211,"Drag the dice or click to go to the desired page.",GLUT_BITMAP_HELVETICA_12);
        if(rty==mi) mouse=0;
        double minu=100000;
        for(int i=20; i<32; i++){
            if(p[i].z>0 && mi%6!=i%6){
                double newvelo=sqrt(p[i].x*p[i].x+p[i].y*p[i].y);
                double newx=150*p[i].x/newvelo, newy=150*p[i].y/newvelo;
                if(mouse<=0){
                    iSetColor(allmode[i%6].cr,allmode[i%6].cg,allmode[i%6].cb);
                    iFilledCircle(newx+200,newy+430,30);
                    iSetColor(0,45,80);
                    double len=strlen(allmode[i%6].name);
                    len/=2;
                    iText(newx+200-5*len,newy+430-2.5, allmode[i%6].name,GLUT_BITMAP_HELVETICA_10);
                }
                if(sqrt((p[i].x+200-x)*(p[i].x+200-x)+(p[i].y+430-y)*(p[i].y+430-y))<minu){
                    minu=sqrt((p[i].x+200-x)*(p[i].x+200-x)+(p[i].y+430-y)*(p[i].y+430-y));
                    rty=i;
                }
            }
        }
        if(mouse==0){
        double gh=sqrt(p[mi].x*p[mi].x+p[mi].y*p[mi].y);
        if(gh==0) gh=1;
            tomovex=-p[mi].x;
            tomovey=-p[mi].y;
        }
        else if(mouse==3){
        double gh=sqrt(p[rty].x*p[rty].x+p[rty].y*p[rty].y);
        if(gh==0) gh=1;
            tomovex=-p[rty].x;
            tomovey=-p[rty].y;
        }
        iSetColor(255,100,0);
        drawpolygons();
        if(currentmode==0){//0drawexit
            iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
            iText(664+60,375, "Are you sure you want you exit?", GLUT_BITMAP_HELVETICA_18);
            iFilledRectangle(644,270,108,60);
            iFilledRectangle(968,270,108,60);
            iSetColor(0,0,0);
            iText(676,295, "YES", GLUT_BITMAP_HELVETICA_18);
            iText(1009,295, "NO", GLUT_BITMAP_HELVETICA_18);
        }
        else if(currentmode==1){//1drawhelp
            iSetColor(81,144,243);
            iText(460,552,"INSTRUCTIONS:", GLUT_BITMAP_HELVETICA_18);
            iText(460,516,"Press keys <right arrow> and <left arrow> or move mouse to drag the paddle and protect the ball.", GLUT_BITMAP_HELVETICA_18);
            iText(460,486,"Break as many bricks as fast as possible, before loosing all the hearts.", GLUT_BITMAP_HELVETICA_18);
            iText(460,456,"Collect perks to gain various modifications.", GLUT_BITMAP_HELVETICA_18);
            iText(460,426,"Hit 'p' or F1 to pause or play during the game.", GLUT_BITMAP_HELVETICA_18);
            for(int i=0; i<15; i++){
                if(i>=9) iSetColor(255, 93, 72);
                else iSetColor(81,144,243);
                iShowBMP(460+172*(i/3), 140-40*(i%3), perkname[i]);
                iText(500+172*(i/3), 151-40*(i%3), perkname2[i],GLUT_BITMAP_HELVETICA_12);
            }
        }
        else if(currentmode==2){//2drawplay
            iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
            iFilledRectangle(644,285,108,60);
            iSetColor(0,0,0);
            iText(647,308, "NEW GAME", GLUT_BITMAP_HELVETICA_18);
            if(previous==1){
                iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
                iFilledRectangle(968,285,108,60);
                iSetColor(0,0,0);
                iText(972,308, "CONTINUE", GLUT_BITMAP_HELVETICA_18);
            }
        }
        else if(currentmode==5){//5drawbests
            iSetColor((2*allmode[currentmode].cr+255)/3,(2*allmode[currentmode].cg+255)/3,(2*allmode[currentmode].cb+255)/3);
            iText(500,510,"Rank",GLUT_BITMAP_HELVETICA_18);
            iText(600,510,"Score",GLUT_BITMAP_HELVETICA_18);
            iText(750,510,"Player",GLUT_BITMAP_HELVETICA_18);
            iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
            for(int i=0; i<10; i++){
                iText(500,460-i*40,int2char(i+1),GLUT_BITMAP_HELVETICA_18);
                if(highest[i]>0){
                    iText(600,460-i*40,int2char(highest[i]),GLUT_BITMAP_HELVETICA_18);
                    iText(750,460-i*40,highscorer[i],GLUT_BITMAP_HELVETICA_18);
                }
            }
        }
        else if(currentmode==4){//4draw
            for(int i=0; i<6; i++){
                if(i==colorset){
                    iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
                    iFilledRectangle(665+15+i*60,495-50,40,40);
                    iSetColor(0,0,0);
                    iFilledRectangle(667+15+i*60,497-50,36,36);
                }
                iSetColor(brickcolor[i][1].x,brickcolor[i][1].y,brickcolor[i][1].z);
                iFilledRectangle(670+15+i*60,500-50,15,15);
                iFilledRectangle(670+15+i*60+15,500+15-50,15,15);
                iSetColor(brickcolor[i][0].x,brickcolor[i][0].y,brickcolor[i][0].z);
                iFilledRectangle(670+15+i*60+15,500-50,15,15);
                iFilledRectangle(670+15+i*60,500+15-50,15,15);
            }
            iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
            iText(810,512,"Brick Colour", GLUT_BITMAP_HELVETICA_18);
            iText(755,303,"OFF", GLUT_BITMAP_HELVETICA_18);
            iText(935,303,"ON", GLUT_BITMAP_HELVETICA_18);
            iText(815,369,"Hard Mode", GLUT_BITMAP_HELVETICA_18);
            iText(790,226,"Reset Highscore", GLUT_BITMAP_HELVETICA_18);
            iFilledRectangle(806,134,108,60);
            iFilledCircle(835,312,25);
            iFilledCircle(885,312,25);
            iFilledRectangle(835,287,50,50);
            iSetColor(231,126, 34);
            if(hard==0) iFilledCircle(835,312,22);
            else iFilledCircle(885,312,22);
            iSetColor(0,0,0);
            iText(828,158,"RESET", GLUT_BITMAP_HELVETICA_18);
        }
        else if(currentmode==3){//3draw
            iSetColor(allmode[currentmode].cr,allmode[currentmode].cg,allmode[currentmode].cb);
            iText(500,494,"Created by: Mashroor Hasan Bhuiyan", GLUT_BITMAP_HELVETICA_18);
            iText(500,464,"Student ID: 1905069", GLUT_BITMAP_HELVETICA_18);
            iText(500,434,"CSE-102 Term Project", GLUT_BITMAP_HELVETICA_18);
            iText(500,404,"Level-1/Term-1, Dept. of Computer Science & Engineering", GLUT_BITMAP_HELVETICA_18);
            iText(500,374,"Bangladesh University of Engineering & Technology", GLUT_BITMAP_HELVETICA_18);
        }
    }
    else if(currentmode==6){//6draw
        background();
        iSetColor(255,255,255);
        texts(int2char(timepassed),15,screen.y-45);
        texts(int2char(marks),525,screen.y-45);
        for(int i=1; i<=heart; i++){
            iShowBMP2(1275-i*45, 585, "image\\heart.bmp", 0x00ffffff);
        }
        if(ball.t==0 || ball.t==1 || ball.t==5) iSetColor(200,200,200);
        if(ball.t==3 || ball.t==7) iSetColor(241,197,14);
        if(ball.t==2 || ball.t==6) iSetColor(44,150,223);
        if(ball.t==4 || ball.t==8) iSetColor(25,187,154);
        if(heart>=1 && ball.t!=9) iFilledCircle(ball.pos.x,ball.pos.y,r);
        int fg=paddlex, cx=(screen.x/bricklen.x), cy=(screen.x/bricklen.x);
        if(shooting==1){
            iSetColor(255, 62, 48);
            iFilledRectangle(paddlex-(paddlelen/2)+paddleheight-3,paddleheight/2,6,3*paddleheight/2);
            iFilledRectangle(paddlex+(paddlelen/2)-paddleheight-3,paddleheight/2,6,3*paddleheight/2);
        }
        iSetColor((((fg)%512<=(1024-(fg)%512)%512)? (fg)%512-1 : (1024-(fg)%512)%512-1), 255, (((fg)%512<=(1024-(fg)%512)%512)? 255-((fg)%512-1) : 255-((1024-(fg)%512)%512-1)));
        iFilledRectangle(paddlex-(paddlelen/2)+paddleheight,0,paddlelen-2*paddleheight,paddleheight);
        iFilledCircle(paddlex-(paddlelen/2)+paddleheight,paddleheight/2,paddleheight/2);
        iFilledCircle(paddlex+(paddlelen/2)-paddleheight,paddleheight/2,paddleheight/2);
        for(int i=1; i<=cx; i++){
            for(int j=1; j<=cy; j++){
                if(block[i][j]>0){
                    int rc=brickcolor[colorset][(i+j)%2].x;
                    int gc=brickcolor[colorset][(i+j)%2].y;
                    int bc=brickcolor[colorset][(i+j)%2].z;
                    if(block[i][j]>1){
                        if((i+j)%2==0){
                            rc=230-(block[i][j]-2)*70;
                            gc=230-(block[i][j]-2)*70;
                            bc=230-(block[i][j]-2)*70;
                        }
                        else{
                            rc=210-(block[i][j]-2)*70;
                            gc=210-(block[i][j]-2)*70;
                            bc=210-(block[i][j]-2)*70;
                        }
                    }
                    iSetColor((rc+255)/2,(gc+255)/2,(bc+255)/2);
                    iFilledRectangle((i-1)*bricklen.x,(j-1)*bricklen.y,bricklen.x,bricklen.y);
                    iSetColor(rc,gc,bc);
                    iFilledRectangle((i-1)*bricklen.x+3,(j-1)*bricklen.y+3,bricklen.x-6,bricklen.y-6);
                    iSetColor(0,0,0);
                }
            }
        }
        for(int i=1; i<=32; i++) if(perks[i].on) iShowBMP2(perks[i].pos.x-15, perks[i].pos.y-15, perkname[perks[i].t], 0x00ffffff);
    }
    else if(currentmode==7){//7drawpause
        background();
        iSetColor(0,60,120);
        iFilledRectangle(444,285,108,60);
        iFilledRectangle(768,285,108,60);
        iSetColor(0,90,180);
        iText(460,310, "RESUME", GLUT_BITMAP_HELVETICA_18);
        iText(792,310, "LEAVE", GLUT_BITMAP_HELVETICA_18);
        for(int i=0; i<15; i++){
            if(i>=9) iSetColor(255, 93, 72);
            else iSetColor(81,144,243);
            iShowBMP(60+252*(i/3), 140-40*(i%3), perkname[i]);
            iText(110+252*(i/3), 151-40*(i%3), perkname2[i],GLUT_BITMAP_HELVETICA_12);
        }
    }
    else if(currentmode==8){//8drawcongrat
        background();
        iSetColor(6,62,66);
        for(int i=0; i<60; i++) iFilledCircle(decoball[i].pos.x,decoball[i].pos.y,8);
        iShowBMP2(287,242,"image\\CONGRATULATIONS.bmp", 0x00ffffff);
    }
    else if(currentmode==9){//9drawgameover
        background();
        iSetColor(64,38,57);
        for(int i=0; i<60; i++) iFilledCircle(decoball[i].pos.x,decoball[i].pos.y,8);
        iShowBMP2(418,240,"image\\GAMEOVER.bmp", 0x00ffffff);
    }
    else if(currentmode==10){//10drawnamewrite
        background();
        iSetColor(62,68,55);
        for(int i=0; i<60; i++) iFilledCircle(decoball[i].pos.x,decoball[i].pos.y,8);
        iSetColor(0,0,0);
        iSetColor(150, 150, 52);
        double fo=namesize; fo/=2; fo*=9; fo=660-fo;
        iText(580,310,"Enter your name:",GLUT_BITMAP_HELVETICA_18);
        iSetColor(203, 203, 152);
        if(newname[0]=='\0') iText(fo,270,"_",GLUT_BITMAP_HELVETICA_18);
        else iText(fo,270,newname,GLUT_BITMAP_HELVETICA_18);
    }
    else if(currentmode==11){
        iSetColor(0,30,60);
        iFilledRectangle(0,0,screen.x,screen.y);
        iSetColor(0,45,80);
        for(int i=0; i<60; i++) iFilledCircle(decoball[i].pos.x,decoball[i].pos.y,8);
        iShowBMP2(388,241,"image\\intro.bmp",0x00ffffff);
    }
    iText(700,700,int2char(100*p[0].x),GLUT_BITMAP_9_BY_15);
}



void iMouseMove(int mx, int my)
{
    if(mouse==1 && currentmode>=0 && currentmode<=5) movesphere(mx-x,my-y);
}

void mouseposition(int mx, int my){
    if(currentmode==6){
        paddlex=mx;
        if(paddlex>screen.x-paddlelen/2) paddlex=screen.x-paddlelen/2;
        if(paddlex<paddlelen/2) paddlex=paddlelen/2;
    }
}


void iMouse(int button, int state, int mx, int my)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if(currentmode>=0 && currentmode<=5){
            x=mx;  y=my;
            for(int i=0; i<n; i++)  q[i]=p[i];
            if(sqrt((mx-200)*(mx-200)+(my-430)*(my-430))>101){
                if(mx<400 && my>230)mouse=3;
                else{//every other buttons
                    if(currentmode==0){//0controls
                        if(mx>=644 && mx<=752 && my>=270 && my<=330){
                            exit(0);
                        }
                        else if(mx>=968 && mx<=1076 && my>=270 && my<=330){
                            current(2);
                        }
                    }
                    else if(currentmode==2){//2controls
                        namesize=0;
                        newname[0]='\0';
                        if(mx>=644 && mx<=752 && my>=285 && my<=345){//from2to6
                            currentmode=6;
                            interpret("txt\\test1.txt");
                            previous=1;
                        }
                        if(previous==1 && mx>=968 && mx<=1076 && my>=285 && my<=345){
                            currentmode=6;
                            interpret("txt\\test2.txt");
                        }
                    }
                    else if(currentmode==4){//4controls
                        for(int i=0; i<6; i++){
                            int ml=650+15+(i*60), mr=680+15+(i*60);
                            if(mx>=ml && mx<=mr && my>=450 && my<=480){
                                colorset=i;
                            }
                        }
                        if(mx>=800 && mx<=920 && my>=284 && my<=337){
                            hard=1-hard;
                        }
                        if(mx>=800 && mx<=920 && my>=130 && my<=198){
                            interpretscore("txt\\freshscores.txt");
                            savescore("txt\\highscores.txt");
                            current(5);
                        }

                    }
                }
            }
            else{
                mouse=1;
            }
        }
        else if(currentmode==6){//6controls
            if(ball.t==0){
                ball.t=1;
                ball.speed.x=0;
                ball.speed.y=4.5;
                shooting=0;
                grab=0;
            }
            else if(ball.t>=5 && ball.t<=8){
                ball.t-=4;
            }
            else if(!perks[31].on && !perks[32].on){
                perks[31].on=1;
                perks[32].on=1;
                perks[31].pos.x=paddlex-paddlelen/2+paddleheight;
                perks[31].pos.y=paddleheight;
                perks[32].pos.x=paddlex+paddlelen/2-paddleheight;
                perks[32].pos.y=paddleheight;
                perks[31].speed.x=0;
                perks[31].speed.y=13;
                perks[32].speed.x=0;
                perks[32].speed.y=13;
                perks[31].t=15;
                perks[32].t=15;
            }
        }
        else if(currentmode==7){
            if(mx>=444 && mx<=552 && my>=285 && my<=345){//from7to6
                currentmode=6;
                iResumeTimer(0);
            }
            else if(mx>=768 && mx<=876 && my>=285 && my<=345){//from7to2
                currentmode=2;
                mi=20;
                previous=1;
                rightwall=400;
                for(int i=0; i<60; i++){
                    if(decoball[i].pos.x<400) decoball[i].pos.x=410;
                }
                saveprogress("txt\\test2.txt");
                iResumeTimer(0);
            }
        }
    }
    else{
        if(currentmode>=0 && currentmode<=5){
            if(mouse!=3){
                mouse=0;
                for(int i=0; i<n; i++){
                    q[i]=p[i];
                }
            }
        }
    }
}


void iKeyboard(unsigned char key)
{
    if(key == 'p')
    {
        if(currentmode==6){
        iPauseTimer(0);
        currentmode=7;
        rightwall=0;
        }
        else if(currentmode==7){
        iResumeTimer(0);
        currentmode=6;
        }
    }
    if(((key<='z' && key>='a')||(key<='Z' && key>='A')||(key<='9' && key>='0')||(key==32)) && currentmode==10 && namesize<=40)
	{
		newname[namesize+1]=newname[namesize];
		newname[namesize]=key;
		namesize++;
	}
	if(key==8 && currentmode==10 && namesize>0)
	{
		newname[namesize-1]=newname[namesize];
		namesize--;
	}
	if(key==13 && currentmode==10){
        rightwall=400;
        for(int i=0; i<60; i++){
            if(decoball[i].pos.x<400) decoball[i].pos.x=410;
        }
        current(5);
        interpretscore("txt\\highscores.txt");
        for(int k=0; k<150; k++) highscorer[10][k]=newname[k];
        highest[10]=marks;
        marks=0;
        arrange();
        savescore("txt\\highscores.txt");
        interpret("txt\\test1.txt");
	}
	if(key==32 && currentmode==6){
        if(ball.t==0){
            ball.t=1;
            ball.speed.x=0;
            ball.speed.y=6;
            shooting=0;
            grab=0;
        }
        else if(ball.t>=5 && ball.t<=8){
            ball.t-=4;
        }
        else if(!perks[31].on && !perks[32].on){
            perks[31].on=1;
            perks[32].on=1;
            perks[31].pos.x=paddlex-paddlelen/2+paddleheight;
            perks[31].pos.y=paddleheight;
            perks[32].pos.x=paddlex+paddlelen/2-paddleheight;
            perks[32].pos.y=paddleheight;
            perks[31].speed.x=0;
            perks[31].speed.y=13;
            perks[32].speed.x=0;
            perks[32].speed.y=13;
            perks[31].t=15;
            perks[32].t=15;
        }
	}
	if(key=='w' && currentmode==6){
        rightwall=0;
        previous=0;
        saveprogress("txt\\test2.txt");//saveprogress-02
        currentmode=8;
	}
}

void cornerbounce(point crnr){
    double velocity=pointgap(ball.speed,zero);
    point gh=intersection(constructline(ball.speed.x,ball.speed.y,crnr),constructline(ball.speed.y,-ball.speed.x,ball.pos));
    double gap=perpendicular(constructline(ball.speed.y,-ball.speed.x,ball.pos),crnr);
    gap=sqrt(r*r-gap*gap);
    gh.x-=(crnr.x+ball.speed.x*gap/velocity);
    gh.y-=(crnr.y+ball.speed.y*gap/velocity);
    point kh={gh.x*gh.x-gh.y*gh.y, 2*gh.x*gh.y};
    gh=kh;
    ball.speed.x*=-1;
    ball.speed.y*=-1;
    kh=ball.speed;
    ball.speed.x=(kh.x*gh.x+kh.y*gh.y);
    ball.speed.y=(kh.x*gh.y-kh.y*gh.x);
    double resultant=pointgap(ball.speed,zero);
    ball.speed.x*=(velocity/resultant);
    ball.speed.y*=(velocity/resultant);
    return;
}

void mover(){
    if((currentmode>=0 && currentmode<=5)||(currentmode>=8 && currentmode<=11)){
        if(mouse==0){
            double gh=sqrt(p[mi].x*p[mi].x+p[mi].y*p[mi].y);
            if(gh>=0.1){
                movesphere(tomovex,tomovey);
                for(int i=0; i<n; i++){
                    q[i]=p[i];
                }
            }
        }
        else if(mouse==3){
            double gh=sqrt(p[rty].x*p[rty].x+p[rty].y*p[rty].y);
            if(gh>=0.1){
            movesphere(tomovex,tomovey);
                for(int i=0; i<n; i++){
                    q[i]=p[i];
                }
            }
        }
        for(int i=0; i<60; i++){
            decoball[i].pos.x+=decoball[i].speed.x;
            decoball[i].pos.y+=decoball[i].speed.y;
            if(decoball[i].pos.x>=1320 || decoball[i].pos.x<=rightwall) decoball[i].speed.x*=-1;
            if(decoball[i].pos.y>= 630 || decoball[i].pos.y<=0) decoball[i].speed.y*=-1;
        }
    }
    else if(currentmode==6){
        if(heart==0){
            previous=0;
            currentmode=9;//from6to9-01
            rightwall=0;
        }
        glutPassiveMotionFunc(mouseposition);
        if(currentmode==6) timepassed+=0.013;
        if(ball.t==9){
            ball.on=0;
            shooting=0;
            grab=0;
            ball.speed.x=0;
            ball.speed.y=4.5;
            killpause+=0.0156;
            r=10;
            scorefactor=1000;
            paddlegap=0;
            paddlelen=180;
            if(killpause>=2 && heart>=1){
                ball.t=0;
                ball.on=1;
                killpause=0;
            }
            else if(killpause>=3 && heart==0){
                currentmode=9;//from6to9-02
            }
        }
        else{

            if(ball.t==0 || (ball.t>=5 && ball.t<=8) ){
                ball.pos.x=paddlex-paddlegap;
                ball.pos.y=paddleheight+2+r;
            }
            else if(ball.pos.x>screen.x || ball.pos.x<0 || ball.pos.y<0){
                ball.t=9;
                heart-=1;
            }
            else{
                stline boundary[4];
                point corner[4];
                if(absol(ball.speed.y)<=0.1){
                    (ball.speed.y>=0)? ball.speed.y=0.1 : ball.speed.y=-0.1;
                }
                point falseinip=ball.pos;
                point inip={ball.pos.x, ball.pos.y};
                ball.pos.x+=ball.speed.x;
                ball.pos.y+=ball.speed.y;
                falseinip.x=bricklen.x*floor(falseinip.x/bricklen.x);
                falseinip.y=bricklen.y*floor(falseinip.y/bricklen.y);
                while(falseinip.x==ball.pos.x || falseinip.y==ball.pos.y){
                    ball.pos.x+=ball.speed.x/pointgap(ball.speed,zero);
                    ball.pos.y+=ball.speed.y/pointgap(ball.speed,zero);
                }
                falseinip.x=(floor(ball.pos.x/bricklen.x)+1)*bricklen.x;
                falseinip.y=(floor(ball.pos.y/bricklen.y)+1)*bricklen.y;
                boundary[0]=constructline(0,1,falseinip);
                boundary[1]=constructline(1,0,falseinip);
                falseinip.x=(floor(ball.pos.x/bricklen.x))*bricklen.x;
                falseinip.y=(floor(ball.pos.y/bricklen.y))*bricklen.y;
                intpoint posp={floor(ball.pos.x/bricklen.x)+1,floor(ball.pos.y/bricklen.y)+1};
                boundary[2]=constructline(0,1,falseinip);
                boundary[3]=constructline(1,0,falseinip);

                corner[0]={(floor(ball.pos.x/bricklen.x)+1)*bricklen.x, (floor(ball.pos.y/bricklen.y)+1)*bricklen.y};
                corner[1]={(floor(ball.pos.x/bricklen.x)  )*bricklen.x, (floor(ball.pos.y/bricklen.y)+1)*bricklen.y};
                corner[2]={(floor(ball.pos.x/bricklen.x)+1)*bricklen.x, (floor(ball.pos.y/bricklen.y)  )*bricklen.y};
                corner[3]={(floor(ball.pos.x/bricklen.x)  )*bricklen.x, (floor(ball.pos.y/bricklen.y)  )*bricklen.y};
                double distance[8];
                stline ballmove=constructline(ball.speed.y,-ball.speed.x,ball.pos);

                for(int i=0; i<4; i++){
                    distance[i]=perpendicular(boundary[i],ball.pos);
                    distance[i+4]=perpendicular(ballmove,corner[i]);
                }

                if((distance[0]<=r && block[posp.x][posp.y+1]) ||
                   (distance[1]<=r && block[posp.x+1][posp.y]) ||
                   (distance[2]<=r && block[posp.x][posp.y-1]) ||
                   (distance[3]<=r && block[posp.x-1][posp.y])){
                    if(distance[0]<=r && block[posp.x][posp.y+1]>0){
                        brickcollide(posp.x,posp.y+1, ball.t);
                        if(ball.t!=2 && ball.t!=4) ball.speed.y=-ball.speed.y;
                    }
                    if(distance[1]<=r && block[posp.x+1][posp.y]>0){
                        brickcollide(posp.x+1,posp.y, ball.t);
                        if(ball.t!=2 && ball.t!=4) ball.speed.x=-ball.speed.x;
                    }
                    if(distance[2]<=r && block[posp.x][posp.y-1]>0){
                        brickcollide(posp.x,posp.y-1, ball.t);
                        if(ball.t!=2 && ball.t!=4) ball.speed.y=-ball.speed.y;
                    }
                    if(distance[3]<=r && block[posp.x-1][posp.y]>0){
                        brickcollide(posp.x-1,posp.y, ball.t);
                        if(ball.t!=2 && ball.t!=4) ball.speed.x=-ball.speed.x;
                    }
                }
                else{
                    intpoint solid[4][3]={posp.x+1,posp.y, posp.x+1,posp.y+1, posp.x,posp.y+1,
                                       posp.x-1,posp.y, posp.x-1,posp.y+1, posp.x,posp.y+1,
                                       posp.x+1,posp.y, posp.x+1,posp.y-1, posp.x,posp.y-1,
                                       posp.x-1,posp.y, posp.x-1,posp.y-1, posp.x,posp.y-1};
                    for(int i=0; i<4; i++){
                        int chek=0;
                        if(block[solid[i][0].x][solid[i][0].y]>=1) chek++;
                        if(block[solid[i][1].x][solid[i][1].y]>=1) chek++;
                        if(block[solid[i][2].x][solid[i][2].y]>=1) chek++;
                        if(distance[i+4]<r &&
                            (pointgap(corner[i],ball.pos)<r ||
                            (corner[i].x*ball.speed.x+corner[i].y*ball.speed.y>=inip.x*ball.speed.x+inip.y*ball.speed.y && corner[i].x*ball.speed.x+corner[i].y*ball.speed.y<=ball.pos.x*ball.speed.x+ball.pos.y*ball.speed.y) ||
                            (corner[i].x*ball.speed.x+corner[i].y*ball.speed.y<=inip.x*ball.speed.x+inip.y*ball.speed.y && corner[i].x*ball.speed.x+corner[i].y*ball.speed.y>=ball.pos.x*ball.speed.x+ball.pos.y*ball.speed.y)) &&
                                (chek==1)){

                            for(int j=0; j<3; j++) brickcollide(solid[i][j].x,solid[i][j].y, ball.t);
                            if(ball.t!=2 && ball.t!=4) cornerbounce(corner[i]);
                            i=10;
                        }
                    }
                }

                if(ball.pos.x>=screen.x-r-2 || ball.pos.x<=r) ball.speed.x=-ball.speed.x;
                if(ball.pos.y>=screen.y-r-(2*bricklen.y)) ball.speed.y=-ball.speed.y;
                else if(ball.pos.y<=r+paddleheight){
                    if(ball.pos.x<=paddlex+paddlelen/2 && ball.pos.x>=paddlex-paddlelen/2){
                        double angle=(90-((ball.pos.x-paddlex)*30/paddlelen))*acos(0)/90;
                        paddlegap=paddlex-ball.pos.x;
                        point gh={ball.pos.x-r*cos(angle),ball.pos.y-r*sin(angle)};
                        cornerbounce(gh);
                        ball.pos.x+=(paddleheight+r+2-ball.pos.y)*ball.speed.x/ball.speed.y;
                        ball.pos.y=paddleheight+r+2;
                        if(grab==1) ball.t+=4;
                    }
                    else if(ball.pos.y<=-r*2){
                        ball.t=9;
                        heart-=1;
                        ball.on=0;
                    }
                    else{
                        heart-=1;
                        ball.t=9;
                        ball.on=0;
                    }
                }
            }
        }
        for(int i=1; i<=30; i++){
            if(perks[i].on){
                perks[i].pos.x+=perks[i].speed.x;
                perks[i].pos.y+=perks[i].speed.y;
                perks[i].speed.y-=0.1;
                if(perks[i].pos.y<0){
                    perks[i].on=0;
                    if(perks[i].pos.x<=paddlex+paddlelen/2 && perks[i].pos.x>=paddlex-paddlelen/2){
                        if(perks[i].t==1) shooting=1;
                        if(perks[i].t==0) (ball.t!=3 && ball.t!=4)? ball.t=2 : ball.t=4;//thru
                        if(perks[i].t==8) (ball.t!=2 && ball.t!=4)? ball.t=3 : ball.t=4;//fire
                        if(perks[i].t==2) grab=1;
                        if(perks[i].t==3) heart++;
                        if(perks[i].t==10){//kill
                            ball.t=9;
                            heart--;
                        }
                        if(perks[i].t==9) {//slow
                            double rf=pointgap(ball.speed,zero);
                            double re=(rf*2+0.5)/3;
                            ball.speed.x=ball.speed.x*re/rf;
                            ball.speed.y=ball.speed.y*re/rf;

                        }
                        if(perks[i].t==12) {//fast
                            double rf=pointgap(ball.speed,zero);
                            double re=(rf*2+(r-0.5))/3;
                            ball.speed.x=ball.speed.x*re/rf;
                            ball.speed.y=ball.speed.y*re/rf;
                        }
                        if(perks[i].t==5) {//expandpaddle
                            if(paddlelen<300) paddlelen+=60;
                        }
                        if(perks[i].t==13) {//shrinkpaddle
                            if(paddlelen>60) paddlelen-=60;
                        }
                        if(perks[i].t==7) {//mega
                            double fr=r;
                            r=15;
                            ball.speed.x*=(r/fr);
                            ball.speed.y*=(r/fr);
                        }
                        if(perks[i].t==11) {//shrink ball
                            double fr=r;
                            if(r>5) r-=5;
                            ball.speed.x*=(r/fr);
                            ball.speed.y*=(r/fr);
                        }
                        if(perks[i].t==4){//zap
                            for(int i=0; i<24; i++){
                                for(int j=0; j<23; j++){
                                    if(block[i][j]>1) block[i][j]=1;
                                }
                            }
                        }
                        if(perks[i].t==6) scorefactor*=2;
                        if(perks[i].t==14){//reset
                            shooting=0;
                            ball.t=1;
                            grab=0;
                            double rf=r;
                            r=10;
                            ball.speed.x*=(r/rf);
                            ball.speed.y*=(r/rf);
                            if(hard==1) scorefactor=1000;
                            paddlelen=180;
                        }
                    }
                }
                if(perks[i].pos.y>=555) perks[i].speed.y*=-1;
                if(perks[i].pos.x>1305) perks[i].speed.x*=-1;
                if(perks[i].pos.x<30) perks[i].speed.x*=-1;
            }
        }
        if(perks[31].on && shooting==1){
            perks[31].pos.x+=perks[31].speed.x;
            perks[31].pos.y+=perks[31].speed.y;
            if(perks[31].pos.y>=570){
                perks[31].on=0;
            }
            intpoint posi1={perks[31].pos.x/bricklen.x,perks[31].pos.y/bricklen.y};
            posi1.x++;
            posi1.y++;
            if(block[posi1.x][posi1.y]>0){
                brickcollide(posi1.x,posi1.y,1);
                perks[31].on=0;
            }
        }
        else{
            perks[31].pos.x=0;
            perks[31].pos.y=0;
            perks[31].speed.x=0;
            perks[31].speed.y=0;
            perks[31].on=0;
        }
        if(perks[32].on && shooting==1){
            perks[32].pos.x+=perks[32].speed.x;
            perks[32].pos.y+=perks[32].speed.y;
            if(perks[32].pos.y>=570){
                perks[32].on=0;
            }
            intpoint posi2={perks[32].pos.x/bricklen.x,perks[32].pos.y/bricklen.y};
            posi2.x++;
            posi2.y++;
            if(block[posi2.x][posi2.y]>0){
                brickcollide(posi2.x,posi2.y,1);
                perks[32].on=0;
            }
        }
        else{
            perks[32].pos.x=0;
            perks[32].pos.y=0;
            perks[32].speed.x=0;
            perks[32].speed.y=0;
            perks[32].on=0;
        }
    }
    if(currentmode==8 || currentmode==9){
        endgame+=0.013;
        if(endgame>=3){
            endgame=0;
            previous=0;
            namesize=0;
            newname[0]='\0';
            currentmode=10;//from8to10-01 from9to10-01
            rightwall=0;
            saveprogress("txt\\test2.txt");//saveprogress-04
        }
    }
    if(currentmode==11){
        endgame+=0.013;
        if(endgame>=3){
            endgame=0;
            initdice();
            rightwall=400;
            for(int i=0; i<60; i++){
                if(decoball[i].pos.x<400) decoball[i].pos.x=410;
            }//from10to2
        }
    }
}

void iSpecialKeyboard(unsigned char key)
{
    if(key==GLUT_KEY_LEFT && currentmode==6){
        if(paddlex>paddlelen/2+3) paddlex-=35;
	}
	if(key==GLUT_KEY_RIGHT && currentmode==6){
        if(paddlex<screen.x-paddlelen/2-3) paddlex+=35;
	}
	if(key == GLUT_KEY_F1)
    {
        if(currentmode==6){
        iPauseTimer(0);
        currentmode=7;
        rightwall=0;
        }
        else if(currentmode==7){
        iResumeTimer(0);
        currentmode=6;
        }
    }
}


void initdice(){
    for(int i=0; i<5; i++){
        p[i]=v[i];
        q[i]=v[i];
    }
    double sr=10*sqrt((4*sin(36*acos(0)/90)*sin(36*acos(0)/90))-(1/(4*sin(18*acos(0)/90)*sin(18*acos(0)/90)))+(1/sin(18*acos(0)/90))-1);
    double rr=(1+4*cos(36*acos(0)/90))/(4*cos(36*acos(0)/90));;
    double fr=0.5/sin(18*acos(0)/90);
    rr*=sr;
    for(int i=0; i<5; i++){
        p[i].z=rr;   p[i+5].z=rr-sr;
        p[i+5].x=fr*p[i].x;  p[i+5].y=fr*p[i].y;
    }
    for(int i=0; i<10; i++){
        p[i+10].x=-p[i].x;  p[i+10].y=-p[i].y;  p[i+10].z=-p[i].z;
    }
    p[20].x=0;  p[20].y=0;  p[20].z=rr;
    sr*=(0.5*tan(54*acos(0)/90)/cos(18*acos(0)/90));
    sr=rr-sr;
    rr=sqrt(rr*rr-sr*sr);
    double velocity=sqrt(p[1].x*p[1].x+p[1].y*p[1].y);
    for(int i=0; i<5; i++){
        p[i+21].z=sr;
        p[i+21].x=p[10+i].x*rr/velocity;
        p[i+21].y=p[10+i].y*rr/velocity;
    }
    for(int i=20; i<26; i++){
        p[i+6].x=-p[i].x;  p[i+6].y=-p[i].y;  p[i+6].z=-p[i].z;
    }
    for(int i=0; i<32; i++){
        q[i].x=p[i].x;  q[i].y=p[i].y;  q[i].z=p[i].z;
    }
    polygons[ 0]={5, 23,156, 82, 0, 1, 2, 3, 4};//green
    polygons[ 1]={5,255, 62, 48, 0, 1, 6,18, 5};//red
    polygons[ 2]={5, 23,107,239, 1, 2, 7,19, 6};//blue
    polygons[ 3]={5,231,126, 34, 2, 3, 8,15, 7};//orange
    polygons[ 4]={5,247,181, 41, 3, 4, 9,16, 8};//yellow
    polygons[ 5]={5,143, 68,173, 0, 4, 9,17, 5};//violet
    polygons[ 6]={5, 23,156, 82,10,11,12,13,14};//green
    polygons[ 7]={5,231,126, 34,12,13,18, 5,17};//orange
    polygons[ 8]={5,247,181, 41,13,14,19, 6,18};//yellow
    polygons[ 9]={5,143, 68,173,10,14,19, 7,15};//violet
    polygons[10]={5,255, 62, 48,10,11,16, 8,15};//red
    polygons[11]={5, 23,107,239,11,12,17, 9,16};//blue
    mi=20;   currentmode=2;;
    return;
}

int main()
{
    currentmode=11;
    endgame=0;
    perkname=(char**)malloc(16*sizeof(char*));
    for(int i=0; i<=16; i++){
        perkname[i]=(char*)malloc(30*sizeof(char));
    }
    for(int i=0; i<60; i++){
        decoball[i].pos.x=screen.x/2;//698;
        decoball[i].pos.y=screen.y/2;
        double angle=(i*6)*acos(0)/90;
        angle+=(0.7)*acos(0)/90;
        decoball[i].speed.x=7*cos(angle);
        decoball[i].speed.y=7*sin(angle);
    }
    perkname[0]="image\\Thru Ball.bmp"; perkname[5]="image\\Expand Paddle.bmp"; perkname[10]="image\\Kill Paddle.bmp";
    perkname[1]="image\\Shooting Paddle.bmp"; perkname[6]="image\\Double Score.bmp"; perkname[11]="image\\Shrink Ball.bmp";
    perkname[2]="image\\Grab Paddle.bmp"; perkname[7]="image\\Mega Ball.bmp"; perkname[12]="image\\Fast Ball.bmp";
    perkname[3]="image\\Extra Life.bmp"; perkname[8]="image\\Fire Ball.bmp"; perkname[13]="image\\Shrink Paddle.bmp";
    perkname[4]="image\\Zap Bricks.bmp"; perkname[9]="image\\Slow Ball.bmp"; perkname[14]="image\\Reset Perks.bmp";
    perkname[15]="image\\Bullet.bmp";

    perkname2=(char**)malloc(16*sizeof(char*));
    for(int i=0; i<=16; i++){
        perkname2[i]=(char*)malloc(30*sizeof(char));
    }
    perkname2[0]="Thru Ball"; perkname2[5]="Expand Paddle"; perkname2[10]="Kill Paddle";
    perkname2[1]="Shooting Paddle"; perkname2[6]="Double Score"; perkname2[11]="Shrink Ball";
    perkname2[2]="Grab Paddle"; perkname2[7]="Mega Ball"; perkname2[12]="Fast Ball";
    perkname2[3]="Extra Life"; perkname2[8]="Fire Ball"; perkname2[13]="Shrink Paddle";
    perkname2[4]="Zap Bricks"; perkname2[9]="Slow Ball"; perkname2[14]="Reset Perks";
    perkname2[15]="Bullet";


    interpret("txt\\test2.txt");
    if(previous==0) interpret("txt\\test1.txt");
    colorset=3;
    hard=0;
    rightwall=0;
    interpretscore("txt\\highscores.txt");
    iSetTimer(4,mover);
    iInitialize(screen.x, screen.y, "1905069_Dx-Ball");
    free(perkname);;
    free(perkname2);
    return 0;
}

