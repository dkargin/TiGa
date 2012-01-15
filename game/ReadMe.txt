
1. start box2d
2. draw unit
3. make texture converter
######################################################################################################
common_gch.gch
<%
function setpara(handle,para_name)
{
	set_para(handle,para_name,request(para_name));
}
function set_multiinst_para(handle, para_name,index){set_para(handle,para_name,request(para_name+index));}
function create_form_start(ID,URL){="<FORM NAME="+ID+" ID="+ID+" METHOD='POST' action="+URL+">\n";}
function create_form_end()
{
	="</FORM>\n";
}
function create_hidden_sep(ID,value)
{
	="<INPUT type='hidden' name="+ID+"     ID="+ID+" value=''>\n";
	="<script language=javascript>Transfer_meaning('";=ID;="','";=value;="');";
	="</script>\n";
}
function set_hidden(ID,value)
{
	="<script language=javascript>Transfer_meaning('";=ID;="','";=value;="');";
	="</script>\n";
}
function create_hidden_newpara(PARA[], num)
{
	for(var i=0;i<num;i++)
	{
		="<INPUT type='hidden' name="+PARA[i]+"   ID="+PARA[i]+" value=''>\n";
		="<script language=javascript>Transfer_meaning('";=PARA[i];="','";="');";
		="</script>\n";
	}
}
function cover_hidden(handle,PARA[],num)
{
	for(var i=0;i<num;i++)
	{
		="<script language=javascript>Transfer_meaning('";=PARA[i];="','";=get_para(handle,PARA[i]);
		="');";
		="</script>\n";
	}
}
function create_hidden_para(handle,PARA[],num)
{
	for(var i=0;i<num;i++)
	{
		="<INPUT type='hidden' name="+PARA[i]+"   ID="+PARA[i]+" value=''>\n";
		="<script language=javascript>Transfer_meaning('";=PARA[i];="','";=get_para(handle,PARA[i]);="');";
		="</script>\n";
	}
}
function create_hidden_multiinstnewpara(handle, PARA[],instindex, num)
{
	for(var i=0;i<num;i++)
	{
		="<INPUT type='hidden' name="+PARA[i]+instindex +"   ID="+PARA[i]+instindex +" value=''>\n";
	}
}
function create_hidden_multiinstpara(handle, PARA[],instindex, num)
{
	for(var i=0;i<num;i++)
	{
		="<INPUT type='hidden' name="+PARA[i]+instindex +"   ID="+PARA[i]+instindex +" value=''>\n";
		="<script language=javascript>Transfer_meaning('";=PARA[i]+instindex;="','";=get_para(handle,PARA[i]);="');";
		="</script>\n";
	}
}
function create_hidden_multiinstpara1(handle, PARA[],instindex, num)
{
	for(var i=0;i<num;i++)
	{
		="<INPUT type='hidden' name="+PARA[i]+instindex +"   ID="+PARA[i]+instindex +" value=''>\n";
		="<script language=javascript>Transfer_meaning('";
		=PARA[i]+instindex;
		="','";
		=get_para(handle,PARA[i]+instindex);
		="');";
		="</script>\n";
	}
}
function set_hidden_multiinstpara(handle, PARA[],instindex, num)
{
	for(var i=0;i<num;i++)
	{
		="<script language=javascript>Transfer_meaning('";
		=PARA[i]+instindex;
		="','";
		=get_para(handle,PARA[i]);
		="');";
		="</script>\n";
	}
}
function create_special_hidden_multiinstpara(handle, inputName, handleName,instindex)
{
	="<INPUT type='hidden' name="+inputName +instindex +"   ID="+inputName +instindex +" value=''>\n";
	="<script language=javascript>Transfer_meaning('";
	=inputName +instindex;
	="','";
	=get_para(handle,handleName);
	="');";
	="</script>\n";
}
%>
######################################################################################################
ddns_gch.gch
<%
IMPORT FILE "common_gch.gch";
IMPORT FILE "ddns_client_gch.gch";
IMPORT FILE "ddns_service_gch.gch";
var FP_ERRORSTR,FP_ERRORPARAM,FP_ACTION,FP_INDEX,CLIENT_INDEX,SERVICE0_INDEX,FP_NAME;
create_form_start("fSubmit","");
create_hidden_sep("IF_ERRORSTR","SUCC");
create_hidden_sep("IF_ERRORPARAM","SUCC");
FP_ACTION=request("IF_ACTION");
FP_INDEX=request("IF_INDEX");
SERVICE0_INDEX=request("IFservice_INDEX");
FP_NAME=request("IF_NAME");
FP_ERRORSTR="SUCC";
FP_ERRORSTR=service_func(FP_ACTION,SERVICE0_INDEX,FP_ERRORSTR);
FP_ERRORSTR=client_func(FP_ACTION,CLIENT_INDEX,FP_ERRORSTR);
create_hidden_sep("IF_ACTION","");
create_hidden_sep("IF_INDEX",FP_INDEX);
create_hidden_sep("IFservice_INDEX",SERVICE0_INDEX);
create_hidden_sep("IF_NAME",FP_NAME);
create_form_end();
%>
######################################################################################################
ddns_service_gch.gch
<%IMPORT FILE "common_gch.gch";
function service_func(FP_ACTION,SERVICE0_INDEX,FP_ERRORSTR)
{
	var SERVICE_ACTION,SERVICE_IDENTITY,SERVICE_HANDLE,SERVICE_INDEX,SERVICE_OBJNAME,SERVICE_INSTNUM,SERVICE_PARANUM,SERVICE_ERRORSTR,SERVICE_ERRORPARAM;
	var PARA[7]={"Name","Server","ServerPort","Request","UpdateInterval","RetryInterval","MaxRetries"};
	SERVICE_OBJNAME="OBJ_DDNSSERVICE_ID";
	SERVICE_PARANUM=7;
	SERVICE_IDENTITY="";SERVICE_INSTNUM="";
	SERVICE_INDEX=SERVICE0_INDEX;
	query_list("OBJ_DDNSCLIENT_ID","IGD");
	var HANDLE=create_paralist();
	var IDENTITY=query_identity(0);
	get_inst(HANDLE, "OBJ_DDNSCLIENT_ID",IDENTITY);
	var Service=get_para(HANDLE,"Service");
	destroy_paralist(HANDLE);
	if(SERVICE_INDEX IS NULL)
	{
		var DDNS_INSTNUM=query_list("OBJ_DDNSSERVICE_ID","IGD");
		for(var i=0;i<DDNS_INSTNUM;i++)
		{
			var DDNS_HANDLE=create_paralist();
			var DDNS_IDENTITY=query_identity(i);
			get_inst(DDNS_HANDLE, "OBJ_DDNSSERVICE_ID",DDNS_IDENTITY);
			var Name=get_para(DDNS_HANDLE,"Name");
			if(Name==Service)
			{
				SERVICE_INDEX=i;
			}
			destroy_paralist(DDNS_HANDLE);
		}
		if(SERVICE_INDEX IS NULL)
		SERVICE_INDEX=0;
	}
	SERVICE_ACTION=FP_ACTION;
	SERVICE_INSTNUM=query_list(SERVICE_OBJNAME,"IGD");
	if(SERVICE_INSTNUM!=888&&SERVICE_INDEX>=0)
	{
		SERVICE_IDENTITY=query_identity(SERVICE_INDEX);
	}
	if(SERVICE_ACTION=="apply")
	{
		if(SERVICE_INDEX>=0)
		{
			SERVICE_HANDLE=create_paralist();
			for(var i=0;i<SERVICE_PARANUM;i++)
			{
				setpara(SERVICE_HANDLE,PARA[i]);
			}
			SERVICE_IDENTITY=set_inst(SERVICE_HANDLE, SERVICE_OBJNAME,SERVICE_IDENTITY);
		}
		if(SERVICE_INDEX<0)
		{
			for(var j=0;j<SERVICE_INSTNUM;j++)
			{
				SERVICE_IDENTITY=query_identity(j);
				SERVICE_HANDLE=create_paralist();
				for(var i=0;i<SERVICE_PARANUM;i++)
				{
					setpara(SERVICE_HANDLE,PARA[i]+j);
				}
				SERVICE_IDENTITY=set_inst(SERVICE_HANDLE, SERVICE_OBJNAME,SERVICE_IDENTITY);
			}
		}
	}
	SERVICE_HANDLE=create_paralist();
	if(SERVICE_INDEX>=0)
	{
		get_inst(SERVICE_HANDLE, SERVICE_OBJNAME,SERVICE_IDENTITY);
		create_hidden_para(SERVICE_HANDLE,PARA,SERVICE_PARANUM);
	}
	if(SERVICE_INDEX<0)
	{
		for(var i=0;i<SERVICE_INSTNUM;i++)
		{
			SERVICE_IDENTITY=query_identity(i);
			get_inst(SERVICE_HANDLE, SERVICE_OBJNAME,SERVICE_IDENTITY);
			create_hidden_multiinstpara(SERVICE_HANDLE, PARA,i,SERVICE_PARANUM);
		}
	}
	destroy_paralist(SERVICE_HANDLE);
	SERVICE_ERRORSTR=get_error();
	if(FP_ERRORSTR=="SUCC"&&SERVICE_ERRORSTR!="SUCC")
	{
		SERVICE_ERRORPARAM=get_errorparam();
		set_hidden("IF_ERRORSTR",SERVICE_ERRORSTR);
		set_hidden("IF_ERRORPARAM",SERVICE_ERRORPARAM);
	}
	return SERVICE_ERRORSTR;
}
%>

