![](images/4ed8807d9f5776aafd7ba40f53db4f4fc83756fb1c18e858ab8c458f2bd9e9f9.jpg)

TRUONG DAI HOC KHOA HOC TU NHIEN TP.HCMKHOA CONG NGHE THONG TINMON: LAP TRINH HUONG DOI TUONG

## HUONG DAN DO AN TRO CHOI BANG QUA DUONG

TP.HCM, ngay 01 tháng 10 näm 2025

## MUC LUC

1 Gioi thieu ... 3   
2 Kich ban tro choi . .3   
3 Cac ki thuat hö tro' . .4   
3.1C6 dinh man hinh chinh. .4   
3.2 Thiét lap vi tri cho con tro man hinh. .4   
3.3 Ki thuat da tiéu trinh .. .5   
3.4 Cac lop trong tro choi. .6   
3.4.1L6p CPEOPLE. .6   
3.4.2L6p CVEHICLE.. .6   
3.4.3Lóp CANIMAL.. .7   
3.4.4Lop CGAME.. .8   
3.4.5So do lop tro choi.. .9   
3.5Doan ma gia minh hoa. .9   
4YEU CAU DO AN. .11   
4.1Cai dat chay duoc giong kich ban mo ta (3d). 11   
4.2 Xay dung thuc don cho tro choi khi via moi vao (1d) . 11   
4.3 Xir ly luu/tai tro choi (3d)... 11   
4.4 Xur ly tam dung cac toa xe (2d).. ..12   
4.5 Xir ly hieu ung khi va cham (0.5d) . .. 12   
4.6 Giao dien (0.5d) ... .12

## 1 Gioi thiéu

Trong phan do an nay ta sé phoi hop cac ki thuat, cáu truc du liéu co ban va kién thtrc lap trinh huong doi trong dé xay dung mot tro choi bang qua duong (road crossing).

Dé thuc hien duoc do an nay ta can cac kién thurc co ban nhu: xir ly tap tin, tiéu trinh, handle, cac cau truc du lieu co ban va kién thirc lap trinh huong dói turong..

Phan huong dan giup sinh vien xay dung tro choi o muc d co ban, cac em tr nghien ciru dé hoan thien mot cach tot nhat c6 thé.

## 2 Kich bän tro choi

Luc dau khi vao game sé xuat hien cac xe va thu chay qua lai va mot ki tur “Y” dai dien cho nguoi qua duong, nguoi choi sur dung cac phim ‘W', ‘A',‘S’, ‘D’ dé diéu chinh huong di chuyén cua nguoi qua duong va có gäng tranh cac xe va thu.

Khi“Y” va cham các xe hay thu thi chuong trinh thong bao yéu cäu nguoi choi chon phim ‘y’ néu muon tiép tuc (chuong trinh sé thiét lap lai dur lieu tro choi lai nhr luc ban dau) hoäc chon ‘bat ki phim nao’ néu muón thoat tro choi.

