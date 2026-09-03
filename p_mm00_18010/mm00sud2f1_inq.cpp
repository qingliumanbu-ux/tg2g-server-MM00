/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      KE1809
Version:     1.0
Date:        2022-07-06 10:33:43
Description: 查询在制品流向统计信息明细
**************************************************/
/*<remark>=========================================================
/// <summary>
/// 查询在制品流向统计信息明细
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表         </para>
/// </summary>
/// <param name="TABLE_NAME">物料形态</param>
/// <param name="UNIT_CODE">产出机组</param>
/// <param name="MAT_STATUS">材料状态</param>
/// <returns>查询在制品流向统计信息明细</returns>
===========================================================</remark>*/


#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//名称空间引用
// Service 入口
BM2F_ENTERACE(mm00sud2f1_inq)

int f_mm00sud2f1_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
    CTracer log(__FUNCTION__);
    /* 程序内部变量 */
    int doFlag = 0;
    int i = 0;
    CString sqlstr = "";
    CString strSql1 = "";
    CString strSql2 = "";
    CDateTime dt1 = CDateTime::Now();
    //	int kk = 0;
    try
    {
        CDbCommand cmd(conn);
        CDbCommand cmdinq(conn);

        CPageInfo pageInfo;
        if (bcls_rec->Tables.Contains("PageInfo"))
            pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
        else
        {
            pageInfo.RecordFrom = 0;
            pageInfo.PageSize = -1;
        }

        CString v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
        CString v_dest = bcls_rec->Tables[0].Rows[0]["DEST"].ToString().Trim();
        CString v_status = bcls_rec->Tables[0].Rows[0]["STATUS"].ToString().Trim();

        //v_mat_kind
        if (v_mat_kind == "板坯")
        {
            strSql1 = " SELECT COUNT(1) FROM TMMSM01 WHERE  MAT_STATUS = @dest ";
            strSql2 = " SELECT * FROM TMMSM01 WHERE MAT_STATUS = @dest ";

            if (v_status == "有委托")
            {
                strSql1 += " AND ORDER_NO > ' ' ";
                strSql2 += " AND ORDER_NO > ' ' ";
            }
            else if (v_status == "无委托")
            {
                strSql1 += " AND ORDER_NO = ' ' ";
                strSql2 += " AND ORDER_NO = ' ' ";
            }
        }

        else if (v_mat_kind == "热卷")
        {
            if (v_dest.Trim().GetLength() == 0)//去向、下游机组为空
            {
                strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE 1=1 ";
                strSql2 = " SELECT * FROM TMMHR01 WHERE 1=1 ";

                if (v_status == "有委托")
                {
                    strSql1 += " AND ORDER_NO > ' '  ";
                    strSql2 += " AND ORDER_NO > ' '  ";
                }
                else if (v_status == "无委托")
                {
                    strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' "
                        ;
                    strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' "
                        ;
                }
                else if (v_status == "返回卷")
                {
                    strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' AND SUBSTR(PLAN_NO,1,4) = ' ' AND WHOLE_BACKLOG_CODE = ' ' ";
                    strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' AND SUBSTR(PLAN_NO,1,4) = ' ' AND WHOLE_BACKLOG_CODE = ' ' ";
                }
            }

            else if (v_dest.Trim().GetLength() == 2)//有去向
            {
                strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE 1=1 ";
                strSql2 = " SELECT * FROM TMMHR01 WHERE 1=1 ";

                if (v_status == "有委托")
                {
                    strSql1 += " AND ORDER_NO > ' ' AND MAT_STATUS = @dest ";
                    strSql2 += " AND ORDER_NO > ' ' AND MAT_STATUS = @dest ";
                }
                else if (v_status == "无委托")
                {
                    strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND MAT_STATUS = @dest ";
                    strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND MAT_STATUS = @dest ";
                }
                //				else if(v_status == "返回卷")
                //				{
                //					strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' ";
                //					strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' ";
                //				}
            }

            else if (v_dest.Trim().GetLength() == 4)//有下游机组
            {
                strSql1 = " SELECT COUNT(1) FROM TMMHR01 WHERE 1=1 ";
                strSql2 = " SELECT * FROM TMMHR01 WHERE 1=1 ";

                if (v_status == "有委托")
                {
                    strSql1 += " AND ORDER_NO > ' ' AND MAT_STATUS = @dest ";
                    strSql2 += " AND ORDER_NO > ' ' AND MAT_STATUS = @dest ";
                }
                else if (v_status == "无委托")
                {
                    strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND MAT_STATUS = @dest ";
                    strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND MAT_STATUS = @dest ";
                }
                else if (v_status == "返回卷")
                {
                    strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' "
                        " AND (SUBSTR(PLAN_NO,1,4) = @dest AND SUBSTR(PLAN_NO,1,4) <> ' ' "
                        " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @dest)) "
                        ;
                    strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' "
                        " AND (SUBSTR(PLAN_NO,1,4) = @dest AND SUBSTR(PLAN_NO,1,4) <> ' ' "
                        " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @dest)) "
                        ;
                }
            }
        }

        else if (v_mat_kind == "冷卷")
        {
            strSql1 = " SELECT COUNT(1) FROM TMMCR01 WHERE 1=1 ";
            strSql2 = " SELECT * FROM TMMCR01 WHERE 1=1 ";

            if (v_status == "有委托")
            {
                strSql1 += " AND ORDER_NO > ' ' AND (MAT_STATUS = @dest) ";
                strSql2 += " AND ORDER_NO > ' ' AND (MAT_STATUS = @dest) ";
            }
            else if (v_status == "无委托")
            {
                strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND (MAT_STATUS = @dest) ";
                strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND (MAT_STATUS = @dest) ";
            }
            else if (v_status == "返回卷")
            {
                strSql1 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' "
                    " AND (SUBSTR(PLAN_NO,1,4) = @dest AND SUBSTR(PLAN_NO,1,4) <> ' ' "
                    " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @dest)) "
                    ;
                strSql2 += " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' "
                    " AND (SUBSTR(PLAN_NO,1,4) = @dest AND SUBSTR(PLAN_NO,1,4) <> ' ' "
                    " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @dest)) "
                    ;
            }
        }

        cmd.Parameters.Set("dest", v_dest);
        cmdinq.Parameters.Set("dest", v_dest);

        Log::Trace("MM00", "mm00sud2f1_inq", "------strSql1{0}--------", (const char*)strSql1);
        cmd.SetCommandText(strSql1);
        CDecimal nRecCount = cmd.ExecuteScalar();//总记录数
        cmd.Close();


        //查询数据并返回前台
        /*此方法返回单行单列数据*/
//			strSql2 = "SELECT * FROM  ";

//		cmdinq.Parameters.Set("", );

        cmdinq.SetCommandText(strSql2);
        Log::Trace("MM00", "mm00sud2f1_inq", "------nRecCount{0}--------", nRecCount.ToInt32());

        cmdinq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
        cmdinq.Close();

        bcls_ret->Tables.Add();
        bcls_ret->Tables[1].Columns.Add(DT_STRING, "total_count");
        bcls_ret->Tables[1].Rows.Add();
        bcls_ret->Tables[1].Rows[0]["total_count"] = nRecCount;

        {
            CFormattable arguments[] = { nRecCount };

            CMessageFormat::Format(s.msg, _RES("GCRSS0000004")/*查询到[{0}]条记录。*/, arguments, 1);
        }

    }
    catch (CDbException& ex)  //捕获数据库操作异常
    {
        CFormattable arguments[] = { ex.GetCode() };
        CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
        CString str = sqlstr + "\r\n" + ex.GetMsg();
        strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
        s.flag = -1;
        doFlag = -1;      //数据库异常时返回-1，事务将被回滚
    }
    catch (CApplicationException& ex)  //捕获应用错误
    {
        s.flag = ex.GetCode();
        doFlag = -1;
    }
    catch (CException& ex)
    {
        strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
        s.flag = ex.GetCode();
        doFlag = -1;
    }

    CTimeSpan ts = CDateTime::Now() - dt1;
    Log::Trace("MMCR", "mm00sud2f1_inq", "------time used{0}--------", ts.TotalSeconds());
    //sprintf(s.msg, "后台时间 %f 秒, 共%d条记录",  ts.TotalSeconds(), kk);

    return(doFlag);
}

