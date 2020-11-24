#include <bits/stdc++.h>

using namespace std;
/*
Version Timestamp
Simulador de memoria virtual 
Sistemas Operativos
Miembros:
Francisco Salgado Guizar A01365047
Gabriel Gomez Novau A01720009
*/


//Structura que tendra los datos de cada proceso
struct proceso{
    //Constructor que inicializa los valores del objeto
    proceso(float idt, float timestampt){
        float id = idt;
        float timestamp_p = timestampt;
    }
    int paginas;
    int pagefaultsLRU;
    int pagefaultsFIFO;
    int turnaround;
    int timestamp_a;
};

float tiempo = 0;//variable para ir manejando el tiempo que llevan los procesos

vector<proceso> agregados;//vector que guardara los procesos agregados a memoria para luego revisarlos 
vector<proceso> liberados; //vector que guarda a los procesos que se vayan liberando de memoria para luego usar como estadisticas de turnaround y pagefaults

//Funcion que hace el swapping con politica de reemplazo FIFO
void swap_FIFO(int pagina, float id, float (&M)[128][3], float (&S)[256][3], int politica){
    while(pagina>0){//mientras que no se acabe de cargar el proceso en swap a la memoria real no se acaba
        proceso temp = procesos.front(); //se crea un objeto con las caracatersiticas del queue de fifo
        if(pagina - temp.paginas<0){//si se ocupa quitar nadamas un pedazo del proceso en memoria nadamas se le quitarian unas paginas y se quedaria el proceso en la queue
            for(int i = 0; i<128; i++){//va a checar por toda la memoria cada vez que encuentre el proceso fisrt in para swapiarlo por el nuevo entrante
                if(M[i][0] == temp.id){
                    M[i][0] = id;
                    for (int j = 0; j < 256; j++) {
                        if (S[j][0] == -1) {
                            S[j][0] = temp.id;
                            break;
                        }
                    }
                    pagina--; //se resta cada vez que se puede meter una pagina del nuevo proceso a memoria real
                }
                if (pagina == 0) {//si ya se acabo de meter todo a memoria se sale del loop
                    break;
                }
            }
        }
        else{//si el proceso nuevo ocupa quitar todo el proceso first in
            for(int i = 0; i<128; i++){
                if(M[i][0] == temp.id){
                    M[i][0] = id;
                    pagina--;
                    for (int j = 0; j < 256; j++) {
                        if (S[j][0] == -1) {
                            S[j][0] = temp.id;
                            break;
                        }
                    }
                }
            }
            //Checar si el temp ID ya esta en el vector
            int tam = swapping.size();// checar tamano del vector que tiene todos los procesos previamente usados
            bool found = false;// bandera que ayuda a saber si ya estaba dentro del vector
            for (int i = 0; i < tam; i++) {
                if (swapping[i].id == temp.id) {//aqui sacariasmos 
                    found = true;
                    swapping[i] = swapping[tam - 1];//cambias el objeto por el de atras para despues borrar el de atras
                    swapping.pop_back();
                }
            }
            if (!found) {// si no estaba el proceso se mete a al vector para futuras referencias
                swapping.push_back(temp);
            }
            procesos.pop();//como se ocupan todas la paginas del proceso first in se borra del la queue
        }
        
    }
}
//Funcion que hace el swapping con politica de reemplazo LRU
void swap_LRU(int pagina, float id, int (&M)[128], int (&S)[256],queue<proceso> &procesos, vector<proceso> &swapping){

}

void cargar(float &bytes, float &id, float (&M)[128][3], float (&S)[256][3],int politica){
    proceso Nuevo(id,tiempo); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
    int pagina = ceil(bytes/16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    bool free;//Variable auxiliar para revisar si existe espacio en memoria
    for(int i = 0; i < 128;i++){ //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        free = true;
        if(M[i][0] == -1 && i + pagina <= 128){ //Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            for(int j = i; j< i + pagina;j++){//En caso de haber encontrado un espacio, revisa que las paginas siguientes esten vacias tambien
                if(M[j][0] >= 0){
                    free = false;
                    break;
                }
                    
            }
            if(free){//Si se ha encontra el espacio libre suficiente para las paginas necesarias se carga el proceso a memoria
                for(int j = i;j<pagina;j++){
                    M[j][0] = id;
                    M[j][1] = j;
                    M[j][2] = tiempo;
                    tiempo++;
                }
                Nuevo.paginas = pagina; //Se guarda la cantidad de paginas que usa
                agregados.push_back(Nuevo);
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
    float M[128][3]; //Arreglo que simula la memoria real para politica FIFO
    float M2[128][3]; //Arreglo que simula la memoria real para politica LRU
    float S[256][3]; //Arreglo auxiliar simulara el area de una memoria real para swapping en FIFO
    float S2[256][3]; //Arreglo auxiliar simulara el area de una memoria real para swapping en LRU

    /*queue<proceso> FIFO; // Queue que guarda los procesos para la politica FIFO
    queue<proceso> LRU; // Queue que guarda los procesos para la politica LRU
    vector<proceso> SFIFO; //Dichos vectores guardaran a los procesos que vayan siendo mandados a swapping
    vector<proceso> SLRU; */

    //Funcion que inicializa la memoria en 0, pues aun no ha llegado ningun proceso
    for(int i =0;i<128;i++){
        M[i][0] = -1;//id
        M[i][1] = -1;//direccion en memoria
        M[i][2] = -1;//timestamp
        M2[i][0] = -1;//id
        M2[i][1] = -1;//direccion en memoria
        M2[i][2] = -1;//timestamp
    }
    //Funcion que incializa el area de disco reservada para swapping en 0
    for(int i = 0; i < 256;i++){
        S[i][0] = -1;//id
        S[i][1] = -1;//direccion en memoria
        S[i][2] = -1;//timestamp
        S2[i][0] = -1;//id
        S2[i][1] = -1;//direccion en memoria
        S2[i][2] = -1;//timestamp
    }

    char process;
    float bytes = 0, id = 0;

    cin >> process;
    cin >> bytes;
    cin >> id;

    switch (process)
    {
    case 'P':
        cargar(bytes,id, M, S, 1);
        cargar(bytes,id, M2, S2, 2);
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