DROP DATABASE IF EXISTS test;
CREATE DATABASE test CHARACTER SET utf8;
use test;
DROP TABLE IF EXISTS urlrecord;
DROP TABLE IF EXISTS wordrecord;
CREATE TABLE urlrecord
(
	num INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
	time varchar(64),
	ip varchar(64),
	url varchar(256)
);
CREATE TABLE wordrecord
(
	num INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,
	time varchar(64),
	ip varchar(64),
	site varchar(32),
	word varchar(256)
);

