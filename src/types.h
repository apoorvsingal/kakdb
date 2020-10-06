typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned char u8;

typedef struct {
	u32 max_collecs;
	u32 collecs_num;
	
	long foot_index;
	long foot_size;
	
	char *path;
	
	int fp;
} DB;

typedef struct {
	u32 num;
	u32 name;
	
	long index;
	long size;
	
	long recs_num;
	u32 rec_size;
	
	u32 columns_num;
	u32 *column_sizes;
	
	DB *db;
} Collec;