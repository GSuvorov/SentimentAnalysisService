<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Test.aspx.cs" Inherits="Test.Digest.Web.Test" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>Digest: Супер-Тест</title>
    <style type="text/css">
        .customFile
        {
            position: absolute;
            cursor: pointer;
            height: 23px;
            z-index: 2;
            filter: alpha(opacity: 0);
            opacity: 0;
        }
    </style>
    <script type="text/javascript" src="JavaScript/WaitBanner.js"></script>
    <script type="text/javascript">
        function onchange_loadingFile() {
            CreateWaitBanner(document, "Обработка теста.....&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
            document.getElementById("<%= runTestButton.ClientID %>").click();
            window.setTimeout(disable_controls, 1)
        }
        function disable_controls() {
            document.getElementById("<%= runTestButton.ClientID %>").disabled = true;
            document.getElementById("<%= showOnlyErrorCheckBox.ClientID %>").disabled = true;
            document.getElementById("<%= testFileUpload1.ClientID %>").style.display = "none";
            document.getElementById("<%= testFileUpload2.ClientID %>").style.display = "none";
        }
    </script>
</head>
<body>
    <form id="form1" runat="server" style="font-size: small;">
    <div style="text-align: right; font-size: small;">
        <asp:HyperLink ID="backHyperLink1" runat="server" Text="назад =>" NavigateUrl="~/Default.aspx"
            ForeColor="Silver" />
    </div>
    <div style="text-align: center;">
        <asp:HyperLink ID="againHyperLink1" runat="server" Text="тестировать еще раз" NavigateUrl="~/Test.aspx"
            ForeColor="Blue" Visible="false" />
        <%--<asp:LinkButton ID="repeatLinkButton1" runat="server" Text="повторить" Visible="false" />--%>
        <asp:FileUpload ID="testFileUpload1" runat="server" CssClass="customFile" Style="margin-left: -150px;
            z-index: 3;" onchange="onchange_loadingFile();" />
        <asp:FileUpload ID="testFileUpload2" runat="server" CssClass="customFile" Style="margin-left: -80px;
            z-index: 2;" onchange="onchange_loadingFile();" />
        <asp:Button ID="runTestButton" runat="server" Text="Запустить тест..." OnClick="runTestButton_Click" />
        <br />
        <br />
        <asp:CheckBox ID="showOnlyErrorCheckBox" runat="server" Text="показывать только ошибки"
            Checked="true" Style="cursor: pointer;" />
        <hr />
    </div>
    <div id="resultDiv" runat="server" />
    <div style="text-align: right; font-size: small;">
        <asp:HyperLink ID="backHyperLink2" runat="server" Text="назад =>" NavigateUrl="~/Default.aspx"
            ForeColor="Silver" Visible="false" />
    </div>
    <div style="text-align: center; font-size: small;">
        <asp:HyperLink ID="againHyperLink2" runat="server" Text="тестировать еще раз" NavigateUrl="~/Test.aspx"
            ForeColor="Blue" Visible="false" />
        <%--<asp:LinkButton ID="repeatLinkButton2" runat="server" Text="повторить" Visible="false" />--%>
    </div>
    </form>
</body>
</html>
