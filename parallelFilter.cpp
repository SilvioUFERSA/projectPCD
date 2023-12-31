#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <omp.h>


struct Pixel {
    unsigned char red, green, blue;
};

Pixel calcularMedia(const std::vector<Pixel>& pixels, int width, int height, int x, int y, int windowSize);

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Uso: " << argv[0] << " <nome_do_arquivo_imagem> <tamanho_da_janela> <nome_do_arquivo_saida> <quantidade_de_threads>" << std::endl;
        return 1;
    }

    const std::string fileName = argv[1];
    const int windowSize = std::atoi(argv[2]);
    const std::string outputFileName = argv[3];
    const int numThreads = std::atoi(argv[4]); // Quantidade de threads

    std::ifstream imageFile(fileName, std::ios::binary);
    if (!imageFile) {
        std::cerr << "Erro ao abrir a imagem." << std::endl;
        return 1;
    }

    std::string format;
    int width, height, maxColor;
    imageFile >> format >> width >> height >> maxColor;

    if (format != "P6" || maxColor != 255) {
        std::cerr << "Formato de imagem não suportado ou profundidade de cor não suportada." << std::endl;
        return 1;
    }

    std::vector<Pixel> pixels(width * height);
    imageFile.read(reinterpret_cast<char*>(pixels.data()), width * height * sizeof(Pixel));

    // Marca tempo inicial
    double start_time = omp_get_wtime(); // Use omp_get_wtime() para medir o tempo inicial

    // Aplicar o filtro de suavização por média simples com OpenMP
    std::vector<Pixel> resultado(width * height);

    #pragma omp parallel num_threads(numThreads) 
    {
        #pragma omp for
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                resultado[y * width + x] = calcularMedia(pixels, width, height, x, y, windowSize);
            }
        }
    }

    // Marca tempo final
    double end_time = omp_get_wtime(); // Use omp_get_wtime() para medir o tempo final

    // Combinar os canais de cores para obter a imagem resultante

    std::ofstream outputImageFile(outputFileName, std::ios::binary);
    if (!outputImageFile) {
        std::cerr << "Erro ao criar a imagem resultante." << std::endl;
        return 1;
    }

    outputImageFile << "P6\n" << width << " " << height << "\n" << maxColor << "\n";

    // Escrever os pixels normalizados no arquivo
    for (const Pixel& p : resultado) {
        outputImageFile.write(reinterpret_cast<const char*>(&p), sizeof(Pixel));
    }

    std::cout << "Filtro aplicado e imagem salva como '" << outputFileName << "'." << std::endl;

    // Imprime tempo de execução
    double duration = end_time - start_time;
    std::cout << "Tempo de execução: " << duration << " segundos." << std::endl;

    return 0;
}

// Função para calcular a média dos pixels vizinhos
Pixel calcularMedia(const std::vector<Pixel>& pixels, int width, int height, int x, int y, int windowSize) {
    int totalPixels = 0;
    int redSum = 0, greenSum = 0, blueSum = 0;

    for (int i = -windowSize / 2; i <= windowSize / 2; i++) {
        for (int j = -windowSize / 2; j <= windowSize / 2; j++) {
            int newX = x + i;
            int newY = y + j;

            if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                totalPixels++;
                redSum += pixels[newY * width + newX].red;
                greenSum += pixels[newY * width + newX].green;
                blueSum += pixels[newY * width + newX].blue;
            }
        }
    }

    Pixel media;
    media.red = static_cast<unsigned char>(redSum / totalPixels);
    media.green = static_cast<unsigned char>(greenSum / totalPixels);
    media.blue = static_cast<unsigned char>(blueSum / totalPixels);

    return media;
}
