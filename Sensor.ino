/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <DHT.h>                     //https://github.com/markruys/arduino-DHT
  // DHT C автоматическим определением датчиков.Поддержка датчиков DHT11,DHT22, AM2302, RHT03.
  DHT dht;
  Нет обращений к глобальным переменным
  Объект ESP8266WebServer HTTP(80); должен быть инециализирован в заголовке
*/

void analog_init() {
  HTTP.on("/analog.json", HTTP_GET, []() {
    HTTP.send(200, "text/json", graf("Illumination", analogRead(A0), 30, 3000));
  });
  modulesReg("analog");
}






