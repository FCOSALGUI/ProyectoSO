#include <bits/stdc++.h>

using namespace std;
/*
Simulador de memoria virtual 
Sistemas Operativos
Miembros:
Francisco Salgado Guizar A01365047
Gabriel Gomez Novau A01720009
*/


//Structura que tendra los datos de cada proceso
struct proceso{
    //Constructor que inicializa los valores del objeto
    proceso(float idt){
        float id = idt;
        int direccion = -1;
        int bit = 0;
        int turnaround = 0;
    }
    float id = 0;
    int direccion = -1;
    int bit = 0;
    int turnaround = 0;
};

void cargar(float &bytes, float &id, int (&M)[128], int (&S)[256],vector<proceso> &procesos){
    proceso Nuevo(id); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
    procesos.push_back(Nuevo); //Se mete al vector el proceso creado
    int pagina = ceil(bytes/16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    bool free;//Variable auxiliar para revisar si existe espacio en memoria
    for(int i = 0; i < 128;i++){ //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        free = true;
        if(M[i] == 0 && i + pagina < 127){ //Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            for(int j = i; j<= i + pagina;j++){//En caso de haber encontrado un espacio, revisa que las paginas siguientes esten vacias tambien
                if(M[j] == 1){
                    free = false;
                    break;
                }
                    
            }
            if(free){//Si se ha encontra el espacio libre suficiente para las paginas necesarias se carga el proceso a memoria
                for(int j = i;j<pagina;j++){
                    M[j] = 1;
                }
               break;
            }
        }
        if(i == 127){//En caso de que no se pueda meter el proceso, se hace swapping 
            //swap();
        }
    }
}


//Funcion que se encarga de leer el input del usuario y determinar el proceso que quiere llevar a cabo
/*void lectura(char process, int bytes, int id){
    switch (process)
    {
    case 'P':
        break;
    case 'A':
        break;
    case 'L':
        break;
    case 'C':
        break;
    case 'E':
        break;
    default:
        break;
    }
}*/


int main(){
    int M[128]; //Arreglo que simula la memoria real para politica FIFO
    int M2[128]; //Arreglo que simula la memoria real para politica LRU
    int S[256]; //Arreglo auxiliar simulara el area de una memoria real para swapping en FIFO
    int S2[256]; //Arreglo auxiliar simulara el area de una memoria real para swapping en LRU

    vector<proceso> FIFO; //Vector que guarda los procesos para la politica FIFO
    vector<proceso> LRU; // Vector que guarda los procesos para la politica LRU

    //Funcion que inicializa la memoria en 0, pues aun no ha llegado ningun proceso
    for(int i =0;i<128;i++){
        M[i] = 0;
        M2[i] = 0; 
    }
    //Funcion que incializa el area de disco reservada para swapping en 0
    for(int i = 0; i < 256;i++){
        S[i] = 0;
        S2[i] = 0;
    }

    char process;
    float bytes = 0, id = 0;

    cin >> process;
    cin >> bytes;
    cin >> id;

    switch (process)
    {
    case 'P':
        cargar(bytes,id, M, S, FIFO);
        cargar(bytes,id, M2, S2, LRU);
        break;
    case 'A':
        break;
    case 'L':
        break;
    case 'C':
        break;
    case 'F':
        break;
    case 'E':
        break;
    default:
        break;
    } 

    
    return EXIT_SUCCESS;
}