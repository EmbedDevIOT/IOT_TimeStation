
var AvailableNetworks = "";
var ConfigFilePars = "";

// var FlagSSID = 0;
var IDLE = 0;
var ClockST = 0;

window.onload = function () {
   setInterval(function () {
      // Seconds
      var seconds = new Date().getSeconds();
      document.getElementById("seconds").innerHTML = (seconds < 10 ? '0' : '') + seconds;
      // Minutes
      var minutes = new Date().getMinutes();
      document.getElementById("minutes").innerHTML = (minutes < 10 ? '0' : '') + minutes;
      // Hours
      var hours = new Date().getHours();
      document.getElementById("hours").innerHTML = (hours < 10 ? '0' : '') + hours;
   }, 1000);
}

// Start "INDEX"
function StartIndex() {
   currentTime();
   getConfig("index");   	   // Load configuration or  config.json 	
   getUpdateIndex();
}

// Start "TIMESET" and one update
function StartTS() {
   getConfig("clock1");
   getUpdateService("clock");
}

// Start "SYSTEM"
function StartSM() {
   getConfig("system");
   // getUpdateSystem();
}

// Start "WIFI"
function StartWiFi() {
   getConfig("network"); //  Получение конфигурации из config.json  
   // getUpdateNetwork();
}

function getUpdateNetwork() {
   let o = new XMLHttpRequest();
   o.open("GET", "UpdateData.json", 1);
   o.send();
   o.onload = function () {
      let Pars = JSON.parse(o.responseText);
      // if ((FlagSSID == 0) && (WiFiMode == 0)) document.getElementById("signal").value = SignalStrength(Pars["signal"]);
      // else document.getElementById("signal").value = "Максимальный";
   }
   setTimeout("getUpdateNetwork()", 3000);
}

function StartService(part) {

   switch (part) {
      case "info":
         // getIP();
         getConfig("info");  // Parsing config.json and get information  
         getUpdateService("info");
         break;

      case "config":
         getConfig("service");  // Parsing config.json and get information  
         getUpdateService("service");
         break;
      default:
   }
}


