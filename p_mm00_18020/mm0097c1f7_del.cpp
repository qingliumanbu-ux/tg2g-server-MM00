/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则明细信息_删除
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件


int f_mm0097c1f7_del(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改

/*<remark>=========================================================
/// <summary>
/// 规则信息删除
/// <para>

</para>
/// </summary>
/// <param name="rule_code">规则代码  </param>
/// <param name="rule_seq_no">规则顺序号  </param>
===========================================================</remark>*/                                                   
// service入口
BM2F_ENTERACE(mm0097c1f7_del)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f7_del(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称
	FunctionEname = "f_mm0097c1f7_del";  //赋值函数英文名称

	//程序用变量
	int doFlag = 0;
	int fetchRowCount  = 0;
	int fetchRowCount2 = 0;
	int i=0;

	CString systime = "";
	CString userid = "";
	int  j=0;
	int  v_seq_now = 0;
	int  v_cnt = 0;
	CString v_oper_flag = "0";  //'oper_flag'= 0/1=一般用户操作/MES操作

	CModel tmm009c("TMM009C");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString sqlstr1;
	CString sqlstr;

	try
	{
		//获得当前时间
		systime=CDateTime::Now().ToString("yyyyMMddHHmmss");

		//获得userid 
		userid = s.userid;   

		//获取2#BLK的信息,操作标志= 0/1=一般用户操作/MES操作
		//==================
		v_oper_flag = bcls_rec->Tables[1].Rows[0]["OPER_FLAG"];         

		//对输入信息循环处理
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{     
			//取得单行传入信息
			tmm009c.MergeFrom(bcls_rec->Tables[0].Rows[i-1]);
			tmm009c.TrimOrBlank();
			//操作标志= 0/1=一般用户操作/MES操作
			if(tmm009c["MATCH_FLAG"].ToString().GetAt(0) == '1' && v_oper_flag.GetAt(0) !='1')
			{
				//若当前规则是‘必选规则’ AND 操作标志 不是 MES操作，那么报错不允许。
				//sprintf(s.msg, "规则代码[%s]序号[%d]的必选标志[%s],操作级别[%s]的用户不允许进行[必选规则]的删除。",  
				//tmm009c["RULE_CODE"],tmm009c["RULE_SEQ_NO"].ToDecimal(),tmm009c["MATCH_FLAG"].ToString(),v_oper_flag);
				//_RES("PMOAS0000059")/*规则代码[{0}]序号[{1}]的必选标志[{2}],操作级别[{3}]的用户不允许进行[必选规则]的删除。*/

				{
					//带变量的信息
					CFormattable arguments[] = {tmm009c["RULE_CODE"].ToString(),tmm009c["RULE_SEQ_NO"].ToDecimal().ToInt32(),tmm009c["MATCH_FLAG"].ToString(),v_oper_flag}; // 定义参数列表的数组
					CMessageFormat::Format(s.msg, _RES("PMOAS0000059"),	arguments, 4); //格式化字符串 
				}
				throw CApplicationException(-1, s.msg, log.Location);

			}  
			//删除   
			sqlstr = " delete from TMM009C ";
			tmm009c.Delete("RULE_CODE,RULE_SEQ_NO"); 

		}

		//转用充当配置表查询
		sqlstr1 = "SELECT * \
				  FROM TMM009C \
				  WHERE RULE_CODE         = @tmm009c.RULE_CODE  \
				  ORDER BY RULE_SEQ_NO ASC";
		sqlstr = sqlstr1;
		cmd_inq1.SetCommandText(sqlstr1);
		cmd_inq1.Parameters.Set("tmm009c.RULE_CODE",tmm009c["RULE_CODE"].ToString());
		cmd_inq1.ExecuteReader();

		//顺序号的重新调整 
		//===============
		for (fetchRowCount2 = 0; cmd_inq1.Read();) 
		{
			cmd_inq1.Fetch(tmm009c);	

			tmm009c.TrimOrBlank();
			fetchRowCount2 ++;
			v_seq_now = fetchRowCount2;

			//一个规则号下序号的调整。
			//=================== 
			sqlstr="UPDATE TMM009C \
				   SET    RULE_SEQ_NO      = @v_seq_now  \
				   WHERE  RULE_CODE        = @tmm009c.RULE_CODE  \
				   AND    RULE_SEQ_NO      = @tmm009c.RULE_SEQ_NO "
				   ;	
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_seq_now",v_seq_now);
			cmd_inq.Parameters.Set("tmm009c.RULE_CODE",tmm009c["RULE_CODE"].ToString());
			cmd_inq.Parameters.Set("tmm009c.RULE_SEQ_NO",tmm009c["RULE_SEQ_NO"].ToDecimal());
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}
		cmd_inq1.Close();

		strcpy(s.msg,  _RES("GCRSS0000002"));//处理成功。  
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	cmd_inq1.Close();
	return doFlag;

}