ddns_client_gch.gch
<%
IMPORT FILE "common_gch.gch";
function client_func(FP_ACTION,CLIENT_INDEX,FP_ERRORSTR)
{
	var CLI_ACTION,CLI_IDENTITY,CLI_HANDLE,CLI_INDEX,CLI_OBJNAME,CLI_INSTNUM,CLI_PARANUM,CLI_ERRORSTR,CLI_ERRORPARAM;
	var PARA[11]={"Enable","Hidden","Status","LastError","Interface","DomainName","Service","Username","Password","Offline","HostNumber"};
	CLI_OBJNAME="OBJ_DDNSCLIENT_ID";
	CLI_PARANUM=11;
	CLI_IDENTITY="";
	CLI_INSTNUM="";
	CLI_INDEX=CLIENT_INDEX;
	if(CLI_INDEX IS NULL)
	{
		CLI_INDEX=0;
	}
	CLI_ACTION=FP_ACTION;
	CLI_INSTNUM=query_list(CLI_OBJNAME,"IGD");
	if(CLI_INSTNUM!=888&&CLI_INDEX>=0)
	{
		CLI_IDENTITY=query_identity(CLI_INDEX);
	}
	if(CLI_ACTION=="apply")
	{
		if(CLI_INDEX>=0)
		{
			CLI_HANDLE=create_paralist();
			for(var i=0;i<CLI_PARANUM;i++)
			{
				setpara(CLI_HANDLE,PARA[i]);
			}
			CLI_IDENTITY=set_inst(CLI_HANDLE, CLI_OBJNAME,CLI_IDENTITY);
		}
		if(CLI_INDEX<0)
		{
			for(var j=0;j<CLI_INSTNUM;j++)
			{
				CLI_IDENTITY=query_identity(j);
				CLI_HANDLE=create_paralist();
				for(var i=0;i<CLI_PARANUM;i++)
				{
					setpara(CLI_HANDLE,PARA[i]+j);
				}
				CLI_IDENTITY=set_inst(CLI_HANDLE, CLI_OBJNAME,CLI_IDENTITY);
			}
		}
	}
	if(CLI_ACTION=="delete")
	{
		del_inst(CLI_OBJNAME, CLI_IDENTITY);
	}
	CLI_HANDLE=create_paralist();
	if(CLI_INDEX>=0)
	{
		get_inst(CLI_HANDLE, CLI_OBJNAME,CLI_IDENTITY);
		var passwd=get_para(CLI_HANDLE,"Password");
		set_para(CLI_HANDLE,"Password",strencode(passwd));
		create_hidden_para(CLI_HANDLE,PARA,CLI_PARANUM);
	}
	if(CLI_INDEX<0)
	{
		for(var i=0;i<CLI_INSTNUM;i++)
		{
			CLI_IDENTITY=query_identity(i);
			get_inst(CLI_HANDLE, CLI_OBJNAME,CLI_IDENTITY);
			create_hidden_multiinstpara(CLI_HANDLE, PARA,i,CLI_PARANUM);
		}
	}
	destroy_paralist(CLI_HANDLE);
	CLI_ERRORSTR=get_error();
	if(FP_ERRORSTR=="SUCC"&&CLI_ERRORSTR!="SUCC")
	{
		CLI_ERRORPARAM=get_errorparam();
		set_hidden("IF_ERRORSTR",CLI_ERRORSTR);
		set_hidden("IF_ERRORPARAM",CLI_ERRORPARAM);
	}
	return CLI_ERRORSTR;
}
%>