// Функция получения настроек конфигурации
function getConfig(information) {
   // 1. Создаём новый XMLHttpRequest-объект с именем o
   let o = new XMLHttpRequest(); // Функция конструктор. При помощи встроенного в браузер обьекта XMLHttpRequest делаем HTTP запрос к серверу без перезагрузки страницы. 
   // 2. Настраиваем параметры запроса ( HTTP метод "GET" /куда запрос: URL или файл/)
   // o.open("GET", "config.json", 1); // поставить "/config.json"
   o.open("GET", "config.json", 1); // поставить "/config.json"
   // 3. Отправка инициализированного запроса 
   o.send();
   // 4. Этот код сработает после того, как мы получим ответ сервера
   o.onload = function () {
      // 5. Декодируем полученный текст ответа от сервера и запишем его в переменную ConfigFilePars
      ConfigFilePars = JSON.parse(o.responseText);

      // 6. Цикл для перебора всех свойств в строке ConfigFilePars
      for (let key in ConfigFilePars) {
         // Выполняется код внутри try, и если в нем нет ошибок - catch игнорируется. 
         try {
            let { type } = input = document.getElementById(key);
            if (type == "checkbox") input.checked = ConfigFilePars[key] > 0;
            else input.value = ConfigFilePars[key];
         }
         catch (e) { };

         if (information == "index") {
            if (ConfigFilePars["WiFiMode"] == 0) {
               document.getElementById("WiFIP").value = "192.168.4.1";
               document.getElementById("ssid").value = ConfigFilePars["apssid"];
            }
            else if (ConfigFilePars["WiFiMode"] == 1) {
               document.getElementById("WiFIP").value = ConfigFilePars["ip1"] + '.' + ConfigFilePars["ip2"] + '.' + ConfigFilePars["ip3"] + '.' + ConfigFilePars["ip4"];
            }
            document.getElementById("volt1").value = ConfigFilePars["volt1"] + " В";
         }

         else if (information == "network") {
            // document.getElementById("MAC").value = ConfigFilePars["MAC"];
            if (ConfigFilePars["WiFiMode"] == 1) {
               document.getElementById("WiFiMode").value = "ТОЧКА ДОСТУПА";
               document.getElementById("WiFIP").value = "192.168.4.1";
               document.getElementById("ssid").value = ConfigFilePars["apssid"];
               // document.getElementById("signal").value = "Максимальный";
            }
            else if (ConfigFilePars["WiFiMode"] == 0) {
               document.getElementById("WiFiMode").value = "КЛИЕНТ";
               document.getElementById("WiFIP").value = Pars["ip1"] + '.' + Pars["ip2"] + '.' + Pars["ip3"] + '.' + Pars["ip4"];
            }
         }
         else if (information == "clock1") {
            document.getElementById("TimeZone").value = ConfigFilePars["tz"];
         }
         else if (information == "system") {
            document.getElementById("volt1").value = ConfigFilePars["volt1"];
            document.getElementById("PulseNormal").value = ConfigFilePars["pulsn"];
            document.getElementById("PulseFast").value = ConfigFilePars["pulsf"];
            document.getElementById("BtnCH").value = ConfigFilePars["btn"];
         }
         else if (information == "info") {
            document.getElementById("volt1").value = ConfigFilePars["volt1"] + " В";

            if (ConfigFilePars["GPSMode"] == 0) {
               document.getElementById("GPSMode").value = "OFF";
               document.getElementById("GPSModeSel").value = 0;
            }
            else if (ConfigFilePars["GPSMode"] == 1) {
               document.getElementById("GPSMode").value = "ONCE";
               document.getElementById("GPSModeSel").value = 1;
            }
            else if (ConfigFilePars["GPSMode"] == 2) {
               document.getElementById("GPSMode").value = "ALWAYS";
               document.getElementById("GPSModeSel").value = 2;
            }
            if (ConfigFilePars["WiFiMode"] == 1) {
               document.getElementById("WiFiMode").value = "ТОЧКА ДОСТУПА";
               document.getElementById("WiFIP").value = "192.168.4.1";
               document.getElementById("ssid").value = ConfigFilePars["apssid"];

               // document.getElementById("signal").value = "Максимальный";
            }
            else if (ConfigFilePars["WiFiMode"] == 0) {
               document.getElementById("WiFiMode").value = "КЛИЕНТ";
               document.getElementById("WiFIP").value = Pars["ip1"] + '.' + Pars["ip2"] + '.' + Pars["ip3"] + '.' + Pars["ip4"];
            }
            document.getElementById("snum").value = ConfigFilePars["sn"];
            document.getElementById("fw").value = ConfigFilePars["firmware"];
         }
         else if (information == "service") {
            document.getElementById("ILimit").value = ConfigFilePars["iLimit"];
            document.getElementById("VBatMIN").value = ConfigFilePars["batlim"];
            document.getElementById("RSMode").value = ConfigFilePars["RS485"];
            document.getElementById("GPSMode").value = ConfigFilePars["GPSMode"];
            document.getElementById("snum").value = ConfigFilePars["sn"];
            document.getElementById("fw").value = ConfigFilePars["firmware"];

            if (ConfigFilePars["sn"] > 0) {
               const element1 = document.getElementById("sn");
               const element2 =  document.getElementById("BTNSaveSN");
               const attribute = document.createAttribute('disabled');
               const attribute2 = document.createAttribute('disabled');
               element1.setAttributeNode(attribute);
               element2.setAttributeNode(attribute2);
            }

         }
      }

      if (ConfigFilePars["sn"] == 0) {
         info_msg = "Внимание! Первый запуск устройства. Необходимо установить серийный номер и параметры конфигурации.";
         alert(info_msg);
      }
   }
}


