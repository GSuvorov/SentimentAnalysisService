<%@ Page Language="C#" EnableViewState="false" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="Test.Digest.Web._Default"
         ValidateRequest="false" %>

<%--<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">--%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>Digest-test</title>
    <script type="text/javascript" src="JavaScript/jquery-1.4.1-vsdoc.js"></script>
    <script type="text/javascript" src="JavaScript/WaitBanner.js"></script>
    <script type="text/javascript">
        function useInquiryClick() {
            var inquiryText = document.getElementById("<%= inquiryText.ClientID %>");
            var useInquiry  = document.getElementById("<%= useInquiry.ClientID %>");
            inquiryText.disabled = !useInquiry.checked;
            inquiryText.style.backgroundColor = inquiryText.disabled ? "#EEEEEE" : "";
            useInquiry.parentNode.style.color = inquiryText.disabled ? "Silver" : "";
            document.getElementById("<%= monitoringObjectLabel.ClientID %>").style.color = inquiryText.disabled ? "Silver" : "";
        }
        function executeTonalityMarkingClick() {
            var executeTonalityMarking = document.getElementById("<%= executeTonalityMarking.ClientID %>");
            executeTonalityMarking.parentNode.style.color = !executeTonalityMarking.checked ? "Silver" : "";
        }
        function IsTextEmpty( text ) {
            return (text.replace(/(^\s+)|(\s+$)/g, "") == "");
        }
        function CheckInputParams() {
            var inputText = document.getElementById("<%= inputText.ClientID %>");
            if ( IsTextEmpty( inputText.value ) ) {
                alert("Введите текст.");
                inputText.focus();
                return (false);
            }
            if (document.getElementById("<%= useInquiry.ClientID %>").checked) {
                var inquiryText = document.getElementById("<%= inquiryText.ClientID %>");
                if ( IsTextEmpty( inquiryText.value ) ) {
                    alert("Укажите субъект/объект.");
                    inquiryText.focus();
                    return (false);
                }
            }
            return (true);
        }
        function CallAxaj() {
            try {
                var _useInquiry  = document.getElementById("<%= useInquiry.ClientID %>").checked;
                var _inquiryText = _useInquiry ? document.getElementById("<%= inquiryText.ClientID %>").value : "";

                var _ajax = ($.ajax) ? $.ajax : jQuery.ajax;
                _ajax({
                    type: "POST",
                    url: "Default.aspx?CallAxaj=true",
                    async: false,
                    data: {
                            inputText:   document.getElementById("<%= inputText.ClientID %>").value,
                            outputType:  document.getElementById("<%= outputTypeDropDownList.ClientID %>").value,
                            objectAllocateMethod: document.getElementById("<%= objectAllocateMethodDropDownList.ClientID %>").value,
                            useInquiry:  _useInquiry,
                            inquiryText: _inquiryText,
                            executeTonalityMarking: document.getElementById("<%= executeTonalityMarking.ClientID %>").checked
                        },
                    error: EBIOnError
                });
            }
            catch (e) {
                EBIOnError(e);
            }
        }
        function EBIOnError(e) {
            alert("Произошла системная ошибка. Обратитесь к системному администратору.\r\n Компонент: 'FUSKING ROTTER' => " + (e.statusText ? e.statusText : e.description));
        }
        function execInWindow() {
            if (!CheckInputParams()) return (false);

            CallAxaj();
            return (true);
        }
        function execInIFrame() {
            if (!CheckInputParams()) return;

            CreateWaitBanner();
            _deleteWaitBanners = false;
            try {
                inputIFrame.location.href = "about:blank"; //"Wait.htm"; //
            } catch (ex) {
                inputIFrame.src = "about:blank"; //"Wait.htm"; //
            }
            window.setTimeout( "execInIFrameNext();", 10 );
        }
        function execInIFrameNext() {
            CallAxaj();

            _deleteWaitBanners = true;
            try {
                inputIFrame.location.href = "Result.aspx?viewInWindow=false&ref=" + Math.random();
            } catch (ex) {
                inputIFrame.src = "Result.aspx?viewInWindow=false&ref=" + Math.random();
            }
        }
        window.onload = function () {
            var useInquiry = document.getElementById("<%= useInquiry.ClientID %>");
            if (document.getElementById("<%= useInquiryHiddenField.ClientID %>").value == "true") {
                if (!useInquiry.checked) useInquiry.click();
            } else {
                if (useInquiry.checked) useInquiry.click();
            }

            //executeTonalityMarkingClick();
            var executeTonalityMarking = document.getElementById("<%= executeTonalityMarking.ClientID %>");
            if (document.getElementById("<%= executeTonalityMarkingHiddenField.ClientID %>").value == "true") {
                if (!executeTonalityMarking.checked) executeTonalityMarking.click();
            } else {
                if (executeTonalityMarking.checked) executeTonalityMarking.click();
            }
        }
        var _deleteWaitBanners = false;
        function inputIFrame_onload() {
            if ( _deleteWaitBanners ) DeleteWaitBanners();
        }        
    </script>
