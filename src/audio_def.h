#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(23800)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(3840)},  //XA_GameOver
	{XA_Menu, XA_LENGTH(1000)},  //XA_GameOverS
	{XA_Menu, XA_LENGTH(5000)},  //XA_GameReset
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(18600)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(18600)}, //XA_Fresh
	{XA_Week1B, XA_LENGTH(18600)}, //XA_Fresh
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1A
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	{"gameovers", false}, //XA_GameOverS
	{"gamereset", false}, //XA_GameReset
	//WEEK1A.XA
	{"bopeebo", true}, //XA_Bopeebo
	{"fresh", true},   //XA_Fresh
	//WEEK1B.XA
	{"dadbattle", true},   //XA_Fresh
	
	{NULL, false}
};
