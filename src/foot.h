void save_foot_index(DB *db){
	lseek64(db->fp, 0, SEEK_SET);
	write(db->fp, &db->foot_index, sizeof(long));
}

long get_foot_index(DB *db){
	long foot_index;
	
	lseek64(db->fp, 0, SEEK_SET);
	read(db->fp, &foot_index, sizeof(long));
	
	return foot_index;
}

void save_foot_size(DB *db){
	lseek64(db->fp, sizeof(long), SEEK_SET);
	write(db->fp, &db->foot_size, sizeof(long));
}

long get_foot_size(DB *db){
	long foot_size;
	
	lseek64(db->fp, sizeof(long), SEEK_SET);
	read(db->fp, &foot_size, sizeof(long));
	
	return foot_size;
}

void save_max_collecs(DB *db){
	lseek64(db->fp, db->foot_index, SEEK_SET);
	write(db->fp, &db->max_collecs, sizeof(u32));
}

u32 get_max_collecs(DB *db){
	u32 max_collecs;
	
	lseek64(db->fp, db->foot_index, SEEK_SET);
	read(db->fp, &max_collecs, sizeof(u32));
	
	return max_collecs;
}

void save_collecs_num(DB *db){
	lseek64(db->fp, db->foot_index + sizeof(u32), SEEK_SET);
	write(db->fp, &db->collecs_num, sizeof(u32));
}

u32 get_collecs_num(DB *db){
	u32 collecs_num;
	
	lseek64(db->fp, db->foot_index + sizeof(u32), SEEK_SET);
	read(db->fp, &collecs_num, sizeof(u32));
	
	return collecs_num;
}

void save_collec_index(DB *db, u32 num, long index){
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(2*db->max_collecs + 3) + sizeof(long)*num, SEEK_SET);
	write(db->fp, &index, sizeof(long));
}

long get_collec_index(DB *db, u32 num){
	long collec_index;
	
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(2*db->max_collecs + 3) + sizeof(long)*num, SEEK_SET);
	read(db->fp, &collec_index, sizeof(long));
	
	return collec_index;
}

void save_info_index(DB *db, u32 num, u32 index){
	lseek64(db->fp, db->foot_index + sizeof(u32)*(db->max_collecs + num + 2), SEEK_SET);
	write(db->fp, &index, sizeof(u32));
}

u32 get_info_index(DB *db, u32 num){
	u32 collec_info_index;
	
	lseek64(db->fp, db->foot_index + sizeof(u32)*(db->max_collecs + num + 2), SEEK_SET);
	read(db->fp, &collec_info_index, sizeof(u32));
	
	return collec_info_index;
}

void save_recs_num(DB *db, u32 num, long recs_num){
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(2*db->max_collecs + 3) + (long)sizeof(long)*(db->max_collecs + num + 1), SEEK_SET);
	write(db->fp, &recs_num, sizeof(long));
}

long get_recs_num(DB *db, u32 num){
	long recs_num;
	
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(2*db->max_collecs + 3) + (long)sizeof(long)*(db->max_collecs + num + 1), SEEK_SET);
	read(db->fp, &recs_num, sizeof(long));
	
	return recs_num;
}

void save_collec_name(DB *db, u32 num, u32 name){
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(num + 2), SEEK_SET);
	write(db->fp, &name, sizeof(u32));
}

u32 get_collec_name(DB *db, u32 num){
	u32 name;
	
	lseek64(db->fp, db->foot_index + (long)sizeof(u32)*(num + 2), SEEK_SET);
	read(db->fp, &name, sizeof(u32));
	
	return name;
}

long get_collec_size(DB *db, u32 num){
	long n1 = get_collec_index(db, num);
	long n2 = get_collec_index(db, num + 1);
	
	return n2 - n1;
}

long get_info_size(DB *db, u32 num){
	long n1 = get_info_index(db, num);
	long n2 = get_info_index(db, num + 1);
	
	return n2 - n1;
}

void save_collec_info(Collec *collec){
	DB *db = collec->db;
	u32 num = collec->num;
	
	u32 info_index = get_info_index(db, num);
	u32 next_info_index = get_info_index(db, num + 1);
	u32 info_size = next_info_index - info_index;
	
	lseek64(db->fp, db->foot_index + info_index, SEEK_SET);
	write(db->fp, collec->column_sizes, info_size);
}

u32 *get_info(DB *db, u32 num){
	
	u32 info_size = get_info_size(db, num);
	long info_offset = db->foot_index + (long)get_info_index(db, num);
	u32 *info = malloc(info_size);
	
	lseek64(db->fp, info_offset, SEEK_SET);
	read(db->fp, info, info_size);
	
	return info;
}

u32 get_num(DB *db, u32 name){
	
	for(u32 num = 0; num < db->collecs_num; ++num){
		//printf("%u, %u\n", get_collec_name(db, num), name);
		if(get_collec_name(db, num) == name)
			return num;
	}
	
	return -1;
}

