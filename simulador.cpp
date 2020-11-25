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
    proceso(int idt, float timestampt){
        int idp = idt;
        float timestamp = timestampt;
    }
    int paginas;
    int pagefaultsLRU = 0;
    int pagefaultsFIFO = 0;
    int turnaround;
    int idp;
    float timestapt;
};

float tiempo = 0;//variable para ir manejando el tiempo que llevan los procesos FIFO
float tiempo2 = 0;//variable para ir manejando el tiempo que llevan los procesos LRU

vector<proceso> agregados;//vector que guardara los procesos agregados a memoria para luego revisarlos 
vector<proceso> liberados; //vector que guarda a los procesos que se vayan liberando de memoria para luego usar como estadisticas de turnaround y pagefaults

//Funcion que hace el swapping unicamente para procesos recien creados que no quepan en memoria principal
void swap_creados(int pagina, float id, float (&M)[128][4], float (&S)[256][4], int politica, bool creado){
    int cont = 0;// se usara para ir contando las paginas nuevas
    while (pagina > 0) {//mientras que no se acabe de cargar el proceso en swap a la memoria real no se acaba
        float temporal = 9999;// para comparar el timestamp y ver cuales es el first in en memoria real
        int tempoIndice = -1;//para saber que indice es el que tiene el timestamp mas bajo
        for (int i = 0; i < 128; i++) {
            if (M[i][2] < temporal) {//se checa caul es el first in the todas las paginas
                temporal = M[i][2];
                tempoIndice = i;
            }
        }
        for (int i = 0; i < 256; i++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
            if (S[i][0] == -1) {
                S[i][0] = id;//el id del proceso swapeado
                S[i][1] = M[tempoIndice][1]; //el numero de pagina del proceso
                if(politica == 1) {
                    S[i][2] = tiempo;//el timestamp al momento de swapear
                    tiempo++;//se incrementa el tiempo por swapear a out
                }
                else {
                    S[i][2] = tiempo2;//el timestamp al momento de swapear
                    tiempo2++;//se incrementa el tiempo por swapear a out
                }
                
                S[i][3] = M[tempoIndice][3];//se ve cual es la direccion en memoria
            }
        }
        M[tempoIndice][0] = id;//se cambia el id al nuevo proceso
        M[tempoIndice][1] = cont;//se asigna el numero de pagina del proceso
        cont++;
        if(politica == 1) {
            M[tempoIndice][2] = tiempo;//time stap de la pagina
            tiempo++;
            if(creado){
              agregados.back().pagefaultsFIFO++;
            }
        }
        else
        {
            M[tempoIndice][2] = tiempo2;//time stap de la pagina
            tiempo2++;
            if(creado){
                agregados.back().pagefaultsLRU++;
            }
        }
        
        if (!creado) {//se checa si es el proceso es nuevo o ya fue agregado al vector de agregados
            proceso temp(id, tiempo);
            temp.paginas = pagina;
            creado = true;
            agregados.push_back(temp);//se agrega el nuevo proceso creado
        }
        M[tempoIndice][3] = tempoIndice;// se asigna el la direccion de memoria
        pagina--;//se va ir a la siguiente pagina a cambiar
    }
    
}