function getUpdateIndex() {
   let o = new XMLHttpRequest();
   o.open("GET", "UpdateData.json", 1);
   o.send();
   o.onload = function () {
      let Pars = JSON.parse(o.responseText);


      if (Pars["IDLE"] == 1) {
         IDLE = 1;
         document.getElementById("btnPWRtext").value = "ОТКЛЮЧИТЬ";
         document.getElementById("btnPRW").classList.remove("btn-success");
         document.getElementById("btnPRW").classList.add("btn-danger");
      }
      else if (Pars["IDLE"] == 0) {
         IDLE = 0;
         document.getElementById("btnPRW").classList.remove("btn-danger");
         document.getElementById("btnPRW").classList.add("btn-success");
         document.getElementById("btnPWRtext").value = "ВКЛЮЧИТЬ";
      }

      if (Pars["voltage"] <= 0) {
         document.getElementById("voltage").value = "ОТКЛЮЧЕН";
         document.getElementById("voltage").classList.remove("E");
         document.getElementById("voltage").classList.add("D");
      }
      else if ((Pars["voltage"] <= 100) && (Pars["voltage"] > 0)) {
         document.getElementById("voltage").value = Pars["voltage"] + " %";
         document.getElementById("voltage").classList.remove("D");
         document.getElementById("voltage").classList.add("E");
      }

      document.getElementById("temperature").value = Pars["tSens"];

      // Проверка инициализации часового механизма №1
      if (Pars["ClockST1"] == 0) {
         document.getElementById("ClockST1").value = "Не настроены";
         document.getElementById("ClockST1").classList.remove("E");
         document.getElementById("ClockST1").classList.add("D");
      } else if (Pars["ClockST1"] == 1) {
         document.getElementById("ClockST1").value = Pars["TimeSC1"];
         document.getElementById("ClockST1").classList.remove("D");
         document.getElementById("ClockST1").classList.add("E");
      }
      else if (Pars["ClockST1"] == 2) {
         document.getElementById("ClockST1").value = "Остановлены";
         document.getElementById("ClockST1").classList.remove("E");
         document.getElementById("ClockST1").classList.add("D");
      }

      // Проверка состояния подсветки 
      if (Pars["BrST"] == "E") {
         document.getElementById("BrST").value = "Включена";
         document.getElementById("BrST").classList.remove("D");
         document.getElementById("BrST").classList.add("E");
      } else if (Pars["BrST"] == "D") {
         document.getElementById("BrST").value = "Отключена";
         document.getElementById("BrST").classList.remove("E");
         document.getElementById("BrST").classList.add("D");
      }

      // Проверка состояния питания 
      if (Pars["PwrST"] == 0) {
         document.getElementById("PwrST").value = "АКБ";
         document.getElementById("PwrST").classList.remove("E");
         document.getElementById("PwrST").classList.add("D");
      }
      else if (Pars["PwrST"] == 1) {
         document.getElementById("PwrST").value = "СЕТЬ";
         document.getElementById("PwrST").classList.remove("D");
         document.getElementById("PwrST").classList.add("E");
      }
      else if (Pars["PwrST"] == 2) {
         document.getElementById("PwrST").value = "USB";
         document.getElementById("voltage").value = "нет";
         document.getElementById("PwrST").classList.remove("E");
         document.getElementById("PwrST").classList.add("D");
      }

      // Проверка состояния GPS 
      if (Pars["GPSFIX"] == 1) {
         document.getElementById("GPSFIX").value = "Включено";
         document.getElementById("GPSFIX").classList.remove("D");
         document.getElementById("GPSFIX").classList.add("E");
      } else if (Pars["GPSFIX"] == 0) {
         document.getElementById("GPSFIX").value = "Отключено";
         document.getElementById("GPSFIX").classList.remove("E");
         document.getElementById("GPSFIX").classList.add("D");
      }

      // Проверка состояния RS-485
      if (Pars["RSST"] == 1) {
         document.getElementById("RS485").value = "Включено";
         document.getElementById("RS485").classList.remove("D");
         document.getElementById("RS485").classList.add("E");
      } else if (Pars["RSST"] == 0) {
         document.getElementById("RS485").value = "Отключено";
         document.getElementById("RS485").classList.remove("E");
         document.getElementById("RS485").classList.add("D");
      }

      // Проверка Режима работы WiFi
      if (Pars["WiFiMode"] == 1) {
         document.getElementById("WiFiMode").value = "Точка доступа";
         document.getElementById("WiFiMode").classList.remove("D");
         document.getElementById("WiFiMode").classList.add("E");
      } else if (Pars["WiFiMode"] == 0) {
         document.getElementById("WiFiMode").value = "Клиент";
         document.getElementById("WiFiMode").classList.remove("E");
         document.getElementById("WiFiMode").classList.add("D");
      }

      // if (FlagSSID == 0) document.getElementById("signal").value = SignalStrength(Pars["signal"]);
      // else document.getElementById("signal").value = "Максимальный";
      // if (FlagSSID == 0) document.getElementById("signal").src = SignalStrength(Pars["signal"], 0); else document.getElementById("signal").src = "/signal-1.bmp";
   }
   setTimeout("getUpdateIndex()", 1000);
}



