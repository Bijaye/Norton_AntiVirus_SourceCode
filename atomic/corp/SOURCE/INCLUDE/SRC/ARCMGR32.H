/*
 * 富士通デバイス・アーカイバ（ファイル圧縮ツール）
 * Arcmanager DLL ヘッダファイル V02L05
 * Copyright (C) FUJITSU DEVICES INC. 1995-1997
 */

// WORKAROUND: On Solaris gcc generates external c++ mangled exports
//  even when compiling c files.
#if !defined(UNIX)
#ifdef  __cplusplus
extern "C" {
#endif
#endif

#define FILE_ID		long
#if defined(_WINDOWS)
#define DECLFUNC	WINAPI
#else
#define DECLFUNC
#endif
#define	ATTRIB_READONLY 0x01
#define ATTRIB_HIDEN	0x02
#define ATTRIB_SYSTEM	0x04
#define ATTRIB_VOLLABEL 0x08
#define ATTRIB_DIR		0x10
#define ATTRIB_NORMAL	0x20
#define	ARCNO		short int
#define SORTNAME		0			//名前順にソート
#define SORTEXT			1			//拡張子順にソート
#define SORTSIZE		2			//サイズ順にソート
#define SORTDAY			3			//日付順にソート
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned char byte;

#if defined(_WINDOWS)
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////
//		タイムスタンプ用構造体
/////////////////////////////////////////////////////////////////////////////
typedef struct {
	byte fs_year;	/* since 1900 */
	byte fs_month;	/* 0-11 */
	byte fs_day;	/* 1-31 */
	byte fs_hour;	/* 0-23 */
	byte fs_min;	/* 0-59 */
	byte fs_sec;	/* 0-59 */
#if defined(UNIX)
} __attribute__ ((packed)) FILESTAT;
#else
} FILESTAT;
#endif

typedef struct {
	word mode;		/* ファイルのモード(DOS形式) */
	word dirno;		/* ディレクトリインデックス */
	FILESTAT ftim;	/* ファイルの日付・時間 */
	dword org_size;	/* 元ファイルのサイズ */
	dword en_size;	/* エンコード後のデータサイズ */
	dword loc;		/* .AGM ファイルの先頭からのオフセット */
	union {
		/* name[0] == 0 : ファイル名(ベース名のみ)インデックス
		   name[0] != 0 : ファイル名(ASCIIZ) */
		dword strx;
		char name[12+1]; /* 注意 : 12 を FNAMMAX とはしないこと */
#if defined(UNIX)
	} __attribute__ ((packed)) n;
#else
	} n;
#endif
	struct {
		unsigned short drive : 5;	/* ドライブ名 */
		unsigned short root : 1;	/* ルートディレクトリフラグ */
		unsigned short hasdir : 1;	/* ディレクトリ名をもつ */
		unsigned short reserve : 1;
		unsigned short encode : 8;	/* エンコード方法
									 *   0 : エンコードなし
									 *   1   : FLDC 符合(V2.6)
									 */
#if defined(UNIX)
	} __attribute__ ((packed)) flag;
#else
	} flag;
#endif

#if defined(UNIX)
} __attribute__ ((packed)) FILEHDR;
#else
} FILEHDR;
#endif

#if defined(_WINDOWS)
#pragma pack()
#endif

/////////////////////////////////////////////////////////////////////////////
//		ファイル表示用構造体
/////////////////////////////////////////////////////////////////////////////
typedef	struct {
	HGLOBAL h;			// ファイルの内容
	char Title[256+8];
	BOOL view;
} VIEW;

typedef VIEW FAR *LPVIEW;

/////////////////////////////////////////////////////////////////////////////
//		書庫ファイルの情報保存用構造体
/////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	FileName[32+256+256];	//書庫ファイル名
	FILESTAT ftim;
	long	DirNum;				//書庫内のディレクトリ総数
	long	NewDirNum;			//現在の書庫内のディレクトリ総数
	long	FileNum;			//書庫内のファイル総数
	long	NewFileNum;			//現在の書庫内のファイル総数
	LONG	ArcSize;			//書庫ファイルのサイズ
	BOOL	SaveDirInfo;			//ディレクトリ情報保存フラグ(保存時 =TRUE)
	BOOL	AutoUpdate;			//自動更新書庫フラグ(自動更新 =TRUE)
	BOOL	SelfExt;			//自己解凍書庫フラグ(自己解凍書庫 = TRUE)
	BOOL	OrgSelfExt;			//オープン時の自己解凍書庫フラグ(自己解凍書庫 = TRUE)
	BOOL	Password;			//パスワード設定フラグ(設定時 = TRUE)
	BOOL	OrgPassword;		//オープン時のパスワード設定フラグ(設定時 = TRUE)
	BOOL	UseLongNameForm;	//ロング名対応フォーマット
	LONG	OrgSizeT;			//選択されているファイルのオリジナルサイズの合計
	LONG	PackSizeT;			//選択されているファイルのパックサイズの合計
} AMINFO;


