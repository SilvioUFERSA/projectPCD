# projectPCD
Aplicação em c++ usando OpenMPI para aplicação do filtro de média em imagens

### Aplicação em c++ usando OpenMPI para aplicação do filtro de média em imagens.

Para utilizar:
1. converta seu arquivo de city.jpg em .ppm. Coloque o terminal: convert city.jpg city.ppm)
2. g++ -o mainFilter mainfilter.cpp
3. ./mainFilter town.ppm 25 town_out.ppm
4. para ver a imagem suavizada, faça no terminal: convert city_output.ppm city_output.jpg
 