function getUpdateService(conf) {
   let o = new XMLHttpRequest();
   o.open("GET", "UpdateData.json", 1);
   o.send();
   o.onload = function () {

      let Pars = JSON.parse(o.responseText);

      document.getElementById("t1").value = Pars["t1"];
      // ovr = document.getElementById("ILimit").value / 1000.0;
      // console.log(ovr);

      // if (document.getElementById("current").value >= ovr){
      //    info_msg = "Внимание! Превышение предельно допустимого значения тока. Пожалуйста, проверьте систему и перезагрузите устройство";
      //    alert(info_msg);
      // }

      switch (conf) {

         case "clock":
            // document.getElementById("t1").value = Pars["t1"];
            document.getElementById("TimeSC1").value = Pars["TimeSC1"];

            if (Pars["ClockST1"] == 0) {
               ClockST = 0;
               // var StartBTN = document.getElementById("StartBTN");
               // StartBTN.parentNode.removeChild(StartBTN);
               document.getElementById("StartBTNtext").value = "СТАРТ";
               document.getElementById("StartBTN").classList.remove("btn-danger");
               document.getElementById("StartBTN").classList.add("btn-success");
               document.getElementById("ClockST1").value = "Не настроены";
               // document.getElementById("ClockST1").classList.remove("D");
               // document.getElementById("ClockST1").classList.add("E");
            }
            else if (Pars["ClockST1"] == 1) {
               ClockST = 1;
               document.getElementById("StartBTNtext").value = "СТОП";
               document.getElementById("StartBTN").classList.remove("btn-success");
               document.getElementById("StartBTN").classList.add("btn-danger");

               document.getElementById("ClockST1").value = Pars["TimeSC1"];
               document.getElementById("ClockST1").classList.remove("D");
               document.getElementById("ClockST1").classList.add("E");
            }
            else if (Pars["ClockST1"] == 2) {
               ClockST = 2;
               document.getElementById("StartBTNtext").value = "СТАРТ";
               document.getElementById("StartBTN").classList.remove("btn-danger");
               document.getElementById("StartBTN").classList.add("btn-success");

               document.getElementById("ClockST1").value = "Остановлены";
               document.getElementById("ClockST1").classList.remove("D");
               document.getElementById("ClockST1").classList.add("E");
            }
            // setTimeout("getUpdateService('clock')", 1000);
            break;

         case "info":
            // document.getElementById("battery").value = Pars["battery"];
            if (Pars["voltage"] <= 0) {
               document.getElementById("voltage").value = "ОТКЛЮЧЕН";
               document.getElementById("voltage").classList.remove("E");
               document.getElementById("voltage").classList.add("D");
            }
            else if ((Pars["voltage"] <= 100) && (Pars["voltage"] > 0)) {
               document.getElementById("voltage").value = Pars["voltage"] + " %";
               document.getElementById("voltage").classList.remove("D");
               document.getElementById("voltage").classList.add("E");
            }
            // Check Power supply state
            if (Pars["PwrST"] == 0) {
               document.getElementById("PwrST").value = "АКБ";
               document.getElementById("PwrST").classList.remove("E");
               document.getElementById("PwrST").classList.add("D");
            }
            else if (Pars["PwrST"] == 1) {
               document.getElementById("PwrST").value = "СЕТЬ";
               document.getElementById("PwrST").classList.remove("D");
               document.getElementById("PwrST").classList.add("E");
            }
            else if (Pars["PwrST"] == 2) {
               document.getElementById("PwrST").value = "USB";
               document.getElementById("voltage").value = " ";
               document.getElementById("battery").value = " ";
               document.getElementById("PwrST").classList.remove("E");
               document.getElementById("PwrST").classList.add("D");

            }
            // Check GPS state
            // document.getElementById("GPSPWR").value = Pars["GPSPWR"];

            if (Pars["GPSPWR"] == 1) {
               document.getElementById("GPSPWR").value = "Включено";
               document.getElementById("GPSPWR").classList.remove("D");
               document.getElementById("GPSPWR").classList.add("E");

               if (Pars["GPSValid"] == 1) {
                  document.getElementById("TimeGPS").value = Pars["TimeGPS"];
                  document.getElementById("TimeGPS").classList.remove("D");
                  document.getElementById("TimeGPS").classList.add("E");
                  document.getElementById("Sattelite").value = Pars["sat"];
                  document.getElementById("GPSage").value = Pars["GPSage"];
                  document.getElementById("GPSFIX").value = Pars["GPSFIX"];
                  document.getElementById("GPSValid").value = "DONE";
                  document.getElementById("GPSValid").classList.remove("D");
                  document.getElementById("GPSValid").classList.add("E");
               } else if (Pars["GPSValid"] == 0) {
                  document.getElementById("TimeGPS").value = Pars["TimeGPS"];
                  document.getElementById("Sattelite").value = "Не установлено";
                  document.getElementById("TimeGPS").classList.remove("E");
                  document.getElementById("TimeGPS").classList.add("D");
                  document.getElementById("GPSage").value = Pars["GPSage"];
                  document.getElementById("GPSFIX").value = Pars["GPSFIX"];
                  document.getElementById("GPSValid").value = "not compleated";
                  document.getElementById("GPSValid").classList.remove("E");
                  document.getElementById("GPSValid").classList.add("D");
               }
            }
            else if (Pars["GPSPWR"] == 0) {
               document.getElementById("GPSPWR").value = "Отключено";
               document.getElementById("GPSPWR").classList.remove("E");
               document.getElementById("GPSPWR").classList.add("D");
            }
            setTimeout("getUpdateService('info')", 1000);

            break;

         case "service":
            // Backlight control
            document.getElementById("current").value = Pars["current"];
            document.getElementById("light").value = Pars["light"];


            if (Pars["BrST"] == "E") {
               document.getElementById("BrST").value = "Включена";
               document.getElementById("BrST").classList.remove("D");
               document.getElementById("BrST").classList.add("E");
               document.getElementById("BTNBLT").value = "ОТКЛЮЧИТЬ";
               document.getElementById("BTNBL").classList.remove("btn-success");
               document.getElementById("BTNBL").classList.add("btn-danger");
            } else if (Pars["BrST"] == "D") {
               document.getElementById("BrST").value = "Отключена";
               document.getElementById("BrST").classList.remove("E");
               document.getElementById("BrST").classList.add("D");
               document.getElementById("BTNBLT").value = "ВКЛЮЧИТЬ";
               document.getElementById("BTNBL").classList.remove("btn-danger");
               document.getElementById("BTNBL").classList.add("btn-success");
            }
            setTimeout("getUpdateService('service')", 1000);
            break;
         default:
            break;
      }
      // document.getElementById("DrIN1").value = Pars["DrIN1"];
      // document.getElementById("DrIN2").value = Pars["DrIN2"];
      // document.getElementById("DrIN3").value = Pars["DrIN3"];
      // document.getElementById("DrIN4").value = Pars["DrIN4"];
      // document.getElementById("DCEN").value = Pars["DCS"];
      // document.getElementById("DCVCC").value = Pars["DCV"];


      // if (FlagSSID == 0) document.getElementById("signal").value = SignalStrength(Pars["signal"]);
      // else document.getElementById("signal").value = "Максимальный";
      // if (FlagSSID == 0) document.getElementById("signal").src = SignalStrength(Pars["signal"], 0); else document.getElementById("signal").src = "/signal-1.bmp";
   }

}

