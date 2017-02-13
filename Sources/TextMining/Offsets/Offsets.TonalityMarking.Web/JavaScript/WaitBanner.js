var div_banner1, div_banner2;
function CreateTranslateWaitBanner(oPatentObject, sMessage) {
    var iframes = document.getElementsByTagName('iframe');
    if (iframes.length > 0) {
        for (var i = 0; i < iframes.length; i++) {
            iframes[i].style.visibility = "hidden";
        }
    }
    CreateWaitBanner2(oPatentObject, sMessage ? sMessage : "Идет перевод.....");
}
function ChangeWaitBannerMessage(sMessage) {
    if (div_banner2 != null) {
        document.getElementById('WaitBannerLabelMessage').innerHTML = sMessage;
    }
}
function CreateWaitBanner2(oPatentObject, sMessage) {
    DeleteWaitBanners();
    if (!oPatentObject) oPatentObject = document;
    if (!sMessage) sMessage = "выполняется обработка...";
    oPatentObject = (oPatentObject.ownerDocument) ? oPatentObject.ownerDocument : oPatentObject;    
    div_banner1 = oPatentObject.createElement("DIV");
    div_banner1.innerHTML =
				"<table style='width: 100%; height: 100%; background-color: black; position: absolute; top: 0px; left: 0px;" +
				"filter:progid:DXImageTransform.Microsoft.Alpha( Opacity=50, FinishOpacity=50, Style=1, StartX=0,  FinishX=100, StartY=0, FinishY=100);'>" +
				"<tr><td></td></tr></table>";
    oPatentObject.body.appendChild( div_banner1 );
    div_banner2 = oPatentObject.createElement("DIV");
    div_banner2.innerHTML =
				"<table style='width: 100%; height: 100%; background-color: Transparent; position: absolute; top: 0px; left: 0px;'>" +
				"<tr><td valign='middle' align='center'>" +
				"<table style='border: solid 1px black; background-color: whitesmoke;'><tr>" +
				"<td><img src='Images/Busy.gif' /></td>" +
				"<td style='padding-left: 5px' valign='middle' align='center'><label style='font-size: x-small' id='WaitBannerLabelMessage'>" + sMessage + "</label></td>" +
				"</tr></table></td></tr></table>";
    oPatentObject.body.appendChild( div_banner2 );
}
function CreateWaitBanner(oPatentObject, sMessage) {
    DeleteWaitBanners();
    if (!oPatentObject) oPatentObject = document;
    if (!sMessage) sMessage = "выполняется обработка...";
    var _document = (oPatentObject.ownerDocument) ? oPatentObject.ownerDocument : oPatentObject;
    /*div_banner1 = _document.createElement("DIV");
    div_banner1.innerHTML =
				"<table style='width: 100%; height: 100%; background-color: black; position: absolute; top: 0px; left: 0px;" +
				"filter:progid:DXImageTransform.Microsoft.Alpha( Opacity=30, FinishOpacity=30, Style=1, StartX=0,  FinishX=100, StartY=0, FinishY=100);'>" +
				"<tr><td></td></tr></table>";
    if (oPatentObject.body) oPatentObject.body.appendChild(div_banner1);
    else oPatentObject.appendChild(div_banner1);*/
    div_banner2 = _document.createElement("DIV");
    div_banner2.innerHTML =
                "<div style='width: 100%; height: 100%; background-color: Transparent; position: absolute; top: 0px; left: 0px;'>" +
				"<table style='width: 100%; height: 100%; background-color: Transparent; position: absolute; top: 0px; left: 0px;'>" +
				"<tr><td valign='middle' align='center'>" +
				"<table style='border: solid 1px black; background-color: whitesmoke;'><tr>" +
				"<td><img src='Images/Busy.gif' /></td>" +
				"<td style='padding-left: 5px' valign='middle' align='center'><label style='font-size: x-small'>" + sMessage + "</label></td>" +
				"</tr></table></td></tr></table></div>";
    if (oPatentObject.body) oPatentObject.body.appendChild(div_banner2);
    else oPatentObject.appendChild(div_banner2);
}
function DeleteWaitBanners() {
    try {
        if (div_banner1 != null) {
            div_banner1.parentElement.removeChild(div_banner1);
            div_banner1 = null;
        } if (div_banner2 != null) {
            div_banner2.parentElement.removeChild(div_banner2);
            div_banner2 = null;
        }
    }
    catch (e) {
        div_banner1 = null;
        div_banner2 = null;
    }
}