void cargar(float bytes, float id, float (&M)[128][4], float (&S)[256][4],int politica){
    bool creado = false;
    int pagina = ceil(bytes/16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    int tempPaginas = pagina;
    int cont = 0;
    for(int i = 0; i < 128;i++){ //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        if(M[i][0] == -1){//Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            M[i][0] = id;
            M[i][1] = cont;
            cont++;
            if (politica == 1) {
                M[i][2] = tiempo;
                tiempo++;
            }
            else {
                M[i][2] = tiempo2;
                tiempo2++;
            }
            M[i][3] = i;
            pagina--;
            if(!creado){
                proceso Nuevo(id,tiempo); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
                Nuevo.paginas = tempPaginas;
                agregados.push_back(Nuevo);
                creado = true;
            }
        }
        if(i == 127){//En caso de que no se pueda meter el proceso, se hace swapping
            //El 1 indica politica de reemplazo FIFO y el 2 LRU 
            if(politica == 1){
                swap_creados(pagina, id, M, S,politica, creado);
            }
            else{
                swap_creados(pagina,id,M,S,politica, creado);
            }
            pagina = 0;
        }
        if(pagina == 0){
            //Output del proceso cargado
            cout << "Asignar " << bytes << " bytes al proceso " << id << "\nSe asignaron los marcos de pagina ";
            int contadortemp = 0;
            int temporalindice = 0;
            for(int k = 0; k<127;k++){
                contadortemp = 0;
                if(M[k][0] == id){
                    temporalindice = M[k][0];
                    cout << M[k][0];
                    while(M[k][0] == M[k+1][0]){ 
                        k++;
                        contadortemp++; 
                    }
                    cout << "Temporalindice + contadortemp: " << temporalindice+contadortemp;
                }
            }
            cout << endl;
            break;
        }
    }
}

void FIFO(int direccion, int id, int modificacion, float (&M)[128][4], float (&S)[256][4]){
    int pagina = floor(direccion/16);
    int real = 0;
    for(int i = 0; i < 128;i++){
        if(M[i][0] == id && M[i][1] == pagina){
            real = (direccion%16) + i * 16;
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
        }

        if(i == 127){
            for(int i = 0; i<agregados.size();i++){
                if(agregados[i].idp == id){
                    agregados[i].pagefaultsFIFO++;
                }
            }
            for(int i = 0; i < 256; i++) {
                if(S[i][0] == id){

                }
            }
        }
    }
}

int main(){
    float M[128][4]; //Arreglo que simula la memoria real para politica FIFO
    float M2[128][4]; //Arreglo que simula la memoria real para politica LRU
    float S[256][4]; //Arreglo auxiliar simulara el area de una memoria real para swapping en FIFO
    float S2[256][4]; //Arreglo auxiliar simulara el area de una memoria real para swapping en LRU

    /*queue<proceso> FIFO; // Queue que guarda los procesos para la politica FIFO
    queue<proceso> LRU; // Queue que guarda los procesos para la politica LRU
    vector<proceso> SFIFO; //Dichos vectores guardaran a los procesos que vayan siendo mandados a swapping
    vector<proceso> SLRU; */

    //Funcion que inicializa la memoria en 0, pues aun no ha llegado ningun proceso
    for(int i =0;i<128;i++){
        M[i][0] = -1;//id
        M[i][1] = -1;//num pagina
        M[i][2] = -1;//timestamp
        M[i][3] = -1;//direccion de memoria
        M2[i][0] = -1;//id
        M2[i][1] = -1;//num pagina
        M2[i][2] = -1;//ultimo uso
        M2[i][3] = -1;//direccion en memoria
    }
    //Funcion que incializa el area de disco reservada para swapping en 0
    for(int i = 0; i < 256;i++){
        S[i][0] = -1;//id
        S[i][1] = -1;//num pagina
        S[i][2] = -1;//timestamp
        S[i][3] = -1;//direccion en memoria
        S2[i][0] = -1;//id
        S2[i][1] = -1;//num pagina
        S2[i][2] = -1;//ultimo uso
        S2[i][3] = -1;//direccion en memoria
    }

    bool finish = false;
    char process;
    float bytes = 0, id = 0, mod = 0, direccion = 0, modificacion = 0;
    while(!finish){

        cin >> process;

        switch (process)
        {
        case 'P':
            cin >> bytes;
            cin >> id;
            cargar(bytes,id, M, S, 1);
            cargar(bytes,id, M2, S2, 2);
            break;
        case 'A':
            cin >> direccion;
            cin >> id;
            cin >> modificacion;
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

    }
    return EXIT_SUCCESS;
}