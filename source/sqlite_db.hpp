#pragma once
#include "sqlite/sqlite3.h"
#include "common.hpp"
#include <functional>
using namespace dbpp;

/// Deleter struct-function for sqlite3_stmt
struct CheckFinalize 
{
	void operator() (sqlite3_stmt* stmt);
};

/// Common data/tools for sqlite connection and cursor
class BaseSqlite
{
protected:
	// ==== data ====
	std::shared_ptr<sqlite3> db;

	// ==== functions ====

	/// autofinalize pointer to single sql statement
	std::unique_ptr<sqlite3_stmt, CheckFinalize> 
		single_sqlite_stmt(std::string const& sql)

	{
		sqlite3_stmt* stmt;
		int rc = sqlite3_prepare_v2(db.get(), sql.c_str(), int(sql.size() + 1), &stmt, nullptr);
		if (rc != SQLITE_OK)
			throw Error("Bad SQL");

		return std::unique_ptr<sqlite3_stmt, CheckFinalize> (stmt);
	}

	void single_step(std::string const& sql)
	{
		(void)sqlite3_step(single_sqlite_stmt(sql).get());
	}
	
	BaseSqlite(std::shared_ptr<sqlite3> db_=std::shared_ptr<sqlite3>())
		: db(db_)
	{}
};


class SqliteCursor : public BaseCursor, public BaseSqlite
{

	struct BindOneParam
	{
		sqlite3_stmt* pStmt;
		int nParam;
	public:
		void operator()(const Null&);
		void operator()(const int& i);
		void operator()(const double& d);
		void operator()(const String& s);
		void operator()(const Blob& b);
	};

// ==== functions ===
    ResultRow GetRow(sqlite3_stmt* pStmt);
public:

	SqliteCursor(std::shared_ptr<sqlite3> db);

	int execute_impl(String const& query, InputRow const& data,
		std::deque<ResultRow>& resultTab, ColumnsInfo& columnsInfo) final;
};


class SqliteConnection : public BaseConnection, public BaseSqlite
{
	
public:

	~SqliteConnection();

	SqliteConnection(std::string const& connectString, std::string const& addParams);
	SqliteConnection(SqliteConnection&&) = default;

    bool autocommit() final;
    void autocommit(bool autocommitFlag) final;
    void commit() final;
    void rollback() final;

	BaseCursor *cursor() final {
		return new SqliteCursor(db);
	}
};