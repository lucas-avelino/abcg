## Processo de desenvolvimento
O projeto Na movimentação circular de um avião em volta da camara onde a camera segue esse movimento.

### openglwindow.cpp

Classe responsável por perpetuar as chamadas do openGl para as demais glasses de rendering.

**Logica de spawn de obstaculos**
Essa lógica é totalmente controlada pela classe, a lógica consiste em obstaculos a cada 10 unidades de medida espacial na cordenada *z*, quando esses elementos alcançam `z > airplane.position.z + 4` o obstaculo é removido e colocado a `-30` em z da coordenada que ele estava fazendo com que ele seja o novo ultimo obstaculo. Quem dita isso é o código abaixo contido na função paintGL:
```cpp
 if (buildings.front().building1.position.z > airplane.position.z + 4)
    respawnBuildings();
```
_Obs: O movimento "para frente" do avião é sempre na direção -z_

Além da função `respawnBuildings()`, reponsável por colocar os obstaculos na posição `zAtual - 30`, temos a função `resetBuildings()` essa função tem um ojetivo um pouco parecido com a outra porém ela é usada em um caso diferente, essa função é usada quando o espaço é pressionado para mudança de `state` do jogo, ela reseta a posição das 3 duplas de obstaculos para z=0, z=-10, z=-20; fazendo com que os obstaculos tome sua posição inicial.

_Obs2: Analisando o render distance verifiquei que a quantidade ideal de duplas de obstaculos era 3_
_Obs3: Os obstaculos das duplas são colocados em uma das 3 posições em x de forma aleatória (código visivel em  `respawnBuildings()` e `resetBuildings()`)_

**Shaders**
O projeto tem dois programas de Shaders um focado em renderizar objetos de forma solida `solidColorShader.frag` e `solidColorShader.vert` na variável (`GLuint textureProgram`), outro direcionado à renderização de objetos com textura `textureShader.frag` e `textureShader.vert` na variável (`GLuint solidColorProgram`), o primeiro Shader é usado para renderizar o chão `ground.cpp`, já o segundo é utilizado para cada prédio `Building.cpp` e para o Avião `Airplane.cpp`.

### Aiplane.cpp
Classe responsável pelo render e lógica de funcionamento do avião.

**Movimento**
A lógica de movimento do avião é simples, ele sempre se movimenta pra frente (direção `-z`), com uma aceleração `float aceleration{-0.25f};`, velocidade inicial `float velocity{-2.0f};` e posição inicial `float forwardInitialPosition{30.0f};`, descrevendo dessa forma um MRUV (Movimento Retilinio Uniformemente Variado). descrevendo a função de posição em `z = forwardInitialPosition + (velocity * time) + (aceleration*t^2)/2`

Já o movimento lateral (acionado pelos botões `a` e `d` do teclado), é baseado em uma lógica de "trilhos" onde a posição estatica do avião pode ser 3 diferentes valores `(-1, 0, 1)`, com isso quando um movimento é solicitado o valor 1 é somado ou subtraido (de acordo com a direção do movimento solicitado) ao valor da posição atual de forma a derivar `int targetPosition{0};` (que é basicamente para qual trilho o avião esta indo); Ja a propriedade `float actualPosition{0};` representa onde o avião se encontra porém esse valor pode ser decimal já que o botão pode ser pressionado durante um movimento, essa propriedade é atualizada de acordo com um MR simples descrito por `x = initialPosition + (curveVelocity * (actualTime - movementStart))` com cap de -1 e 1(para que o avião 'fuja' dos trilhos), porém cada trilho não pussui o tamanho exato de 1 float então temos um multiplicador `const float positionModifier{1.15f};` para efetuar esse posicionamento de forma correta. Ainda sobre a troca de trilho, ou curva, temos uma velocidade base `const float curveVelocitybase{0.003f};` para esse movimento o que é relativamente lento porém com o avanço do jogo a `float curveVelocity{0.003f};` é incrementada pela formula `curveVelocitybase + (curveVelocitybase * timeElapsed / 10)` esse é um recurso para que a troca de trilho seja mais rapida com o avanço do jogo.

**Ciclo de vida**
O ciclo de vida da classe é bem simples e parecido com o da classe basica do opengl.

`void initializeGL(GLuint program, std::string assetsPath);` é a função responsável pelo inicio de vida da classe airplane essa função chama o load do modelo 3d e textura do avião usando as funções:
```cpp
  void loadModelFromFile(std::string_view path, std::string texturePath);
  void loadDiffuseTexture(std::string_view path);
```
Após isso ela convoca as funções de inicio dos buffers de rendering usando as funções: 
```cpp
  void createBuffers();
  void setupVAO();
```
Ela também ativa os listeners de eventos(botões pressionados e afins) com do globalInput.
```cpp
void Airplane::bindControlls();
```
A proxima função do ciclo de vida é a `void paintGL(GameState gameSate, LightProperties light);`, função essa responsável pelo render frame by frame do avião, além do acionamento da função `void move();` que realiza os movimentos descritos na explicação de "Movimento".

### Modelos
O modelos estão disponiveis na internet de graça nos links:
- [Avião](https://free3d.com/pt/3d-model/airplane-v2--549103.html);
- [Prédio](https://www.turbosquid.com/3d-models/apartment-house-building-3d-model-1650425);