#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "src/main.h"

struct users_rec {
	u8 key;
	u8 name[10];
	u8 age;
};

#define NUM_OF_COLLECTIONS 2

DB *db = NULL;
Collec *users = NULL;
Collec *blacklist = NULL;

long users_max_rec_num = 256;
u32 users_num_of_columns = 3;
u32 users_sizes_of_columns[3] = { sizeof(u8), 10 * sizeof(u8), sizeof(u8) };

long blacklist_max_rec_num = 256;
u32 blacklist_num_of_columns = 1;
u32 blacklist_sizes_of_columns[1] = { sizeof(u8) };

void setup_db(){
	
	db = create_db("./db", NUM_OF_COLLECTIONS);

	init_db(db);
	
	users = create_collec(db, (u32 *)"usrs", users_sizes_of_columns, users_num_of_columns, users_max_rec_num);
	blacklist = create_collec(db, (u32 *)"blst", blacklist_sizes_of_columns, blacklist_num_of_columns, blacklist_max_rec_num);
	
	init_collec(users);
	init_collec(blacklist);
	
	set_pos(users, 0);
	
	for(u8 x = 0; x < 32; ++x){
		
		struct users_rec rec = {
			.key = x,
			.name = "Kakashi",
			.age = 173
		};
		
		put_rec(users, (void *)&rec);
	}
	
	set_pos(blacklist, 0);
	
	for(u8 x = 0; x < 8; ++x){
		u8 key = 4*x;
		put_rec(blacklist, (void *)&key);
	}
	
	save_collec_changes(users);
	save_collec_changes(blacklist);
}

u8 key_global;

u32 blacklist_for_each(void *rec, long index){
	return *((u8 *)rec) == key_global;
}

u32 is_user_blacklisted(u8 key){
	key_global = key;
	return for_each_rec(blacklist, &blacklist_for_each);
}

u32 users_for_each(void *buff, long index){
	
	struct users_rec *rec = buff;
	char *is_blacklisted = is_user_blacklisted(rec->key) ? "true" : "false";
	
	printf("User %u { name: \"%s\", age: %u, blacklisted: %s };\n", rec->key, rec->name, rec->age, is_blacklisted);
	
	set_pos(users, index + 1);
	return 0;
}

void print_users_info(){
	_print_foot(db);
	for_each_rec(users, &users_for_each);
}

void get_db(){
	db = connect_db("./db");
	
	users = get_collec(db, (u32 *)"usrs");
	blacklist = get_collec(db, (u32 *)"blst");
}

void delete_a_rec(){
	set_pos(users, 11);
	delete_rec(users);
}

void delete_users(){
	delete_collec(users);
}

u32 print_blacklist_rec(void *rec, long index){
	printf("%i, ", *((u8 *)rec));
	return 0;
}

void print_blacklist(){
	_print_foot(db);
	printf("Blacklisted Users:\t[");
	for_each_rec(blacklist, &print_blacklist_rec);
	printf("\b\b];\n");
}

int main(){
	setup_db();
	print_users_info();
	
	//same as setup_db() but insted of creating a db, it gets the necessary info from the db itself and creates a reference to it
	get_db();
	print_users_info();
	
	//deletes user 11 which is then not printed in print_users_info();
	delete_a_rec();
	print_users_info();
	
	delete_users();
	get_db();
	print_blacklist();
}