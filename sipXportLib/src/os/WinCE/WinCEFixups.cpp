
#ifdef __cplusplus
extern "C" int errno = 1;
#else
extern int errno;
#endif

char	*_tzname[ 2 ] = {"DST","STD"};


//****************************************************************
int	write( int fd, const void *buffer, unsigned int count )
{
	BOOL	fRet		= 0;
	if( fd  &&  fd != -1 )
	{
		DWORD		dwNumWritten;
		fRet = WriteFile( (HANDLE) fd, buffer, count, &dwNumWritten, NULL );
		if( fRet )
			return (int) dwNumWritten;
	}

	return -1;
}


//****************************************************************
int	read( int fd, void *buffer, unsigned int count )
{
	BOOL	fRet;
	if( fd  &&  fd != -1 )
	{
		DWORD		dwNumRead;
		fRet = ReadFile( (HANDLE) fd, buffer, count, &dwNumRead, NULL );
		if( fRet  ==  0 )
			return -1;
		else
			return (int) dwNumRead;
	}
	return -1;
}


//****************************************************************
HANDLE CreateFileB( const char *pFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
					LPSECURITY_ATTRIBUTES lpSecAttr, DWORD dwCreationDisp, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
//	printf( "CreateFileB( ) NOT IMPLEMENTED\n" );

	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( pFileName )
	{
		iRet = MultiByteToWideChar( CP_ACP, 0, pFileName, strlen( pFileName ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
	}
	if( iRet )
	{
		return CreateFileW( pW, dwDesiredAccess, dwShareMode,
							lpSecAttr, dwCreationDisp, dwFlagsAndAttributes, hTemplateFile );
	}
	else
		return NULL;
}


//****************************************************************
int open( const char *filename, int oflag, int pmode = 0 )
{
	//	oflag may be:
	//		_O_APPEND  _O_BINARY  _O_CREAT  _O_RDONLY  _O_RDWR  _O_WRONLY
	//  pmode used only if oflag == _O_CREAT
	//		_S_IREAD  _S_IWRITE
	DWORD	dwAccess;
	if( oflag & O_RDONLY )
		dwAccess = GENERIC_READ;
	else if( oflag & O_WRONLY )
		dwAccess = GENERIC_WRITE;
	else if( oflag & O_RDWR )
		dwAccess = GENERIC_WRITE | GENERIC_READ;;

	HANDLE	hRet;
	hRet = CreateFile( filename, dwAccess, 0, NULL, OPEN_ALWAYS, 0, NULL );
	if( INVALID_HANDLE_VALUE == hRet )
		return -1;
	else
		return (int) hRet;
}


//****************************************************************
int open( const char *filename, int oflag )
{
//	printf( "open( fname, oflag ) NOT IMPLEMENTED\n" );
	return open( filename, oflag, 0 );
}


//****************************************************************
int close( int fd )
{
	if( fd  &&  fd != -1 )
		if( CloseHandle( (HANDLE) fd ) )
			return 0;
	return -1;
}

//****************************************************************
long lseek( int fd, long offset, int origin )
{
	DWORD	dwRet;
	if( fd  &&  fd != -1 )
	{
		LONG	lHigh;
		DWORD	dwType;
		if( origin == SEEK_SET )
			dwType = FILE_BEGIN;		//beginning of file
		else if( origin == SEEK_CUR )
			dwType = FILE_CURRENT;		//current locbeginning of file
		else if( origin == SEEK_END )
			dwType = FILE_END;			//end of file
		dwRet = SetFilePointer( (HANDLE) fd, offset, &lHigh, dwType );
	}

	return dwRet;
}


//****************************************************************
int fstat( int fd, struct stat *buffer )
{
	printf( "fstat( ) NOT IMPLEMENTED\n" );
	return -1;
}


//****************************************************************
BOOL GetVersionExA( LPOSVERSIONINFOA lpVerInfo )
{
	printf( "GetVersionExA( ) NOT IMPLEMENTED\n" );
	return FALSE;
}

//****************************************************************
BOOL CreateProcessB( char *pName,
					   char *pCmdLine,
					   LPSECURITY_ATTRIBUTES lpsaProcess,
					   LPSECURITY_ATTRIBUTES lpsaThread,
					   BOOL fInheritHandles,
					   DWORD dwCreate,
					   LPVOID lpvEnv,
					   char *pCurDir,
					   LPSTARTUPINFO lpStartUpInfo,
					   LPPROCESS_INFORMATION lpProcInfo )
{
	printf( "CreateProcessB( ) NOT IMPLEMENTED\n" );
	return NULL;
}


//****************************************************************
HANDLE CreateSemaphoreB( LPSECURITY_ATTRIBUTES lpSemaphoreAttr, LONG lInitialCount, LONG lMaxCount, char *pName )
{
//	printf( "entering CreateSemaphoreB( )\n" );
//	printf( "  pName = *%s*", pName );

	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( pName )
	{
		iRet = MultiByteToWideChar( CP_ACP, 0, pName, strlen( pName ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
	}
	if( iRet )
	{
		return CreateSemaphoreW( lpSemaphoreAttr, lInitialCount, lMaxCount, pW );
	}
	else
		return NULL;
}


//****************************************************************
HANDLE CreateMutexB( LPSECURITY_ATTRIBUTES lpMutexAttr, BOOL bInitialOwner, char *pName )
{
//	printf( "CreateMutexB( ) NOT IMPLEMENTED\n" );
//	printf( "entering CreateMutexB( ) - pName is *%s*\n", pName );

	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( pName )
	{
		iRet = MultiByteToWideChar( CP_ACP, 0, pName, strlen( pName ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
	}
	if( iRet )
	{
		return CreateMutexW( lpMutexAttr, bInitialOwner, pW );
	}
	else
		return NULL;
}


//****************************************************************
DWORD FormatMessageB( DWORD dwFlags, void *lpVoid, DWORD dwMessageID, DWORD dwLangID, char *pBuffer, DWORD dwSize, va_list *Args )
{
	printf( "FormatMessageB( ) NOT IMPLEMENTED\n" );
	return 0;
}


//****************************************************************
struct tm * __cdecl localtime( const time_t *timer )
{
	printf( "localtime( ) NOT IMPLEMENTED\n" );
	static struct tm staticTMlocalTime;
	return &staticTMlocalTime;
}


//****************************************************************
struct tm * __cdecl gmtime( const time_t *timer )
{
	printf( "gmtime( ) NOT IMPLEMENTED\n" );
	static struct tm staticTMgmtime;
	return &staticTMgmtime;
}


//****************************************************************
void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	 // Note that LONGLONG is a 64-bit value
	LONGLONG ll;

	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = (DWORD)(ll >> 32);
}


//****************************************************************
time_t FileTimeToUnixTime( LPFILETIME pft, int *pMillisecs = NULL )
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	time_t	ttRet;

	ll = pft->dwHighDateTime;
	ll = ll << 32;
	ll += pft->dwLowDateTime;
	ll -= 116444736000000000;
	if( pMillisecs )
	{
		int		iT;
		iT = (int) ( ll / 10000 );
		printf( "in FileTimeToUnixTime( ) - RAW number of milliseconds is %d\n", iT );
		iT = iT % 1000;
		printf( "                         - final number of milliseconds is %d\n", iT );
		*pMillisecs = iT % 1000;
	}
	ll /= 10000000;
	ttRet = (time_t)ll;
	return ttRet;
}


//****************************************************************
time_t __cdecl time( time_t *ptt )
{
	BOOL		fRet;
	int			iRet;
	FILETIME	ft;
	SYSTEMTIME	st;
	//  get SYSTEMTIME
	GetSystemTime( &st );
	//  convert SYSTEMTIME to FILETIME
	fRet = SystemTimeToFileTime( &st, &ft );
	//  convert FILETIME to UnixTime
	iRet = FileTimeToUnixTime( &ft );
//	printf( "time( ) is about to return %d\n", iRet );
	if( ptt )
		*ptt = iRet;
	return iRet;
}


//****************************************************************
void _ftime( struct _timeb *pTb )
{
	static int	iSavedTime	= 0;
	static int	iSavedTicks	= 0;
	static int	iLastTicks	= 0;

//	static int	iArr[ 10 ] ;
//	static int	iIdx		= 0;

	int			iTicks;
	int			iSecs;

//	BOOL		fRet;
//	int			iMilli;
//	FILETIME	ft;
//	SYSTEMTIME	st;

	if( iSavedTime  ==  0 )
	{
		Sleep( 0 );		//  give up the remainder of our current timeslice so we (hopefully)
						//	won't get interrupted between the next two function calls.
		iSavedTime = time( NULL );
		iSavedTicks = GetTickCount( );
	}

	iTicks = GetTickCount( );
	if( iTicks  <  iLastTicks )
	{
		//  GetTickCount( ) wrapped!!
		//  Determine number of seconds that have elapsed during the last 2^32 milliseconds and add that to iSavedTime
		//  Reset iSavedTicks to zero and continue
	}

	iTicks -= iSavedTicks;
	iSecs = iTicks / 1000;
	pTb->time = iSavedTime + iSecs;
	pTb->millitm = iTicks - (iSecs * 1000);
	iLastTicks = iTicks;
//	iArr[ iIdx++ ] = pTb->millitm;
//	if( iIdx  >=  20 )
//	{
//		printf( "_ftime( ) last 20 millitm are\n%3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d\n%3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d\n",
//				iArr[ 0 ], iArr[ 1 ], iArr[ 2 ], iArr[ 3 ], iArr[ 4 ], iArr[ 5 ], iArr[ 6 ], iArr[ 7 ], iArr[ 8 ], iArr[ 9 ],
//				iArr[ 10 ], iArr[ 11 ], iArr[ 12 ], iArr[ 13 ], iArr[ 14 ], iArr[ 15 ], iArr[ 16 ], iArr[ 17 ], iArr[ 18 ], iArr[ 19 ] );
//		iIdx = 0;
//	}

	
//	//  get SYSTEMTIME
//	GetSystemTime( &st );
//	printf( "GetSystemTime( ) returned milliseconds of %d\n", st.wMilliseconds );
//	//  convert SYSTEMTIME to FILETIME
//	fRet = SystemTimeToFileTime( &st, &ft );
//	//  convert FILETIME to UnixTime
//	pTb->time = FileTimeToUnixTime( &ft, &iMilli );
//	pTb->millitm = iMilli;
////	printf( "_ftime( ) is about to return %d - - -  millitm is %d\n", pTb->time, pTb->millitm );
}


//****************************************************************
size_t __cdecl strftime( char *strDest, size_t maxSize, const char *pformat, const struct tm *timeptr )
{
	printf( "strftime( ) NOT IMPLEMENTED\n" );
	return 0;
}


//****************************************************************
long RegQueryValueExB( HKEY hKey, const char *lpName, DWORD *lpReserved, DWORD *lpType, unsigned char *lpData, DWORD *lpcbData )
{
	printf( "RegQueryValueExB( ) NOT IMPLEMENTED\n" );
	return 0;
}





//****************************************************************
HMODULE
WINAPI
LoadLibraryExA( const char *pIn, HANDLE hIn, DWORD dwIn )
{
	printf( "LoadLibraryExA( ) NOT IMPLEMENTED - <filename> is *%s*\n", pIn );
	return NULL;
}



//****************************************************************
HINSTANCE
LoadLibraryA( LPCSTR pIn )
{
	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( pIn )
	{
		printf( "entering LoadLibraryA( *%s* )\n", pIn );
		iRet = MultiByteToWideChar( CP_ACP, 0, pIn, strlen( pIn ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
		printf( "  Unicode version of name is (upper S) *%S*\n", pW );
	}
	else
		printf( "entering LoadLibraryA( <NULL> )\n" );

	if( iRet )
	{
		HINSTANCE hRet;
		hRet = LoadLibraryW( pW );
		printf( "   LoadLibraryA( )returning %8.8X\n", hRet );
		return hRet;
	}
	else
	{
		printf( "   LoadLibraryA( )returning NULL\n" );
		return NULL;
	}
}



//****************************************************************
HMODULE
WINAPI
GetModuleHandleA( LPCSTR pIn )
{
	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( pIn )
	{
		iRet = MultiByteToWideChar( CP_ACP, 0, pIn, strlen( pIn ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
	}
	if( iRet )
	{
		return GetModuleHandleW( pW );
	}
	else
		return NULL;
	
}



//****************************************************************
void OutputDebugStringB( const char *pC )
{
	printf( pC );
//	printf( "OutputDebugStringA( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
}


//****************************************************************
//void WINAPI OutputDebugStringW( const unsigned short *pC )
//{
//	printf( "OutputDebugStringW( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
//}


//****************************************************************
int rmdir( const char * dirname )
{
	printf( "rmdir( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return -1;
}


//****************************************************************
char * getcwd( char *buffer, int maxlen )
{
	printf( "getcwd( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return NULL;
}


//****************************************************************
unsigned int GetSystemDirectory( char *cP, unsigned int uSize )
{
	//printf( "GetSystemDirectory( ) NOT IMPLEMENTED\n" );
    //assert( 0 );
	strcpy(cP,"\\\\Windows");
	return 0;
}

//****************************************************************
void GetSystemTimeAsFileTime( LPFILETIME pFT )
{
//	printf( "GetSystemTimeAsFileTime( ) NOT IMPLEMENTED\n" );
	SYSTEMTIME	st;
	//  get SYSTEMTIME
	GetSystemTime( &st );
	//  convert SYSTEMTIME to FILETIME
	SystemTimeToFileTime( &st, pFT );
}


//****************************************************************
void WINAPI perror( char *pC )
{
	printf( "perror( ) NOT IMPLEMENTED\n" );
	//  Just a stub...
}


//****************************************************************
int SetErrorMode( int iUnused )
{
	printf( "SetErrorMode( ) NOT IMPLEMENTED\n" );
	//  Just a stub...
	return 0;
}


//****************************************************************
char * WINAPI strerror( int iErrNo )
{
	printf( "strerror( ) NOT IMPLEMENTED\n" );
	//  Just a stub...
	static char cErrMsg[ 100 ] = { 0 };

	if( iErrNo  ||  strlen( cErrMsg ) == 0 )
		sprintf( cErrMsg, "Unable to lookup error message for error %d\n", iErrNo );

	return cErrMsg;
}


//****************************************************************
int _chdir( const char *p1 )
{
	printf( "_chdir( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return 0;
}


//****************************************************************
int _mkdir( const char *p1 )
{
	printf( "_mkdir( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return 0;
}


//****************************************************************
int rename( const char *p1, const char *p2 )
{
	printf( "rename( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return 0;
}


//****************************************************************
int remove( const char *p1 )
{
	printf( "remove( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return 0;
}

//#define _O_RDONLY		0x0000
//#define _O_WRONLY		0x0001
//#define _O_RDWR		0x0002
//#define _O_APPEND		0x0008
//
//#define _O_CREAT		0x0100
//#define _O_TRUNC		0x0200
//#define _O_EXCL		0x0400

//int _wopen( const wchar_t *filename, int oflag );


//****************************************************************
int _open( const char *filename, int oflag )
{
	printf( "_open( ) NOT IMPLEMENTED\n" );
	return -1;

#if 0
	printf( "entering _open( ) - filename is *%s*\n", filename );

	wchar_t	wBuf[ MAX_PATH + 1 ];
	wchar_t	*pW			= NULL;
	int		iRet		= 1;
	if( filename )
	{
		iRet = MultiByteToWideChar( CP_ACP, 0, filename, strlen( filename ), wBuf, MAX_PATH );
		wBuf[ iRet ] = 0;
//	printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//	printf( "  wBuf is *%S*\n", wBuf );
		pW = wBuf;
	}
	if( iRet )
	{
		return _wopen( pW, oflag );
	}
	else
		return -1;
#endif
}


//****************************************************************
int _close( int fd )
{
	printf( "_close( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return -1;
}


//****************************************************************

//typedef __int64 intptr_t;


//****************************************************************
void * _findclose( long hFile )
{
	printf( "_findclose( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return NULL;
}


//****************************************************************
intptr_t _findnext( long hFile, struct _finddata_t *pFD )
{
	printf( "_findnext( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return -1;
}



//****************************************************************
intptr_t _findfirst( const char *pName, struct _finddata_t *pFD )
{
	printf( "_findfirst( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return -1;
}



//****************************************************************
HANDLE GetStdHandle( int nStdHandle )
{
	printf( "GetStdHandle( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return INVALID_HANDLE_VALUE;
}


//****************************************************************
int _putenv( const char *pIn )
{
	printf( "_putenv( ) NOT IMPLEMENTED\n" );
//	assert( 0 );
	return -1;
}

// Have to build stuff not just from WinBase but also from WinUser.h

//****************************************************************
int PostThreadMessageA(unsigned long idThread,unsigned int Msg,
                      unsigned int wParam,
                      long lParam) 
{
	int	iRet;

	if( wParam  ==  NULL )
	{
		iRet = PostThreadMessageW( idThread, Msg, wParam, lParam );
		printf( "PostThreadMessageA( ) - nothing to translate - PostThreadMessageW( ) returned %d\n", iRet );
		return iRet;
	}
	else
	{
		printf( "PostThreadMessageA( ) - Msg is 0x%8.8X,  wParam is 0x%8.8X,  *wParam = 0x%8.8X\n", Msg, wParam, *((DWORD *)wParam) );
		iRet = PostThreadMessageW( idThread, Msg, wParam, lParam );
		printf( "PostThreadMessageA( ) - PostThreadMessageW( ) returned %d\n\n", iRet );
		return iRet;
	}

	return 0;
}


//***************************************************************
int GetMessageA( LPMSG lpMsg, HWND hWnd, unsigned int wMsgFilterMin, unsigned int wMsgFilterMax ) 
{
//	printf( "GetMessageA( ) NOT IMPLEMENTED\n" );
    int iRet;
    iRet = GetMessageW( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax );
    printf( "GetMessageW( ) just returned %d\n", iRet );
    return iRet;
};



/**
  CreateEventA Redefined

  The header redefines this ison purpose and the code will not work
  for other librariesif this is not done.
  #undef CreateEventA
  #define CreateEventA CE_CreateEventA

**/

HANDLE CE_CreateEventA(
					  LPSECURITY_ATTRIBUTES lpEventAttributes, 
					  BOOL bManualReset, 
					  BOOL bInitialState, 
					  LPTSTR lpName) 
{
//	printf( "CE_CreateEventA( ) NOT IMPLEMENTED\n" );
    wchar_t wBuf[ MAX_PATH + 1 ];
   	wchar_t	*pW  = NULL;
    int     iRet = 1;
    if( lpName )
    {
        iRet = MultiByteToWideChar( CP_ACP, 0, lpName, strlen( lpName ), wBuf, MAX_PATH );
        wBuf[ iRet ] = 0;
//  printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//  printf( "  wBuf is *%S*\n", wBuf );
        pW = wBuf;
    }
    if( iRet )
    {
        HANDLE hRet;
        hRet = CreateEventW( lpEventAttributes, bManualReset, bInitialState, pW );
        printf( "CE_CreateEventA( *%S* ) returned %8.8X\n", pW, hRet );
        return hRet;
    }
    else
    {
        printf( "CE_CreateEventA( *%S* ) returned NULL\n", pW );
        return NULL;
    }
    return NULL;
}


/**
  RegOpenKeyExA Redefined

  The header redefines this ison purpose and the code will not work
  for other librariesif this is not done.
  #undef  RegOpenKeyExA
  #define RegOpenKeyExA CE_RegOpenKeyExA

**/
long CE_RegOpenKeyExA (	 HKEY hKey,
						 LPCSTR lpSubKey,
						 DWORD ulOptions,
						 REGSAM samDesired,
						 PHKEY phkResult
						)
{
	printf( "CE_RegOpenKeyExA( ) NOT IMPLEMENTED\n" );
	return NULL;
}


/* Came from mmsystem.h */
MMRESULT WINAPI timeSetEvent( UINT           uDelay,      
                              UINT           uResolution, 
                              LPTIMECALLBACK lpTimeProc,  
                              DWORD_PTR      dwUser,      
                              UINT           fuEvent      
                            )
{
	printf( "timeSetEvent( ) NOT IMPLEMENTED\n" );
	return NULL;
}

#ifdef __cplusplus
extern "C" int _getpid()
#else
extern int  _getpid();
#endif
{
	return 1;
}
