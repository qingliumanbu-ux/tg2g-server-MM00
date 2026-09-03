/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    郝东炜
Version:    1.0
Date:     2015-7-30 10:31:34
Description: MM0097C1规则明细信息_顺序调整
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"


//程序用头文件

#define zero 0.00001

int f_mm0097c1f8_seq(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);
//-EP_CODE_VERSION 1
//-EP_SYSTEM_HEAD_BEGIN                                              
//-此节代码请勿更改 
/*<remark>=========================================================
/// <summary>
/// 调整转用充当规则
/// <para>
 
</para>
/// </summary>
/// <param name="RULE_CODE">规范代码  </param>
/// <param name="RULE_SEQ_NO">规则序号  </param>
===========================================================</remark>*/                                                  
// service入口
BM2F_ENTERACE(mm0097c1f8_seq)
//-EP_SYSTEM_HEAD_END                                                
int f_mm0097c1f8_seq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/*定义函数名*/
	CString FunctionEname;                 //定义函数英文名称
	CString FunctionCname;                //定义函数中文名称

	FunctionEname = "f_mm0097c1f8_seq";  //赋值函数英文名称

	//程序用变量
	int doFlag = 0;
	int fetchRowCount  = 0;
	int i;
	 
	CString v_time_cur = "";
	CString v_person = "";
	int  v_seq_now = 0;  
	int  v_cnt = 0;

	CString systime = "";
	CString userid = "";   
	CString v_keyvalue_6 = "";
	CString v_oper_flag = "0";  //'oper_flag'= 0/1=一般用户操作/MES操作
	 
	CModel tmm009c("TMM009C");
	CDbCommand cmd_inq(conn);
	CString sqlstr;

	try
	{
		//获得当前时间
		systime=CDateTime::Now().ToString("yyyyMMddHHmmss");//systime  
		//获得userid
		userid = s.userid;   

		//获取2#BLK的信息,操作标志= 0/1=一般用户操作/MES操作
		v_oper_flag = bcls_rec->Tables[1].Rows[0]["OPER_FLAG"];             

		//对输入信息循环处理
		v_seq_now = 0; //初始化当前序号
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{     
			//取得单行传入信息
			tmm009c.MergeFrom(bcls_rec->Tables[0].Rows[i-1]);   
			tmm009c["REC_REVISE_TIME"] = systime;
			tmm009c["REC_REVISOR"] = s.userid;	
			tmm009c.TrimOrBlank();      
			v_seq_now++;

			//修改 ,记录一个规则组内的序号  
			//======
			tmm009c["KEYVALUE_1"]=CConvert::ToString(v_seq_now);
			sqlstr = "update tmm009c ";
			tmm009c.Update("REC_REVISE_TIME,REC_REVISOR,KEYVALUE_1","RULE_CODE,RULE_SEQ_NO");
		}

		//一组(物料类型+成品标志)下序号的调整。
		 
		sqlstr="UPDATE TMM009C SET RULE_SEQ_NO = TO_NUMBER(KEYVALUE_1) WHERE RULE_CODE = @tmm009c.RULE_CODE";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm009c.RULE_CODE",tmm009c["RULE_CODE"].ToString());
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close(); 

		//返回规则代码。
		//========== 
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "RULE_CODE");
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["RULE_CODE"] = tmm009c["RULE_CODE"];
		 
		doFlag = 0; 
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
	return doFlag;

}
