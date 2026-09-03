#include "MM_Prod.h"

BM2F_ENTERACE(mm0000a1_cfm)
int f_mm0000a1_cfm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDynaTable tmm0000("TMM0000", conn);

	try
	{
		CTracer log(__FUNCTION__);

		//物料跟踪 -- 新增材料信息
		if (bcls_rec->Tables.IndexOf("MM0099") < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}

		bcls_rec->Tables["MM0099"].Clear();
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();
			MergDataRow(bcls_rec->Tables[0].Rows[i], bcls_rec->Tables["MM0099"].Rows[i], true, true);
		}

		AddColValue(bcls_rec->Tables["MM0099"], "EVENT_ID", "MM02");
		AddColValue(bcls_rec->Tables["MM0099"], "EVENT_LINE_TYPE", "00");
		AddColValue(bcls_rec->Tables["MM0099"], "SYSTEM_ID", "MM" + bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().ToUpper());
		AddColValue(bcls_rec->Tables["MM0099"], "FUNC_ID", s.svc_name);
		AddColValue(bcls_rec->Tables["MM0099"], "MAT_ORIGIN", "X");

		doFlag = f_mmcr99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmm0000.CopyFrom(bcls_rec->Tables[0]);
		tmm0000.AddFilterColName("MAT_NO");

		tmm0000.AddUpdateColName("AFFIRM_FLAG");
		tmm0000.AddUpdateColName("AFFIRM_TIME");
		tmm0000.AddUpdateColName("AFFIRM_BY");

		tmm0000.SetColValAllRow("AFFIRM_FLAG", "Y");
		tmm0000.SetColValAllRow("AFFIRM_TIME", s.datetime);
		tmm0000.SetColValAllRow("AFFIRM_BY", s.userid);

		if (tmm0000.Update() < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}