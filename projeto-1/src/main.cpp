//============================================================================>>
//
// Autor: Luiz Gustavo Takeda
// Data: 23/12/2024
//
//============================================================================>>
//
// Programa desenvolvido para projeto 1, da matéria Sistemas Operacionais.
//
//============================================================================>>

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define YODA_TEMPO_ESPERA_ENTRADA (rand() % 200) + 150 // Tempo que o yoda espera a entrada aberta
#define PADAWANS_N 100                                 // Quantidade de padawans para realizar o teste
#define ESPECTADORES_N 300                             // Quantidade de espectadores para assistir
#define ESPECTADORES_TEMPO_PARA_SAIR 8000              // Tempo para o espectador sair do salão
#define SALAO_MAX_PADAWNS 35                           // Define o maximo de padawans no salão
#define COR_ESPECTADOR "\033[0m"                       // Cor utilizada para destacar no cout
#define COR_PADAWAN "\033[34m"                         // Cor utilizada para destacar no cout
#define COR_YODA "\033[32m"                            // Cor utilizada para destacar no cout
#define COR_DEFAULT "\033[33m"                         // Cor utilizada para destacar no cout
#define COR_APROVADO "\033[32m"                        // Cor utilizada para destacar no cout
#define COR_REPROVADO "\033[31m"                       // Cor utilizada para destacar no cout

sem_t sem_entrada;                              // Controla acesso a entrada
sem_t sem_entrada_aberta;                       // Utilizada para acordar todos os processos esperando a entrada ser aberta
sem_t sem_aguarda_avalicao[SALAO_MAX_PADAWNS];  // Utilizado para organizar a avaliação de cada padawan
sem_t sem_aguarda_resultado[SALAO_MAX_PADAWNS]; // Utilizado para organizar o resultado de cada padawan
sem_t sem_aguarda_realizar_avaliacao;           // Utilizado para aguardar o padawan realizar a avaliação
sem_t sem_aguarda_corte;                        // Utilizado para padawans aprovados esperarem pelo corte da trança
sem_t sem_aguarda_cumprimento;                  // Utilizado para padawans reprovados esperarem pelo cumprimento
sem_t sem_aguarda_padawans_sairem;              // Utilizado para yoda esperar até todos os padawans terem saído do salão
sem_t sem_modificar_testes_finalizados;         // Utilizado para garantir acesso a apenas um processo ao testes_finalizados

unsigned int padawans_no_salao = 0;         // Conta a quantidade de padawans dentro do salão
unsigned int padawans_avaliados = 0;        // Conta a quantidade de padawans que já foram avaliados
unsigned int espectadores_no_salao = 0;     // Conta a quantidade de espectadores dentro do salão
unsigned int espectadores_que_entraram = 0; // Conta a quantidade de espectadores que já passaram pelo salão

bool resultados_padawans[SALAO_MAX_PADAWNS]; // Utilizado para armazenar o resultado de cada padawan
bool entrada_aberta = false;                 // Indica se a entrada está aberta
bool testes_finalizados = false;             // Indica se o teste finalizou

void *threadYoda(void *arg);
void *threadPadawan(void *arg);
void *threadEspectador(void *arg);

//============================================================================>>
//
// Função principal, realiza os seguintes passos:
//
// 1. Inicializa srand, e semaforos.
// 2. Realiza a cofiguração das threads, onde define a prioridade para execução da thread yoda.
// 3. Cria as threads.
// 4. Aguarda todas as threads finalizarem e desaloca recursos
//
//============================================================================>>
int main(void)
{
  std::cout << COR_DEFAULT << "Algoritimo iniciado !" << std::endl;

  srand((unsigned int)time(NULL)); // Inicializa gerador de numeros aleatorios

  // ### Inicializando semaforos ###
  sem_init(&sem_entrada, 0, 1);
  sem_init(&sem_entrada_aberta, 0, 0);
  sem_init(&sem_aguarda_realizar_avaliacao, 0, 0);
  sem_init(&sem_aguarda_corte, 0, 0);
  sem_init(&sem_aguarda_cumprimento, 0, 0);
  sem_init(&sem_aguarda_padawans_sairem, 0, 0);
  sem_init(&sem_modificar_testes_finalizados, 0, 1);

  for (int i = 0; i < SALAO_MAX_PADAWNS; i++)
  {
    sem_init(&sem_aguarda_avalicao[i], 0, 0);
    sem_init(&sem_aguarda_resultado[i], 0, 0);
  }
  // ### END ###

  // ### Configurando threads ###
  pthread_attr_t attr;
  struct sched_param param;

  pthread_t thread_yoda;
  pthread_t threads_padawan[PADAWANS_N];
  pthread_t threads_public[ESPECTADORES_N];

  // Inicializa atributos da thread
  pthread_attr_init(&attr);

  // Define a política de escalonamento como SCHED_RR
  pthread_attr_setschedpolicy(&attr, SCHED_RR);

  // Cria threads dos padawans e espectadores com prioridade media
  param.sched_priority = 15;
  pthread_attr_setschedparam(&attr, &param);
  for (int i = 0; i < PADAWANS_N; i++)
  {
    auto id = new int(i + 1);
    auto status = pthread_create(&threads_padawan[i], &attr, threadPadawan, id);

    if (status < 0)
      std::cout << COR_DEFAULT << "Falha ao criar thread Padawan " << id << std::endl;
  }

  for (int i = 0; i < ESPECTADORES_N; i++)
  {
    auto status = pthread_create(&threads_public[i], &attr, threadEspectador, NULL);

    if (status < 0)
      std::cout << COR_DEFAULT << "Falha ao criar thread Espectador " << std::endl;
  }

  // Cria a thread do yoda com prioridade alta
  param.sched_priority = 20;
  pthread_attr_setschedparam(&attr, &param);

  auto status = pthread_create(&thread_yoda, &attr, threadYoda, NULL);

  if (status < 0)
    std::cout << COR_DEFAULT << "Falha ao criar thread yoda " << std::endl;

  // Aguarda as threads finalizarem
  pthread_join(thread_yoda, NULL);

  for (int i = 0; i < PADAWANS_N; i++)
    pthread_join(threads_padawan[i], NULL);

  for (int i = 0; i < ESPECTADORES_N; i++)
    pthread_join(threads_public[i], NULL);

  // Destroi os atributos
  pthread_attr_destroy(&attr);

  std::cout << COR_DEFAULT << "Algoritimo finalizado !" << std::endl;

  return 0;
}