function SignalStrength(strength) {
   if (strength >= -50) return "Отличный";
   if (-51 >= strength && strength >= -65) return "Хороший";
   if (-66 >= strength && strength >= -75) return "Средний";
   if (-76 >= strength && strength >= -85) return "Плохой";
   if (-86 >= strength) return "Нет сигнала";
}

function Stop() {
   let o = new XMLHttpRequest();
   o.open("GET", "Stop", 1);
   o.send();
}

function ButtonStart() {

   let o = new XMLHttpRequest();
   switch (ClockST) {
      case 0:
         document.getElementById("StartBTNtext").value = "СТАРТ";
         document.getElementById("StartBTN").classList.remove("btn-danger");
         document.getElementById("StartBTN").classList.add("btn-success");
         break;
      case 1:
         document.getElementById("StartBTNtext").value = "СТАРТ";
         document.getElementById("StartBTN").classList.remove("btn-danger");
         document.getElementById("StartBTN").classList.add("btn-success");
         break;
      case 2:
         document.getElementById("StartBTNtext").value = "СТОП";
         document.getElementById("StartBTN").classList.remove("btn-success");
         document.getElementById("StartBTN").classList.add("btn-danger");
         break;
      default:
         break;
   }
   o.open("GET", "Start", 1);
   o.send();
   // setTimeout("getUpdateService('clock')", 1000); // WTF костыль ?
}

function ButtonDebug(state) {
   let o = new XMLHttpRequest();

   switch (state) {
      case "current":
         o.open("GET", "BCurrDebug", true);
         break;

      default:
         o.open("GET", "BDebug", true);
         break;
   }
   o.send();
}

function ButtonPulse() {
   let o = new XMLHttpRequest();
   o.open("GET", "BPulse", 1);
   o.send();
}

function ButtonClockPulse(time_value) {
   let o = new XMLHttpRequest();
   let buf = "?";

   switch (time_value) {
      case 1:
         buf += "CH=" + document.getElementById("Clock_CH").value + "&";
         buf += "pulse=" + "1" + "&";
         break;

      case 2:
         buf += "CH=" + document.getElementById("Clock_CH").value + "&";
         buf += "pulse=" + "60" + "&";
         break;
   }
   o.open("GET", "BCP" + buf, 1);
   o.send();
}

function ButtonGoHome() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "CH=" + document.getElementById("Clock_CH").value + "&";
   o.open("GET", "BGHP" + buf, 1);
   o.send();
}

function ButtonGetSSID() {
   document.getElementById("loadd").style.display = "block";
   let o = new XMLHttpRequest();
   o.open("GET", "/getssid.json", 1);
   o.send();
   o.onload = function () {
      AvailableNetworks = JSON.parse(o.responseText);
      var objSel = document.getElementById("SsidSelect");
      let i = 0;
      for (let key in AvailableNetworks) {
         try {
            objSel.options[i] = new Option(key + "~" + SignalStrength(AvailableNetworks[key], 1), key);
            i++;
         }
         catch (e) { };
      }
      objSel.options[i] = new Option("Нет Сети", "NoNetwork");
      document.getElementById("loadd").style.display = "none";
   }
}

function ButtonEditSSID() {
   document.getElementById("pass").value = ConfigFilePars["pass"];
   window.location.href = '#openSSID';
}

