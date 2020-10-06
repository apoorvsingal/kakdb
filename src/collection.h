Collec *create_collec(DB *db, u32 *name, u32 *column_sizes, u32 columns_num, u32 max_recs){

	Collec *collec = malloc(sizeof(Collec));
	u32 *_column_sizes = malloc(sizeof(u32) * columns_num);
	
  u32 rec_size = sizeof(u8);
  
  for(int x = 0; x < columns_num; ++x){
  	u32 s = *(column_sizes + x);
    rec_size += s;
    *(_column_sizes + x) = s;
  }
  
  u32 size = rec_size * max_recs;
  
  collec->name = *name;
  collec->num = db->collecs_num;
  collec->columns_num = columns_num;
  collec->rec_size = rec_size;
  collec->recs_num = 0;
  collec->column_sizes = _column_sizes;
  collec->db = db;
  collec->size = size;
	
	++db->collecs_num;
	
	return collec;
}

Collec *get_collec(DB *db, u32 *name){
	Collec *collec = malloc(sizeof(Collec));
	
	collec->db = db;
	collec->name = *name;
	collec->num = get_num(db, *name);
	
	get_collec_info(collec);
	
	if(collec->recs_num != -1){
		return collec;
	}
	
	return NULL;
}

Collec *get_collec_from_num(DB *db, u32 num){
	Collec *collec = malloc(sizeof(Collec));
	
	collec->db = db;
	collec->num = num;
	
	get_collec_info(collec);
	
	if(collec->recs_num != -1){
		return collec;
	}
	
	return NULL;
}

void delete_collec(Collec *collec){
	save_recs_num(collec->db, collec->num, -1);
}

void init_collec(Collec *collec){
	push_collec_info(collec);
	save_collecs_num(collec->db);
	shift_foot(collec->db, collec->size);
}

void set_pos(Collec *collec, long index){
	long offset = collec->index + (long)collec->rec_size * index;
	lseek64(collec->db->fp, offset, SEEK_SET);
}

void save_collec_changes(Collec *collec){
	save_recs_num(collec->db, collec->num, collec->recs_num);
}

void put_rec(Collec *collec, void *rec){
	
	u32 rec_size = collec->rec_size;
	u8 r[rec_size];
	
	r[0] = 0;
	
	memcpy(r + 1, rec, rec_size - sizeof(u8));
	
	write(collec->db->fp, r, rec_size);
	
	++collec->recs_num;
}

void get_rec(Collec *collec, void *rec){
	read(collec->db->fp, rec, collec->rec_size);
}

void delete_rec(Collec *collec){
	u8 c = 1;
	write(collec->db->fp, &c, sizeof(u8));
}

u32 for_each_rec(Collec *collec, u32 (*for_each)(void *, long)){
	
	long recs_num = collec->recs_num;
	u8 rec[collec->rec_size];
	
	set_pos(collec, 0);
		
	for(long i = 0; i < recs_num; ++i){
		get_rec(collec, (void *)rec);
		
		if(rec[0] == 0){
			if((*for_each)(rec + 1, i)) return 1;
		}
	}
	
	return 0;
}
