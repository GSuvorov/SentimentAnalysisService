<%@ Page Language="C#" EnableViewState="false" AutoEventWireup="true" CodeBehind="Default.aspx.cs" 
         Inherits="Offsets.TonalityMarking.Web._Default"
         ValidateRequest="false" %>

<%--<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">--%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>Offsets-TonalityMarking-test</title>
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
                    alert("Укажите объект.");
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
                            inputText:  document.getElementById("<%= inputText.ClientID %>").value,
                            outputType: document.getElementById("<%= outputTypeDropDownList.ClientID %>").value,
                            useInquiry: _useInquiry,
                            inquiryText: _inquiryText,
                            offsetCreationXmlType: document.getElementById("<%= offsetCreationXmlTypeDropDownList.ClientID %>").value
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

            //CreateWaitBanner2( $("#controlTable")[ 0 ] );
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
                            <asp:Label ID="Label1" runat="server" Text="Входной текст для TonalityMarking:" Width="75%" />
                        </td>
                        <td style="padding: 5px; height: 1px; border: solid 1px silver;" align="center">
                            <asp:Label ID="monitoringObjectLabel" ForeColor="Silver" runat="server" Text="Объект:" 
                                       style="width: 100%; white-space: nowrap; overflow-x: hidden; text-overflow: ellipsis;" />
                        </td>
                        <td style="padding: 5px; height: 1px; border: solid 1px silver;" align="center">
                            <asp:Label ID="Label3" runat="server" Text="Команды:" />
                        </td>
                    </tr>
                    <tr>
                        <td style="padding: 2px; border: solid 1px silver; width: 100%;" align="center">
                            <asp:TextBox ID="inputText" runat="server" Height="99%" Width="100%" TextMode="MultiLine" />
                        </td>
                        <td style="padding: 2px; border: solid 1px silver; height: 100%;">
                            <asp:TextBox ID="inquiryText" runat="server" Height="85%" Width="100%" 
                                TextMode="MultiLine" BackColor="#EEEEEE" disabled="true" />
                            <asp:CheckBox ID="useInquiry" runat="server" Height="1px" ForeColor="Silver"
                                Text="по заданному объекту"
                                ToolTip="по заданному объекту" 
                                Font-Size="X-Small"
                                style="cursor: pointer; width: 180px; height: 20px; white-space: nowrap; overflow-x: hidden; text-overflow: ellipsis;" onclick="useInquiryClick();" />
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
                                              Width="145px" style="cursor: pointer;"  ToolTip="выходной формат:">
                                <asp:ListItem Value="Xml_Custom">&nbsp;Xml-custom</asp:ListItem><%--xml-custom rendering--%>
                                <asp:ListItem Value="Html_FinalTonality" Selected="True">&nbsp;final tonality</asp:ListItem><%--1. Paint fore-color final tonality items--%>
                                <%--<asp:ListItem Value="Html_FinalTonalityDividedSentence">&nbsp;final tonality-sentence</asp:ListItem>--%>
                            </asp:DropDownList>
                            <asp:DropDownList ID="offsetCreationXmlTypeDropDownList" runat="server" Font-Size="X-Small" 
                                              Width="145px" style="cursor: pointer;" ToolTip="offset-creation-xml-type:">
                                <asp:ListItem Value="Flat" Selected="True">&nbsp;Flat</asp:ListItem>
                                <asp:ListItem Value="Hierarchy"           >&nbsp;Hierarchy</asp:ListItem>
                            </asp:DropDownList>
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
    </div>
    </form>
</body>
</html>