function ButtonSaveSSID() {
   var objSel = document.getElementById("SsidSelect");
   var objout = document.getElementById("ssid");
   if (objSel.selectedIndex != (-1)) {
      if (objSel.options[objSel.selectedIndex].value != ConfigFilePars["ssid"]) FlagSSID = 1;
      objout.value = objSel.options[objSel.selectedIndex].value;
      let Signal = AvailableNetworks[objSel.options[objSel.selectedIndex].value];
      document.getElementById("signal").src = SignalStrength(Signal, 0);
      if (objSel.options[objSel.selectedIndex].value == "NoNetwork") ConfigFilePars["pass"] = "null";
      else ConfigFilePars["pass"] = document.getElementById("pass").value;
   }
   else
      ConfigFilePars["pass"] = document.getElementById("pass").value;
   window.location.href = '#close';
}

function ButtonStateRS() {

   var coffee = document.forms[0];
   var txt = "";
   var i;
   for (i = 0; i < coffee.length; i++) {
      if (coffee[i].checked) {
         txt = txt + coffee[i].value + " ";
      }
   }
   document.getElementById("out").value = txt;

}

function ButtonSetGPS() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "S=" + document.getElementById("GPSModeSel").value + "&";
   o.open("GET", "BGPS" + buf, 1);
   o.send();

   setTimeout("reload()", 5000);
}


function ButtonReboot() {
   let o = new XMLHttpRequest();
   o.open("GET", "BReboot", 1);
   o.send();
   setTimeout("reload()", 10000);
}

function ButtonFactoryReset() {
   let o = new XMLHttpRequest();
   o.open("GET", "BFReset", 1);
   o.send();
   setTimeout("reload()", 10000);
}

function ButtonIDLE() {
   let o = new XMLHttpRequest();
   let msg;
   let buf = "?";

   switch (IDLE) {
      case 1:
         msg = "Часовая станция остановлена";
         alert(msg);
         buf += "IDLE=0" + "&";
         o.open("GET", "BPWR" + buf, 1);
         console.log(msg);
         break;
      case 0:
         msg = "Часовая станция запушена";
         alert(msg);
         console.log(msg);
         buf += "IDLE=1" + "&";
         o.open("GET", "BPWR" + buf, 1);
         break;

      default:
         break;
   }
   o.send();
   setTimeout("reload()", 5000);
}

function ButtonSynchro(watch_mumber) {

   let o = new XMLHttpRequest();
   let now = new Date();

   let buf = "?";

   buf += "H=" + now.getHours() + "&";
   buf += "M=" + now.getMinutes() + "&";
   buf += "S=" + now.getSeconds() + "&";
   buf += "tz=" + document.getElementById("TimeZone").value + "&";
   buf += "TimeSC=" + document.getElementById("TimeSC1").value + "&";

   if (watch_mumber == 1) {
      o.open("GET", "W1_UPD" + buf, 1);
      o.send();
   }
   else if (watch_mumber == 2) {
      buf += "TimeSC2=" + document.getElementById("TimeSC2").value + "&";
      buf += "tz=" + document.getElementById("TimeZone").value + "&";
      o.open("GET", "W2_UPD" + buf, 1);
      o.send();
   }
   o.onload = function () {
      setTimeout("DataSynch()", 2000); // data synchronization (timeout 2000ms)
   }
}

function ButtonNTPSynch() {

   if (!document.getElementById("ntpurl").value) {
      msg = "Неверный параметр."
      alert(msg);
   }

   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "Server=" + document.getElementById("ntpurl").value + "&";
   o.open("GET", "BNTPS" + buf, 1);
   o.send();
}

function ButtonWiFiControl(mode) {
   let o = new XMLHttpRequest();
   let info_msg;

   switch (mode) {

      case 1:
         info_msg = "Внимание! Устройство будет переведено в режим работы: ТОЧКА ДОСТУПА. Для дальнейшего использования в параметрах сети WiFi найдите и подключитесь к ClockStation  пароль: 1234qwerty  IP:192.168.4.1";
         alert(info_msg);
         o.open("GET", "BWPS", 1);
         break;

      case 2:
         info_msg = "Внимание! Устройство будет переведено в режим работы: КЛИЕНТ. Убедитесь в подключении к сети WiFi";
         alert(info_msg);
         o.open("GET", "BWPC", 1);
         break;

      default: 0
         break;
   }
   o.send();
   setTimeout("ButtonReboot()", 2000);
}


// function ButtonEditIP() {
//    document.getElementById("ip1Modal").value = ConfigFilePars["ip1"];
//    document.getElementById("ip2Modal").value = ConfigFilePars["ip2"];
//    document.getElementById("ip3Modal").value = ConfigFilePars["ip3"];
//    document.getElementById("ip4Modal").value = ConfigFilePars["ip4"];

//    document.getElementById("gw1").value = ConfigFilePars["gw1"];
//    document.getElementById("gw2").value = ConfigFilePars["gw2"];
//    document.getElementById("gw3").value = ConfigFilePars["gw3"];
//    document.getElementById("gw4").value = ConfigFilePars["gw4"];

//    document.getElementById("mk1").value = ConfigFilePars["mk1"];
//    document.getElementById("mk2").value = ConfigFilePars["mk2"];
//    document.getElementById("mk3").value = ConfigFilePars["mk3"];
//    document.getElementById("mk4").value = ConfigFilePars["mk4"];

