#include "sqlite_db.hpp"
#include <cstring>
// =====================   SqliteConnection =====================

SqliteConnection::~SqliteConnection()
{
	//sqlite3_close(db);
}

SqliteConnection::SqliteConnection(
	std::string const& connectString, std::string const& addParams)
{
	sqlite3 *tmp;
	int exit = sqlite3_open(connectString.c_str(), &tmp);
	db.reset(tmp, sqlite3_close);

	if (exit) {
		throw Error(sqlite3_errmsg(db.get()));
	}

	/// @todo parsing and tuning addParams
}


bool SqliteConnection::autocommit() 
{
    return !!sqlite3_get_autocommit(db.get());
}

void SqliteConnection::autocommit(bool autocommitFlag) 
{
    // Do smth if status need to change
    bool currentAutoCommit = autocommit();
    if (currentAutoCommit && !autocommitFlag)
    {
        single_step("BEGIN");
    }
    else if (!currentAutoCommit && autocommitFlag)
    {
        single_step("COMMIT");
    }
}

void SqliteConnection::commit() 
{
    if (!autocommit())
        single_step("COMMIT");
} 

void SqliteConnection::rollback() 
{
    if (!autocommit())
        single_step("ROLLBACK");
    /*
    if (!sqlite3_get_autocommit(self->db)) {
        int rc;

        Py_BEGIN_ALLOW_THREADS
        sqlite3_stmt *statement;
        rc = sqlite3_prepare_v2(self->db, "ROLLBACK", 9, &statement, NULL);
        if (rc == SQLITE_OK) {
            (void)sqlite3_step(statement);
            rc = sqlite3_finalize(statement);
        }
        Py_END_ALLOW_THREADS

        if (rc != SQLITE_OK) {
            (void)_pysqlite_seterror(self->state, self->db);
            return NULL;
        }

    }

*/
} 

// =============== SqliteCursor =====================

SqliteCursor::SqliteCursor(std::shared_ptr<sqlite3> db_)
	: BaseSqlite(db_)
{
}


ResultRow SqliteCursor::GetRow(sqlite3_stmt* pStmt)
{
    ResultRow row;
    int nData = sqlite3_data_count(pStmt);
    for (int i = 0; i < nData; i++)
    {
        int type = sqlite3_column_type(pStmt, i);
        switch (type)
        {
        case SQLITE_INTEGER: {
            row.emplace_back(sqlite3_column_int(pStmt, i));
            break; }
        case SQLITE_FLOAT: {
            row.emplace_back(sqlite3_column_double(pStmt, i));
            break; }
        case SQLITE_BLOB: {
            int n = sqlite3_column_bytes(pStmt, i);
            Blob blob(n);
            std::memcpy(&blob[0], sqlite3_column_blob(pStmt, i), n);
            row.push_back(std::move(blob));
            break; }
        case SQLITE_NULL: {
            row.emplace_back(null);
            break; }
        case SQLITE_TEXT: {
            row.emplace_back(String(
                (char*)sqlite3_column_text(pStmt, i)));
            break; }
        default:
            break;
        }
    }
    return row;
}

void SqliteCursor::BindOneParam::operator()(const Null&)
{
    sqlite3_bind_null(pStmt, nParam);
}

void SqliteCursor::BindOneParam::operator()(const int& i)
{
    sqlite3_bind_int(pStmt, nParam, i);
}

void SqliteCursor::BindOneParam::operator()(const double& d)
{
    sqlite3_bind_double(pStmt, nParam, d);
}

void SqliteCursor::BindOneParam::operator()(const String& s)
{
    sqlite3_bind_text(pStmt, nParam, s.c_str(), (int)s.size(), 
        SQLITE_TRANSIENT);
}

void SqliteCursor::BindOneParam::operator()(const Blob& b)
{
    sqlite3_bind_blob(pStmt, nParam, b.empty() ? nullptr : &b[0],
        (int)b.size(), SQLITE_TRANSIENT);
}


int SqliteCursor::execute_impl(String const& sql, InputRow const& data, 
    std::deque<ResultRow>& resultTab, ColumnsInfo& columnsInfo)
{
	const char* zSql = sql.c_str();
	int rc = SQLITE_OK;         /* Return code */
	const char* zLeftover;      /* Tail of unprocessed SQL */
	sqlite3_stmt* pStmt = 0;    /* The current SQL statement */
    //char** pzErrMsg;
    void* pArg = this;  // FIXME remove

	// FIXME checking  sqlite3_finalize()

	// Cycle for SQL single statements
	while (rc == SQLITE_OK && zSql[0]) {
		int nCol = 0;
		char** azVals = 0;
		pStmt = nullptr;
		rc = sqlite3_prepare_v2(db.get(), zSql, -1, &pStmt, &zLeftover);
		if (rc != SQLITE_OK) {
			continue;
		}
		if (!pStmt) {
			/* this happens for a comment or white-space */
			zSql = zLeftover;
			continue;
		}

		// binding input data
        if (int paramsCount = sqlite3_bind_parameter_count(pStmt))
        {
            if (paramsCount != data.size())
            {
                ; // fixme ������
            }

            for (int n = 0; n < paramsCount; n++)
            {
                    std::visit(BindOneParam{ pStmt, n + 1 }, data[n]);
            }
        }
		
		// Cycle for result tab
		for (;;)
		{
			rc = sqlite3_step(pStmt);
			if (rc != SQLITE_ROW)
				break;
			if (columnsInfo.empty())  // fill columns names in first step
			{
				int nCol = sqlite3_column_count(pStmt);
                for (int i = 0; i < nCol; i++)
                {
                    OneColumnInfo oki = { sqlite3_column_name(pStmt, i) };
                    columnsInfo.push_back(oki);
                }
			}
			resultTab.push_back(std::move(GetRow(pStmt)));
		}
		rc = sqlite3_finalize(pStmt);
		zSql = zLeftover;
    }  // Cycle for all SQL single statements

    // FIXME check RC and throw exception
    return -1;
}

void CheckFinalize::operator()(sqlite3_stmt* stmt)
{
    // FIXME ������� � ��������� RC
    int rc = sqlite3_finalize(stmt);
}