#####################################################################################################################
app_ddns_conf_js.gch
<script language="javascript">
var CLIENT_PARA=new Array("Enable","Hidden","Status","LastError","Interface","DomainName","Service","Username","Password","Offline","HostNumber");
var SERVICE_PARA=new Array("Name","Server","ServerPort","Request","UpdateInterval","RetryInterval","MaxRetries");
function getmsg(id)
{
	var str=new Array();
	str[0]=new Array(101,"&?error_101;");
	str[1]=new Array(102,"&?error_102;");
	str[2]=new Array(103,"&?error_103;");
	str[3]=new Array(104,"&?error_104;");
	str[4]=new Array(114,"&?error_114;");
	str[5]=new Array(118,"&?error_118;");
	str[6]=new Array(119,"&?error_119;");
	str[7]=new Array(122,"&?error_122;");
	str[8]=new Array(106,"&?error_106;");
	return getMsgFormArray(str,arguments)
}
function pageLoad(url)
{
	getObj("fSubmit").action=url;
	var errstr=getValue("IF_ERRORSTR");
	if(errstr!="SUCC")
	{
		var errpara=getValue("IF_ERRORPARAM");
		if(errpara!="SUCC")
		{
			if(document.getElementById("Fnt"+errpara)==null)
			{
				msg=getmsg(122);
				ShowErrorForCom(null,null,msg);
			}
			else 
			{
				msg=getmsg(118,"Fnt"+errpara);
				ShowErrorForCom("Frm"+errpara,null,msg);
			}
		}
		else 
		{
			msg=getmsg(122);
			ShowErrorForCom(null,null,msg);
		}
	}
	moveErrLayer(595,93);
	pageGetValue();
}
function pageGetValue()
{
	var pwdStr="";
	jslPostToViewCheckBox("Frm_Enable","Enable");
	jslSetValue("Frm_Username","Username");
	setValue("Frm_Password",decode64(getValue("Password")));
	if(getValue("Password")=="AA==")
	{
		pwdStr="";
	}
	else
	{
		pwdStr=decode64(getValue("Password"));
	}
	document.getElementById("Frm_Password").value=pwdStr;
	jslDoShowComboBox("Frm_Interface","Interface");
	var service=getValue("Service");
	var name=getValue("IF_NAME");
	if(service==""&&name=="NULL")
	{
		setValue("Frm_Name","dipc");
	}
	else if(service==""&&name!="NULL")
	{
		setValue("Frm_Name",name);
	}
	else 
	{
		jslSetValue("Frm_Name","Service");
	}
	<%var 
	DDNS_INSTNUM=query_list("OBJ_DDNSSERVICE_ID","IGD");
	for(var i=0;i<DDNS_INSTNUM;i++)
	{
		var DDNS_HANDLE=create_paralist();
		var DDNS_IDENTITY=query_identity(i);
		get_inst(DDNS_HANDLE, "OBJ_DDNSSERVICE_ID",DDNS_IDENTITY);
		var Name=get_para(DDNS_HANDLE,"Name");%>
		if("<%=Name;%>"==getValue("Frm_Name"))
		{<%var Server=get_para(DDNS_HANDLE,"Server");%>
		setValue("Frm_Server","<%=Server;%>");
		}
		<%destroy_paralist(DDNS_HANDLE);
	}%>
	jslSetValue("Frm_DomainName","DomainName");
	if(getValue("Frm_Name")=="dyndns")
	{
		jslEnDisplay("Frm_DomainName","Fnt_DomainName");
		document.getElementById("Fnt_DomainName").innerHTML="&?ddns006;";
	}
	else if(getValue("Frm_Name")=="dipc")
	{
		jslEnDisplay("Frm_DomainName","Fnt_DomainName");
		document.getElementById("Fnt_DomainName").innerHTML="&?ddns004;";
	}
	else if(getValue("Frm_Name")=="DtDNS")
	{
		jslDiDisplay("Frm_DomainName","Fnt_DomainName");
	}
	else if(getValue("Frm_Name")=="No-IP")
	{
		jslDiDisplay("Frm_DomainName","Fnt_DomainName");
	}
}
function pageSetValue()
{
	HiddenParaInit(CLIENT_PARA);
	HiddenParaInit(SERVICE_PARA);
	jslViewToPostCheckBox("Enable","Frm_Enable");
	jslSetValue("Server","Frm_Server");
	jslSetValue("Username","Frm_Username");
	jslSetValue("Password","Frm_Password");
	jslSetValue("Interface","Frm_Interface");
	jslSetValue("Name","Frm_Name");
	jslSetValue("Service","Frm_Name");
	jslSetValue("DomainName","Frm_DomainName");
	if(getValue("Frm_Name")=="dyndns")
	{
		setValue("IFservice_INDEX","1");
	}
	else if(getValue("Frm_Name")=="dipc")
	{
		setValue("IFservice_INDEX","0");
	}
	else if(getValue("Frm_Name")=="DtDNS")
	{
		setValue("IFservice_INDEX","2");
	}
	else if(getValue("Frm_Name")=="No-IP")
	{
		setValue("IFservice_INDEX","3");
	}
	}