//    window.location.href = '#openIP';
// }

// function ButtonSaveIP() {
//    ConfigFilePars["ip1"] = document.getElementById("ip1Modal").value;
//    ConfigFilePars["ip2"] = document.getElementById("ip2Modal").value;
//    ConfigFilePars["ip3"] = document.getElementById("ip3Modal").value;
//    ConfigFilePars["ip4"] = document.getElementById("ip4Modal").value;

//    ConfigFilePars["gw1"] = document.getElementById("gw1").value;
//    ConfigFilePars["gw2"] = document.getElementById("gw2").value;
//    ConfigFilePars["gw3"] = document.getElementById("gw3").value;
//    ConfigFilePars["gw4"] = document.getElementById("gw4").value;

//    ConfigFilePars["mk1"] = document.getElementById("mk1").value;
//    ConfigFilePars["mk2"] = document.getElementById("mk2").value;
//    ConfigFilePars["mk3"] = document.getElementById("mk3").value;
//    ConfigFilePars["mk4"] = document.getElementById("mk4").value;

//    document.getElementById("ip1").value = ConfigFilePars["ip1"];
//    document.getElementById("ip2").value = ConfigFilePars["ip2"];
//    document.getElementById("ip3").value = ConfigFilePars["ip3"];
//    document.getElementById("ip4").value = ConfigFilePars["ip4"];
//    window.location.href = '#close';
// }

function ButtonSaveSystem() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "BrStart=" + document.getElementById("BrStart").value + "&";
   buf += "BrStop=" + document.getElementById("BrStop").value + "&";
   buf += "LightEN=" + "1" + "&";
   buf += "volt1=" + document.getElementById("volt1").value + "&";
   // buf += "volt2=" + document.getElementById("volt2").value + "&";
   buf += "pulsn=" + document.getElementById("PulseNormal").value + "&";
   buf += "pulsf=" + document.getElementById("PulseFast").value + "&";
   buf += "BtnAct=" + document.getElementById("BtnCH").value + "&";
   o.open("GET", "BSysSave" + buf, 1);
   o.send();
   // info_msg = "Сохранить настройки на устройстве ?";
   // alert(info_msg);
}

function ButtonSaveService() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "TimSGPS=" + document.getElementById("TimSGPS").value + "&";
   // buf += "GPSSynhEN=" + "1" + "&";
   // buf += "iLimit=" + document.getElementById("ILimit").value + "&";
   buf += "batlim=" + document.getElementById("VBatMIN").value + "&";
   buf += "GPSMode=" + document.getElementById("GPSMode").value + "&";
   buf += "RSMode=" + document.getElementById("RSMode").value + "&";

   info_msg = "Сохранить настройки на устройстве ?";
   alert(info_msg);
   o.open("GET", "BServSave" + buf, 1);
   o.send();
   setTimeout("reload()", 1000);
}

function ButtonSaveIP() {
   let o = new XMLHttpRequest();
   // let buf = "?";
   // buf += "ip1=" + document.getElementById("ip1").value + "&";
   // buf += "ip2=" + document.getElementById("ip2").value + "&";
   // buf += "ip3=" + document.getElementById("ip3").value + "&";
   // buf += "ip4=" + document.getElementById("ip4").value + "&";

   // o.open("GET", "BSIP" + buf, 1);
   // o.send();

   buf = "?";
   buf += "apssid=" + document.getElementById("apssid").value + "&";
   buf += "appass=" + document.getElementById("appass").value + "&";

   o.open("GET", "BSNS" + buf, 1);
   o.send();

   msg = "Внимание! Устройство будет принудительно перезагружено."
   alert(msg);

   o.onload = function () {
      setTimeout("ButtonReboot()", 1000); // data synchronization (timeout 2000ms)
      setTimeout("reload()", 10000);
   }
}

function ButtonSaveSN() {

   if (ConfigFilePars["sn"] == 0) {
      if (document.getElementById("sn").value >= 1) {
         let o = new XMLHttpRequest();
         let buf = "?";
         buf += "sn=" + document.getElementById("sn").value + "&";
         o.open("GET", "BSSN" + buf, 1);
         o.send();
         setTimeout("reload()", 1000);
      }
      else {
         info_msg = "Серийный номер не может принимать отрицательное значение. И должен принимать значение от 1 до 65535";
         alert(info_msg);
      }
   }
   else {
      info_msg = "Серийный номер уже установлен. Изменений не требуется.";
      alert(info_msg);
   }
}

// Current Sensor Sensetivity update 
function ButtonCurSensUPD(_element) {
   let buf = "?";
   buf += "sensetiv=" + document.getElementById("i_sens").value + "&";
   buf += "error_cor=" + document.getElementById("i_cor").value + "&";
   buf += "iLimit=" + document.getElementById("ILimit").value + "&";
   console.log(buf);
   let o = new XMLHttpRequest();
   o.open("GET", "/CurLim" + buf, true);
   o.send();
}

