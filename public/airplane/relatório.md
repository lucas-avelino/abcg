## Processo de desenvolvimento
O projeto consiste basicamente em algumas classes.

### openglwindow.cpp

Classe que basicamente comanda o state do jogo, e os eventos globais como controlar o render dos pontos e mensagens na tela, além de ser a responsável pela propagação do ciclo de vida para as outras classes como a player e a pipes.

além das funções de ciclo de vida do GL essa classe tem duas funções
```cpp
  bool checkTubeColision(TubesColision tubeColision, ColisionCircle colisionCircle);
  void addPoint();
```
- ***checkTubeColision*** função responsável por checar se o o player colidiu com algum pipe;
- ***addPoint*** função que é envidada para a classe pipes como referência para que seja contabilizado os pontos no state global do jogo;

### player.cpp

A classe player é a classe responsável por controlar toda lógica do jogador.
ela possui 3 funcões publicas:
```cpp
  void initializeGL(GLuint program);
  void paintGL(int8_t gameState);
  void terminateGL();
  void reset();
```
Que são as funções relacionadas ao ciclo de vida do Gl, então temos:
- ***initializeGl*** que configura vao, vbo e ebo, (buffers para a renderização dos elementos na tela);
- ***paintGL*** função que roda a cada frame e disparas as funções de calculo de posição do player;
- ***reset*** função responsavel por dar um reset no player para que o jogo seja reiniciado.

Além dessas funções temos as seguintes funções privadas
```cpp
  void updatePosition();
  void spaceKeyDown();
  void spaceKeyUp();

```
Essas funções estão relacionadas ao comportamento geral do player:
- ***updatePosition*** é a função que faz o calculo da posição do player se baseando na posição inicial(posição em que o jogador aperta espaço) em y, o calculo é baseado na formula fisica MRUV com aceleração negativa simulando a gravidade;
- ***spaceKeyDown*** e spaceKeyUp são as funções que são executadas quando o botão espaço é pressionado e solto, basicamente zera o timer do movimento e altera a posição inicial do movimento.

### pipes.cpp
Classe que ficou responsavel por comandar o comportamento dos tubos ela possui 4 funções publicas
```cpp
  void initializeGL(GLuint program);
  void paintGL(int8_t gameState);
  void terminateGL();
  void reset();
  void registryAddPointFunction(std::function<void()> f);

```
- ***initializeGl*** que configura vao, vbo e ebo, (buffers para a renderização dos elementos na tela);
- ***paintGL*** função que roda a cada frame e disparas as funções de calculo de posição dos tubos;
- ***reset*** função responsavel por dar um reset nos pipes para que o jogo seja reiniciado;
- ***registryAddPointFunction*** função responsavel por receber a referência da função de adicionar ponto que fica localizada no openglwindow.


E as seguinte funções privadas:
```cpp
  void updatePosition();
  std::function<void()> addPoint;
```
- ***updatePosition*** função responsável por atualizar a posição dos tubos de acordo com seu tempo de vida; 
- ***addPoint*** referência da função *openglwindow::addPoint* é usada para que os pontos sejam adicionados quando a posição do tubo for menor que 0;

### input.cpp
Foi uma classe de auxilio criada para que fosse possivel criar listeners de eventos(entradas do usuário) de forma mais simples de qualquer lugar da aplicação.
