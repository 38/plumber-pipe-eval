#define C(x) struct _c_##x {char ch[x];}
C(1);
C(64);
C(4096);
C(65536);
C(131072);

#define D(x) struct _c_##x f##x(struct _c_##x)
D(1);
D(64);
D(4096);
D(65536);
D(131072);
