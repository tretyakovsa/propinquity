void webUpgrade() {
  Serial.println("Update module...");
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"40;/\">Update module...</head></html>";
  HTTP.send(200, "text/html", refresh);
  spiffsData = HTTP.arg("spiffs");
  if (spiffsData != ""){
  spiffsData = spiffsData.substring(spiffsData.lastIndexOf("/")+1); // выделяем имя файла
  Serial.println(spiffsData);
  ESPhttpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(HTTP.arg("spiffs"));
  timer_Save();
  saveConfig();
  }
  buildData = HTTP.arg("build");
  if (buildData != ""){
  buildData = buildData.substring(buildData.lastIndexOf("/")+1); // выделяем имя файла
  Serial.println(buildData);
  saveConfig();
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return jet = ESPhttpUpdate.update(HTTP.arg("build"));
  }
}

// Перезагрузка модуля
void handle_restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  } else {                                        // иначе
    HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

// Установка языка
void handle_leng() {
  Language = HTTP.arg("set");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
// Сохраняет все настройки в файле
void handle_save_config() {
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}



void HTTP_init(void) {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Кешировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  HTTP.on("/upgrade", webUpgrade);                // запустить обнавление
  HTTP.on("/save", handle_save_config);      // Сохранить настройки в файл
  HTTP.on("/config.live.json", handle_ConfigJSON); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  // Запускаем HTTP сервер
  HTTP.begin();
}



void handle_ConfigJSON() {
  //{"SSDP":"SSDP-test","ssid":"home","ssidPass":"i12345678","ssidAP":"WiFi","ssidApPass":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json
  // Заполняем поля json
  json["spiffsData"] = spiffsData;
  json["buildData"] = buildData;
  //String temp = "";
  //temp += dht.getTemperature();
  //json["Temperature"] = temp;
  //temp = "";
  //temp += dht.getHumidity();
  //json["Humidity"] = temp;
  json["SSDP"] = ssdpName;
  json["space"] = spaceName;
  json["timeZone"] = timezone;
  json["timeSonoff"] = timeSonoff; //  Время работы

  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime(); // Текущее время
  json["lang"] = Language;  // Язык
  json["state"] = state0;
  // Помещаем созданный json в переменную root
  jsonConfig = "";
  json.printTo(jsonConfig);
  HTTP.send(200, "text/json", jsonConfig);
}

void handle_ip_list() {
  HTTP.send(200, "text/json", "[" + modulesNew + Devices + "]");
}

void handle_modules() {
  HTTP.send(200, "text/json", modulesNew);
}


void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}



String graf(String title, int in, int points, int  refresh) {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
 // {"data":[1],"points":"10","refresh":"1","title":"Analog"}
 // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
 DynamicJsonBuffer jsonBuffer;
 // вызовите парсер JSON через экземпляр jsonBuffer
 JsonObject& json = jsonBuffer.parseObject(root);
 // Заполняем поля json
 JsonArray& data = json.createNestedArray("data");
 data.add(in);
 json["points"] = points;
 json["refresh"] = refresh;
 json["title"] = title;
 // Помещаем созданный json в переменную root
 root="";
 json.printTo(root);
 return root;
  }