</head>
<body style="margin: 0px; overflow: hidden;">
    <form id="form1" runat="server" target="_blank" style="margin: 0px;">
    <div style="text-align: center">
        <table style="border: 1px solid #C0C0C0; width: 100%; height: 100%">
            <tr>
                <td>
                <div id="controlDiv">
                  <table style="width: 100%; height: 210px" cellpadding="0" cellspacing="1">
                    <tr>
                        <td style="padding: 5px; height: 1px; border: solid 1px silver;" align="center">
                            <asp:Label ID="L1" runat="server" Text="Входной текст для разметки тональности:" Width="60%" style="text-align: right;" />
                            <asp:Label ID="L2" runat="server" Font-Size="Small" Width="35%" 
                                       style="text-align: right; overflow: hidden; vertical-align: middle; text-overflow: ellipsis;" 
                                       ForeColor="Silver">(<%= ENDPOINT_URL%>)</asp:Label>
                        </td>
                        <td style="padding: 5px; height: 1px; border: solid 1px silver;" align="center">
                            <asp:Label ID="monitoringObjectLabel" ForeColor="Silver" runat="server" Text="Субъект/Объект:" 
                                       style="width: 100%; white-space: nowrap; overflow-x: hidden; text-overflow: ellipsis;" />
                        </td>
                        <td style="padding: 5px; height: 1px; border: solid 1px silver;" align="center">
                            <asp:Label ID="Label3" runat="server" Text="Команды:" />
                        </td>
                    </tr>
                    <tr>
                        <td style="padding: 2px; border: solid 1px silver; width: 100%;" align="center" rowspan="2">
                            <asp:TextBox ID="inputText" runat="server" Height="99%" Width="100%" TextMode="MultiLine" />
                        </td>
                        <td style="padding: 2px; border: solid 1px silver; height: 100%;">
                            <asp:TextBox ID="inquiryText" runat="server" Height="99%" Width="100%" 
                                TextMode="MultiLine" BackColor="#EEEEEE" disabled="true" />
                        </td>
                        <td style="padding: 2px; border: solid 1px silver; text-align: center;">
                            <hr />
                            <asp:Button ID="execInWindowButton" runat="server" Text="выполнить в новом окне" 
                                OnClientClick="if (!execInWindow()) return(false);" PostBackUrl="~/Result.aspx" 
                                Font-Size="XX-Small" style="cursor: pointer;" Width="135px"/>
                            <hr />
                            <asp:Button ID="execInIFrameButton" runat="server" Text="выполнить" 
                                OnClientClick="execInIFrame(); return (false);" 
                                Font-Size="XX-Small" style="cursor: pointer;" Width="135px"/>
                            <hr />
                            <asp:DropDownList ID="outputTypeDropDownList" runat="server" Font-Size="X-Small" 
                                              Width="145px" style="cursor: pointer;" ToolTip="выходной формат">
                                <asp:ListItem Value="Xml" Selected="True"        >&nbsp;Xml</asp:ListItem>
                                <asp:ListItem Value="Xml_Custom"                 >&nbsp;Xml-custom</asp:ListItem>
                                <asp:ListItem Value="Table_And_Xml_Custom"       >&nbsp;Table & Xml</asp:ListItem>
                                <asp:ListItem Value="Table_And_Xml_Custom_BySent">&nbsp;Table & Xml by Sent</asp:ListItem>
                                <asp:ListItem Value="Table"                      >&nbsp;Table</asp:ListItem>
                            </asp:DropDownList>
                            <br />
                            <asp:DropDownList ID="objectAllocateMethodDropDownList" runat="server" Font-Size="X-Small" 
                                              Width="145px" style="cursor: pointer;" ToolTip="метод выбора объектов">
                                <asp:ListItem Value="PhysicalOrJuridicalPersons">physical-or-juridical-persons</asp:ListItem>
                                <asp:ListItem Value="FirstEntityWithRoleSubj"   >first-entity-with-role-subj</asp:ListItem>
                                <asp:ListItem Value="FirstEntityWithTypePronoun">first-entity-with-type-pronoun</asp:ListItem>
                                <asp:ListItem Value="FirstVerbEntityWithRoleObj" Selected="True">first-entity-with-role-obj</asp:ListItem>
                            </asp:DropDownList>
                        </td>
                    </tr>
                    <tr>
                        <td style="padding: 2px; border: solid 1px silver; ">
                            <asp:CheckBox ID="useInquiry" runat="server" Height="1px" ForeColor="Silver"
                                Text="по заданному субъекту/объекту"
                                ToolTip="по заданному субъекту/ объекту" 
                                Font-Size="X-Small"
                                style="cursor: pointer; width: 180px; height: 20px; white-space: nowrap; overflow-x: hidden; text-overflow: ellipsis;" onclick="useInquiryClick();" />
                            <asp:CheckBox ID="executeTonalityMarking" runat="server" Height="1px" ForeColor="Silver" 
                                Text="разметить тональность"
                                ToolTip="разметить тональность" 
                                Font-Size="X-Small"
                                style="cursor: pointer; width: 180px; height: 20px; white-space: nowrap; overflow-x: hidden; text-overflow: ellipsis;" onclick="executeTonalityMarkingClick();" />
                        </td>
                        <td style="padding: 2px; border: solid 1px silver; text-align: center;">
                            <%--<asp:HyperLink ID="testHyperLink" runat="server" NavigateUrl="~/Test.aspx" Text="тест =>" ForeColor="Silver" Font-Size="X-Small" />--%>
                        </td>
                    </tr>
                    </table>
                </div>
                </td>
            </tr>
            <tr>
                <td style="width: 100%; height: 100%">
                    <iframe id="inputIFrame" onload="inputIFrame_onload();" style="border: 1px solid #C0C0C0; width: 100%; height: 100%;" frameborder="no"></iframe>
                </td>
            </tr>
        </table>
        <asp:HiddenField ID="useInquiryHiddenField" runat="server" Value="false" />
        <asp:HiddenField ID="executeTonalityMarkingHiddenField" runat="server" Value="true" />
    </div>
    </form>
</body>
</html>