void init_foot(DB *db){
	db->foot_index = 2*sizeof(long);
	db->foot_size = sizeof(u32)*(2*db->max_collecs + 3) + sizeof(long)*(2*db->max_collecs + 1);
	
	save_max_collecs(db);
	save_collecs_num(db);
	save_collec_index(db, 0, 2*sizeof(long));
	save_info_index(db, 0, sizeof(u32)*(2*db->max_collecs + 3) + sizeof(long)*(2*db->max_collecs + 1));
}

void push_collec_info(Collec *collec) {
  DB *db = collec->db;
  
  u32 num = collec->num;
	u32 info_index = get_info_index(db, num);
	u32 info_size = sizeof(u32)*collec->columns_num;
	
	collec->index = get_collec_index(db, num);
	
	db->foot_size += (long)info_size;
	db->foot_index += collec->size;
	
	save_foot_index(db);
	save_foot_size(db);
	save_collec_name(db, num, collec->name);
	save_recs_num(db, num, 0);
	save_collec_index(db, num + 1, collec->index + collec->size);
	save_info_index(db, num + 1, info_size + info_index);
	save_collec_info(collec);
}

void shift_foot(DB *db, long num){
	shift_to_right(db, db->foot_index, num);
}

void get_db_info(DB *db){
	db->foot_index = get_foot_index(db);
	db->foot_size = get_foot_size(db);
	db->max_collecs = get_max_collecs(db);
	db->collecs_num = get_collecs_num(db);
}

u32 get_collec_info(Collec *collec){
	DB *db = collec->db;
	
	u32 num = collec->num;
	
	if(num == -1){
		printf("No such collection.\n");
		return 1;
	}
	
	u32 columns_num = get_info_size(db, num) / sizeof(u32);
	u32 *info = get_info(db, num);
	u32 rec_size = sizeof(u8);
	
	for(u32 i = 0; i < columns_num; ++i){
		rec_size += info[i];
	}
	
	long recs_num = get_recs_num(db, num);
	
	if(recs_num == -1){
		printf ("\nErr: Collection was deleted and cannot be used now.\n");
		return 1;
	}
	
	collec->recs_num = recs_num;
	collec->name = get_collec_name(db, num);
	collec->columns_num = columns_num;
	collec->column_sizes = info;
	
	collec->size = get_collec_size(db, num);
	collec->index = get_collec_index(db, num);
	
	collec->rec_size = rec_size;
	
	return 0;
}

void _print_foot(DB *db){
	
	long foot_index = get_foot_index(db);
	long foot_size = get_foot_size(db);
	
	printf("\n--------------------FOOTER INFO--------------------\n\n");
	printf("Size of footer: %ld\n", foot_size);
	printf("Index of footer: %ld\n\n", foot_index);
	
	u32 collecs[2];
	
	lseek64(db->fp, foot_index, SEEK_SET);
	read(db->fp, collecs, 2*sizeof(u32));
	
	u32 max_collecs = get_max_collecs(db);
	u32 current_collecs = get_collecs_num(db);
	
	printf("Maximum number of collections: %u\n", max_collecs);
	printf("Current number of collections: %u\n\n", current_collecs);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	for(u32 collec_num = 0; collec_num < current_collecs; ++collec_num){
		
		u8 name[sizeof(u32)+1];
		name[sizeof(u32)] = '\0';
		
		u32 nam = get_collec_name(db, collec_num);
		
		memcpy(name, &nam, sizeof(u32));
		
		u32 info_index = get_info_index(db, collec_num);
		u32 info_size = get_info_size(db, collec_num);
		
		long collec_index = get_collec_index(db, collec_num);
		long collec_size = get_collec_size(db, collec_num);
		
		u32 columns_num = info_size / sizeof(u32);
		u32 *info = get_info(db, collec_num);
		long recs_num = get_recs_num(db, collec_num);
		
		u32 rec_size = sizeof(u8);
		
		for(u32 i = 0; i < columns_num; ++i){
			rec_size += info[i];
		}
		
		long max_recs = collec_size / rec_size;
		long used_size = recs_num * rec_size;
		
		printf("|Info of collection \"%s\":\n|\n", name);
		
		printf("|\tNumber of records\t\t: %ld\n", recs_num);
		printf("|\tMax number of records\t: %ld\n|\n", max_recs);
		printf("|\tNumber of columns\t\t: %u\n",info_size);
		printf("|\tColumn Sizes\t\t\t: [%u", info[0]);
		
		for(int i = 1; i < columns_num; ++i)
			printf(", %u", info[i]);
		
		printf("]\n|\n");
		
		printf("|\tMax size of collection\t: %ld\n", collec_size);
		printf("|\tUsed size of collection\t: %ld\n|\n", used_size);
		printf("|\tIndex of collection\t\t: %ld\n|\n", collec_index);
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
	
	printf("------------------------------------------------------\n\n");
}