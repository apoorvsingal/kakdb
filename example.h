#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "src/main.h"

#define NUM_OF_COLLECTIONS 2

DB *db = NULL;
Collec *users = NULL;
Collec *blacklist = NULL;

// maximum records users collection can hold
long users_max_rec_num = 256;
// number of columns in users collection
u32 users_num_of_columns = 3;
// size of each column
u32 users_sizes_of_columns[3] = { sizeof(u8), 10 * sizeof(u8), sizeof(u8) };

// alignment doesnt mess up this struct because of all types being u8
struct users_rec {
	u8 key;
	u8 name[10];
	u8 age;
};

long blacklist_max_rec_num = 256;
u32 blacklist_num_of_columns = 1;
u32 blacklist_sizes_of_columns[1] = { sizeof(u8) };

void setup_db(){
	
	// create and initialize the db
	db = create_db("./db", NUM_OF_COLLECTIONS);
	init_db(db);
	
	// create and initialize users and bloacklist collection
	users = create_collec(db, (u32 *)"usrs", users_sizes_of_columns, users_num_of_columns, users_max_rec_num);
	init_collec(users);
	
	blacklist = create_collec(db, (u32 *)"blst", blacklist_sizes_of_columns, blacklist_num_of_columns, blacklist_max_rec_num);
	init_collec(blacklist);
	
	// set the read/write position of db to 0th record of users collection
	set_pos(users, 0);
	
	for(u8 x = 0; x < 32; ++x){
		struct users_rec rec = {
			.key = x,
			.name = "Kakashi",
			.age = 173
		};
		
		// write record to the current position and increment the position to next record
		put_rec(users, (void *)&rec);
	}
	
	// set the read/write position of db to 0th record of blacklist collection
	set_pos(blacklist, 0);
	
	for(u8 x = 0; x < 8; ++x){
		u8 key = 4*x;
		put_rec(blacklist, (void *)&key);
	}
	
	// save changes to the db
	save_collec_changes(users);
	save_collec_changes(blacklist);
}

u8 key_global; // used by blacklist_for_each and is_user_blacklisted to store shared data

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
	
	// skip one record (this callback will executed with alternate records as argument) 
	set_pos(users, index + 1);
	return 0;
}

void print_users_info(){
	_print_foot(db);
	for_each_rec(users, &users_for_each);
}

void get_db(){
	// connect to an existing db
	db = connect_db("./db");
	
	// get existing collections
	users = get_collec(db, (u32 *)"usrs");
	blacklist = get_collec(db, (u32 *)"blst");
}

void delete_a_rec(){
	set_pos(users, 11);
	// delete a record (11th record is null now)
	delete_rec(users);
}

void delete_users(){
	// delete collection
	delete_collec(users);
}

u32 print_blacklist_rec(void *rec, long index){
	printf("%i, ", *((u8 *)rec));
	return 0;
}

void print_blacklist(){
	_print_foot(db);
	printf("Blacklisted Users:\t[");

	// call print_blacklist_rec with each record as argument
	for_each_rec(blacklist, &print_blacklist_rec);
	printf("\b\b];\n");
}

int main(){
	// create and initialize db
	setup_db();
	print_users_info();
	
	// same as setup_db() but insted of creating a db, it gets the necessary info from the existing db and creates a reference to it
	get_db();
	print_users_info();
	
	// deletes user 11 which is then not printed in print_users_info();
	delete_a_rec();
	print_users_info();
	
	delete_users();
	get_db();
	print_blacklist();
}