//============================================================================>>
//
// Função para thread responsavel pela logica do yoda, seu funcionamento:
//
// 1. Libera e interrompe a entrada de padawans e espectadores no salão.
// 2. Avalia na ordem os padawans gerando se foram aprovados ou reprovados.
// 3. Informa o resultado da avaliação.
// 4. Corta as tranças dos aprovados e cumprimenta os reprovados.
// 5. Aguarda os padawans se retirarem.
// 6. Realiza os passos 1-5 até que não sobre nenhum padawan.
// 7. Libera espectadores e finaliza.
//
//============================================================================>>
void *threadYoda(void *arg)
{
  // Loop que executa as seções de teste
  while (1)
  {
    // ### Liberando entrada ###
    sem_wait(&sem_entrada);
    std::cout << COR_YODA << "Yoda libera entrada" << std::endl;
    entrada_aberta = true;
    sem_post(&sem_entrada);

    // Acorda todos os processos para disputarem pela entrada
    for (unsigned int i = 0; i < PADAWANS_N + ESPECTADORES_N; i++)
      sem_post(&sem_entrada_aberta);

    // Aguarda tempo
    usleep(YODA_TEMPO_ESPERA_ENTRADA);

    // Fecha a entrada
    sem_wait(&sem_entrada);
    std::cout << COR_YODA << "Yoda interrompe entrada" << std::endl;
    entrada_aberta = false;
    sem_post(&sem_entrada);
    // ### END ###

    std::cout << COR_YODA << "Yoda deixou entrar " << padawans_no_salao << " padawans e " << espectadores_no_salao << " espectadores" << std::endl;

    // ### Inicia os testes ###
    std::cout << COR_YODA << "Yoda inicia as avaliacoes" << std::endl;

    // Liberando os padawns para realizar a avaliação
    for (unsigned int i = 0; i < padawans_no_salao; i++)
    {
      std::cout << COR_YODA << "Yoda libera padawan para realizar teste" << std::endl;
      sem_post(&sem_aguarda_avalicao[i]);

      sem_wait(&sem_aguarda_realizar_avaliacao); // Aguarda o padawan finalizar a avaliacao

      resultados_padawans[i] = rand() % 101 >= 50; // Gerando resultado para padawan
    }

    std::cout << COR_YODA << "Yoda finaliza as avaliacoes" << std::endl;
    // ### END ###

    // ### Anuncia resultado & corta tranca ou cumprimenta ###
    unsigned int aprovados = 0;

    // Anuncia o resultado de todos os padawans
    for (unsigned int i = 0; i < padawans_no_salao; i++)
    {
      std::cout << COR_YODA << "Yoda anuncia resultado" << std::endl;
      sem_post(&sem_aguarda_resultado[i]);

      if (resultados_padawans[i])
        aprovados++;
    }

    // Corta a trança dos padawans que foram aprovados
    for (unsigned int i = 0; i < aprovados; i++)
      sem_post(&sem_aguarda_corte);

    // Cumprimenta padawans reprovados
    for (unsigned int i = 0; i < padawans_no_salao - aprovados; i++)
      sem_post(&sem_aguarda_cumprimento);

    // Aguarda os padawans sairem do salão para poder começar outra rodada de avaliações
    for (unsigned int i = 0; i < padawans_no_salao; i++)
      sem_wait(&sem_aguarda_padawans_sairem);

    // Limpando resultado
    padawans_avaliados += padawans_no_salao;
    padawans_no_salao = 0;

    espectadores_que_entraram += espectadores_no_salao;
    espectadores_no_salao = 0;
    // ### END ###

    // ### END ### os testes ###
    if (padawans_avaliados >= PADAWANS_N)
    {
      std::cout << COR_YODA << "Yoda faz um discurso" << std::endl;
      std::cout << COR_YODA << "Yoda finaliza os testes" << std::endl;
      break;
    }
    // ### END ###
  }

  sem_wait(&sem_modificar_testes_finalizados);
  testes_finalizados = true;
  sem_post(&sem_modificar_testes_finalizados);

  // Libera os espectadores restantes
  for (unsigned int i = 0; i < ESPECTADORES_N - espectadores_que_entraram; i++)
    sem_post(&sem_entrada_aberta);

  pthread_exit(NULL);
}

