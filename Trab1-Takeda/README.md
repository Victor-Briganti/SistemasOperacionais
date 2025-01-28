# Projeto 1 -> Resolvendo problemas clássicos de concorrência e sincronização usando Semáforos e Monitores

## Descrição

Este projeto simula uma sessão de avaliação dos novos candidatos (Padawans) a Mestre Jedi no salão da Ordem Jedi. A avaliação é controlada por Mestre Yoda, que organiza os testes, controla a entrada de público e garante a ordem dos eventos no salão. O código foi implementado utilizando semáforos para controlar a sincronização e o acesso ao salão.

### Requisitos

- **Número controlado de público**: O número de espectadores (público) e Padawans no salão é limitado.
- **Entrada dos Padawans**: Padawans devem entrar no salão, cumprimentar os Mestres Avaliadores e aguardar o início dos testes.
- **Yoda inicia os testes**: Yoda controla quando os testes começam. Após o início, não é mais permitida a entrada de novos Padawans no salão.
- **Espectadores**: O público pode entrar e sair do salão livremente, sem restrições.
- **Testes e resultados**: Cada Padawan será avaliado por Yoda na sequência de entrada. Se aprovado, terá sua trança cortada por um Sabre de Luz. Se reprovado, sairá do salão sem cortes. Yoda anuncia os resultados após os testes.
- **Finalização**: Ao final das avaliações, Yoda realiza um discurso e encerra oficialmente os testes.

## Estrutura

O código é implementado utilizando **semaforos** para controlar a entrada de Padawans, o controle da entrada e saída dos espectadores, e a sincronização das ações de avaliação. A estrutura geral é:

1. **Padawan**:
    - Cada Padawan entra no salão e cumprimenta os Mestres Avaliadores.
    - Espera o sinal de Yoda para iniciar os testes.
    - Realiza o teste.
    - Aguarda o resultado.
    - Se aprovado aguarda o cortar da trança.
    - Se reprovado aguarda cumprimento do Yoda.
    - Após o resultado, o Padawan sai do salão.

2. **Espectador**:
    - O espectador entra no salão e permanece lá por um tempo aleatório.

3. **Yoda**:
    - Yoda libera entrada por um tempo aleatorio.
    - Yoda inicia os testes.
    - Yoda mostra resultado, corta as tranças ou cumprimenta o padawan
    - Após as avaliações, Yoda inicia a próxima sessão caso tenha padawans a serem avaliados.
    - Após realizar todos os testes, faz um discurso e encerra.

## Como Funciona

1. **Garantir a ordem de avaliação de acordo com a entrada**

Para garantir a ordem de avaliação conforme os padawans entram, utiliza-se um vetor de semáforos. Cada padawan ocupa uma posição específica no vetor e, ao concluir sua tarefa, incrementa o semáforo para o próximo padawan.

Yoda, por sua vez, processa os resultados na sequência, de 0 até o número de padawans que entraram, armazenando os resultados em um vetor de resultados.

Após Yoda anunciar os resultados, cada padawan consulta o vetor de resultados usando sua posição atribuída para verificar seu respectivo resultado.

2. **Controle de entrada por Yoda**

O controle de entrada é realizado por meio de uma variável que indica o estado do portão (aberto ou fechado). Além disso, a prioridade da thread de Yoda é ajustada, garantindo que, ao término do tempo de entrada, Yoda seja imediatamente executado para fechar o portão o mais rápido possível.

## Compilação

Para compilar o código:

```bash
make
```

### Execução

Após compilar, execute o programa gerado:

```bash
./main
```

Caso tenha problema, o código utiliza da manipulação de prioridade de thread, tente executar como sudo:

```bash
sudo ./main
```
## Considerações Finais

Este projeto foi desenvolvido com foco na simulação de um processo de avaliação controlado, utilizando a sincronização de threads e semáforos. Ele serve como um exemplo de como controlar concorrência em sistemas que envolvem múltiplas entidades com comportamentos distintos, como no caso de Padawans, Yoda e espectadores.
