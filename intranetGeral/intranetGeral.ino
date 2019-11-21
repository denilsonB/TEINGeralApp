#include <ESP8266WebServer.h>
#include <Servo.h>

#define SERVO D6
Servo s; // Variável Servo
int pos; // Posição Servo
const uint8_t trig_pin = D0;
const uint8_t echo_pin = D1;
const byte pino_microfone = D2;
int ledPin = D3; //led do lrd
int ldrPin = A0; //LDR ligado na porta analogica
int ldrValor = 0; //Valor lido do LDR
const byte pino_led = D4;//led do microfone
volatile byte rele = LOW; 
uint32_t print_timer;
uint32_t print_timer_microfone;//variavel pra mudar o valor do led do microfone(não deixar receber muitas vezes)
int pino_buzzer = D5;

ESP8266WebServer server(80);
String guardaUltrassom="",guardaLdr="",guardaLuzLdr="", guardaMicrofone="", guardaLuzMicrofone="", atualizaPagina="N";

void setup() {
  Serial.begin(9600); // Habilita Comunicação Serial a uma taxa de 9600 bauds.
  s.attach(SERVO);
  s.write(0);// Inicia motor posição zero
  // Configuração do estado inicial dos pinos Trig e Echo.
  pinMode(pino_microfone, INPUT_PULLUP); //Coloca o pino do sensor de som como entrada
  pinMode(pino_led, OUTPUT); 
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(pino_buzzer, OUTPUT);
  pinMode(ledPin,OUTPUT);
  digitalWrite(trig_pin, LOW);
  attachInterrupt(digitalPinToInterrupt(pino_microfone), interrupcaoMicrofone, RISING);//Função para ativar o led quando o microfone receber alguma entrada

  configurarAP();
  // Serial.begin(115200);
  server.on("/",paginaPrincipal);
  server.on("/salvo1",LigaSeguranca);
  server.on("/salvo2",LigaLdr);
  server.on("/salvo3",LigaLuzLdr);
  server.on("/salvo4",LigaMicrofone);
  server.on("/salvo5",LigaLuzDoMicrofone); 
  server.begin();

}

void loop() {
  server.handleClient();
    if(guardaUltrassom=="checked"){
          ultrassom();                 
    }
    if(guardaLdr=="checked" and guardaLuzLdr==""){
          resistorLdr();
          Serial.println("Clicado");                 
    }
    if(guardaMicrofone=="checked"){
          digitalWrite(pino_led, rele); // Manda o valor da variavel para o rele
          if(rele==HIGH){
             atualizaPagina="S";  
          }else{
             atualizaPagina="N";
          }
    }
    if(rele==LOW){
          guardaLuzMicrofone="";
     }
}
void configurarAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();//Faz esse processo para ter certeza que saiu do modo Station;
  delay(100);
  if (WiFi.softAP("Esp ", "ADMINISTRADOR", 10) == 1) {
    Serial.println("ESP agora possui uma rede propria");
    Serial.println("\nSeu ip na rede:");
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(IPAddress(192, 168, 0, 125), IPAddress(192, 168, 0, 1), subnet);
    WiFi.softAPConfig (IPAddress(192, 168, 0, 65), IPAddress (192, 168, 0, 1), subnet);
    Serial.println(WiFi.softAPIP());
    }
}

void paginaPrincipal(){
  String s="";
             s+="<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Testando</title><meta charset=\"utf-8\"><style type=\"text/css\">";
             s+=".onoffswitch {position: relative; width: 90px;-webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;margin-left:70%;}" ;
             s+=".entradaonoff {display: none;}.texto-onoff {display: block; overflow: hidden; cursor: pointer;border: 2px solid #999999; border-radius: 20px;}";
             s+=".spanonoff {display: block; width: 200%; margin-left: -100%;transition: margin 0.3s ease-in 0s;}";
             s+=".spanonoff:before, .spanonoff:after {display: block; float: left; width: 50%; height: 30px; padding: 0; line-height: 30px;";
             s+="font-size: 14px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold;box-sizing: border-box;}";
             s+=".spanonoff:before {content: 'ON';padding-left: 10px;background-color: green; color: #FFFFFF;}" ;
             s+=".spanonoff:after {content: 'OFF';padding-right: 10px;background-color: red; color: #FFF;text-align: right;}"; 
             s+=".bola-onoff {display: block; width: 18px; margin: 6px;background: #FFFFFF;position: absolute; top: 0; bottom: 0;" ;
             s+="right: 56px;border: 2px solid #999999; border-radius: 20px;transition: all 0.3s ease-in 0s;}" ;
             s+=".entradaonoff:checked + .texto-onoff .spanonoff {margin-left: 0px;}.entradaonoff:checked + .texto-onoff .bola-onoff {right: 0px;}" ;
             s+=".paginaPrincipal{background-color: green;border: 2px solid black;}</style></head>";
          
          s += "<body onload=aoCarregar()> <h1 class='paginaPrincipal'>Segurança</h1> <div c";
          s += "lass='onoffswitch'> <form action='/salvo1' method='POST' id='formu1'>  <input onclick=enviar1() name";
          s += "='tes1' type='checkbox' class='entradaonoff' id='onoff' "+guardaUltrassom+"> <label class='texto-onoff' for=";
          s += "'onoff'> <span class='spanonoff'></span> <span class='bola-onoff'></span> </label> </form> </div><br";
          s += "> <h1 class='paginaPrincipal'>Ldr</h1>  <div class='onoffswitch'> <form action='/salvo2' method='POST' id='formu2'>";
          s += "<input name='tes2' type='checkbox' class='entradaonoff' id='onoff2' onclick= enviar2() "+guardaLdr+">";
          s += "<label class='texto-onoff' for='onoff2'> <span class='spanonoff'></span>  <span class='bola-";
          s += "onoff'></span> </label> </form> </div><br> <h1 class='paginaPrincipal'>Luz ldr</h1>  <div class='onoffswitch'>";
          s += " <form action='/salvo3' method='POST' id='formu3'>  <input name='tes3' type='checkbox' class='entrad";
          s += "aonoff' id='onoff3' onclick= enviar3() "+guardaLuzLdr+">  <label class='texto-onoff' for='onoff3'> <span ";
          s += "class='spanonoff'></span>  <span class='bola-onoff'></span> </label> </form> </div><br/> <h1 class='";
          s += "paginaPrincipal'>Microfone</h1> <div class='onoffswitch'> <form action='/salvo4' method='POST' id='formu4'>  <";
          s += "input name='tes4' type='checkbox' class='entradaonoff' id='onoff4' onclick= enviar4() "+guardaMicrofone+"> <";
          s += "label class='texto-onoff' for='onoff4'> <span class='spanonoff'></span> <span class='bola-onoff'></s";
          s += "pan> </label> </form> </div><br> <h1 class='paginaPrincipal'>Luz Microfone</h1> <div class='onoffswitch'> <for";
          s += "m action='/salvo5' method='POST' id='formu5'>  <input name='tes5' type='checkbox' class='entradaonof";
          s += "f' id='onoff5' onclick= enviar5() "+guardaLuzMicrofone+"> <label class='texto-onoff' for='onoff5'> <span class=";
          s += "'spanonoff'></span> <span class='bola-onoff'></span> </label> </form> </div><br> <script type='text/";
          s += "javascript'> var myVar; function enviar1() {document.getElementById('formu1').submit();} function enviar2() {do";
          s += "cument.getElementById('formu2').submit();} function enviar3() {document.getElementById('formu3').sub";
          s += "mit();}  function enviar4() {document.getElementById('formu4').submit();} function enviar5() {docume";
          s += "nt.getElementById('formu5').submit();} function aoCarregar(){myVar = setInterval(verifica, 3000);} function verifica(){if("+atualizaPagina+"=='N'){location.reload();}}  </script> </body> </html> ";

  server.send(200,"text/html",s);
  }