function pageCheckValue()
{
	if(checkNull(getValue("Frm_Server"))!=true)
	{
		msg=getmsg(101);
		ShowErrorForCom("Frm_Server","Fnt_Server",msg);
		return false;
	}
	if(checkGenStringForASC(getValue("Frm_Server"))!=true)
	{
		msg=getmsg(114);
		ShowErrorForCom("Frm_Server","Fnt_Server",msg);
		return false;
	}
	var value=getValue("Frm_Server");
	if(checkStrLengthRange(value,1,256)!=true)
	{
		msg=getmsg(102,1,256);
		ShowErrorForCom("Frm_Server","Fnt_Server",msg);
		return false;
	}
	if(checkNull(getValue("Frm_Username"))!=true)
	{
		msg=getmsg(101);
		ShowErrorForCom("Frm_Username","Fnt_Username",msg);
		return false;
	}
	if(checkGenStringForASC(getValue("Frm_Username"))!=true)
	{
		msg=getmsg(114);
		ShowErrorForCom("Frm_Username","Fnt_Username",msg);
		return false;
	}
	var value=getValue("Frm_Username");
	if(checkStrLengthRange(value,1,256)!=true)
	{
		msg=getmsg(102,1,256);
		ShowErrorForCom("Frm_Username","Fnt_Username",msg);
		return false;
	}
	if(checkGenStringForASC(getValue("Frm_Password"))!=true)
	{
		msg=getmsg(114);
		ShowErrorForCom("Frm_Password","Fnt_Password",msg);
		return false;
	}
	var value=getValue("Frm_Password");
	if(checkStrLengthRange(value,1,256)==-3)
	{
		msg=getmsg(102,1,256);
		ShowErrorForCom("Frm_Password","Fnt_Password",msg);
		return false;
	}
	if(getValue("Frm_Name")!="DtDNS"&&getValue("Frm_Name")!="No-IP")
	{
		if(checkNull(getValue("Frm_DomainName"))!=true)
		{
			msg=getmsg(101);
			ShowErrorForCom("Frm_DomainName","Fnt_DomainName",msg);
			return false;
		}
		var value=getValue("Frm_DomainName");
		if(checkStrLengthRange(value,1,64)!=true)
		{
			msg=getmsg(102,1,64);
			ShowErrorForCom("Frm_DomainName","Fnt_DomainName",msg);
			return false;
		}
		if(getValue("Frm_Name")=="dyndns")
		{
			if(checkDomainName(getValue("Frm_DomainName"))!=true)
			{
				msg=getmsg(106);
				ShowErrorForCom("Frm_DomainName","Fnt_DomainName",msg);
				return false;
			}
		}
		else if(getValue("Frm_Name")=="dipc")
		{
			if(checkDomainName(getValue("Frm_DomainName"))!=true)
			{
				msg=getmsg(104);
				ShowErrorForCom("Frm_DomainName","Fnt_DomainName",msg);
				return false;
			}
		}
	}
	return true;
}
function pageCancel()
{
	jslDisable("Btn_Submit","Btn_Cancel");
	getObj("fSubmit").submit();
}
function pageSubmit()
{
	if(pageCheckValue()==true)
	{
		jslDisable("Btn_Submit","Btn_Cancel");
		pageSetValue();
		setValue("IF_ACTION","apply");
		getObj("fSubmit").submit();
	}
	else {}
}
function Slect_Servise()
{
	if(getValue("Frm_Name")=="dyndns")
	{
		document.getElementById("Fnt_DomainName").innerHTML="&?ddns006;";
		setValue("IFservice_INDEX","1");
		setValue("IF_NAME","dyndns");
	}
	else if(getValue("Frm_Name")=="dipc")
	{
		document.getElementById("Fnt_DomainName").innerHTML="&?ddns004;";
		setValue("IFservice_INDEX","0");setValue("IF_NAME","dipc");
	}
	else if(getValue("Frm_Name")=="DtDNS")
	{
		setValue("IFservice_INDEX","2");
		setValue("IF_NAME","DtDNS");
	}else if(getValue("Frm_Name")=="No-IP")
	{
		setValue("IFservice_INDEX","3");
		setValue("IF_NAME","No-IP");
	}
	jslSetValue("Name","Frm_Name");
	jslSetValue("Service","Frm_Name");
	setValue("IF_ACTION","delete");
	getObj("fSubmit").submit();
}
</script>
######################################################################################################################
app_ddns_conf_t.gch
<%
IMPORT FILE "ddns_gch.gch";
IMPORT FILE "app_ddns_conf_js.gch";
%>
<table id="TestContent"  class="table" width="450px" border="0">
<tr>
<td class="td1">&?ddns001;</td>
<td class="td2"><input type="checkbox" name="Frm_Enable" id="Frm_Enable" value=""/></td>
</tr>
<tr>
<td class="td1">&?ddns007;</td>
<td class="td2">
<select  name="Frm_Name" id="Frm_Name" size="1" class="list_1" onchange= "Slect_Servise()">
<%
var  DDNS_INSTNUM = query_list("OBJ_DDNSSERVICE_ID", "IGD");
for(var i=0;i<DDNS_INSTNUM;i++)
{
var DDNS_HANDLE = create_paralist();
var DDNS_IDENTITY = query_identity(i);
get_inst(DDNS_HANDLE, "OBJ_DDNSSERVICE_ID", DDNS_IDENTITY);
var Name = get_para(DDNS_HANDLE, "Name");
%>
<option value="<%=Name;%>" ><%=Name;%></option>
<%
destroy_paralist(DDNS_HANDLE);
}
%>
</select></td>
</tr>
<tr>
<td class="td1"><font id="Fnt_Server">&?ddns002;</font></td>
<td class="td2"><input type="text" class="inputId6" name="Frm_Server" id="Frm_Server" value=""/></td>
</tr>
<tr>
<td class="td1"><font id="Fnt_Username">&?wan019;</font></td>
<td class="td2"><input type="text" class="inputId" name="Frm_Username" id="Frm_Username" size="15" value=""/></td>
</tr>
<tr>
<td class="td1"><font id="Fnt_Password">&?va02;</font></td>
<td class="td2"><input type="password" class="inputId" name="Frm_Password" id="Frm_Password" size="15" value=""/></td>
</tr>
<tr>
<td class="td1">&?comm002;</td>
<td class="td2">
<select size="1" class="list_4" name="Frm_Interface" id="Frm_Interface">
<%   var FP_INSTNUM = "";
var identity   = "";
var WANLAN_HANDLE     = null;
var WANLAN_name       = "";
FP_INSTNUM = query_list("OBJ_LISTWANC_ID", "ddns");
if(FP_INSTNUM >= 1 && FP_INSTNUM != 888)
{
for(var i = 0; i < FP_INSTNUM; i++ )
{
identity = query_identity(i);
WANLAN_HANDLE   = create_paralist();
get_inst(WANLAN_HANDLE, "OBJ_WANLAN_ID", identity);
WANLAN_name = get_para(WANLAN_HANDLE, "Name");
%>
<option value="<%=identity;%>" ><%=WANLAN_name;%></option>
<%
destroy_paralist(WANLAN_HANDLE);
}
}
%>
</select></td>
</tr>
<tr>
<td class="td1"><font id="Fnt_DomainName"></font></td>
<td class="td2"><input type="text" class="inputId" name="Frm_DomainName" id="Frm_DomainName" size="15" value=""/></td>
</tr>
<tr><td class="td1" colspan="2"></td></tr>
</table>


