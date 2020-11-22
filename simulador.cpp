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
    float id;
    int turnaround;
    int paginas;
};

//Funcion que hace el swapping con politica de reemplazo FIFO
void swap_FIFO(int pagina, float id, int (&M)[128], int (&S)[256],queue<proceso> &procesos, vector<proceso> &swapping){
    while(pagina>0){
        proceso temp = procesos.front();
        if(pagina - temp.paginas<0){
            for(int i = 0; i<128; i++){
                if(M[i] == temp.id){
                    M[i] = id;
                    pagina--;
                }
                if(pagina == 0)
                    break;
            }
        }
        else{
            for(int i = 0; i<128; i++){
                if(M[i] == temp.id){
                    M[i] = id;
                    pagina--;
                }
            }
        }
        //Checar si el temp ID ya esta en el vector
        swapping.push_back(temp);
        procesos.pop();
    }
}
//Funcion que hace el swapping con politica de reemplazo LRU
void swap_LRU(int pagina, float id, int (&M)[128], int (&S)[256],queue<proceso> &procesos, vector<proceso> &swapping){

}

void cargar(float &bytes, float &id, int (&M)[128], int (&S)[256],queue<proceso> &procesos, int politica, vector<proceso> &swapping){
    proceso Nuevo(id); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
    int pagina = ceil(bytes/16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    bool free;//Variable auxiliar para revisar si existe espacio en memoria
    for(int i = 0; i < 128;i++){ //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        free = true;
        if(M[i] == 0 && i + pagina <= 128){ //Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            for(int j = i; j< i + pagina;j++){//En caso de haber encontrado un espacio, revisa que las paginas siguientes esten vacias tambien
                if(M[j] >= 0){
                    free = false;
                    break;
                }
                    
            }
            if(free){//Si se ha encontra el espacio libre suficiente para las paginas necesarias se carga el proceso a memoria
                for(int j = i;j<pagina;j++){
                    M[j] = id;
                }
                Nuevo.paginas = pagina; //Se guarda la cantidad de paginas que usa
                procesos.push(Nuevo); //Se mete a la fila el proceso creado
                //Output del proceso cargado
                cout << "Asignar " << bytes << " bytes al proceso " << id << "\nSe asignaron los marcos de pagina " << i << "-" <<i+pagina-1 << " al proceso "<< id << endl;
               break;
            }
        }
        if(i == 127){//En caso de que no se pueda meter el proceso, se hace swapping
            //El 1 indica politica de reemplazo FIFO y el 2 LRU 
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

    queue<proceso> FIFO; // Queue que guarda los procesos para la politica FIFO
    queue<proceso> LRU; // Queue que guarda los procesos para la politica LRU
    vector<proceso> SFIFO; //Dichos vectores guardaran a los procesos que vayan siendo mandados a swapping
    vector<proceso> SLRU; 

    //Funcion que inicializa la memoria en 0, pues aun no ha llegado ningun proceso
    for(int i =0;i<128;i++){
        M[i] = -1;
        M2[i] = -1; 
    }
    //Funcion que incializa el area de disco reservada para swapping en 0
    for(int i = 0; i < 256;i++){
        S[i] = -1;
        S2[i] = -1;
    }

    char process;
    float bytes = 0, id = 0;

    cin >> process;
    cin >> bytes;
    cin >> id;

    switch (process)
    {
    case 'P':
        cargar(bytes,id, M, S, FIFO, 1, SFIFO);
        cargar(bytes,id, M2, S2, LRU, 2, SLRU);
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