
#include "stdafx.h"
//#include "Log.h"

// Service 入口
BM2F_ENTERACE(mm00hrqc_inq)
int f_mm00hrqc_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */	
	int doFlag = 0;

	CString sqlstr("");

	try
	{
		CDbCommand cmd_count(conn);//总行数
		CDbCommand cmd_inq(conn);//查询
		//记录总条数
		CDecimal rc = 0;
		//分页参数
		int nStart = (int)bcls_rec->Tables[0].Rows[0]["START"];
		int nPageSize = (int)bcls_rec->Tables[0].Rows[0]["PAGE_SIZE"];

		CString sqlCount = "SELECT COUNT(*) FROM TMM00HRQC";

		CString sqlSelect = "SELECT * FROM TMM00HRQC";

		//计算行数
		cmd_count.SetCommandText(sqlCount);
		sqlstr = sqlCount;
		rc = cmd_count.ExecuteScalar();
		if(rc <= 0)
		{
			strcpy(s.msg, _RES("GCRSS0000013")/*没有满足条件的记录。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/*返回记录数，分页显示用*/
		bcls_ret->ExtendedProperties.Add("RECORD_TOTAL", rc.ToString());
		cmd_inq.SetCommandText(sqlSelect);		sqlstr = sqlSelect ;
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],nStart,nPageSize);
		bcls_ret->Tables[0].set_TableName("TMM00HRQC");	

		if (bcls_ret->Tables[0].Rows.get_Count() > 0)
		{
			strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = {"TMM00HRQC",ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000017")/*读取数据失败，表[{0}]，sqlcode=[{1}]。请联系系统维护人员*/, arguments, 2);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		Log::Error("MM00", "mm00hrqc_inq","查询失败","TMM00HRQC","SELECT", ex.GetCode());
		s.flag = -1; 
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch(const CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	Log::Trace("","mm00hrqc_inq","Program End:doFlag[{0}]s.msg[{1}],s.sysmsg[{2}]",doFlag,s.msg,s.sysmsg);
	//返回-1时事务将回滚，返回为0是事务将提交
	bcls_ret->SetSYS(s);
	return(doFlag);
}