tank = Unit
{
	health = 100,
	speed = 50,
	weapon = Cannon
	{
		damage = 30,
		shootDelay = 0.5,
	},
	...
}

Unit
{
	...
	hard part
	...
	custom=luaRef
	{
		object=this,	(tolua_pushobject(l,"Unit",this))
		create=function(self,...) ... end
		update=function(self,deltaTime) ... end
	}
}

Device
{
	...
	hard part
	...
	lua=luaRef
	{
		object=this
		create=function(self,...) ... end
		update=function(self,deltaTime) ... end
	}
}

unit=
{
	hardpoints=
	{
		
		locked=true,
	}
}
Turret
	out=manual{left,right}
Manual	- keyboard control
Position

///////////////////////////////////////////////////////////////////////////////////
// Generic device section

Each device is controlled throught set of ports. Each port has specific type and command set
int Device::getPorts() const; // returns avialable port types


Unit::onInit
1. Create mounts

Unit::onCreate
1. Add devices to specific mounts
2. Connect devices
3. Connect to manual control

control: player,device

Описание редактора ресурсов
Слева - дерево со всеми доступными ресурсами. Корни: текстуры, спрайты, анимации, частицы, курсоры, звуки
Дополнительно переключатель режимов отображения ресурсов (отобразить зависимые объекты)

