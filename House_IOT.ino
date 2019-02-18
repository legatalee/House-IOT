#include <SPI.h>
#include <ESP8266WiFi.h>

char ssid[] = "DimiFi_2G";
char pass[] = "newdimigo";
int keyIndex = 0;

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
//  pinMode(D3, INPUT_PULLUP);
  pinMode(D1, OUTPUT);
  status = WiFi.begin(ssid, pass);
  delay(5000);
  server.begin();
  printWifiStatus();
}

int check = 1;

void loop() {
  if (digitalRead(D3) == LOW) {
    check = !check;
    delay(500);
  }
  digitalWrite(LED_BUILTIN, check);
  digitalWrite(D1, check);
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    //http 요청이 빈 라인으로 끝났을 때
    boolean currentLineIsBlank = true;
    String buffer = ""; //버퍼 선언
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        buffer += c;    //버퍼 할당
        Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {
          //표준 http 응답 헤더 전송 시작
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          //client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body>");
          client.println("<div style =\"text-align:center\">");

          //LED에 상황을 판단한후 상태를 알림
          if (check == 0) {
            client.println("<font size=800>Light is <font color='red'>ON</font>");
          } else {
            client.println("<font size=800>Light is <font color='black'>OFF</font>");
          }
          client.println("<br />");
          client.println("<FORM method=\"get\" action=\"/led.cgi\">");
          client.println("<P> <INPUT type=\"submit\" name=\"status\" value=\"ON\" style=\"font-size:150px;width:500px;height:300px\">");
          client.println("<p><p><INPUT type=\"submit\" name=\"status\" value=\"OFF\" style=\"font-size:150px;width:500px;height:300px\">");
          client.println("<br /><p><a href=\"/led.htm\">Dev. by Jimin Lee</a>");
          client.println("</body>");
          client.println("</div >");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
          buffer = "";
        }
        else if ( c == '\r') {
          if (buffer.indexOf("GET /led.cgi?status=ON") >= 0) {
            check = 0;
          }

          //off를 선택했을 때
          if (buffer.indexOf("GET /led.cgi?status=OFF") >= 0) {
            check = 1;
          }
          //client.println("<a href=\"/led.htm\">Go to control-page</a>");
        }
        else { //if( c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

