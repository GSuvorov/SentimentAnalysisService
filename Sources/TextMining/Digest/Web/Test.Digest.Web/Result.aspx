<%@ Page Language="C#" EnableViewState="false" AutoEventWireup="true" CodeBehind="Result.aspx.cs" 
         Inherits="Test.Digest.Web.Result" %>

<%--<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">--%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Digest: <%= this.InputText4Title %></title>
</head>
<body style="margin: 0px; padding: 0px;">
    <form id="form1" runat="server">
    <%--<div style="text-align: center">--%>
        <div id="resultHtmlDiv" runat="server" style="padding: 10px; vertical-align: top;"></div>
        <table id="resultXmlTable" runat="server" style="width: 100%; height: 100%; margin: 0px; padding: 0px;" cellpadding="0" cellspacing="0">
            <tr>
                <td>
                    <div id="resultXmlDiv" runat="server" style="height: 100px; vertical-align: top; overflow-y: scroll; border: 1px solid #C0C0C0;"></div>
                </td>
            </tr>
            <tr>
                <td style="width: 100%; height: 100%;">
                    <iframe id="resultXmlIFrame" runat="server" src="ResultXml.aspx" style="border: 1px solid #C0C0C0; width: 100%; height: 100%;" frameborder="no"></iframe>
                </td>
            </tr>
        </table>
    <%--</div>--%>
    </form>
</body>
</html>
