﻿var currentlang = lang = "en";
function langPrefix() {
    if (getCookie('LANG') == null) {
        var language = (navigator.language) ? navigator.language : navigator.userLanguage;
        currentlang = lang = language.substring(0, 2)
        //setCookie('LANG_COOKIE', userLang.substring(0,2));
    } else {
        currentlang = lang = getCookie('LANG');
    }
   if((currentlang != "zh"))
	currentlang = "en";

    $("input[lang]").each(function () { $(this).val(message[currentlang][$(this).attr("lang")]); });
    $("[lang]").not("input").each(function () { $(this).html(message[currentlang][$(this).attr("lang")]); });
}

var message = {};
message.zh = {
    "device": "hidisk",
    "upload": "文件上传成功，要继续升级吗？",
    "language": "简体中文",
    "name": "名称",
    "modif": "最后修改",
    "size": "大小",
    "description":"描述",
    "parent": "返回上级目录",
    "remove": "删除",
    "choose": "选择网络...",

    "joinmode": "接入方式",
    "wired": "有线",
    "wireless": "无线",
    "3g":"3G",
    "dynamicip": "自动获取IP地址",
    "staticip": "使用如下IP地址",
    "pppoe": "PPPoE宽带接入",
    "auto3g":"自动拨号",
    "manual3g":"手动拨号",
    "apn":"APN：",
    "ip": "IP地址:",
    "gateway": "网关:",
    "mask": "子网掩码:",
    "dns1": "首选DNS:",
    "dns2": "备选DNS:",
    "autoscan": "无线接入:",
    "availableconnection": "可用Wi-Fi热点:",

    "username": "用户名:",
    "password": "密码:",
    "port": "端口:",
    "status": "状态:",
    "anonymousstatus": "匿名使用<br/>状态:",
    "path":"地址:",

    "networktype": "外网接口:",
    "powerstatus": "电源状态:",
    "storagestatus": "磁盘状态:",
    "scanningmsg": "正在扫描Wi-Fi热点...",
    "savingconfig": "正在保存设置,请稍候...",
    "loadingmsg": "正在读取数据，请稍候...",
    "refreshalert": "请刷新页面或重新连接！",
    "x8021alert": "不支持连接802.1X加密的Wi-Fi热点！",
    "connectpwdalert": "请输入Wi-Fi的密码!",
    "basicsettingsave": "当前设置未做修改，是否保存？",

    "noap":"没有可用的Wi-Fi热点！",
		
    "ipemptyalert": "IP地址不能为空！",
    "iperroralert": "IP地址错误！",
    "gatewayemptyalert": "网关不能为空！",
    "gatewayerroralert": "网关地址错误！",
    "maskemptyalert": "子网掩码不能为空！",
    "maskerroralert": "子网掩码地址错误！",
    "dns1emptyalert": "首选DNS不能为空！",
    "dns1erroralert": "首选DNS地址错误！",
    "dns2erroralert": "备选DNS地址错误！",
    "checkip":"请检查您所输入的网络参数是否正确！",
    "selectwifialert": "请选择一个可用的Wi-Fi!",
    "pwdlengthalert":"WEP密码位数错误！",
    "nostorage": "无磁盘挂载!",
    "wirednotice": "有线接入模式，请先插入网线。",
    "connectfailed":"Wi-Fi连接失败，请确认密码是否正确!",
    "emptyalert":"不能为空！",
    "dial":"拨号号码：",
    "statusok":"已连接上",
    "statusfail":"未连接上",
    "statusing":"正在连接...",
    "operator":"运营商:",

    "file": "文 件 管 理",
    "setting": "基 本 设 置",
    "networkconnect": "外 网 接 入",
    "advancedsetting": "高 级 设 置",
    "upgrade": "固 件 升 级",
    "fw": "版本号:",
    
    "devicename": "设备名称:",
    "security": "加密:",
    "encrypt_len":"加密长度",
    "format":"密码格式",
    "passwd": "密码:",
    "confpasswd": "确认<br/>密码:",
    "join": "加入网络",
    "wait": "请稍后...",
    "tips": "请输入13个有效字符.",
    "tips1": "提示1:在有线网连接模式下，您的设备将自动从您所连接的路由器获取IP地址和DNS。",
    "tips2": "提示2:在无线连接模式下，您需要扫描并连接到一个无线热点。",
    "checkstatus": "检查连接状态...",
    "alert1": "网络连接成功!",
    "alert2": "连接失败!\n请检查密码和网络!",
    "alert3": "超时!",
    "alert4": "请重新连接您的设备以检查是否成功。",
    "alert5": "设置成功！请确保您的网线已正确连接。",
    "warning": "警告",
    "alert7": "请留意设备上的Wi-Fi灯会闪烁。",
    "alert8": "升级的过程中请勿断电或重启设备！",
    "notice": "提示:您需要先从电脑上传固件再执行升级。",
    "done": "完成",
    "back": "返回",
    "browse": "浏览",
    "availablefilealert":"无效的固件!",
    "fileemptyalert": "请您先选择升级文件!",
    "correntfilealert": "请上传正确的BIN文件!",

    "power_status1":"正常",
    "power_status2":"充电中...",
    "power_status3":"放电...",
    "power_status4":"低电!",
    "status_all": "总容量:",
    "status_usage": "剩余容量:",
	
    "confirm": "确定",
    "cancel": "取消",	
    "emptyname":"请输入设备名。",
    "connecting":"正在连接",
    "errorpasswd1":"对于WPA/WPA2加密，密码位数最少为8位。",
    "errorpasswd2":"两次输入的密码不匹配。",

    "pwd": "无线密码:",
    "ssidlengthalert": "当前输入已超过32个字符(最大限定值)，请重新输入! ",
    "passwordlengthalert": "当前输入已超过32个字符(最大限定值)，请重新输入! ",
    "maxlength":"当前输入已超过32个字符(最大限定值)，请重新输入! ",
    "dmsnamemask":"设备名称不能包含以下任何字符：& \< >"
};
message.en = {
    "device": "hidisk",
    "upload": "File uploaded successfully! Continue to upgrade?",
    "language": "English",
    "name": "Name",
    "modif": "Modified",
    "size": "Size",
    "description":"Description",
    "parent": "Parent Directory",
    "remove": "Delete",
    "choose": "Choose a Network...",
	
    "ssidlengthalert": "The current input has more than 32 characters (maximum limit value), please enter again.",
    "passwordlengthalert": "The current input has more than 32 characters (maximum limit value), please enter again.",
    
    "joinmode": "Join Mode",
    "wired": "Wired",
    "wireless": "Wireless",
    "3g":"3G",
    "dynamicip": "Dynamic IP",
    "pppoe": "PPPoE",
    "staticip": "Static IP",
    "auto3g":"Auto",
    "manual3g":"Manual",
    "ip": "IP address:",
    "gateway": "Gateway:",
    "mask": "Subnet mask:",
    "dns1": "Preferred DNS:",
    "dns2": "Alternative DNS:",
    "autoscan": "Wireless access:",
    "availableconnection": "Available Wi-Fi hotspot:",
    
    "username": "Username:",
    "password": "Password:",
    "port": "Port:",
    "status": "Status:",
    "anonymousstatus": "Anonymous<br>enabled:",
    "path":"Path:",

    "networktype": "External Network Interface:",
    "powerstatus": "Power status:",
    "storagestatus": "Storage status:",
    "scanningmsg": "Scanning for Wi-Fi Hotspot...",
    "savingconfig": "Saving settings, please wait...",
    "loadingmsg": "Loading, please wait...",

    "refreshalert": "Please refresh the page or reconnect your device!",
    "x8021alert": "Wi-Fi hotspot encrypted by 802.1X is not supported!",
    "connectpwdalert": "Please enter Wi-Fi password!",
    "noap":"No available Wi-Fi!",
	
    "ipemptyalert": "IP address cannot be empty!",
    "iperroralert": "Wrong IP address!",
    "gatewayemptyalert": "Default gateway cannot be empty!",
    "gatewayerroralert": "Default gateway error!",
    "maskemptyalert": "Subnet mask cannot be empty!",
    "maskerroralert": "Subnet mask error!",
    "dns1emptyalert": "Preferred DNS cannot be empty!",
    "dns1erroralert": "Preferred DNS error!",
    "dns2erroralert": "Alternative DNS error!",
    "checkip":"Please check your network parameters!",
    "selectwifialert": "Please select a Wi-Fi network!",
    "pwdlengthalert": "Wrong WEP password digit!",
    "nostorage": "No external disk is detected!",
    "wirednotice": "Wired access mode, please plug in a network cable first.",
    "connectfailed": "Wi-Fi connection failed, password incorrect!",
    "emptyalert":"Can not be blank!",
    "dial":"Dial Number:",
    "statusok":"Connected",
    "statusfail":"Not Connected",
    "statusing":"Connecting...",
    "operator":"Operator:",

    "emptyname":"Device name can't be empty.",
    "connecting":"Connecting to ",
    "errorpasswd1":"Please enter minimum 8 characters under WPA/WPA2 encryption mode!",
    "errorpasswd2":"Passwords do not match, please enter again!",
	
    "confirm": "OK",
    "cancel": "Cancel",
    "pwd": "Password:",

    "file": "File Management",
    "setting": "Basic Settings",
    "networkconnect": "Internet Connection",
    "upgrade": "Upgrade Firmware",
    "advancedsetting": "Advanced Settings",
    "fw": "FW:",
    

    "devicename": "DeviceName:",
    "security": "Security:",
    "encrypt_len": "Encrypt<br>Length",
    "format": "Password<br>Format",
    "passwd": "Password:",
    "confpasswd": "Confirm<br>Password:",
    "join": "Join This Network",
    "wait": "Please wait...",
    "tips": "Please input exactly 13 characters.",
    "tips1": "Tips1:In wired network, getting IP address and DNS automatically form the connected router(DHCP mode).",
    "tips2": "Tips2:In wireless Network,you should connect to a wireless access point.",
    "checkstatus": "Checking status...",
    "alert1": "Network connect success !",
    "alert2": "Connection failed !\n Please check your password and network.",
    "alert3": "Time out !",
    "alert4": "Please reconnect to your device.",
    "alert5": "Set success !\nMake sure your wired network connected correctly.",
    "warning": "WARNINGS",
    "alert7": "Please wait and keep an eye on your device.<br>One LED should blink during upgrading.",
    "alert8": "Do not power off or reboot your device now!",
    "notice": "Notice: You need to upload the firmware from your computer before implementing the upgrade.",
    "done": "Done",
    "back": "Back",
    "browse": "Browse",
    "availablefilealert":"Invalid firmware!",
    "fileemptyalert":"Please select the upgrade file first!",
    "correntfilealert":"Please upload correct .bin file!",

    "power_status1": "Normal",
    "power_status2": "Charging...",
    "power_status3": "Discharging...",
    "power_status4": "Low power!",
    "status_all": "Total:",
    "status_usage": "Free:",
    
    "maxlength":"The current input has more than 32 characters (maximum limit value), please enter again.",
    "dmsnamemask":"Device name cannot contain any of the following characters: & < >"
};

function setLanguage(lang) {
    setCookie('LANG', lang);
    window.location.reload();
    //document.getElementById("langimg").src="icon/en-us.png";
    //alert(getCookie('LANG_COOKIE'));
}

//cookie相关
function getCookie(name) {
    var arr = document.cookie.split("; ");
    for (i = 0; i < arr.length; i++)
        if (arr[i].split("=")[0] == name)
            return unescape(arr[i].split("=")[1]);
    return null;
}
function setCookie(name, value) {
    var today = new Date();
    var expires = new Date();
    expires.setTime(today.getTime() + 1000 * 60 * 60 * 24 * 2000);
    document.cookie = name + "=" + escape(value) + "; expires=" + expires.toGMTString();

}