/////////////////////////////////////////////////////////////////////////////
//		書庫ファイル内のファイル/ディレクトリ情報保存用構造体
/////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	FileName[256];			//ファイル名
	LONG	OrgSize;				//元のサイズ
	LONG 	PackSize;				//圧縮後のサイズ
	FILESTAT	ftim;				//作成日時
	word	Attrib;					//属性
									//	bit0 : 読み出し専用ファイル
									//	bit1 : 隠しファイル
									//	bit2 : システムファイル
									//	bit3 : ボリュームラベル
									//	bit4 : サブディレクトリ
									//	bit5 : 通常のファイル
									//	bit6 :
									//	bit7 :
	BOOL	Compress;				//ディレクトリの圧縮状態(=TRUE)
									//属性がサブディレクトリ時のみ有効
	BOOL	Empty;					//空ディレクトリ(=TRUE)
									//属性がサブディレクトリ時のみ有効
	BOOL	Mark;					//ファイルの選択(=TRUE)/非選択(=FALSE)フラグ
	short int NestLevel;			//このファイルのネスティングレベル(0 : root)
} FILEINFO;


/////////////////////////////////////////////////////////////////////////////
//	DLL関数のプロトタイプ宣言
/////////////////////////////////////////////////////////////////////////////

ARCNO DECLFUNC AMCreate(HANDLE hInstance, HWND root);
//引数
// hInstance	: アプリケーションのインスタンスハンドル
//					(AfxGetInstanceHandle()の戻り値)
// root			: アプリケーションのルートウィンドウハンドル
//					(CWinAppのデータメンバ m_pMainWnd)
//戻り値
// 書庫ファイルハンドル
// -1			: エラー(メモリ確保に失敗):AMGetErrno関数を実行してください.
// 0 ～ 31		: 書庫番号


ARCNO DECLFUNC AMOpen(LPSTR amgpath, HANDLE hInstance, HWND root, short int FAR *status);
//引数
// amgpath		: オープンする書庫ファイルのフルパス名
// hInstance	: アプリケーションのインスタンスハンドル
//					(AfxGetInstanceHandle()の戻り値)
// root			: アプリケーションのルートウィンドウハンドル
//					(CWinAppのデータメンバ m_pMainWnd)
// status		: 書庫のオープン時の状態を格納します
//戻り値
// -1			: エラー(メモリ確保に失敗):AMGetErrno関数を実行してください.
// 0 ～ 31		: 書庫番号


ARCNO DECLFUNC AMOpenArc(LPSTR amgpath, HANDLE hInstance, HWND root, short int FAR *status, LPSTR PassWord);
//引数
// amgpath		: オープンする書庫ファイルのフルパス名
// hInstance	: アプリケーションのインスタンスハンドル
//					(AfxGetInstanceHandle()の戻り値)
// root			: アプリケーションのルートウィンドウハンドル
//					(CWinAppのデータメンバ m_pMainWnd)
// status		: 書庫のオープン時の状態を格納します
// PassWord		: 指定したパスワードで書庫を開きます。
//					パスワード付きの書庫を開くときこのパラメータにNULLを設定するとパスワード入力の
//					ダイアログが表示されます
//戻り値
// -1			: エラー(メモリ確保に失敗):AMGetErrno関数を実行してください.
// 0 ～ 31		: 書庫番号


short int DECLFUNC AMClose(ARCNO handle, LPSTR amgpath);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// amgpath		: クローズする書庫ファイルのフルパス名
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)


short int DECLFUNC AMReadDir(ARCNO handle, FILEINFO FAR *info, short int direction);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// *info		: ファイル情報保存用構造体(struct FILEINFO)へのポインタ
// direction    : 1 = 先へ進める (それ以外は未使用)
//戻り値
// 0			:正常終了
// -1			: エラー(ファイル情報の読み出し失敗):(AMGetErrno関数を実行してください.)


short int DECLFUNC AMSeekDir(ARCNO handle, long offs, long origin);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// offs			: 引数 origin	で指定される位置からのオフセット
//					(現在は 0 固定)
// origin		: ファイル情報を読みだす位置
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)


