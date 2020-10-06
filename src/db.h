
DB *create_db(char *path, u32 max_collecs){
	
	DB *db = malloc(sizeof(DB));

	db->path = path;
	db->max_collecs = max_collecs;
	db->collecs_num = 0;
	
	return db;
}

DB *connect_db(char *path){
	
	DB *db = malloc(sizeof(DB));
		
	int fp = open(path, O_RDWR);
	
	db->path = path;
	db->fp = fp;
	
	get_db_info(db);
	
	return db;
}

void init_db(DB *db){

	db->fp = open(db->path, O_CREAT | O_RDWR);
	db->foot_index = 2*sizeof(long);
	
	init_foot(db);
	
	save_foot_index(db);
	save_foot_size(db);
}

void open_db(DB *db){
	db->fp = open(db->path, O_RDWR);
}

void close_db(DB *db){
	close(db->fp);
	free(db);
}

void clean_db(DB *db){
	
	int fp = db->fp;
	
	long foot_index = db->foot_index;
	long db_size = db->foot_index + db->foot_size;
	long foot_size = db->foot_size;
	
	u32 max_collecs = db->max_collecs;
	
	for(u32 i = 0; i < db->collecs_num; ++i){
		
		long recs_num = get_recs_num(db, i);
		
		if(recs_num == -1){
			
			long size = get_collec_size(db, i);
			long index = get_collec_index(db, i);
			long next_index = get_collec_index(db, i + 1);
			
			u32 info_index = get_info_index(db, i);
			u32 next_info_index = get_info_index(db, i + 1);
			u32 info_size = get_info_size(db, i);
			
			foot_index -= size;
			
			shift_to_left(db, next_index, size);
			shift_to_left(db, foot_index + next_info_index + info_size, info_size);
			
			shift_to_left(db, foot_index + sizeof(u32)*(2*max_collecs + 3) + sizeof(long)*(max_collecs + i + 2), sizeof(long));
			shift_to_left(db, foot_index + sizeof(u32)*(2*max_collecs + 3) + sizeof(long)*(i + 1), sizeof(long));
			shift_to_left(db, foot_index + sizeof(u32)*(max_collecs + i + 3), sizeof(u32));
			shift_to_left(db, foot_index + sizeof(u32)*(i + 3), sizeof(u32));
			
			db->collecs_num -= 1;
			db->foot_index = foot_index;
			db->foot_size -= info_size + 2*(sizeof(u32) + sizeof(long));
			
			for(u32 x = i + 1; x < max_collecs; ++x){
				long m = get_collec_index(db, x) - index;
				u32 n = get_info_index(db, x) - info_index;
				
				save_collec_index(db, x, m);
				save_info_index(db, x, n);
			}
			
			save_foot_size(db);
			save_foot_index(db);
			save_collecs_num(db);
		}
		
		else {
			Collec *collec = get_collec_from_num(db, i);
			
			u8 rec[collec->rec_size];
			
			for(long x = 0; x < recs_num; ++x){
				get_rec(collec, rec);
				
				if(rec[0] != 0){
					long n = collec->rec_size*(collec->recs_num - x);
					long d = collec->rec_size*x + collec->index;
					
					shift_nbytes_to_left(db, d, collec->rec_size, n);
					
					--collec->recs_num;
				}
				
				save_recs_num(db, i, collec->recs_num);
			}
		}
	}
}