void ativaBuzzer(){
    for (int i = 1; i <= 3; i++)
    {
      digitalWrite(pino_buzzer, HIGH);
      delay(100);
      digitalWrite(pino_buzzer, LOW);
      delay(100);
    }
    // Envia mensagem para o Serial Monitor
    Serial.println("Movimento detectado!");
    delay(100); 
          
}
void ultrassom(){
   if (millis() - print_timer > 500) {
          print_timer = millis();
           
          // Pulso de 5V por pelo menos 10us para iniciar medição.
          digitalWrite(trig_pin, HIGH);
          delayMicroseconds(11);
          digitalWrite(trig_pin, LOW);
           
          /* Mede quanto tempo o pino de echo ficou no estado alto, ou seja,
          o tempo de propagação da onda. */
          uint32_t pulse_time = pulseIn(echo_pin, HIGH);
           
          /* A distância entre o sensor ultrassom e o objeto será proporcional a velocidade
          do som no meio e a metade do tempo de propagação. Para o ar na
          temperatura ambiente Vsom = 0,0343 cm/us. */
          double distance = 0.01715 * pulse_time;
           
          // Imprimimos o valor na porta serial;
          Serial.print(distance);
          Serial.println(" cm");

          if( distance<19){//função que ativa o alarme quando algum movimento é detectado
                ativaBuzzer();            
          }          
    }
}

void resistorLdr(){

 ldrValor = analogRead(ldrPin); //O valor lido será entre 0 e 1023
 
 if (ldrValor>= 600){ 
    digitalWrite(ledPin,HIGH);
    }

 else {digitalWrite(ledPin,LOW);}
        
}
ICACHE_RAM_ATTR void interrupcaoMicrofone(){
  if (millis() - print_timer_microfone > 1000) {    
     print_timer_microfone = millis();           
     rele = !rele; //Operacao NAO: Se estiver LOW, passa pra HIGH. Se estiver HIGH passa para LOW
     Serial.println("RECEBEU SOM");
     paginaPrincipal();
    }   
}
void LigaSeguranca(){
  if(server.arg("tes1")=="on"){
          guardaUltrassom="checked";  
                
  }else{
          guardaUltrassom="";
          }
  paginaPrincipal();
}
void LigaLdr(){
    //  aux=server.arg("tes2");
    //Serial.println("CLICLADO---------");
    //  if (aux.indexOf("on")>=0){
    if (server.arg("tes2")=="on"){
      guardaLdr="checked";
    }
    else{
       guardaLdr="";
     }
  paginaPrincipal();
}
void LigaLuzLdr(){
  if (server.arg("tes3")=="on"){
      guardaLuzLdr="checked";
      digitalWrite(ledPin,HIGH);
      }
  else{
      guardaLuzLdr="";
      digitalWrite(ledPin,LOW);
      }
  paginaPrincipal();
}
void LigaMicrofone(){
  if (server.arg("tes4")=="on"){
          guardaMicrofone="checked";
  }
  else{
          guardaMicrofone="";
  }
  paginaPrincipal();

}
void LigaLuzDoMicrofone(){
  if (server.arg("tes5")=="on"){
          rele=HIGH;
          guardaLuzMicrofone="checked";
          digitalWrite(pino_led,rele);
          }else{
           rele=LOW;
           guardaLuzMicrofone="";
           digitalWrite(pino_led,rele);
           }
  paginaPrincipal();

}
