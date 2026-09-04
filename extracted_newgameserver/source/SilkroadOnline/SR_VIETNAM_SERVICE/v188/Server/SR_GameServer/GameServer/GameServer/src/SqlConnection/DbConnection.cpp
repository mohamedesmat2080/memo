#include "DbConnection.h"

CDbConnection::CDbConnection(std::string strConnStr)
{
    m_strConnStr = strConnStr;
    m_hEnv = SQL_NULL_HENV;
    m_hConn = SQL_NULL_HDBC;
}

CDbConnection::~CDbConnection()
{

}

bool CDbConnection::Connect()
{
    SQLCHAR retConnStr[1024] = { 0 };
    SQLRETURN retCode;

    retCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate env handle" << std::endl;
        return false;
    }

    retCode = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to set ODBC version env attr" << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        return false;
    }

    retCode = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hConn);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate conn handle" << std::endl;
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        return false;
    }

    retCode = SQLDriverConnectA(m_hConn, NULL, (SQLCHAR*)m_strConnStr.c_str(), SQL_NTS, retConnStr, sizeof(retConnStr), NULL, SQL_DRIVER_NOPROMPT);
    if (!SQL_SUCCEEDED(retCode))
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        SQLFreeHandle(SQL_HANDLE_DBC, m_hConn);
        return false;
    }

    return true;
}

void CDbConnection::Disconnect()
{
    if (GetConnHandle() != SQL_NULL_HDBC)
    {
        SQLDisconnect(GetConnHandle());
        SQLFreeHandle(SQL_HANDLE_DBC, GetConnHandle());
    }

    if (GetEnvHandle() != SQL_NULL_HENV)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, GetEnvHandle());
    }

    m_hEnv = SQL_NULL_HENV;
    m_hConn = SQL_NULL_HDBC;
}

SQLHANDLE CDbConnection::GetEnvHandle() const
{
    return m_hEnv;
}

SQLHANDLE CDbConnection::GetConnHandle() const
{
    return m_hConn;
}

std::string CDbConnection::GetConnString() const
{
    return m_strConnStr;
}

bool CDbConnection::AllocStmt(SQLHANDLE& hStmt)
{
    if (GetEnvHandle() == SQL_NULL_HENV || GetConnHandle() == SQL_NULL_HDBC)
    {
        std::cout << __FUNCTION__ << " - Cannot allocate stmt (null env/conn handle)" << std::endl;
        return false;
    }

    const SQLRETURN retCode = SQLAllocStmt(GetConnHandle(), &hStmt);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        return false;
    }

    return true;
}

bool CDbConnection::FreeStmt(SQLHANDLE& hStmt)
{
    if (hStmt == SQL_NULL_HSTMT)
        return true;

    SQLRETURN retCode = SQLFreeStmt(hStmt, SQL_CLOSE);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to free stmt" << std::endl;
        return false;
    }

    retCode = SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to free stmt handle" << std::endl;
        return false;
    }

    return true;
}