FILE_ID DECLFUNC AMTellDir(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// ファイルリストポインタ位置
// -1			:エラー(AMGetErrno関数を実行してください.)


short int DECLFUNC AMRewindDir(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)


short int DECLFUNC AMCompressDir(ARCNO handle, FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// dest			: ディレクトリを圧縮表示させるファイル(ディレクトリ)ID
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)


short int DECLFUNC AMExpandDir(ARCNO handle, FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// dest			: ディレクトリを伸張表示させるファイル/ディレクトリID
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)


FILE_ID DECLFUNC AMAddObject(ARCNO handle, LPSTR FilePath, FILE_ID position, long FileNum);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// FilePath		: 書庫に追加するファイルのフルパス名
// position		: 追加する位置(ファイル/ディレクトリID)
//戻り値
// 最後に追加した表示位置(ファイル/ディレクトリID)
// ｰ1			:エラー(AMGetErrno関数を実行してください.)

FILE_ID DECLFUNC AMRenameObject(ARCNO handle, LPSTR NewFileName, FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// NewFileName	: 変更するファイル/ディレクトリ名
// dest			: 変更するファイル/ディレクトリの位置(ファイル/ディレクトリID)
//戻り値
// 変更されたファイル・ディレクトリの表示位置(ファイル/ディレクトリID)
// -1			:エラー(AMGetErrno関数を実行してください.)
//

FILE_ID DECLFUNC AMMakeDir(ARCNO handle, LPSTR pDirName ,FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// pDirName		: 作成するディレクトリ名
// dest			: ディレクトリを作成する位置(ファイル/ディレクトリID)
//戻り値
// 作成したディレクトリの表示位置(ディレクトリID)
// -1			:エラー(ディレクトリの作成失敗):(AMGetErrno関数を実行してください.)
//

long DECLFUNC AMMarkObject(ARCNO handle, FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// dest			: マークするファイル/ディレクトリの位置(ファイル/ディレクトリID)
// OrgSizeT		:関数終了時にマークの設定しているファイルの復元に必要なディスク容量がセットされる
// PackSizeT	:関数終了時にマークの設定しているファイルの圧縮サイズの合計がセットされる
//戻り値
// マーク総数
// -1			:エラー(AMGetErrno関数を実行してください.)

long DECLFUNC AMMarkAllObjects(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// OrgSizeT		:関数終了時にマークの設定しているファイルの復元に必要なディスク容量がセットされる
// PackSizeT	:関数終了時にマークの設定しているファイルの圧縮サイズの合計がセットされる
//戻り値
// マーク総数
// -1			:エラー(AMGetErrno関数を実行してください.)

long DECLFUNC AMUnmarkObject(ARCNO handle, FILE_ID dest);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// dest			: マーク解除するファイル/ディレクトリの位置(ファイル/ディレクトリID)
//戻り値
// マーク総数
// -1			:エラー(AMGetErrno関数を実行してください.)

long DECLFUNC AMUnmarkAllObjects(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMUpdateArchives(ARCNO handle, LPSTR amgpath, LPSTR AutoPath, short int modify, BOOL Form95);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// amgpath		: 作成する書庫ファイルのフルパス名
// AutoPath		: 自動更新のための検索パス
// modify		: 修正が加わっているかどうかのフラグ
// Fom95		: ロング名を使用しているか、いないか
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)
// ※AMGetErrno()の戻り値が 0の場合は中断されたことを示す。

short int DECLFUNC AMDeleteMarkedObjects(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMViewMarkedObjects(LPVIEW lpViewBuffer, ARCNO handle,
									short int view_max, short int view_mode);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// for 95
//	lpViewbuffer:VIEW[n]の配列のポインタ
//  view_max    :VIEWの個数(1 ～ 64)
//	view_mode: -1:自動判定, 0:テキスト表示, 1:ダンプ表示
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

FILE_ID DECLFUNC AMCopyMarkedObjects(ARCNO dest, ARCNO src, FILE_ID position);
//引数
// dest			: コピー先の書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// src			: コピー元の書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// position		: コピー先書庫のコピーするファイル/ディレクトリの位置
//戻り値
// 最後にコピーしたファイル・ディレクトリの表示位置(ファイル・ディレクトリID)
// -1			:エラー(AMGetErrno関数を実行してください.)

FILE_ID DECLFUNC AMMoveMarkedObjects(ARCNO dest, ARCNO src, FILE_ID position);
//引数
// dest			: 移動先の書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// src			: 移動元の書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// position		: 移動先書庫の移動するファイル/ディレクトリの位置
//戻り値
// 最後に移動したファイル・ディレクトリの表示位置(ファイル・ディレクトリID)
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetPassword(ARCNO handle, LPSTR passwd);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// passwd		: 設定するパスワード
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMClearPassword(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetSelfExtract(ARCNO handle, short int flag);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// flag			: 1 = 自己解凍書庫とする
//				  0 = 自己解凍書庫としない
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetVerboseMode(ARCNO handle, short int flag);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// flag			: 1 = 進捗表示を行う
//				  0 = 進捗表示を行わない
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSaveWithDirInfo(ARCNO handle, short int flag);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// flag			: 1 = ディレクトリ情報を保存
//				  0 = ディレクトリ情報を保存しない
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetAutoUpdate(ARCNO handle, short int flag);
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// flag			:  1 = 書庫の内容を自動更新する
//				   0 = 書庫の内容を自動更新しない
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetSortMethod(ARCNO handle, short int method);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// method		:
//					enum { SORTNAME,			//名前順にソート
//		   				   SORTEXT,				//拡張子順にソート
//		   				   SORTSIZE,			//サイズ順にソート
//		   				   SORTDAY};			//日付順にソート
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetSortOrder(ARCNO handle, short int order);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// order		: 1 = 昇順にソート
//				  0 = 降順にソート
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMGetAMInfo(ARCNO handle, AMINFO FAR *aminfo);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// *aminfo		: 書庫情報構造体へのポインタ
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

DWORD DECLFUNC AMGetVersion(void);
//引数
//	なし
//戻り値
// アークマネージャのバージョン・レベル
// 上位バイト0x00, 次のバイトからバージョン、レベル、リリース番号 

FILE_ID DECLFUNC AMMoveObject(ARCNO handle, FILE_ID dest, FILE_ID src);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// dest			: 移動するファイルの移動先の位置(ファイル/ディレクトリID)
// src			: 移動するファイルの移動元の位置(ファイル/ディレクトリID)
//戻り値
// 最後に移動したファイル・ディレクトリの表示位置(ファイル・ディレクトリID)
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMGetMaxName(ARCNO handle, short int nestlvl);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// nestlvl		: 検索対象のネストレベル
//戻り値
// 0			:指定された検索対象ネストレベルが存在しない
// !0			:95版では、ファイル・ディレクトリが存在すれば 16 なければ 0
// -1			:エラー(AMGetErrno関数を実行してください.)

FILE_ID DECLFUNC AMGetSize(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 現在表示すべきファイル/ディレクトリ数
// -1			:エラー(AMGetErrno関数を実行してください.)

short int	DECLFUNC AMGetErrorno(ARCNO an, LPSTR ErrMess, short int FAR *ErrEp);
//引数
// an			: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// ErrMess		:エラーの要因の説明文
// ErrEp		:この後の処理
// 戻り値		:エラー番号

long DECLFUNC AMGetMarkedObjNum(ARCNO handle);
//引数
// handle		: 書庫ファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// 現在マークされているファイルの数
// -1			:エラー(AMGetErrno関数を実行してください.)

long DECLFUNC AMExpandDirAll(ARCNO handle);
//引数
// handle		: 書庫のファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
//戻り値
// ファイル・ディレクトリの総数
// -1			:エラー(AMGetErrno関数を実行してください.)

FILE_ID DECLFUNC AMSearchFile(ARCNO handle, LPSTR FileName);
//引数
// handle		: 書庫のファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// FileName		: ポジション(表示行)の知りたいフルパス名
//		下に検索
//戻り値
// 指定したファイルのポジション（表示行）
// -1			:エラー(AMGetErrno関数を実行してください.)


long DECLFUNC AMGetLongFileNameNum(ARCNO handle);
// 戻り値
// ロング名を使用しているファイル数
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMExtractDir(ARCNO handle, LPSTR Extpath);
//引数
// handle(ARCNO): 書庫のファイルハンドル
//					(AMOpen(),AMCreate()の戻り値)
// Extpath		: 展開先のディレクトリ名
//戻り値
// 0			:正常終了
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMSetMessageBox(ARCNO handle, BOOL dlgflg);
// 引数
// handle		: 書庫ハンドル
// dlgflg		: MessageBox表示のON/OFF Flag.
// 戻り値
// 0			: 正常
// -1			:エラー(AMGetErrno関数を実行してください.)

short int DECLFUNC AMCheckDirStr(ARCNO handle);
// 引数
// handle		: 書庫ハンドル
// 説明：
// ディレクトリ名を重複しないようにディレクトリ番号を
// つけながら文字列テーブルを作成する
// 戻り値
// 0: ディレクトリ名が64Kバイト以内に収まった --> Win95/Win3.1
// 1: ディレクトリ名が64Kバイト以上になった   --> Win95
// -1: error

short int DECLFUNC AMGetExtractSize(ARCNO handle, unsigned long *PackSize, unsigned long *ExtSize);
// 引数
// handle		: 書庫ハンドル
// 説明：
// マークのついているファイルのトータルの展開サイズ、圧縮サイズの合計を
// PackSize, ExtSizeに設定する
// 戻り値
// -1: Error

//////////////////////////////////////////////////////////////////
// ERROR TABLE
#define		NORMAL				0
// param
#define		ERR_HANDLE			30		// ハンドルが異常
#define		ERR_ID				31		// パラメータの異常
#define		ERR_HANDLEMAX		32		// これ以上書庫領域を確保できない
// disk operation
#define		ERR_DELFILE			39		// ファイルの削除エラー
#define		ERR_NOFILE			40		// 指定のファイルがない
#define		ERR_FSEEK			41		// ファイルシークに失敗した
#define		ERR_FREAD			42		// ファイルリードに失敗した
#define		ERR_FWRITE			43		// ファイルに書き込めない
#define		ERR_FCLOSE			44		// ファイルクローズに失敗した
#define		ERR_EACCESS			45		// 共有違反エラー
#define		ERR_CHDIR			46		// ディレクトリの移動に失敗した
#define		ERR_GETDIR			47		// カレントディレクトリが得られない
#define		ERR_ACCESS			48		// ファイルが見つからない
#define		ERR_MKDIR			49		// ディレクトリの作成に失敗した
#define		ERR_CHMOD			50		// 属性変更が出来ない
#define		ERR_NEEDNOFILE		51		// 必要なファイルが見つからない（*.eh）
#define		ERR_DIR_FILE		52		// ファイル名とディレクトリ名の重複
#define		ERR_EMPTY_ARC		53		// 書庫内にファイルが存在しないためセーブできない
#define		ERR_FCREAT			54		// ファイルの作成に失敗した
#define		ERR_FOPEN			55		// ファイルのオープンに失敗 (ERR_NOFILE, ERR_EACCESS 以外の原因)
#define		ERR_DUPNAME			56		// ファイル名重複
#define		ERR_NOTCOMPFILE		57		// 展開しようとしているファイルは書庫上のファイルではない
// amg
#define		ERR_LONGDIRNAME		59		// 書庫内のパスが240文字を超えた
#define		ERR_NOTAMGFILE		60		// 書庫ファイルではない
#define		ERR_WRONGNAME		61		// ファイル名長が既定値を越えた
#define		ERR_NESTLEVEL		62		// ネストレベルが 32 を越えた
#define		ERR_DIRDICCNT		63		// ディレクトリ辞書が64KBを超えた
#define		ERR_FILEADD			64		// ファイルの追加に失敗した
#define		ERR_ILGDATA			65		// 書庫のデータがおかしい (for EXTRACT)
#define		ERR_ILGPASSWD		66		// パスワードが違う
#define		ERR_VIEWMAX			67		// view表示は最大６４枚まで
#define		ERR_TOOMANY_FILE	68		// ファイルが多すぎる
#define		ERR_TOOMANY_DIR		69		// 書庫内のディレクトリが多すぎる
#define		ERR_TOOLONG_STR		70		// 文字列テーブルを使い果たした
#define		ERR_FILENOTFOUND	71		// ファイルが見つからない
#define		ERR_NONAME			72		// ファイル名が指定されていない
#define		ERR_ILGNAME			73		// ファイル名に使用してはいけない文字を使用した
#define		ERR_TOOMANY_OBJ		74		// ファイル・ディレクトリのトータルが65000個を超えた
#define		ERR_NOTCOMPRESS		75		// 未圧縮のファイルはリネームできない
#define		ERR_TOOMANYFILE_INDIR 76	// 1 Dir内に8191個以上のファイルを格納することはできない
// memory
#define		ERR_ALLOC			80		// メモリアロケーションに失敗した
#define		ERR_LOCK			81		// メモリのロックに失敗した
#define		ERR_REALLOC			82		// リアロックに失敗した
#define		ERR_WORKMEM			83		// バンクを使い果たした
// DLL error
#define		ERR_INNER			90		// 内部エラー
#define		ERR_API				91		//AM関数が呼ばれることが禁止状態
#define		ERR_OTHER			99		// リザーブ

#if !defined(UNIX)
#ifdef  __cplusplus
}
#endif
#endif

/*  End of Arcmgr32.h  */
