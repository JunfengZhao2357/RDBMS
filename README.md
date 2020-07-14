# Relational DataBase Management System
-------
### This database system has two modes:
* Interactive mode
* "Script" mode

Interactive mode is where the program waits for you to provide input and then processees it like help, version,quit.   

Script mode runs when you run your application from the command line, and pass a script as the first argument. In this mode, the program tries to run the given script, and then quits automatically.For example:
* create database : create database city
* drop databas : drop database city
* use database : use database city
* describe database : describe database city
* show databases : show databases
  

Table Operation:
* create table:  
CREATE TABLE tasks (  
  id INT AUTO_INCREMENT PRIMARY KEY,  
  title VARCHAR(100) NOT NULL,  
  price FLOAT DEFAULT 0.0,  
  due_date TIMESTAMP, //2020-04-15 10:11:12  
  status BOOLEAN DEFAULT FALSE,  
)  
* show tables:    
```
+----------------------+    
| Tables_in_mydb       |    
+----------------------+   
| groups               |
| users                |
+----------------------+
2 rows in set 
```
* drop table "table-name"
* describe "table-name":
```
> DESCRIBE tasks;
+-----------+--------------+------+-----+---------+-----------------------------+
| Field     | Type         | Null | Key | Default | Extra                       |
+-----------+--------------+------+-----+---------+-----------------------------+
| id        | integer      | NO   | YES | NULL    | auto_increment primary key  |
| title     | varchar(100) | NO   |     | NULL    |                             |
| price     | float        | YES  |     | 0.0     |                             |
| due_date  | date         | YES  |     | NULL    |                             |
| status    | boolean      | YES  |     | FALSE   |                             |
+-----------+--------------+------+-----+---------+-----------------------------+
5 rows in set
```
* insert into "table name"(....) values (...),(...);  
insert into users (field1, field2...) values (record-1-values), (record-2-values);

* delete from "table name";
* select ... from "table name" order by/where/limit
```
> SELECT id, first_name, last_name from Users order by first_name LIMIT 3
+--------------------+--------------+
| id  | first_name   | last_name    |
+-----+--------------+--------------+
| 1   | chandhini    | grandhi      |
| 3   | rick         | gessner      |
| 2   | savya        |              |
+-----+--------------+--------------+
3 rows in set 
```

* Update:    
UPDATE users SET state='CA' WHERE zipcode='92127' 


### This Database has two levels storage system:
* One storage system is that storing database file into the disk;
*Another storage system is that storing updated database in cache(new class for database)

### This Database has indexing system:
* Indexing system is designed to reduce the searching time when selecting or updating database.