//============================================================================>>
//
// Função para thread responsavel pela logica do padawan, seu funcionamento:
//
// 1. Aguarda entrada ser liberada.
// 2. Caso entrada esteja liberada entra na fila para avaliaçõa.
// 3. Realiza avaliação e aguarda o resultado.
// 4. Caso aprovado aguarda corte da trança, se reprovado aguarda cumprimento do yoda.
// 5. Vai embora do salão.
//
//============================================================================>>
void *threadPadawan(void *arg)
{
  int id = *(int *)arg;

  // ### Entra no salão & Cumprimenta mestres avaliadores ###
  while (1)
  {
    sem_wait(&sem_entrada_aberta); // Aguarda a entrada ser aberta

    sem_wait(&sem_entrada);
    if (entrada_aberta && padawans_no_salao < SALAO_MAX_PADAWNS)
      break;
    sem_post(&sem_entrada);
  }

  std::cout << COR_PADAWAN << " Padawan " << id << " entrou no salao" << std::endl;
  std::cout << COR_PADAWAN << " Padawan " << id << " cumprimentou os mestres" << std::endl;
  std::cout << COR_PADAWAN << " Padawan " << id << " aguarda avaliacao" << std::endl;

  auto posicao = padawans_no_salao;
  padawans_no_salao++;
  sem_post(&sem_entrada);

  sem_wait(&sem_aguarda_avalicao[posicao]); // Aguarda a vez na avaliação
  // ### END ###

  // ### Realiza avaliacao ###
  std::cout << COR_PADAWAN << " Padawan " << id << " realiza avaliacao" << std::endl;
  sem_post(&sem_aguarda_realizar_avaliacao);
  // ### END ###

  // ### Aguarda resultado & espera corte ou cumprimento ###
  sem_wait(&sem_aguarda_resultado[posicao]);

  auto resultado = resultados_padawans[posicao];
  std::cout << COR_PADAWAN << " Padawan " << id << " foi " << (resultado ? COR_APROVADO : COR_REPROVADO) << (resultado ? "Aprovado" : "Reprovado") << std::endl;

  if (resultado)
  {
    sem_wait(&sem_aguarda_corte);
    std::cout << COR_PADAWAN << " Padawan " << id << " teve sua tranca cortada" << std::endl;
  }
  else
  {
    sem_wait(&sem_aguarda_cumprimento);
    std::cout << COR_PADAWAN << " Padawan " << id << " cumprimentou Yoda" << std::endl;
  }
  // ### END ###

  // ### Sai do salao ###
  std::cout << COR_PADAWAN << " Padawan " << id << " sai do salao" << std::endl;
  sem_post(&sem_aguarda_padawans_sairem);
  // ### END ###

  delete (int *)arg;
  pthread_exit(NULL);
}

//============================================================================>>
//
// Função para thread responsavel pela logica do espectador, seu funcionamento:
//
// 1. Aguarda a entrada ser aberta.
// 2. Caso testes finalizados, vai embora.
// 4. Ao assistir espera um tempo e tem a chance de ir embora, em loop.
// 5. Vai embora do salão.
//
//============================================================================>>
void *threadEspectador(void *arg)
{
  // ### entra no salão ###
  while (1)
  {
    sem_wait(&sem_entrada_aberta); // Aguarda a entrada ser aberta

    // Verifica se os testes foram finalizados para poder ir embora
    sem_wait(&sem_modificar_testes_finalizados);
    if (testes_finalizados)
    {
      sem_post(&sem_modificar_testes_finalizados);
      pthread_exit(NULL);
      return NULL;
    }
    sem_post(&sem_modificar_testes_finalizados);

    sem_wait(&sem_entrada);
    if (entrada_aberta)
      break;
    sem_post(&sem_entrada);
  }

  std::cout << COR_ESPECTADOR << " Espectador entrou no salão" << std::endl;
  espectadores_no_salao++;
  sem_post(&sem_entrada);
  // ### END ###

  // ### Assiste testes ###
  std::cout << COR_ESPECTADOR << " Espectador esta assistindo o teste" << std::endl;
  // ### END ###

  // ### Sai do salão ###
  while (1)
  {
    usleep(ESPECTADORES_TEMPO_PARA_SAIR);

    int chance_embora = rand() % 101;
    if (chance_embora >= 80)
      break;
  }

  std::cout << COR_ESPECTADOR << " Espectador vai embora" << std::endl;
  // ### END ###

  pthread_exit(NULL);
}