Khi “Y” di qua duoc hét cac xe va thu thi sé len cáp ké tiép,do kh6 cua tro choi chinh la só luong xe va thu tham gia di chuyén trén duong (Vi tri cua“Y" moi sé xuat hién tró lai khi len cap). Khi len cap tói da nao dó thi dur lieu sé khoi dong lai nhu luc ban dau.

![](images/b21f3fc44a56d4e55aec9fd40bf0be8d8b811140b64fcc633d55f3ba40cc57a3.jpg)  
Hinh 1: So dö kich ban tro choi

## 3 Cac ki thuät ho trd

Trong phan nay ta sé lan luot dugc gioi thiéu cac ki thuat hö tro qua trinh phat trién tro choi (sinh vien tur thiét ké mau phu hop trong qua trinh lam dö an).

## 3.1 Có dinh man hinh chinh

Trong qua trinh choi tro choi, nguoi dung có thé thay di kich thuoc cira só, diéu nay sé lam cho cac doi tuong thay doi kich thuoc gay khó khan trong qua trinh tinh toan. Ta nen có dinh man hinh truoc khi thuc hien cac tinh toan.

<table><tr><td rowspan=1 colspan=1>Dong</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>void FixConsoleWindow(）{</td></tr><tr><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>HWND consoleWindow = GetConsoleWindow();</td></tr><tr><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>LONG style = GetWindowLong(consoleWindow, GWL_STYLE);</td></tr><tr><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>style = style&amp; ~(WS_MAXIMIZEBOX)&amp; ~(WS_THICKFRAME);</td></tr><tr><td rowspan=1 colspan=1>5</td><td rowspan=1 colspan=1>SetWindowLong(consoleWindow, GWL_STYLE， style);</td></tr><tr><td rowspan=1 colspan=1>6</td><td rowspan=1 colspan=1>}</td></tr></table>

Trong doan ma trén, kiéu HWND la mót con tró tró toi chinh cura só Console. Dé lam viéc voi cac dói tuong dö hoa nay, ta can c6 nhung kiéu nhu thé. Co GWL\_STYLE dugc xem la dau hieu dé ham GetWindowLong lay các dac tinh ma cira só Console dang c6. Két qua tra vé cua ham GetWindowLong la mót só kiéu long, ta sé hiéu chinh tai dong só 4. Y nghia la dé lam mo di nut maximize va khong cho nguoi dung thay doi kich thuoc cira só hien hanh. Sau khi da hiéu chinh xong, ta dung ham SetWindowLong dé gan két qua hieu chinh tro lai. Ta có thé thur nghiem ham tren va tu xem két qua.

## 3.2 Thiet läp vi tri cho con tró man hinh

Trong tro choi sé có rat nhiéu vi tri ma ta muón in tai dó, vi vay ta can có khä nang di chuyén toi tat ca cac vi tri trong man hinh console.

<table><tr><td rowspan=1 colspan=1>Dong</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>void GotoXY(int x, int y) {</td></tr><tr><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>COORD coord;</td></tr><tr><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>coord.X = x;</td></tr><tr><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>coord.Y = y;</td></tr><tr><td rowspan=1 colspan=1>5</td><td rowspan=1 colspan=1>SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);</td></tr><tr><td rowspan=1 colspan=1>6</td><td rowspan=1 colspan=1>}</td></tr></table>

Trong doan ma nay ta sur dung struct \_COORD (COORD), day la mót cáu truc danh xur ly cho toa d tren man hinh console. Ta gan hoanh d@ va tung dó cho bién coord sau d6 thiét lap vi tri len man hinh bang ham SetConsoleCursorPosition. Luu y: ham nay can mot doi tuong chinh la man hinh console (man hinh den), vi vay ta cung can có mot con tró tró toi doi tuong nay (HANDLE thurc chat la void\*). Ta có duoc bang cach goi ham GetStdHandle voi tham só la m@t co STD\_OUTPUT\_HANDLE.

## 3.3 Ki thuät da tieu trinh

Tiép theo ta xem xét co ché hai tiéu trinh chay song song gom mot tiéu trinh main va mot tiéu trinh phu.

<table><tr><td rowspan=1 colspan=1>Dong</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>void exitGame(thread* t){</td></tr><tr><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>system(&quot;cls&quot;);</td></tr><tr><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>IS_RUNNING = false;</td></tr><tr><td rowspan=1 colspan=1>5</td><td rowspan=1 colspan=1>t-&gt;joinO;</td></tr><tr><td rowspan=1 colspan=1>6</td><td rowspan=1 colspan=1>}</td></tr><tr><td rowspan=1 colspan=1>7</td><td rowspan=1 colspan=1>void ThreadFunc10{</td></tr><tr><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1>while(IS_RUNNING){</td></tr><tr><td rowspan=1 colspan=1>9</td><td rowspan=1 colspan=1> //..thuc hien in ra man hinh console nhung doi tuong trong tro choi</td></tr><tr><td rowspan=1 colspan=1>10</td><td rowspan=1 colspan=1>！</td></tr><tr><td rowspan=1 colspan=1>11</td><td rowspan=1 colspan=1>void mainO{</td></tr><tr><td rowspan=1 colspan=1>12</td><td rowspan=1 colspan=1> $/ / . . .$ </td></tr><tr><td rowspan=1 colspan=1>13</td><td rowspan=1 colspan=1> thread t1(ThreadFunc1); //Tao m@t thread phu chay song song voi thread main</td></tr><tr><td rowspan=1 colspan=1>14</td><td rowspan=1 colspan=1>while(1){</td></tr><tr><td rowspan=1 colspan=1>15</td><td rowspan=1 colspan=1> int temp = toupper(getchO);</td></tr><tr><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1> $/ / . . .$ </td></tr><tr><td rowspan=1 colspan=1>17</td><td rowspan=1 colspan=1> if(temp == 27) {// nguoi dung muon thoat</td></tr><tr><td rowspan=1 colspan=1>18</td><td rowspan=1 colspan=1>exitGame(&amp;t1); return;</td></tr><tr><td rowspan=1 colspan=1>19</td><td rowspan=1 colspan=1>}</td></tr><tr><td rowspan=1 colspan=1>20</td><td rowspan=1 colspan=1> $/ / . . .$ </td></tr><tr><td rowspan=1 colspan=1>21</td><td rowspan=1 colspan=1>}</td></tr></table>

Trong doan ma tren ta thay ham‘main’ sé tao ra mót thread con chay song song voi minh. Trong ham ThreadFunc1 (dai dien cho thread con nay) sé có mót vong läp chay voi diéu kien bién IS\_RUNNING con la true (vi vay luc dau bién nay phai có gia tri true). Trong qua trinh ‘main’ chay néu nguoi dung nhan phim ‘ESC’ thi sé goi ham ‘exitGame'. Trong ham ‘exitGame’ ta sé gan gia tri false cho bién IS\_RUNNING dé ham ThreadFunc1 dung lai, dong thoi ta cho tiéu trinh ‘t’ join voi ham ‘main’ (Vi theo quy dinh tiéu trinh ‘main' phai két thuc sau cac tiéu trinh con).

## 3.4 Cac lop trong tro choi

Tiép theo ta sé gioi thieu cac di trong can thiet trong tro choi bang qua duong cung nhu sur tac dong qua lai gitra cac doi tuong.

## 3.4.1 Lóp CPEOPLE

Trong tro choi nay sé c6 mót doi tuong nguoi di chuyén qua cac lan xe chay. C6 thé dung ki tu ‘Y' dai dien hoäc tu thiét ké mot hinh mäu nao dó tuy y. Co ban lop nguoi có cac thong tin co ban sau:

<table><tr><td rowspan=1 colspan=1>class CPEOPLE{</td></tr><tr><td rowspan=1 colspan=1>int mX, mY;</td></tr><tr><td rowspan=1 colspan=1>bool mState; //Trang thai song chét</td></tr><tr><td rowspan=1 colspan=1>public:</td></tr><tr><td rowspan=1 colspan=1>CPEOPLEO;</td></tr><tr><td rowspan=1 colspan=1>void Up(int);</td></tr><tr><td rowspan=1 colspan=1>void Left(int);</td></tr><tr><td rowspan=1 colspan=1>void Right(int);</td></tr><tr><td rowspan=1 colspan=1>void Down(int);</td></tr><tr><td rowspan=1 colspan=1>bool isImpact(const CVEHICLE*&amp;);</td></tr><tr><td rowspan=1 colspan=1>bool isImpact(const CANIMAL*&amp;);</td></tr><tr><td rowspan=1 colspan=1>bool isFinish(;</td></tr><tr><td rowspan=1 colspan=1>bool isDead();</td></tr><tr><td rowspan=1 colspan=1>~</td></tr></table>

Trong dó cac phuong thtrc Up,Left, Right, Down cap nhat vi tri cua dói tuong CPEOPLE, hai phuong thirc isImpact kiém tra truong hop khi cham cac doi tuong CVEHICLE va CANIMAIL, vi du néu va cham thi state phai la false (chét)... Ngoai cac phuong thurc co ban dó ra, sinh vien có thé tr thiét ké thém cac thu@c tinh va phuong thurc can thiét khac.

## 3.4.2 Lóp CVEHICLE

Trong qua trinh nguoi bäng qua duong,cac xe sé chay, vé co ban lop xe có cac thong tin nhu sau

<table><tr><td rowspan=1 colspan=1>class CVEHICLE{</td></tr><tr><td rowspan=1 colspan=1>int mX, mY;</td></tr><tr><td rowspan=1 colspan=1>public:</td></tr><tr><td rowspan=1 colspan=1>virtual void Move(int, int);</td></tr><tr><td rowspan=1 colspan=1> $/ / . .$ </td></tr><tr><td rowspan=1 colspan=1>}</td></tr></table>

Trong dó phuong thurc ao Move sé thuc hien cap nhat vi tri moi cua dói tuong CVEHICLE. Ngoai ra, sinh vien tur thiét ké thém mot só phuong thtrc va thu@c tinh khac. Luu y lop CVEHICLE chi la lop tong quat, ta sé có hai lop con la CTRUCK va CCAR

<table><tr><td>class CTRUCK: public CVEHICLE{</td></tr><tr><td>public:</td></tr><tr><td> $/ / . . .$ </td></tr><tr><td>}</td></tr></table>

<table><tr><td>class CCAR: public CVEHICLE{</td></tr><tr><td>public:</td></tr><tr><td> $\underline { { / / . . . } }$ </td></tr><tr><td>}</td></tr></table>

Sau day la hinh anh minh hoa khi vé ra man hinh hai lan xe

![](images/8085e1bf77ae5dd7dfe4100db556d6c9db0ffdc22e9992b1637993db88398b7f.jpg)

## 3.4.3 Lóp CANIMAL

Tuong tur nhu lop CVEHICLE, ta sé có lop CANIMAL voi các thong tin co ban nhu sau

<table><tr><td rowspan=1 colspan=1>class CANIMAL{</td></tr><tr><td rowspan=1 colspan=1>int mX, mY;</td></tr><tr><td rowspan=1 colspan=1>public:</td></tr><tr><td rowspan=1 colspan=1>virtual void Move(int, int);</td></tr><tr><td rowspan=1 colspan=1>virtual void Tell();</td></tr><tr><td rowspan=1 colspan=1>~</td></tr></table>

Trong dó phuong thurc Move sé thuc hién cap nhat vi tri moi cua dói tuong CANIMAL tuy vao tung loai, phuong thurc Tell() sé phat ra tiéng keu voi tng voi ting loai. Ngoai ra, sinh vien tur thiét ké thém mót só phuong thurc va thu@c tinh khac. Luu y lop CANIMAL chi la lop tng quat, ta sé có hai lop con la CBIRD va CDINAUSOR

<table><tr><td>class CDINAUSOR: public CANIMAL{</td></tr><tr><td>public:</td></tr><tr><td> $/ / . . .$ </td></tr><tr><td></td></tr></table>

<table><tr><td>class CBIRD: public CANIMAL{</td></tr><tr><td>public:</td></tr><tr><td> $/ / . . .$ </td></tr><tr><td>人</td></tr></table>

Sau day la hinh anh minh hoa in ra hai lan thu

![](images/d933f0b6bd8f8e4d98e4b4cae442c66aa14455ff2b81a7e66577a228b11420c0.jpg)

## 3.4.4 Lop CGAME

Nhu vay cac dóituong trong tro choi da day du. Cuoi cung ta c6 lop CGAME,lop nay d6ng vai tro trung tam tro choi sé dieu phi toan bó cac di tuong trong tro choi nay.Lop game bao gom cac thong tin quan trong sau

<table><tr><td rowspan=1 colspan=1>Dong</td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>class CGAME{</td></tr><tr><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>CTRUCK* axt;</td></tr><tr><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>CCAR* axh;</td></tr><tr><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>CDINAUSOR* akl;</td></tr><tr><td rowspan=1 colspan=1>5</td><td rowspan=1 colspan=1>CBIRD* ac;</td></tr><tr><td rowspan=1 colspan=1>6</td><td rowspan=1 colspan=1>CPEOPLE cn;</td></tr><tr><td rowspan=1 colspan=1>7</td><td rowspan=1 colspan=1>public:</td></tr><tr><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1> CGAMEQ; //Chuan bi dur lieu cho tat ca cac doi tuong</td></tr><tr><td rowspan=1 colspan=1>9</td><td rowspan=1 colspan=1> void drawGame(); //Thuc hién vé tro choi ra man hinh sau khi c6 dur lieu</td></tr><tr><td rowspan=1 colspan=1>10</td><td rowspan=1 colspan=1>~CGAME(); // Huy tai nguyen da cap phat</td></tr><tr><td rowspan=1 colspan=1>11</td><td rowspan=1 colspan=1> CPEOPLE getPeople()://Lay thong tin nguoi</td></tr><tr><td rowspan=1 colspan=1>12</td><td rowspan=1 colspan=1>CVEHICLE* getVehicle()://Lay danh sach cäc xe</td></tr><tr><td rowspan=1 colspan=1>13</td><td rowspan=1 colspan=1> CANIMAL* getAnimal(); //Lay danh sach cac thu</td></tr><tr><td rowspan=1 colspan=1>14</td><td rowspan=1 colspan=1> void resetGame(); // Thuc hien thiét lap lai toan b@ du lieu nhu luc dau</td></tr><tr><td rowspan=1 colspan=1>15</td><td rowspan=1 colspan=1> void exitGame(HANDLE); // Thuc hien thoat Thread</td></tr><tr><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1> void startGame(); // Thuc hien bät dau vao tro choi</td></tr><tr><td rowspan=1 colspan=1>17</td><td rowspan=1 colspan=1> void loadGame(istream); // Thuc hien tai lai tro choi da luru</td></tr><tr><td rowspan=1 colspan=1>18</td><td rowspan=1 colspan=1> void saveGame(istream); // Thuc hién luu lai dur lieu tro choi</td></tr><tr><td rowspan=1 colspan=1>19</td><td rowspan=1 colspan=1> void pauseGame(HANDLE); // Tam dung Thread</td></tr><tr><td rowspan=1 colspan=1>20</td><td rowspan=1 colspan=1> void resumeGame(HANDLE); //Quay lai Thread</td></tr><tr><td rowspan=1 colspan=1>21</td><td rowspan=1 colspan=1> void updatePosPeople(char); //Thuc hién diéu khién di chuyén cua CPEOPLE</td></tr><tr><td rowspan=1 colspan=1>22</td><td rowspan=1 colspan=1> void updatePosVehicle(); //Thuc hien cho CTRUCK &amp; CCAR di chuyén</td></tr><tr><td rowspan=1 colspan=1>23</td><td rowspan=1 colspan=1> void updatePosAnimal()://Thuc hien cho CDINAUSOR &amp; CBIRD di chuyén</td></tr><tr><td rowspan=1 colspan=1>24</td><td rowspan=1 colspan=1>}</td></tr></table>

Bén canh dó, sinh vién có thé tu dinh nghia m@t vai hang só (const) quy dinh các tham só trong tro choi vi du MAX\_LEVEL qui dinh só cáp toi da hay MAX\_BIRD qui dinh só chim bay toi da...

Hinh änh minh hoa tro choi hoan chinh

![](images/375d2a57986e39b0308c7f1271cbef4133a5ba13cc06c1d3d0cde19f535ff220.jpg)

## 3.4.5 So do lop tro choi

Phan nay cho ta cai nhin tóng thé vé cac lop tham gia trong tro choi nay

![](images/9e0e895fca60ba0cb2f7f92f8e43d7a9a5e998a61351f3b75cc0e58d611a8caf.jpg)

## 3.5 Doan ma gia minh hoa

Phan nay trinh bay cach sur dung trong ham main voi tieu trinh va cac doi tuong trong tro choi.

<table><tr><td colspan="1" rowspan="1">Dong</td><td colspan="1" rowspan="1"></td></tr><tr><td colspan="1" rowspan="1">1</td><td colspan="1" rowspan="1">//Cac häng só va bién toän cuc cän thiét</td></tr><tr><td colspan="1" rowspan="1">2</td><td colspan="1" rowspan="1">char MOVING;</td></tr><tr><td colspan="1" rowspan="1">3</td><td colspan="1" rowspan="1">CGAME cg;</td></tr><tr><td colspan="1" rowspan="1">4</td><td colspan="1" rowspan="1">void main()</td></tr><tr><td colspan="1" rowspan="1">5</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">6</td><td colspan="1" rowspan="1">cg = new CGAME();</td></tr><tr><td colspan="1" rowspan="1">7</td><td colspan="1" rowspan="1">int temp;</td></tr><tr><td colspan="1" rowspan="1">8</td><td colspan="1" rowspan="1">FixConsoleWindow();</td></tr><tr><td colspan="1" rowspan="1">9</td><td colspan="1" rowspan="1">cg.startGame();</td></tr><tr><td colspan="1" rowspan="1">10</td><td colspan="1" rowspan="1">thread t1(SubThread);</td></tr><tr><td colspan="1" rowspan="1">11</td><td colspan="1" rowspan="1">while (1)</td></tr><tr><td colspan="1" rowspan="1">12</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">13</td><td colspan="1" rowspan="1">temp = toupper(getch());</td></tr><tr><td colspan="1" rowspan="1">14</td><td colspan="1" rowspan="1">if (!cg.getPeople().isDead())</td></tr><tr><td colspan="1" rowspan="1">15</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">16</td><td colspan="1" rowspan="1">if (temp == 27）{</td></tr><tr><td colspan="1" rowspan="1">17</td><td colspan="1" rowspan="1">cg.exitGame(t1.native_handle());</td></tr><tr><td colspan="1" rowspan="1">18</td><td colspan="1" rowspan="1">return;</td></tr><tr><td colspan="1" rowspan="1">19</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">20</td><td colspan="1" rowspan="1">else if (temp == 'P'){</td></tr><tr><td colspan="1" rowspan="1">21</td><td colspan="1" rowspan="1">cg.pauseGame(t1.native_handle());</td></tr><tr><td colspan="1" rowspan="1">22</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">23</td><td colspan="1" rowspan="1">else{</td></tr><tr><td colspan="1" rowspan="1">24</td><td colspan="1" rowspan="1">cg.resumeGame((HANDLE)t1.native_handle());</td></tr><tr><td colspan="1" rowspan="1">25</td><td colspan="1" rowspan="1">MOVING = temp； //Cap nhät buóc di chuyén</td></tr><tr><td colspan="1" rowspan="1">26</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">27</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">28</td><td colspan="1" rowspan="1">else</td></tr><tr><td colspan="1" rowspan="1">29</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">30</td><td colspan="1" rowspan="1"> if (temp == 'Y'） cg.startGame();</td></tr><tr><td colspan="1" rowspan="1">31</td><td colspan="1" rowspan="1">else{</td></tr><tr><td colspan="1" rowspan="1">32</td><td colspan="1" rowspan="1">cg.exitGame(t1.native_handle());</td></tr><tr><td colspan="1" rowspan="1">33</td><td colspan="1" rowspan="1">return;</td></tr><tr><td colspan="1" rowspan="1">34</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">35</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">36</td><td colspan="1" rowspan="1">广</td></tr><tr><td colspan="1" rowspan="1">37</td><td colspan="1" rowspan="1">}</td></tr></table>

Ngoai ra phan tiéu trinh chay song song cüng quan trong diéu phoi chinh cac dói tuong CANIMAL va CVEHICLE trén man hinh

<table><tr><td colspan="1" rowspan="1">Dong</td><td colspan="1" rowspan="1"></td></tr><tr><td colspan="1" rowspan="1">1</td><td colspan="1" rowspan="1">void SubThread()</td></tr><tr><td colspan="1" rowspan="1">2</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">3</td><td colspan="1" rowspan="1">while(（IS_RUNNING）{</td></tr><tr><td colspan="1" rowspan="1">4</td><td colspan="1" rowspan="1">if (!cg.getPeople().isDead()） //Néu nguoi vän con song</td></tr><tr><td colspan="1" rowspan="1">5</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">6</td><td colspan="1" rowspan="1">cg.updatePosPeople(M0VING);//Cap nhat vi tri nguoi theo thong tin tir main</td></tr><tr><td colspan="1" rowspan="1">7</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">8</td><td colspan="1" rowspan="1"> MOVING = ' ‘;// Tam kh6a khóng cho di chuyén， chα nhan phim tur ham main</td></tr><tr><td colspan="1" rowspan="1">9</td><td colspan="1" rowspan="1">cg.updatePosVehicle();//Cap nhat vi tri xe</td></tr><tr><td colspan="1" rowspan="1">10</td><td colspan="1" rowspan="1">cg.updatePosAnimal()； //Cap nhät vi tri thu</td></tr><tr><td colspan="1" rowspan="1">11</td><td colspan="1" rowspan="1">cg.drawGame();</td></tr><tr><td colspan="1" rowspan="1">12</td><td colspan="1" rowspan="1">if (cg.getPeople().isImpact(cg.getVehicle() Ilcg.getPeople().isImpact(cg.getAnimal())</td></tr><tr><td colspan="1" rowspan="1">13</td><td colspan="1" rowspan="1">{</td></tr><tr><td colspan="1" rowspan="1">14</td><td colspan="1" rowspan="1">// Xir ly khi dung xe hay thu</td></tr><tr><td colspan="1" rowspan="1">15</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">16</td><td colspan="1" rowspan="1">if((cg.getPeople().isFinish()){</td></tr><tr><td colspan="1" rowspan="1">17</td><td colspan="1" rowspan="1">// Xir ly khi vé dich</td></tr><tr><td colspan="1" rowspan="1">18</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">19</td><td colspan="1" rowspan="1">Sleep(100);</td></tr><tr><td colspan="1" rowspan="1">20</td><td colspan="1" rowspan="1">}</td></tr><tr><td colspan="1" rowspan="1">21</td><td colspan="1" rowspan="1">}</td></tr></table>

## 4 YEU CAU D AN

Trong phan huong dan tren ta con thiéu mot vai chtrc nang co ban

## 4.1 Cai dät chay dugc giong kich ban mo ta (3d)

Xem lai mo ta kich ban va cai dat dé tro choi hoat dong gióng mó ta, luu y: cac hinh ve xe tai, xe hoi ó cap d nay chua can thiet, sinh vien có thé dung ki tu nao d6 dai dien.

## 4.2 Xay du'ng thurc don cho tro choi khi vura moi vao (1d)

Xay dung trinh menu cho tro choi, vi du nhu luc däu sé hién lén ban

<table><tr><td>1. New game 2.Load g game 3. Settings</td></tr></table>

Khi nguoi dung chon phan ‘New game’ thi sé vao tro choi, néu chon phan ‘Load game’ hay ‘Setings’ thi tam chua xur ly va yéu cau nguoi dung chon lai.

## 4.3 Xi ly luu/tai tro choi (3d)

Sinh vien bó sung chirc näng ‘Load game’ khi nguoi dung nhan vao menu luc dau hoac nhän phim ‘T’ khi dang choi. Luc nay chuong trinh tam dung va in ra dong yéu cau nguoi dung nhap duong dan tap tin da luu. Khi d6 chuong trinh thiét lap du lieu va vao tro choi.

Sinh vien bó sung chtrc nang‘Save game’ khi nguoi dung nhan phim‘L’ khi dang choi. Luc nay chuong trinh tam ding in ra dong yéu cáu nguoi dung nhap duong dan tap tin can luu. Sau khi lu xong thi chuong trinh tiép tuc choi (có thé hoi nguoi choi c6 muón tiép tuc hay khong).

Huong dan: Sinh vien tur to chirc cau truc tap tin dé luu dur lieu bién toan cuc trong chuong trinh

## 4.4 Xu ly tam dung cac toa xe (2d)

Trong huong dän, cac xe di chuyén lien tuc, sinh vien hay hiéu chinh lai sao cho moi toa xe déu có thé durng lai trong m@t khoang thoi gian tuy y dé giup tro choi dé choi hon khi len cap. Goi y: C6 thé xay dung lóp CTRAFFICLIGHT két hop voi lop CVEHICLE dé tam dung khi tin hieu den do.

![](images/f3bb7dd46f95a575e8c4c95adf711cf3f146572eb1456124dad7fe29ea9fec41.jpg)

## 4.5 Xir ly hiéu üng khi va cham (0.5d)

Khi nguoi qua duong va cham xe thi tao hieu ing don gian minh hoa viéc va cham, khi cac con thu di chuyén có thé phat ra tiéng keu...

## 4.6 Giao dien (0.5d)

Cach thurc bó tri cac thóng tin tro choi ‘hop ly',có hinh vé cu thé cua các xe,thu, nguoi va den tin hieu...