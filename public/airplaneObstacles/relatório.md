## Processo de desenvolvimento
O projeto Na movimentação circular de um avião em volta da camara onde a camera segue esse movimento.

### openglwindow.cpp

Classe central que chama a classe de camera e a classe que renderiza o chão e avião. Nela são controlados os parametro que cada classe "menor" precisa como por exemplo a chamada do metodo da camera seguir a coordenada do avião.

### camera.cpp
Classe responsável pelo controle da camera foi adicionada uma função chamada follow para que a camera siga uma cordenada no caso do projeto a cordenada do avião.

### Airplane.cpp
Classe responsável pela renderização e movimento do avião. são responsável por fazer a rotação do avião de forma que a mesma lateral dele sem fique apontada pra camera no movimento circular que ele faz


### Modelo
O modelo do avião está disponivel na internet de graça no [link](https://free3d.com/pt/3d-model/airplane-v2--549103.html).
