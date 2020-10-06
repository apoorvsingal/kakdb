
void shift_to_left(DB *db, long index, long num){
	
	int fp = db->fp;
	long db_size = db->foot_index + db->foot_size;
	
	for(long i = index; i < db_size; ++i){
		u8 c;
		
		lseek64(fp, i, SEEK_SET);
		read(fp, &c, sizeof(u8));
		
		lseek64(fp, i - num, SEEK_SET);
		write(fp, &c, sizeof(u8));
	}
	
}

void shift_to_right(DB *db, long index, long num){
	
	int fp = db->fp;
	long db_size = db->foot_index + db->foot_size - 1;
	
	for(long i = db_size; i >= index; --i){
		u8 c;
		
		lseek64(fp, i, SEEK_SET);
		read(fp, &c, sizeof(u8));
		
		lseek64(fp, i + num, SEEK_SET);
		write(fp, &c, sizeof(u8));
	}
}


void shift_nbytes_to_left(DB *db, long index, long num, long n){
	
	int fp = db->fp;
	long size = index + n;
	
	for(long i = index; i < size; ++i){
		u8 c;
		
		lseek64(fp, i, SEEK_SET);
		read(fp, &c, sizeof(u8));
		
		lseek64(fp, i - num, SEEK_SET);
		write(fp, &c, sizeof(u8));
	}
	
}

void shift_nbytes_to_right(DB *db, long index, long num, long n){
	
	int fp = db->fp;
	long size = index + n - 1;
	
	for(long i = size; i >= index; --i){
		u8 c;
		
		lseek64(fp, i, SEEK_SET);
		read(fp, &c, sizeof(u8));
		
		lseek64(fp, i + num, SEEK_SET);
		write(fp, &c, sizeof(u8));
	}
}