function check2() {
   alert("selected: " + $('input[RS485OFF]:checked').val());
}

function TimeSynch() {
   let o = new XMLHttpRequest();
   let now = new Date();
   o.open("GET", "BTS?Hours=" + now.getHours() + "&Minutes=" + now.getMinutes() + "&Seconds=" + now.getSeconds(), true);
   o.send();
   o.onload = function () {
      setTimeout("DataSynch()", 2000); // data synchronization (timeout 2000ms)
   }
   // alert("Время часовой станции синхронизировано c временем на данном устройстве");
}

function DataSynch() {
   let o = new XMLHttpRequest();
   let now = new Date();

   o.open("GET", "BDS?Year=" + now.getFullYear() + "&Month=" + (now.getMonth() + 1) + "&Date=" + now.getDate(), true);
   o.send();
   // setTimeout("reload()", 4000);
}

// BackLight ON - OFF
function Backlight() {
   let o = new XMLHttpRequest();
   o.open("GET", "Light", 1);
   o.send();
}

// Light sensor trigger update 
function SensorLimitUpdate(_element) {
   var sensor_value = document.getElementById("sens").value;
   // document.getElementById("light").innerHTML = sensor_value;
   console.log(sensor_value);
   let o = new XMLHttpRequest();
   o.open("GET", "/senslim?Value=" + sensor_value, true);
   o.send();
}



// RS ON OF
function RS_CONTROL() {
   let o = new XMLHttpRequest();
   console.log(document.getElementById("RSST").value);
   o.open("GET", "RS", 1);
   o.send();
}

function DrControl(pin_number) {
   let o = new XMLHttpRequest();
   if (pin_number == 1) { o.open("GET", "IN1", 1); }
   else if (pin_number == 2) o.open("GET", "IN2", 1);
   else if (pin_number == 3) o.open("GET", "IN3", 1);
   else if (pin_number == 4) o.open("GET", "IN4", 1);
   else if (pin_number == 5) o.open("GET", "EN1", 1);
   else if (pin_number == 6) o.open("GET", "EN2", 1);
   o.send();
}

function DCDCControl() {
   let o = new XMLHttpRequest();
   o.open("GET", "DCS", 1);
   o.send();
}





function CheckLogin() {
   let o = new XMLHttpRequest();
   o.open("GET", "BSIN?L=" + document.getElementById("user").value + "&P=" + document.getElementById("pwd").value, true);
   o.send();
   //  setTimeout(location.reload(),5000);
   // setTimeout("reload()", 5000);
}

function FirmwareUPD() {

}

// function SbrosOK() {
//    document.getElementById("ending").classList.remove("B");
// }



function ServiceSave() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "sens=" + document.getElementById("sens").value + "&";
   o.open("GET", "SSave" + buf, 1);
   o.send();
}


function ButtonSave() {
   let o = new XMLHttpRequest();
   let buf = "?";
   buf += "t2=" + document.getElementById("t2").value + "&";
   buf += "t3=" + document.getElementById("t3").value + "&";
   buf += "t4=" + document.getElementById("t4").value + "&";
   buf += "on=" + (document.getElementById("on").checked ? 1 : 0) + "&";
   buf += "sens=" + document.getElementById("sens").value + "&";
   buf += "ssid=" + document.getElementById("ssid").value + "&";
   buf += "pass=" + ConfigFilePars["pass"] + "&";
   buf += "ip1=" + ConfigFilePars["ip1"] + "&";
   buf += "ip2=" + ConfigFilePars["ip2"] + "&";
   buf += "ip3=" + ConfigFilePars["ip3"] + "&";
   buf += "ip4=" + ConfigFilePars["ip4"] + "&";
   buf += "gw1=" + ConfigFilePars["gw1"] + "&";
   buf += "gw2=" + ConfigFilePars["gw2"] + "&";
   buf += "gw3=" + ConfigFilePars["gw3"] + "&";
   buf += "gw4=" + ConfigFilePars["gw4"] + "&";
   buf += "mk1=" + ConfigFilePars["mk1"] + "&";
   buf += "mk2=" + ConfigFilePars["mk2"] + "&";
   buf += "mk3=" + ConfigFilePars["mk3"] + "&";
   buf += "mk4=" + ConfigFilePars["mk4"];
   o.open("GET", "BSave" + buf, 1);
   o.send();
   // o.onload = function () {
   //    if (this.responseText == "OK") {
   //       document.getElementById("ending").classList.add("B");
   //       setTimeout("SbrosOK()", 2000);
   //    }
   // }
}

// Данный метод перезагружает страницу 
function reload() {
   window.location.reload();
}