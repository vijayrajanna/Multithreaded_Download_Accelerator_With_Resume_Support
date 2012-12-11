#define PACKET_SIZE		600
#define MAX_JOBS		20
#define PACKET_CHUNK_SIZE	(PACKET_SIZE*100)

#define SUCCESS		0
#define FAILURE		1

enum
{
	FETCH_CONTENTS,		//0
	FILE_DETAILS,
	END,				//2
	CHDIR,
	GET_CWD,			//4
	CWD,
	ACK,				//6
	GET,
	PUT,				//8
	DATA,
	REPLY,				//10
	GET_SIZE,
	SIZE,				//12
	GET_CRC,			
	CRC					//14
};
	

enum
{
		REGULAR,DIRECTORY,CHAR,BLOCK,FIFO,
		SYMLINK,SOCKET
};

struct DirEntry
{
	char fileName[256];
	int type;
	off_t size;
	int perm;
	uid_t owner;
	gid_t group;
	time_t mtime;
};

struct details
{
	char filename[256];
	char type[100];
	char perm[100];
	char owner[100];
	char group[100];
	char time[100];
	char size[100];
};

