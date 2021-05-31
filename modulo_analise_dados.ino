#include <ESP8266WiFi.h>

// CONSTANTES
const int CHUVEIRO_LIGADO = 16;
const int CHUVEIRO_DESILGADO = 05;
const int BOTAO = 12;

// Nome do seu wifi
const char* ssid = "******"; 

// Senha do seu wifi
const char* password = "******"; 

// Host
IPAddress server(192,168,100,4);

// Inicializa cliente
WiFiClient client;

// VARIAVEIS GLOBAIS
uint32_t tempoFinal = 0;
uint32_t tempoAtual = 0;
uint32_t tempoInicial = 0;
String codigo = "78945"; // código chuveiro

int flagControle = 0;

// INICIO PROGRAMAÇÃO
void setup() {
  // inicializa as portas da indicação se o chuveiro está ligado
  pinMode(CHUVEIRO_LIGADO, OUTPUT);
  pinMode(CHUVEIRO_DESILGADO, OUTPUT);

  // Para o node mcu esp8266 
  Serial.begin(9600);
  
  // inicializa porta do simulador do sensor do chuveiro
  pinMode(BOTAO, INPUT);

  // configuração conexão
  configWifi();
}

void loop() {
  
  byte valor = digitalRead(BOTAO);

  analisaTempoChuveiro(valor);

}

void analisaTempoChuveiro(byte valor) {
    if(valor == HIGH) {
      // simulação sensor chuveiro
      digitalWrite(CHUVEIRO_LIGADO, HIGH);
      digitalWrite(CHUVEIRO_DESILGADO, LOW);  
      
      tempoAtual = millis();
      if(flagControle == 0) {
        flagControle = 1;
        tempoInicial = tempoAtual;
        Serial.println("ligou chuveiro");
      }
      
  } else {
      // simulação sensor chuveiro
      digitalWrite(CHUVEIRO_DESILGADO, HIGH);
      digitalWrite(CHUVEIRO_LIGADO, LOW);

      if (flagControle == 1) {
        // reseta flag de controle
        flagControle = 0;
        
        // tempo utilizado do chuveiro
        tempoFinal = tempoAtual - tempoInicial;

        // reset variável para próximma utilização do chuveiro
        tempoAtual = 0;
        tempoInicial = 0;

        // envio do tempo para o servidor
        sendServidor(tempoFinal);
        Serial.print("tempo total no chuveiro: ");
        Serial.println(tempoFinal);
        Serial.println("chuveiro desligado");
      }

      delay(10);
  }
}

void sendServidor(uint32_t tempo){
  if (client.connect(server, 8080)) {
      Serial.println("connected");
      // cosntroi um HTTP request:
      String clausula = "GET /chuveiro/";
      String parametros = String(tempo) + " HTTP/1.0";
      String result = clausula + parametros;
      client.println(result);
      client.println();
      Serial.println(result);
      Serial.println("Info enviado ao servidor");
    }
}

void configWifi() {
  delay(10);
  
  // Mostra no monitor serial informacoes de conexao da rede
  Serial.println();
  Serial.println();
  Serial.print("conectando em ");
  Serial.println(ssid);
  
  // Inicializando a conexao
  WiFi.begin(ssid, password); 
  
  /* Enquanto não conseguir conectar
    imprime um ponto na tela (dá a ideia de que esta carregando :)) */
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print("."); 
  }

  Serial.println("");
  Serial.println("WiFi connectado");

  // Mostra o IP do servidor
  Serial.println(WiFi.localIP());

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}
