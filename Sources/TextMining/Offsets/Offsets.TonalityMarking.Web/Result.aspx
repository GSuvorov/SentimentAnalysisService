<%@ Page Language="C#" EnableViewState="false" AutoEventWireup="true" CodeBehind="Result.aspx.cs" 
         Inherits="Offsets.TonalityMarking.Web.Result" %>

<%--<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">--%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Digest: <%= this.InputText4Title %></title>
</head>
<body style="margin: 0px; padding: 0px;">
    <form id="form1" runat="server">
        <div id="resultHtmlDiv" runat="server" style="padding: 10px; vertical-align: top;"></div>
    </form>
</body>
</html>