mover
{
	in:dcmdUp=		"move forward",
	in:dcmdDown=	"move backward",
	in:dcmdLeft=	"turn left",
	in:dcmdRight=	"turn right",
};

moverDriver	// executes moveTo commands
{
	in:dcmdPosition="move to"
	out(mover,1)
	{
		dcmdUp=		"move forward",
		dcmdDown=	"move backward",
		dcmdLeft=	"turn left",
		dcmdRight=	"turn right",
	}
}

turret
{	
	in:dcmdLeft=	"turn left",
	in:dcmdRight=	"turn right",
}

turretDriver
{
	in: dcmdPosition="aim",
	out(turret)
	{
		dcmdLeft=	"turn left",
		dcmdRight=	"turn right",
	}
	out(weapon)
	{
		dcmdAction1="shoot"
	}
}

weapon
{
	dcmdAction="shoot"
}

[12.07.10] Still problems with beam weapons - wrong beam tracing. Also there are some issues with FxAnimation2


[14.07.10]
	+ All tolua game definitions are stored in Types table. Field name corresponds to tolua exported class name
	~ It's time to rename World to Game
	+ Turned on HGE GUI. It works. Good.
need to turn on bots. Flocking? Maybe.
Let's try to reimplement weapons... again.

enum CollisionType
{
	Collide,
	Notify,
	Ignore
};

[29.08.10] That was really bad idea to eliminate DeviceDef
[4.10.10]	Should unite perception and Unit.