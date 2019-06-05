#define tam 8  //Tamanho do barramento do contador
/*
 * Intervalo da interrupção no sistema, em microssegundos
 */
#define intervalo 1000000

/*
 * Biblioteca que contém interrupção por tempo
 */
#include <TimerOne.h>


/*
 * Valor que vem do contador de 8 bits
 * Intervalo de 0 a 255
 */
int oldContagem = 0;
int contagem = 0;

int pinos[] = {
  46, 48, 50, 52, 47, 49, 51, 53
};
int valores[8];

/*
 * Valores das variáveis do robô
 */
double oldPosicao = 0;
double newPosicao = 0;
double velocidade = 0;
/*
 * Configuração inicial do Arduino
 */
void setup() 
{

  for (int i = 46; i < 54; i++) {
      pinMode(pinos[i], INPUT);
  }
  
/*
 * Inicialização do timer
 * O intervalo de interrupção pode ser definido aqui
 * Valor mínimo de 1us, que gera uma frequência máxima de 1MHz
 */
  Timer1.initialize(intervalo);

  /* Vinculação da função de interrupção com o temporizador
   * A função de interrupção é chamada sempre que o temporizador entra em overflow
   * Cuidado na configuração do período do temporizador, pois ele pode travar seu
   *  programa para nunca voltar para a função principal se o período for curto e
   *  a função de interrupção for complexa
   */
  Timer1.attachInterrupt(leEncoder);

  Serial.begin(9600);
}

/*
 * Função de interrupção
 */
void leEncoder()
{

  contagem = binToInt();        //Converte os valores que vêm do contador em um valor inteiro
  getPosicao();                 //Obtem a posição do robô a partir da contagem do contador
  getVelocidade();              //Obtem a velocidade a partir da nova posição do robô

}

int binToInt()
{
  int recebe;
  int somaParcial = 0;

  for (int pos = 0; pos < tam; pos++)
  {
    // Variável que recebe o valor lido no pino
    recebe = digitalRead(pinos[pos]);

    /* Calcula o shift do valor, assim evita de fazer
     * potências e fica mais eficiente, por conta dos
     * valores lidos serem só 0 ou 1
     */
    recebe = recebe << pos;

    // Coloca em uma soma parcial;
    somaParcial += recebe;
  }

  return somaParcial;
}

void getPosicao ()
{
  oldPosicao = newPosicao;
  
  int deltaContagem = 0;                                //Diferença entre a última contagem e a contagem atual
  
  if (contagem < oldContagem)                           //O contador completou um ciclo de contagem
  {
    deltaContagem = ((256 - oldContagem) + contagem);   //Soma do que faltava para completar uma volta com o que passou depois da volta completa
  } else
  {
    deltaContagem = contagem - oldContagem;             //Diferença entre a última contagem e a contagem atual
  }

  oldContagem = contagem;
/*
 * Necessário as especificações do encoder e do roscamento em que o robô irá se deslocar
 * Isso irá definir sua posição vertical através do tempo
 */
  convert(deltaContagem);

 /*
  * Aqui, a posição que o braço do robô vai estar já foi definida
  */

}

void convert(int deltaContagem)
{
  /*
   * Medindo no braço do robô, foi constatado que as 400 contagens do encoder equivalem a uma variação
   * vertical de (4,0  0,5)mm, ou seja, cada contagem equivale a 0,01mm de movimento vertical
   */
  newPosicao = oldPosicao + (double)deltaContagem*0.01;
}

void getVelocidade ()
{
  /*
   * A velocidade do motor é definida pela diferença de posição no tempo entre chamadas de interrupção
   * Uma velocidade negativa indica que o motor está descendo
   * Unidade: mm/s
   */
  velocidade = (newPosicao - oldPosicao)*1000000/intervalo;

}

void loop(){
/*
  atualPos = newPosicao - oldPosicao;

  if (atualPos != 0) {
    
  }
  */
}
