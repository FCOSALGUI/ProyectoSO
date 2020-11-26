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
        float temporal = 999999;// para comparar el timestamp y ver cuales es el first in en memoria real
        int tempoIndice = -1;//para saber que indice es el que tiene el timestamp mas bajo
        for (int i = 0; i < 128; i++) {
            if (M[i][2] < temporal) {//se checa cual es el first in the todas las paginas
                temporal = M[i][2];
                tempoIndice = i;
            }
        }
        for (int i = 0; i < 256; i++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
            if (S[i][0] == -1) {
                S[i][0] = M[tempoIndice][0];//el id del proceso swapeado
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
                cout << "Pagina " << S[i][1] << " del proceso " << S[i][0] << " swappeada al marco " << i << " del area de swapping" << endl; 
                break;
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
            /*for(int x = 0; x<128;x++){
                //cout << "ID del proceso: " << M[x][0] << endl;
                cout << "Numero del pagina del proceso " << M[x][0] << " es " << M[x][1]<<  endl;
                cout << "Timestamp del proceso " << M[x][0] << " es " << M[x][2]<< endl;
                cout << "Direccion de una pagina del proceso " << M[x][0] << " es " << M[x][3]<< endl;
            }*/

            break;
        }
    }
}

void FIFO(int direccion, int id, int modificacion, float (&M)[128][4], float (&S)[256][4]){
    int pagina = floor(direccion/16);
    int real = 0;
    int menor = 999999;
    int ind = 0;
    cout << "Obtener la direccion real correspondiente a la direccion virtual " << direccion << " del proceso " << id << endl;
    for(int k = 0; k < 128;k++){
        if(M[k][0] == id && M[k][1] == pagina){
            real = (direccion%16) + k * 16;
            if(modificacion == 1){
                cout << "y modificar dicha direccion" <<"\nPagina " << M[k][1] << " del proceso " << M[k][0]<< " modificada " << endl;
            }
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
            tiempo+=0.1;
            break;
        }

        if(k == 127){
            for(int i = 0; i<agregados.size();i++){
                if(agregados[i].idp == id){
                    agregados[i].pagefaultsFIFO++;
                }
            }
            for(int i = 0; i < 256; i++) {
                if(S[i][0] == id && S[i][1] == pagina){
                    for(int j = 0;j<128;j++){
                        if(M[j][2] < menor){
                            menor = M[j][2];
                            ind = j;
                        }
                        for (int x = 0; x < 256; x++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
                            if (S[x][0] == -1) {
                                S[x][0] = M[ind][0];//el id del proceso swapeado
                                S[x][1] = M[ind][1]; //el numero de pagina del proceso
                                S[x][2] = tiempo;//el timestamp al momento de swapear
                                tiempo++;//se incrementa el tiempo por swapear a out
                                S[x][3] = M[ind][3];//se ve cual es la direccion en memoria

                                M[ind][0] = S[i][0];
                                M[ind][1] = S[i][1];
                                M[ind][2] = tiempo;
                                tiempo++;
                                M[ind][3] = S[i][3];

                                S[i][0] = -1;
                                S[i][1] = -1;
                                S[i][2] = -1;
                                S[i][3] = -1;

                                cout << "Pagina " << S[x][1] << " del proceso " << S[x][0] << " swapeada al marco " << x << " del area de swapping" << endl;
                                cout << "Se localizo la pagina " << M[ind][1] << " que estaba en la posicion " << i << " de swapping y cargo al marco " << ind << endl;
                                real = (direccion%16) + k * 16;
                                if(modificacion == 1){
                                    cout << "y modificar dicha direccion" <<"\nPagina " << M[k][1] << " del proceso " << M[k][0]<< " modificada " << endl;
                                }
                                cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
                                tiempo+=0.1;
                                break;
                            }
                        }
                    }
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
            //Output del proceso generado
            cout << "FIFO: Asignar " << bytes << " bytes al proceso " << id << endl;
            cargar(bytes,id, M, S, 1);
            cout << "Se asignaron los marcos de pagina ";
            for(int k = 0; k<127;k++){
                if(M[k][0] == id){
                    cout << k;
                    while(M[k][0] == M[k+1][0]){ 
                        k++;
                    }
                    cout << "-" << k << ", " << endl;
                }
            }
            //Output del proceso cargado
            cout << "LRU: Asignar " << bytes << " bytes al proceso " << id << endl;
            cargar(bytes,id, M2, S2, 2);
            cout << "Se asignaron los marcos de pagina ";
            for(int k = 0; k<127;k++){
                if(M[k][0] == id){
                    cout << k;
                    while(M[k][0] == M[k+1][0]){ 
                        k++;
                    }
                    cout << "-" << k << ", " << endl;
                }
            }
            break;
        case 'A':
            cin >> direccion;
            cin >> id;
            cin >> modificacion;
            FIFO(direccion, id, modificacion, M, S);
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