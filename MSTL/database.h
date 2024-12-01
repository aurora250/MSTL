#ifndef MSTL_DATABASE_H__
#define MSTL_DATABASE_H__
#include "basiclib.h"
#if MSTL_DLL_LINK__
#include "errorlib.h"
#include <mysql.h>
#include <iostream>
#include <string>
MSTL_BEGIN_NAMESPACE__

//const char* splice_char(const char* lh, const char* rh) {
//	std::string fst(lh);
//	std::string tem = fst.append(rh);
//	int len = fst.size();
//	char* fin = new char[len + 1];
//	strcpy(fin, fst.c_str());
//	return static_cast<const char*>(fin);
//}

class mysql_db {
private:
	MYSQL* mysql;
	std::string username;
	std::string db_name;
	std::string encode;
public:
	mysql_db(const char* user, const char* passwd, const char* db, unsigned int port = 3306,
		const char* host = "localhost", const char* encode = "gbk") :
		mysql(0), username(user), db_name(db), encode(encode) {
		mysql_init(mysql);
		mysql_options(mysql, MYSQL_SET_CHARSET_NAME, encode);
		MSTL_TRY__{
			Exception(mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0) == NULL,
				new SQLError(mysql_error(mysql)));
		}
		MSTL_CATCH_ERROR__{
			mysql_close(mysql);
		}
	}
	//void display() {
	//	int ret = mysql_query(mysql, splice_char("SELECT * FROM ", splice_char(db_name.c_str(), ";")));
	//	MYSQL_RES* res = mysql_store_result(mysql);
	//	MYSQL_ROW row;
	//	while (row = mysql_fetch_row(res)) {
	//		cout << row[0] << "  ";//打印ID
	//		cout << row[1] << "  ";//打印name
	//		cout << row[2] << "  ";//打印age
	//		cout << row[3] << endl;//打印score
	//	}
	//	mysql_free_result(res);
	//}
	~mysql_db() {
		mysql_close(mysql);
	}
};

MSTL_END_NAMESPACE__
#endif // MSTL_DLL_LINK__
#endif // MSTL_DATABASE_H__
